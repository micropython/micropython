/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 OpenMV LLC.
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

#include "py/mphal.h"
#include "py/mperrno.h"
#include "mpuart.h"

#define DEFAULT_UART_BAUDRATE (115200)
#define DEFAULT_UART_BITS (8)
#define DEFAULT_UART_PARITY (MP_ROM_NONE)
#define DEFAULT_UART_STOP (1)
#define DEFAULT_UART_RX_BUFFER_SIZE (256)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    unsigned int uart_id;
    uint32_t baudrate;
    UART_DATA_BITS bits;
    UART_PARITY parity;
    UART_STOP_BITS stop;
    mp_hal_pin_obj_t tx;
    mp_hal_pin_obj_t rx;
    mp_hal_pin_obj_t rts;
    mp_hal_pin_obj_t cts;
    uint32_t flow;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint8_t *rx_ringbuf_array;
    ringbuf_t rx_ringbuf;
    uint32_t irq_flags;
    uint32_t irq_trigger;
    mp_irq_obj_t irq_obj;
} machine_uart_obj_t;

typedef struct _machine_uart_default_pins_t {
    mp_hal_pin_obj_t tx;
    mp_hal_pin_obj_t rx;
    mp_hal_pin_obj_t rts;
    mp_hal_pin_obj_t cts;
} machine_uart_default_pins_t;

static const machine_uart_default_pins_t machine_uart_default_pins[UART_MAX] = {
    [0] = {
        #if defined(MICROPY_HW_UART0_TX) && defined(MICROPY_HW_UART0_RX)
        MICROPY_HW_UART0_TX, MICROPY_HW_UART0_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART0_RTS) && defined(MICROPY_HW_UART0_CTS)
        MICROPY_HW_UART0_RTS, MICROPY_HW_UART0_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [1] = {
        #if defined(MICROPY_HW_UART1_TX) && defined(MICROPY_HW_UART1_RX)
        MICROPY_HW_UART1_TX, MICROPY_HW_UART1_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART1_RTS) && defined(MICROPY_HW_UART1_CTS)
        MICROPY_HW_UART1_RTS, MICROPY_HW_UART1_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [2] = {
        #if defined(MICROPY_HW_UART2_TX) && defined(MICROPY_HW_UART2_RX)
        MICROPY_HW_UART2_TX, MICROPY_HW_UART2_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART2_RTS) && defined(MICROPY_HW_UART2_CTS)
        MICROPY_HW_UART2_RTS, MICROPY_HW_UART2_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [3] = {
        #if defined(MICROPY_HW_UART3_TX) && defined(MICROPY_HW_UART3_RX)
        MICROPY_HW_UART3_TX, MICROPY_HW_UART3_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART3_RTS) && defined(MICROPY_HW_UART3_CTS)
        MICROPY_HW_UART3_RTS, MICROPY_HW_UART3_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [4] = {
        #if defined(MICROPY_HW_UART4_TX) && defined(MICROPY_HW_UART4_RX)
        MICROPY_HW_UART4_TX, MICROPY_HW_UART4_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART4_RTS) && defined(MICROPY_HW_UART4_CTS)
        MICROPY_HW_UART4_RTS, MICROPY_HW_UART4_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [5] = {
        #if defined(MICROPY_HW_UART5_TX) && defined(MICROPY_HW_UART5_RX)
        MICROPY_HW_UART5_TX, MICROPY_HW_UART5_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART5_RTS) && defined(MICROPY_HW_UART5_CTS)
        MICROPY_HW_UART5_RTS, MICROPY_HW_UART5_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [6] = {
        #if defined(MICROPY_HW_UART6_TX) && defined(MICROPY_HW_UART6_RX)
        MICROPY_HW_UART6_TX, MICROPY_HW_UART6_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART6_RTS) && defined(MICROPY_HW_UART6_CTS)
        MICROPY_HW_UART6_RTS, MICROPY_HW_UART6_CTS,
        #else
        NULL, NULL,
        #endif
    },
    [7] = {
        #if defined(MICROPY_HW_UART7_TX) && defined(MICROPY_HW_UART7_RX)
        MICROPY_HW_UART7_TX, MICROPY_HW_UART7_RX,
        #else
        NULL, NULL,
        #endif
        #if defined(MICROPY_HW_UART7_RTS) && defined(MICROPY_HW_UART7_CTS)
        MICROPY_HW_UART7_RTS, MICROPY_HW_UART7_CTS,
        #else
        NULL, NULL,
        #endif
    },
};

