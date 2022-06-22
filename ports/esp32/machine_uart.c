/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "modmachine.h"
#include "uart.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 1, 0)
#define UART_INV_TX UART_INVERSE_TXD
#define UART_INV_RX UART_INVERSE_RXD
#define UART_INV_RTS UART_INVERSE_RTS
#define UART_INV_CTS UART_INVERSE_CTS
#else
#define UART_INV_TX UART_SIGNAL_TXD_INV
#define UART_INV_RX UART_SIGNAL_RXD_INV
#define UART_INV_RTS UART_SIGNAL_RTS_INV
#define UART_INV_CTS UART_SIGNAL_CTS_INV
#endif

#define UART_INV_MASK (UART_INV_TX | UART_INV_RX | UART_INV_RTS | UART_INV_CTS)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uart_port_t uart_num;
    uart_hw_flowcontrol_t flowcontrol;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    int8_t tx;
    int8_t rx;
    int8_t rts;
    int8_t cts;
    uint16_t txbuf;
    uint16_t rxbuf;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint32_t invert;        // lines to invert
} machine_uart_obj_t;

STATIC const char *_parity_name[] = {"None", "1", "0"};

/******************************************************************************/
// MicroPython bindings for UART

STATIC void machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t baudrate;
    uart_get_baudrate(self->uart_num, &baudrate);
    mp_printf(print, "UART(%u, baudrate=%u, bits=%u, parity=%s, stop=%u, tx=%d, rx=%d, rts=%d, cts=%d, txbuf=%u, rxbuf=%u, timeout=%u, timeout_char=%u",
        self->uart_num, baudrate, self->bits, _parity_name[self->parity],
        self->stop, self->tx, self->rx, self->rts, self->cts, self->txbuf, self->rxbuf, self->timeout, self->timeout_char);
    if (self->invert) {
        mp_printf(print, ", invert=");
        uint32_t invert_mask = self->invert;
        if (invert_mask & UART_INV_TX) {
            mp_printf(print, "INV_TX");
            invert_mask &= ~UART_INV_TX;
            if (invert_mask) {
                mp_printf(print, "|");
            }
        }
        if (invert_mask & UART_INV_RX) {
            mp_printf(print, "INV_RX");
            invert_mask &= ~UART_INV_RX;
            if (invert_mask) {
                mp_printf(print, "|");
            }
        }
        if (invert_mask & UART_INV_RTS) {
            mp_printf(print, "INV_RTS");
            invert_mask &= ~UART_INV_RTS;
            if (invert_mask) {
                mp_printf(print, "|");
            }
        }
        if (invert_mask & UART_INV_CTS) {
            mp_printf(print, "INV_CTS");
        }
    }
    if (self->flowcontrol) {
        mp_printf(print, ", flow=");
        uint32_t flow_mask = self->flowcontrol;
        if (flow_mask & UART_HW_FLOWCTRL_RTS) {
            mp_printf(print, "RTS");
            flow_mask &= ~UART_HW_FLOWCTRL_RTS;
            if (flow_mask) {
                mp_printf(print, "|");
            }
        }
        if (flow_mask & UART_HW_FLOWCTRL_CTS) {
            mp_printf(print, "CTS");
        }
    }
    mp_printf(print, ")");
}

