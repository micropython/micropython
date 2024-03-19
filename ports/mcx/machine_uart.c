/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
 * Copyright (c) 2024 NXP
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

#include "py/ringbuf.h"

#include "drv_uart.h"

#define MACHINE_UART_FC_FLAG_RTS 1U
#define MACHINE_UART_FC_FLAG_CTS 2U

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(MACHINE_UART_FC_FLAG_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(MACHINE_UART_FC_FLAG_CTS) }, \

struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uint8_t id;
    mp_uint_t timeout;
    drv_uart_t drv;
    ringbuf_t rx_buf;
};

static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    drv_uart_send_break(&self->drv);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return ringbuf_avail(&self->rx_buf);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return true;
}

static mp_int_t mp_machine_uart_readchar(machine_uart_obj_t *self) {
    uint8_t ch = 0U;
    drv_uart_read(&self->drv, &ch, 1U, self->timeout);
    return ch;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    uint8_t *rx_buf = buf_in;

    mp_uint_t buf_avail = ringbuf_avail(&self->rx_buf);
    if (buf_avail > size) {
        buf_avail = size;
    }

    ringbuf_get_bytes(&self->rx_buf, rx_buf, buf_avail);

    if (buf_avail < size) {
        mp_uint_t bytes_left = size - buf_avail;

        mp_uint_t time_start = mp_hal_ticks_ms();

        while (bytes_left) {
            if (ringbuf_avail(&self->rx_buf)) {
                rx_buf[buf_avail++] = ringbuf_get(&self->rx_buf);
                bytes_left--;
            }

            if (self->timeout && ((mp_hal_ticks_ms() - time_start) > self->timeout)) {
                *errcode = MP_EAGAIN;
                break;
            }

            MICROPY_EVENT_POLL_HOOK
        }
    }

    return buf_avail;
}

static void mp_machine_uart_writechar(machine_uart_obj_t *self, uint16_t data) {
    uint8_t data_buf[1] = { data };

    /* TODO: Use async write? */
    /* TODO: Check return values */
    drv_uart_write(&self->drv, data_buf, 1U, self->timeout);
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    /* TODO: Use async write? */
    /* TODO: Check return values */
    drv_uart_write(&self->drv, buf_in, size, self->timeout);

    return 0;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_uint_t ret = 0;

    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    switch (request) {
        case MP_STREAM_POLL:
            if ((arg & MP_STREAM_POLL_RD) && ringbuf_avail(&self->rx_buf)) {
                ret |= MP_STREAM_POLL_RD;
            }
            break;

        case MP_STREAM_FLUSH:
            /* TX is blocking, so no flush is needed. */
            break;

        default:
            ret = MP_STREAM_ERROR;
            break;
    }

    return ret;
}

static void mp_machine_uart_async_rx_callback(void *uart, void *param, uint8_t len) {
    machine_uart_obj_t *self = param;

    uint8_t ch;

    for (uint8_t i = 0; i < len; i++) {
        if (drv_uart_async_read(uart, &ch, 1) == 1) {
            ringbuf_put(&self->rx_buf, ch);
        }
    }
}

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    mp_printf(print, "UART(%d)", self->id);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop,
           ARG_tx, ARG_rx, ARG_cts, ARG_rts, ARG_flow,
           ARG_timeout, ARG_timeout_char, ARG_txbuf, ARG_rxbuf };

    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate,     MP_ARG_INT,                  {.u_int = 9600U} },
        { MP_QSTR_bits,         MP_ARG_INT,                  {.u_int = 8} },
        { MP_QSTR_parity,       MP_ARG_OBJ,                  {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_stop,         MP_ARG_INT,                  {.u_int = 1} },
        { MP_QSTR_tx,           MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rx,           MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_cts,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_rts,          MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_flow,         MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_txbuf,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
        { MP_QSTR_rxbuf,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    drv_uart_config_t uart_cfg;

    uart_cfg.baud_rate = args[ARG_baudrate].u_int;

    switch (args[ARG_bits].u_int) {
        case 7:
            uart_cfg.data_bits = DRV_Uart7DataBits;
            break;

        case 8:
            uart_cfg.data_bits = DRV_Uart8DataBits;
            break;

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported combination of bits and parity"));
            break;
    }


    if (args[ARG_parity].u_obj == mp_const_none) {
        uart_cfg.parity = DRV_UartNoParity;
    } else {
        if (args[ARG_parity].u_int) {
            uart_cfg.parity = DRV_UartOddParity;
        } else {
            uart_cfg.parity = DRV_UartEvenParity;
        }
    }

    switch (args[ARG_stop].u_int) {
        case 1:
            uart_cfg.stop_bits = DRV_Uart1StopBits;
            break;
        case 2:
            uart_cfg.stop_bits = DRV_Uart2StopBits;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unsupportd stop bit"));
            break;
    }

    switch (args[ARG_flow].u_int) {
        case (MACHINE_UART_FC_FLAG_RTS):
            uart_cfg.flow_control = DRV_UartRtsFlowControl;
            break;

        case (MACHINE_UART_FC_FLAG_CTS):
            uart_cfg.flow_control = DRV_UartCtsFlowControl;
            break;

        case (MACHINE_UART_FC_FLAG_RTS | MACHINE_UART_FC_FLAG_CTS):
            uart_cfg.flow_control = DRV_UartCtsRtsFlowControl;
            break;

        default:
            uart_cfg.flow_control = DRV_UartNoFlowControl;
            break;
    }

    if (args[ARG_tx].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for TX */
    } else {
        /* TODO: Non LP-Flexcomm pinmap for future A series */
        mp_hal_pin_obj_t pin = args[ARG_tx].u_obj;
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P1);
        if (af == NULL) {
            af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P2);
            if (af == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("unsupported TX AF"));
            }

            /* TODO: Validate if both TX and RX are I2C-remapped mode. */
            uart_cfg.i2c_shared = true;
        }

        mp_hal_pin_af(pin, af->af_id);
    }

    uart_cfg.i2c_shared = false;

    if (args[ARG_rx].u_obj == mp_const_none) {
        /* TODO: Prepare a default pin for RX */
    } else {
        /* TODO: Non LP-Flexcomm pinmap for future A series */
        mp_hal_pin_obj_t pin = args[ARG_rx].u_obj;
        const machine_pin_af_t *af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P0);
        if (af == NULL) {
            af = mp_hal_pin_find_af(pin, MACHINE_PIN_AF_TYPE_LPFC, self->id, MACHINE_PIN_AF_ATTR_LPFC_P3);
            if (af == NULL) {
                mp_raise_ValueError(MP_ERROR_TEXT("unsupported RX AF"));
            }

            /* TODO: Validate if both TX and RX are I2C-remapped mode. */
            uart_cfg.i2c_shared = true;
        }

        mp_hal_pin_af(pin, af->af_id);
    }

    self->timeout = args[ARG_timeout].u_int;

    ringbuf_alloc(&self->rx_buf, args[ARG_rxbuf].u_int + 1);

    drv_uart_init(&self->drv, self->id, &uart_cfg);

    drv_uart_async_start(&self->drv, mp_machine_uart_async_rx_callback, self);
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);


    if (!mp_obj_is_int(args[0])) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported UART id."));
        return mp_const_none;
    }

    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);

    self->id = mp_obj_get_int(args[0]);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    drv_uart_async_cancel(&self->drv);

    drv_uart_deinit(&self->drv);
}