static void machine_uart_irq_callback(unsigned int uart_id, unsigned int trigger);

MP_REGISTER_ROOT_POINTER(struct _machine_uart_obj_t *machine_uart_obj_all[UART_MAX]);

/******************************************************************************/
// MicroPython bindings for UART

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_uart___del___obj) }, \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_FLOW_CONTROL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_FLOW_CONTROL_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RX), MP_ROM_INT(MP_UART_IRQ_RX) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_RXIDLE), MP_ROM_INT(MP_UART_IRQ_RXIDLE) }, \
    { MP_ROM_QSTR(MP_QSTR_IRQ_TXIDLE), MP_ROM_INT(MP_UART_IRQ_TXIDLE) }, \

#define GET_PIN_WITH_DEFAULT(uart_id_, pin_name, pin_selection) \
    (pin_selection == mp_const_none ? machine_uart_default_pins[uart_id_].pin_name : mp_hal_get_pin_obj(pin_selection))

static void machine_uart_set_bits(machine_uart_obj_t *self, mp_int_t bits) {
    if (!(5 <= bits && bits <= 8)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid bits"));
    }
    self->bits = UART_DATA_BITS_5 + (bits - 5);
}

static void machine_uart_set_parity(machine_uart_obj_t *self, mp_obj_t parity) {
    if (parity == mp_const_none) {
        self->parity = UART_PARITY_NONE;
    } else if (mp_obj_get_int(parity) & 1) {
        self->parity = UART_PARITY_ODD;
    } else {
        self->parity = UART_PARITY_EVEN;
    }
}

static void machine_uart_set_stop(machine_uart_obj_t *self, mp_int_t stop) {
    if (!(1 <= stop && stop <= 2)) {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid stop"));
    }
    self->stop = UART_STOP_BITS_1 + (stop - 1);
}

