/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// This file is never compiled standalone, it's included directly from
// extmod/machine_uart.c via MICROPY_PY_MACHINE_UART_INCLUDEFILE.

// std includes
#include <string.h>

// MTB / PDL includes
#include "cybsp.h"
#include "cy_scb_uart.h"
#include "cy_scb_common.h"
#include "cy_sysclk.h"

// MicroPython includes
#include "py/ringbuf.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "py/misc.h"
#include "py/stream.h"

// Port-specific includes
#include "genhdr/pins_af.h"
#include "modmachine.h"
#include "mplogger.h"
#include "mphalport.h"
#include "machine_scb.h"
#include "sys_int.h"

/******************************************************************************/
// Defaults and constants

#define uart_assert_raise_val(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

#define DEFAULT_UART_BAUDRATE     (115200)
#define DEFAULT_UART_BITS         (8)
#define DEFAULT_UART_STOP         (1)
#define DEFAULT_UART_RXBUF_SIZE   (256)

// Hardware flow control flags (compatible with machine.UART.RTS / machine.UART.CTS)
#define UART_FLOW_CONTROL_NONE    (0)
#define UART_FLOW_CONTROL_RTS     (1)
#define UART_FLOW_CONTROL_CTS     (2)

// Internal parity encoding (maps to CY_SCB_UART_PARITY_* when configuring PDL)
#define UART_PARITY_NONE          (0)   // parity=None
#define UART_PARITY_EVEN          (2)   // parity=0
#define UART_PARITY_ODD           (3)   // parity=1

// Class-level constants exposed to Python
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_FLOW_CONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_FLOW_CONTROL_CTS) }, \

/******************************************************************************/
// Object type

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t id;                       // Private variable in this port. ID not associated to any port pin UART group.
    machine_scb_obj_t *scb_obj;       // SCB hardware block + IRQ config
    mp_hal_pin_obj_t tx;
    mp_hal_pin_obj_t rx;
    mp_hal_pin_obj_t rts;             // NULL when not used
    mp_hal_pin_obj_t cts;             // NULL when not used
    uint32_t baudrate;
    uint32_t flow;                    // UART_FLOW_CONTROL_* bitmask
    uint8_t bits;                     // 5-9
    uint8_t parity;                   // UART_PARITY_*
    uint8_t stop;                     // 1 or 2
} machine_uart_obj_t;

static machine_uart_obj_t *mp_machine_uart_alloc(uint8_t uart_id) {
    (void)uart_id;
    machine_uart_obj_t *self = NULL;
    for (uint8_t i = 0; i < MICROPY_PY_MACHINE_UART_NUM_ENTRIES; i++) {
        if (MP_STATE_PORT(machine_uart_obj[i]) == NULL) {
            self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
            MP_STATE_PORT(machine_uart_obj[i]) = self;
            self->id = i;
            break;
        }
    }

    if (self == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("All available UART instances are allocated"));
    }

    return self;
}

static void mp_machine_uart_free(machine_uart_obj_t *self) {
    MP_STATE_PORT(machine_uart_obj[self->id]) = NULL;
}

MP_REGISTER_ROOT_POINTER(struct _machine_uart_obj_t *machine_uart_obj[MICROPY_PY_MACHINE_UART_NUM_ENTRIES]);

static void machine_uart_hw_init(machine_uart_obj_t *self) {

}

/******************************************************************************/
// MicroPython bindings

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    /** TODO: Implement! */
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);

    machine_uart_obj_t *self = mp_machine_uart_alloc(0);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args, args, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

enum {
    ARG_tx, ARG_rx,
    ARG_baudrate, ARG_bits, ARG_parity, ARG_stop,
    ARG_rts, ARG_cts, ARG_flow,
    ARG_timeout, ARG_timeout_char, ARG_rxbuf, ARG_txbuf
};

