/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2023 Damien P. George
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

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "uart.h"

#if SOC_UART_SUPPORT_XTAL_CLK
// Works independently of APB frequency, on ESP32C3, ESP32S3.
#define UART_SOURCE_CLK UART_SCLK_XTAL
#else
#define UART_SOURCE_CLK UART_SCLK_DEFAULT
#endif

#define UART_INV_TX UART_SIGNAL_TXD_INV
#define UART_INV_RX UART_SIGNAL_RXD_INV
#define UART_INV_RTS UART_SIGNAL_RTS_INV
#define UART_INV_CTS UART_SIGNAL_CTS_INV

#define UART_INV_MASK (UART_INV_TX | UART_INV_RX | UART_INV_RTS | UART_INV_CTS)

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    uart_port_t uart_num;
    uart_hw_flowcontrol_t flowcontrol;
    uint8_t bits;
    uint8_t parity;
    uint8_t stop;
    gpio_num_t tx;
    gpio_num_t rx;
    gpio_num_t rts;
    gpio_num_t cts;
    uint16_t txbuf;
    uint16_t rxbuf;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint32_t invert;        // lines to invert
} machine_uart_obj_t;

static const char *_parity_name[] = {"None", "1", "0"};

/******************************************************************************/
// MicroPython bindings for UART

#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_INV_TX), MP_ROM_INT(UART_INV_TX) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_RX), MP_ROM_INT(UART_INV_RX) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_RTS), MP_ROM_INT(UART_INV_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_INV_CTS), MP_ROM_INT(UART_INV_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(UART_HW_FLOWCTRL_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(UART_HW_FLOWCTRL_CTS) }, \

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t baudrate;
    check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
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

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_tx, ARG_rx, ARG_rts, ARG_cts, ARG_txbuf, ARG_rxbuf, ARG_timeout, ARG_timeout_char, ARG_invert, ARG_flow };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_tx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rx, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
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
        #if MICROPY_HW_ENABLE_UART_REPL
        if (self->uart_num == MICROPY_HW_UART_REPL) {
            mp_raise_ValueError(MP_ERROR_TEXT("UART buffer size is fixed"));
        }
        #endif

        if (args[ARG_txbuf].u_int >= 0) {
            self->txbuf = args[ARG_txbuf].u_int;
        }
        if (args[ARG_rxbuf].u_int >= 0) {
            self->rxbuf = args[ARG_rxbuf].u_int;
        }
        uart_config_t uartcfg = {
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 0,
            .source_clk = UART_SOURCE_CLK,
        };
        uint32_t baudrate;
        check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
        uartcfg.baud_rate = baudrate;
        check_esp_err(uart_get_word_length(self->uart_num, &uartcfg.data_bits));
        check_esp_err(uart_get_parity(self->uart_num, &uartcfg.parity));
        check_esp_err(uart_get_stop_bits(self->uart_num, &uartcfg.stop_bits));
        check_esp_err(uart_driver_delete(self->uart_num));
        check_esp_err(uart_param_config(self->uart_num, &uartcfg));
        check_esp_err(uart_driver_install(self->uart_num, self->rxbuf, self->txbuf, 0, NULL, 0));
    }

    // set baudrate
    uint32_t baudrate = 115200;
    if (args[ARG_baudrate].u_int > 0) {
        check_esp_err(uart_set_baudrate(self->uart_num, args[ARG_baudrate].u_int));
    }
    check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));

    if (args[ARG_tx].u_obj != MP_OBJ_NULL) {
        self->tx = machine_pin_get_id(args[ARG_tx].u_obj);
    }

    if (args[ARG_rx].u_obj != MP_OBJ_NULL) {
        self->rx = machine_pin_get_id(args[ARG_rx].u_obj);
    }

    if (args[ARG_rts].u_obj != MP_OBJ_NULL) {
        self->rts = machine_pin_get_id(args[ARG_rts].u_obj);
    }

    if (args[ARG_cts].u_obj != MP_OBJ_NULL) {
        self->cts = machine_pin_get_id(args[ARG_cts].u_obj);
    }
    check_esp_err(uart_set_pin(self->uart_num, self->tx, self->rx, self->rts, self->cts));

    // set data bits
    switch (args[ARG_bits].u_int) {
        case 0:
            break;
        case 5:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_5_BITS));
            self->bits = 5;
            break;
        case 6:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_6_BITS));
            self->bits = 6;
            break;
        case 7:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_7_BITS));
            self->bits = 7;
            break;
        case 8:
            check_esp_err(uart_set_word_length(self->uart_num, UART_DATA_8_BITS));
            self->bits = 8;
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid data bits"));
            break;
    }

    // set parity
    if (args[ARG_parity].u_obj != MP_OBJ_NULL) {
        if (args[ARG_parity].u_obj == mp_const_none) {
            check_esp_err(uart_set_parity(self->uart_num, UART_PARITY_DISABLE));
            self->parity = 0;
        } else {
            mp_int_t parity = mp_obj_get_int(args[ARG_parity].u_obj);
            if (parity & 1) {
                check_esp_err(uart_set_parity(self->uart_num, UART_PARITY_ODD));
                self->parity = 1;
            } else {
                check_esp_err(uart_set_parity(self->uart_num, UART_PARITY_EVEN));
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
            check_esp_err(uart_set_stop_bits(self->uart_num, UART_STOP_BITS_1));
            self->stop = 1;
            break;
        case 2:
            check_esp_err(uart_set_stop_bits(self->uart_num, UART_STOP_BITS_2));
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
    if (args[ARG_timeout_char].u_int != -1) {
        self->timeout_char = args[ARG_timeout_char].u_int;
    }
    // make sure it is at least as long as a whole character (12 bits here)
    uint32_t char_time_ms = 12000 / baudrate + 1;
    uint32_t rx_timeout = self->timeout_char / char_time_ms;
    if (rx_timeout < 1) {
        check_esp_err(uart_set_rx_full_threshold(self->uart_num, 1));
        check_esp_err(uart_set_rx_timeout(self->uart_num, 1));
    } else {
        check_esp_err(uart_set_rx_timeout(self->uart_num, rx_timeout));
    }

    // set line inversion
    if (args[ARG_invert].u_int != -1) {
        if (args[ARG_invert].u_int & ~UART_INV_MASK) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid inversion mask"));
        }
        self->invert = args[ARG_invert].u_int;
    }
    check_esp_err(uart_set_line_inverse(self->uart_num, self->invert));

    // set hardware flow control
    if (args[ARG_flow].u_int != -1) {
        if (args[ARG_flow].u_int & ~UART_HW_FLOWCTRL_CTS_RTS) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid flow control mask"));
        }
        self->flowcontrol = args[ARG_flow].u_int;
    }
    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
    uint8_t uart_fifo_len = UART_HW_FIFO_LEN(self->uart_num);
    #else
    uint8_t uart_fifo_len = UART_FIFO_LEN;
    #endif
    check_esp_err(uart_set_hw_flow_ctrl(self->uart_num, self->flowcontrol, uart_fifo_len - uart_fifo_len / 4));
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
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
        .rx_flow_ctrl_thresh = 0,
        .source_clk = UART_SOURCE_CLK,
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

    #if MICROPY_HW_ENABLE_UART_REPL
    // Only reset the driver if it's not the REPL UART.
    if (uart_num != MICROPY_HW_UART_REPL)
    #endif
    {
        // Remove any existing configuration
        check_esp_err(uart_driver_delete(self->uart_num));

        // init the peripheral
        // Setup
        check_esp_err(uart_param_config(self->uart_num, &uartcfg));

        check_esp_err(uart_driver_install(uart_num, self->rxbuf, self->txbuf, 0, NULL, 0));
    }

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    // Make sure pins are connected.
    check_esp_err(uart_set_pin(self->uart_num, self->tx, self->rx, self->rts, self->cts));

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    check_esp_err(uart_driver_delete(self->uart_num));
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    size_t rxbufsize;
    check_esp_err(uart_get_buffered_data_len(self->uart_num, &rxbufsize));
    return rxbufsize;
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return uart_wait_tx_done(self->uart_num, 0) == ESP_OK;
}