static void machine_uart_set_timeout_char(machine_uart_obj_t *self, mp_int_t timeout_char) {
    // Make sure timeout_char is at least as long as a whole character (13 bits to be safe).
    uint32_t min_timeout_char = 13000 / self->baudrate + 1;
    self->timeout_char = MAX(min_timeout_char, timeout_char);
}

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    static const char *parity_name[] = {[UART_PARITY_NONE] = "None", [UART_PARITY_EVEN] = "0", [UART_PARITY_ODD] = "1"};

    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    unsigned int bits = 5 + (self->bits - UART_DATA_BITS_5);
    const char *parity = parity_name[self->parity];
    unsigned int stop = 1 + (self->stop - UART_STOP_BITS_1);

    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, "
        "tx=%q, rx=%q, ",
        self->uart_id, self->baudrate, bits, parity, stop,
        mp_hal_pin_name(self->tx), mp_hal_pin_name(self->rx));
    if (self->rts != NULL) {
        mp_printf(print, "rts=%q, ", mp_hal_pin_name(self->rts));
    }
    if (self->cts != NULL) {
        mp_printf(print, "cts=%q, ", mp_hal_pin_name(self->cts));
    }
    mp_printf(print, "flow=", self->rts);
    if (self->flow == 0) {
        mp_printf(print, "0");
    } else {
        if (self->flow & UART_FLOW_CONTROL_RTS) {
            mp_printf(print, "RTS");
            if (self->flow & UART_FLOW_CONTROL_CTS) {
                mp_printf(print, "|");
            }
        }
        if (self->flow & UART_FLOW_CONTROL_CTS) {
            mp_printf(print, "CTS");
        }
    }
    mp_printf(print, ", timeout=%u, timeout_char=%u, rxbuf=%u)",
        self->timeout, self->timeout_char, self->rx_ringbuf.size - 1);
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum {
        ARG_id,
        ARG_baudrate, ARG_bits, ARG_parity, ARG_stop,
        ARG_tx, ARG_rx, ARG_rts, ARG_cts,
        ARG_flow, ARG_timeout, ARG_timeout_char, ARG_rxbuf,
    };
    static const mp_arg_t allowed_args[] = {
        #if !defined(MICROPY_HW_DEFAULT_UART_ID)
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = -1} },
        #else
        { MP_QSTR_id, MP_ARG_INT, {.u_int = MICROPY_HW_DEFAULT_UART_ID} },
        #endif
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = DEFAULT_UART_BAUDRATE} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = DEFAULT_UART_BITS} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = DEFAULT_UART_PARITY} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = DEFAULT_UART_STOP} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_UART_RX_BUFFER_SIZE} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get UART bus.
    int uart_id = args[ARG_id].u_int;
    if (uart_id < 0 || uart_id >= UART_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) doesn't exist"), uart_id);
    }

    machine_uart_obj_t *self;
    bool need_init;
    if (MP_STATE_PORT(machine_uart_obj_all)[uart_id] == NULL) {
        // Create new UART object.
        self = mp_obj_malloc_with_finaliser(machine_uart_obj_t, &machine_uart_type);
        self->uart_id = uart_id;
        need_init = true;
    } else {
        // Reference existing UART object.
        self = MP_STATE_PORT(machine_uart_obj_all)[uart_id];
        need_init = n_args > 1 || n_kw > 0;
    }

    if (need_init) {
        // Set the UART parameters.
        self->baudrate = args[ARG_baudrate].u_int;
        machine_uart_set_bits(self, args[ARG_bits].u_int);
        machine_uart_set_parity(self, args[ARG_parity].u_obj);
        machine_uart_set_stop(self, args[ARG_stop].u_int);
        self->tx = GET_PIN_WITH_DEFAULT(self->uart_id, tx, args[ARG_tx].u_obj);
        self->rx = GET_PIN_WITH_DEFAULT(self->uart_id, rx, args[ARG_rx].u_obj);
        self->rts = GET_PIN_WITH_DEFAULT(self->uart_id, rts, args[ARG_rts].u_obj);
        self->cts = GET_PIN_WITH_DEFAULT(self->uart_id, cts, args[ARG_cts].u_obj);
        self->flow = args[ARG_flow].u_int;
        self->timeout = args[ARG_timeout].u_int;
        uint32_t min_timeout_char = 13000 / self->baudrate + 1; // 13 bits to be safe
        self->timeout = MAX(min_timeout_char, args[ARG_timeout_char].u_int);

        // Check TX/RX pins are given.
        if (self->tx == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("tx not given"));
        }
        if (self->rx == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("rx not given"));
        }

        // Configure flow control.
        mp_hal_pin_obj_t rts = NULL;
        mp_hal_pin_obj_t cts = NULL;
        if (self->flow & UART_FLOW_CONTROL_RTS) {
            if (self->rts == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("rts not given"));
            }
            rts = self->rts;
        }
        if (self->flow & UART_FLOW_CONTROL_CTS) {
            if (self->cts == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("cts not given"));
            }
            cts = self->cts;
        }

        // Allocate the RX buffer.
        size_t rxbuf_len = args[ARG_rxbuf].u_int;
        self->rx_ringbuf_array = m_new(uint8_t, rxbuf_len + 1);
        self->rx_ringbuf.buf = self->rx_ringbuf_array;
        self->rx_ringbuf.size = rxbuf_len + 1;
        self->rx_ringbuf.iput = 0;
        self->rx_ringbuf.iget = 0;

        // Reset the IRQ state
        self->irq_flags = 0;
        self->irq_trigger = 0;
        self->irq_obj.base.type = NULL;

        // Initialize the UART hardware.
        mp_uart_init(self->uart_id, self->baudrate, self->bits, self->parity, self->stop, self->tx, self->rx, &self->rx_ringbuf);

        mp_uart_set_flow(self->uart_id, rts, cts);
    }

    MP_STATE_PORT(machine_uart_obj_all)[uart_id] = self;

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum {
        ARG_baudrate, ARG_bits, ARG_parity, ARG_stop,
        ARG_timeout, ARG_timeout_char,
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_INT(-1)} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    bool change_baudrate = false;
    bool change_bits_parity_stop = false;

    // Change baudrate if requested.
    if (args[ARG_baudrate].u_int > 0) {
        self->baudrate = args[ARG_baudrate].u_int;
        change_baudrate = true;
    }

    // Change data bits if requested.
    if (args[ARG_bits].u_int > 0) {
        machine_uart_set_bits(self, args[ARG_bits].u_int);
        change_bits_parity_stop = true;
    }

    // Change parity if requested.
    if (args[ARG_parity].u_obj != MP_OBJ_NEW_SMALL_INT(-1)) {
        machine_uart_set_parity(self, args[ARG_parity].u_obj);
        change_bits_parity_stop = true;
    }

    // Change stop bits if requested.
    if (args[ARG_stop].u_int > 0) {
        machine_uart_set_stop(self, args[ARG_stop].u_int);
        change_bits_parity_stop = true;
    }

    // Change timeout if requested.
    if (args[ARG_timeout].u_int >= 0) {
        self->timeout = args[ARG_timeout].u_int;
    }

    // Change timeout_char if requested.
    if (args[ARG_timeout_char].u_int >= 0) {
        machine_uart_set_timeout_char(self, args[ARG_timeout_char].u_int);
    }

    // Reconfigure the hardware parameters that have changed.
    if (change_baudrate) {
        mp_uart_set_baudrate(self->uart_id, self->baudrate);
    }
    if (change_bits_parity_stop) {
        mp_uart_set_bits_parity_stop(self->uart_id, self->bits, self->parity, self->stop);
    }
}