static const mp_arg_t allowed_args[] = {
    { MP_QSTR_tx,           MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_rx,           MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_baudrate,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_BAUDRATE} },
    { MP_QSTR_bits,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_BITS} },
    { MP_QSTR_parity,       MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_stop,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_STOP} },
    { MP_QSTR_rts,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_cts,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_flow,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_FLOW_CONTROL_NONE} },
    { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_rxbuf,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_RXBUF_SIZE} },
    { MP_QSTR_txbuf,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
};

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // -- TX / RX pins --
    self->tx = mp_hal_get_pin_obj(args[ARG_tx].u_obj);
    self->rx = mp_hal_get_pin_obj(args[ARG_rx].u_obj);

    #define MAX_MACHINE_UART_OBJ_AF_PINS 4
    mp_hal_pin_af_config_t uart_pins_config[MAX_MACHINE_UART_OBJ_AF_PINS];

    MP_HAL_PIN_AF_INIT(uart_pins_config[0], self->tx, CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_UART_TX);
    MP_HAL_PIN_AF_INIT(uart_pins_config[1], self->rx, CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_UART_RX);

    // -- Flow control pins --
    uint8_t num_af_pins = 2U; // TX and RX are always present, RTS and CTS are optional depending on flow control settings
    self->flow = (uint32_t)args[ARG_flow].u_int;
    if (self->flow & UART_FLOW_CONTROL_RTS) {
        if (args[ARG_rts].u_obj == mp_const_none) {
            mp_raise_ValueError(MP_ERROR_TEXT("rts pin required for RTS flow control"));
        }
        self->rts = mp_hal_get_pin_obj(args[ARG_rts].u_obj);
        MP_HAL_PIN_AF_INIT(uart_pins_config[num_af_pins], self->rts, CY_GPIO_DM_STRONG_IN_OFF, 1, MACHINE_PIN_AF_SIGNAL_UART_RTS);
        num_af_pins += 1U;
    } else {
        self->rts = NULL;
    }

    if (self->flow & UART_FLOW_CONTROL_CTS) {
        if (args[ARG_cts].u_obj == mp_const_none) {
            mp_raise_ValueError(MP_ERROR_TEXT("cts pin required for CTS flow control"));
        }
        self->cts = mp_hal_get_pin_obj(args[ARG_cts].u_obj);
        MP_HAL_PIN_AF_INIT(uart_pins_config[num_af_pins], self->cts, CY_GPIO_DM_HIGHZ, 0, MACHINE_PIN_AF_SIGNAL_UART_CTS);
        num_af_pins += 1U;
    } else {
        self->cts = NULL;
    }

    mp_hal_periph_pins_af_config(uart_pins_config, num_af_pins);

    // -- Baudrate --
    self->baudrate = (uint32_t)args[ARG_baudrate].u_int;
    if (self->baudrate == 0U) {
        mp_raise_ValueError(MP_ERROR_TEXT("baudrate must be non-zero"));
    }

    // -- Data bits --
    int bits = args[ARG_bits].u_int;
    if (bits < 5 || bits > 9) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits must be 5-9"));
    }
    self->bits = (uint8_t)bits;

    // -- Parity --
    mp_obj_t parity_arg = args[ARG_parity].u_obj;
    if (parity_arg == mp_const_none) {
        self->parity = UART_PARITY_NONE;
    } else {
        int p = mp_obj_get_int(parity_arg);
        if (p == 0) {
            self->parity = UART_PARITY_EVEN;
        } else if (p == 1) {
            self->parity = UART_PARITY_ODD;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("parity must be None, 0 (even) or 1 (odd)"));
        }
    }

    // -- Stop bits --
    int stop = args[ARG_stop].u_int;
    if (stop < 1 || stop > 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("stop must be 1 or 2"));
    }
    self->stop = (uint8_t)stop;

    // -- Timeouts --
    // TODO:

    // -- RX software ring buffer --
    // TODO:

    // -- Initialise hardware --
    // machine_uart_hw_init(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    /** TODO: Implement! */
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    /** TODO: Implement! */
    return 0;
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    /** TODO: Implement! */
    return true;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    /** TODO: Implement! */
    return 0;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    /** TODO: Implement! */
    return 0;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    /** TODO: Implement! */
    return 0;
}