STATIC void machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_rts, ARG_cts, ARG_txbuf, ARG_rxbuf, ARG_timeout, ARG_timeout_char, ARG_invert, ARG_flow };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_PIN_NO_CHANGE} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_PIN_NO_CHANGE} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_PIN_NO_CHANGE} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_PIN_NO_CHANGE} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_invert, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // wait for all data to be transmitted before changing settings
    uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000));

    if (args[ARG_txbuf].u_int >= 0 || args[ARG_rxbuf].u_int >= 0) {
        // must reinitialise driver to change the tx/rx buffer size
        if (self->uart_num == MICROPY_HW_UART_REPL) {
            mp_raise_ValueError(MP_ERROR_TEXT("UART buffer size is fixed"));
        }

        if (args[ARG_txbuf].u_int >= 0) {
            self->txbuf = args[ARG_txbuf].u_int;
        }
        if (args[ARG_rxbuf].u_int >= 0) {
            self->rxbuf = args[ARG_rxbuf].u_int;
        }
        uart_config_t uartcfg = {
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 0
        };
        uint32_t baudrate;
        uart_get_baudrate(self->uart_num, &baudrate);
        uartcfg.baud_rate = baudrate;
        uart_get_word_length(self->uart_num, &uartcfg.data_bits);
        uart_get_parity(self->uart_num, &uartcfg.parity);
        uart_get_stop_bits(self->uart_num, &uartcfg.stop_bits);
        uart_driver_delete(self->uart_num);
        uart_param_config(self->uart_num, &uartcfg);
        uart_driver_install(self->uart_num, self->rxbuf, self->txbuf, 0, NULL, 0);
    }

    // set baudrate
    uint32_t baudrate = 115200;
    if (args[ARG_baudrate].u_int > 0) {
        uart_set_baudrate(self->uart_num, args[ARG_baudrate].u_int);
    }
    uart_get_baudrate(self->uart_num, &baudrate);

    uart_set_pin(self->uart_num, args[ARG_tx].u_int, args[ARG_rx].u_int, args[ARG_rts].u_int, args[ARG_cts].u_int);
    if (args[ARG_tx].u_int != UART_PIN_NO_CHANGE) {
        self->tx = args[ARG_tx].u_int;
    }

    if (args[ARG_rx].u_int != UART_PIN_NO_CHANGE) {
        self->rx = args[ARG_rx].u_int;
    }

    if (args[ARG_rts].u_int != UART_PIN_NO_CHANGE) {
        self->rts = args[ARG_rts].u_int;
    }

    if (args[ARG_cts].u_int != UART_PIN_NO_CHANGE) {
        self->cts = args[ARG_cts].u_int;
    }

    // set data bits
    switch (args[ARG_bits].u_int) {
        case 0:
            break;
        case 5:
            uart_set_word_length(self->uart_num, UART_DATA_5_BITS);
            self->bits = 5;
            break;
        case 6:
            uart_set_word_length(self->uart_num, UART_DATA_6_BITS);
            self->bits = 6;
            break;
        case 7:
            uart_set_word_length(self->uart_num, UART_DATA_7_BITS);
            self->bits = 7;
            break;
        case 8:
            uart_set_word_length(self->uart_num, UART_DATA_8_BITS);
            self->bits = 8;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid data bits"));
            break;
    }

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            uart_set_parity(self->uart_num, UART_PARITY_DISABLE);
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            if (parity & 1) {
                uart_set_parity(self->uart_num, UART_PARITY_ODD);
                self->parity = 1;
            } else {
                uart_set_parity(self->uart_num, UART_PARITY_EVEN);
                self->parity = 2;
            }
        }
    }

    // set stop bits
    switch (args[ARG_stop].u_int) {
        // FIXME: ESP32 also supports 1.5 stop bits
        case 0:
            break;
        case 1:
            uart_set_stop_bits(self->uart_num, UART_STOP_BITS_1);
            self->stop = 1;
            break;
        case 2:
            uart_set_stop_bits(self->uart_num, UART_STOP_BITS_2);
            self->stop = 2;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid stop bits"));
            break;
    }

    // set timeout
    if (args[ARG_timeout].u_int != -1) {
        self->timeout = args[ARG_timeout].u_int;
    }

    // set timeout_char
    // make sure it is at least as long as a whole character (13 bits to be safe)
    if (args[ARG_timeout_char].u_int != -1) {
        self->timeout_char = args[ARG_timeout_char].u_int;
        uint32_t min_timeout_char = 13000 / baudrate + 1;
        if (self->timeout_char < min_timeout_char) {
            self->timeout_char = min_timeout_char;
        }
    }

    // set line inversion
    if (args[ARG_invert].u_int != -1) {
        if (args[ARG_invert].u_int & ~UART_INV_MASK) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid inversion mask"));
        }
        self->invert = args[ARG_invert].u_int;
    }
    uart_set_line_inverse(self->uart_num, self->invert);

    // set hardware flow control
    if (args[ARG_flow].u_int != -1) {
        if (args[ARG_flow].u_int & ~UART_HW_FLOWCTRL_CTS_RTS) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid flow control mask"));
        }
        self->flowcontrol = args[ARG_flow].u_int;
    }
    uart_set_hw_flow_ctrl(self->uart_num, self->flowcontrol, UART_FIFO_LEN - UART_FIFO_LEN / 4);
}