static void mp_machine_uart_sendbreak(machine_uart_obj_t *self) {
    // Save settings
    uint32_t baudrate;
    check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));

    // Synthesise the break condition by reducing the baud rate,
    // and cater for the worst case of 5 data bits, no parity.
    check_esp_err(uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000)));
    check_esp_err(uart_set_baudrate(self->uart_num, baudrate * 6 / 15));
    char buf[1] = {0};
    uart_write_bytes(self->uart_num, buf, 1);
    check_esp_err(uart_wait_tx_done(self->uart_num, pdMS_TO_TICKS(1000)));

    // Restore original setting
    check_esp_err(uart_set_baudrate(self->uart_num, baudrate));
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
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

    bool release_gil = time_to_wait > 0;
    if (release_gil) {
        MP_THREAD_GIL_EXIT();
    }

    int bytes_read = uart_read_bytes(self->uart_num, buf_in, size, time_to_wait);

    if (release_gil) {
        MP_THREAD_GIL_ENTER();
    }

    if (bytes_read <= 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_read;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int bytes_written = uart_write_bytes(self->uart_num, buf_in, size);

    if (bytes_written < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    // return number of bytes written
    return bytes_written;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = self_in;
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        size_t rxbufsize;
        check_esp_err(uart_get_buffered_data_len(self->uart_num, &rxbufsize));
        if ((flags & MP_STREAM_POLL_RD) && rxbufsize > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && 1) { // FIXME: uart_tx_any_room(self->uart_num)
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        // The timeout is estimated using the buffer size and the baudrate.
        // Take the worst case assumptions at 13 bit symbol size times 2.
        uint32_t baudrate;
        check_esp_err(uart_get_baudrate(self->uart_num, &baudrate));
        uint32_t timeout = (3 + self->txbuf) * 13000 * 2 / baudrate;
        if (uart_wait_tx_done(self->uart_num, timeout) == ESP_OK) {
            ret = 0;
        } else {
            *errcode = MP_ETIMEDOUT;
            ret = MP_STREAM_ERROR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}