static mp_obj_t machine_uart___del__(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uart_deinit(self->uart_id);
    MP_STATE_PORT(machine_uart_obj_all)[self->uart_id] = NULL;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_uart___del___obj, machine_uart___del__);

// Turn off the UART bus.
static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    mp_uart_deinit(self->uart_id);
}

// Return number of characters waiting.
static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return mp_uart_rx_any(self->uart_id);
}

// Since uart.write() waits up to the last byte, uart.txdone() always returns True.
static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return !!mp_uart_tx_any(self->uart_id);
}

// Change the trigger for the UART IRQ.
static mp_uint_t machine_uart_irq_trigger(mp_obj_t self_in, mp_uint_t new_trigger) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    self->irq_trigger = new_trigger;
    mp_uart_set_irq_callback(self->uart_id, self->irq_trigger, machine_uart_irq_callback);
    return 0;
}

// Get UART IRQ state.
static mp_uint_t machine_uart_irq_info(mp_obj_t self_in, mp_uint_t info_type) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (info_type == MP_IRQ_INFO_FLAGS) {
        return self->irq_flags;
    } else if (info_type == MP_IRQ_INFO_TRIGGERS) {
        return self->irq_trigger;
    }
    return 0;
}

static const mp_irq_methods_t machine_uart_irq_methods = {
    .trigger = machine_uart_irq_trigger,
    .info = machine_uart_irq_info,
};

// Retrieve and configure the UART IRQ object.
static mp_irq_obj_t *mp_machine_uart_irq(machine_uart_obj_t *self, bool any_args, mp_arg_val_t *args) {
    if (self->irq_obj.base.type == NULL) {
        mp_irq_init(&self->irq_obj, &machine_uart_irq_methods, MP_OBJ_FROM_PTR(self));
    }

    if (any_args) {
        self->irq_obj.handler = args[MP_IRQ_ARG_INIT_handler].u_obj;
        self->irq_obj.ishard = args[MP_IRQ_ARG_INIT_hard].u_bool;
        machine_uart_irq_trigger(MP_OBJ_FROM_PTR(self), args[MP_IRQ_ARG_INIT_trigger].u_int);
    }

    return &self->irq_obj;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t start = mp_hal_ticks_ms();
    mp_uint_t timeout = self->timeout;
    uint8_t *dest = buf_in;

    for (size_t i = 0; i < size; i++) {
        // Wait for the first/next character
        while (!mp_uart_rx_any(self->uart_id)) {
            mp_uint_t elapsed = mp_hal_ticks_ms() - start;
            if (elapsed > timeout) {  // timed out
                if (i <= 0) {
                    *errcode = MP_EAGAIN;
                    return MP_STREAM_ERROR;
                } else {
                    return i;
                }
            }
            mp_event_handle_nowait();
        }
        *dest++ = mp_uart_rx_char(self->uart_id);
        start = mp_hal_ticks_ms(); // Inter-character timeout
        timeout = self->timeout_char;
    }
    return size;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uart_tx_data(self->uart_id, buf_in, size);
    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && mp_uart_rx_any(self->uart_id)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        uint32_t t0 = mp_hal_ticks_ms();
        const uint32_t timeout_ms = 100 + 10 * mp_uart_tx_any(self->uart_id);
        while (mp_uart_tx_any(self->uart_id)) {
            if (mp_hal_ticks_ms() - t0 > timeout_ms) {
                *errcode = MP_ETIMEDOUT;
                ret = MP_STREAM_ERROR;
            }
            mp_event_handle_nowait();
        }
        return 0;
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

static void machine_uart_irq_callback(unsigned int uart_id, unsigned int trigger) {
    machine_uart_obj_t *self = MP_STATE_PORT(machine_uart_obj_all)[uart_id];
    self->irq_flags = trigger;
    if (self->irq_obj.base.type != NULL && (self->irq_flags & self->irq_trigger)) {
        mp_irq_handler(&self->irq_obj);
    }
}