STATIC mp_obj_t machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // get uart id
    mp_int_t uart_num = mp_obj_get_int(args[0]);
    if (uart_num < 0 || uart_num >= UART_NUM_MAX) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("UART(%d) does not exist"), uart_num);
    }

    // Defaults
    uart_config_t uartcfg = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 0
    };

    // create instance
    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
    self->uart_num = uart_num;
    self->bits = 8;
    self->parity = 0;
    self->stop = 1;
    self->rts = UART_PIN_NO_CHANGE;
    self->cts = UART_PIN_NO_CHANGE;
    self->txbuf = 256;
    self->rxbuf = 256; // IDF minimum
    self->timeout = 0;
    self->timeout_char = 0;
    self->invert = 0;
    self->flowcontrol = 0;

    switch (uart_num) {
        case UART_NUM_0:
            self->rx = UART_PIN_NO_CHANGE; // GPIO 3
            self->tx = UART_PIN_NO_CHANGE; // GPIO 1
            break;
        case UART_NUM_1:
            self->rx = 9;
            self->tx = 10;
            break;
        #if SOC_UART_NUM > 2
        case UART_NUM_2:
            self->rx = 16;
            self->tx = 17;
            break;
        #endif
    }

    // Only reset the driver if it's not the REPL UART.
    if (uart_num != MICROPY_HW_UART_REPL) {
        // Remove any existing configuration
        uart_driver_delete(self->uart_num);

        // init the peripheral
        // Setup
        uart_param_config(self->uart_num, &uartcfg);

        uart_driver_install(uart_num, self->rxbuf, self->txbuf, 0, NULL, 0);
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    // Make sure pins are connected.
    uart_set_pin(self->uart_num, self->tx, self->rx, self->rts, self->cts);

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_uart_init(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    machine_uart_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(machine_uart_init_obj, 1, machine_uart_init);

STATIC mp_obj_t machine_uart_deinit(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uart_driver_delete(self->uart_num);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_deinit_obj, machine_uart_deinit);

STATIC mp_obj_t machine_uart_any(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size_t rxbufsize;
    uart_get_buffered_data_len(self->uart_num, &rxbufsize);
    return MP_OBJ_NEW_SMALL_INT(rxbufsize);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_any_obj, machine_uart_any);

STATIC mp_obj_t machine_uart_sendbreak(mp_obj_t self_in) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Save settings
    uart_word_length_t word_length;
    uart_parity_t parity;
    uart_get_word_length(self->uart_num, &word_length);
    uart_get_parity(self->uart_num, &parity);

    // Synthesise the break condition by either a longer word or using even parity
    uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000));
    if (word_length != UART_DATA_8_BITS) {
        uart_set_word_length(self->uart_num, UART_DATA_8_BITS);
    } else if (parity == UART_PARITY_DISABLE) {
        uart_set_parity(self->uart_num, UART_PARITY_EVEN);
    } else {
        // Cannot synthesise break
        mp_raise_OSError(MP_EPERM);
    }
    char buf[1] = {0};
    uart_write_bytes(self->uart_num, buf, 1);
    uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000));

    // Restore original settings
    uart_set_word_length(self->uart_num, word_length);
    uart_set_parity(self->uart_num, parity);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_uart_sendbreak_obj, machine_uart_sendbreak);

STATIC const mp_rom_map_elem_t machine_uart_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_uart_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&machine_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&machine_uart_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendbreak), MP_ROM_PTR(&machine_uart_sendbreak_obj) },

    { MP_ROM_QSTR(MP_QSTR_INV_TX), MP_ROM_INT(UART_INV_TX) },
    { MP_ROM_QSTR(MP_QSTR_INV_RX), MP_ROM_INT(UART_INV_RX) },
    { MP_ROM_QSTR(MP_QSTR_INV_RTS), MP_ROM_INT(UART_INV_RTS) },
    { MP_ROM_QSTR(MP_QSTR_INV_CTS), MP_ROM_INT(UART_INV_CTS) },

    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HW_FLOWCTRL_RTS) },
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HW_FLOWCTRL_CTS) },
};

STATIC MP_DEFINE_CONST_DICT(machine_uart_locals_dict, machine_uart_locals_dict_table);

STATIC mp_uint_t machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    TickType_t time_to_wait;
    if (self->timeout == 0) {
        time_to_wait = 0;
    } else {
        time_to_wait = pdMS_TO_TICKS(self->timeout);
    }

    int bytes_read = uart_read_bytes(self->uart_num, buf_in, size, time_to_wait);

    if (bytes_read <= 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_read;
}

STATIC mp_uint_t machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int bytes_written = uart_write_bytes(self->uart_num, buf_in, size);

    if (bytes_written < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // return number of bytes written
    return bytes_written;
}

STATIC mp_uint_t machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        size_t rxbufsize;
        uart_get_buffered_data_len(self->uart_num, &rxbufsize);
        if ((flags & MP_STREAM_POLL_RD) && rxbufsize > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && 1) { // FIXME: uart_tx_any_room(self->uart_num)
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

STATIC const mp_stream_p_t uart_stream_p = {
    .read = machine_uart_read,
    .write = machine_uart_write,
    .ioctl = machine_uart_ioctl,
    .is_text = false,
};

const mp_obj_type_t machine_uart_type = {
    { &mp_type_type },
    .name = MP_QSTR_UART,
    .print = machine_uart_print,
    .make_new = machine_uart_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &uart_stream_p,
    .locals_dict = (mp_obj_dict_t *)&machine_uart_locals_dict,
};
