/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 * Copyright (c) 2020 Yonatan Schachter
 * Copyright (c) 2025 Daniel Campora on behalf of REMOTE TECH LTD
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

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>

#include "py/mperrno.h"
#include "py/ringbuf.h"
#include "zephyr_device.h"


#define MACHINE_UART_RTS                            1
#define MACHINE_UART_CTS                            2

// This class needs a finalizer, so we add it here
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_RTS), MP_ROM_INT(MACHINE_UART_RTS) }, \
    { MP_ROM_QSTR(MP_QSTR_CTS), MP_ROM_INT(MACHINE_UART_CTS) }, \
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&machine_uart_deinit_obj) },

#define UART_RX_RING_BUF_DEF_SIZE                   128
#define UART_TX_RING_BUF_DEF_SIZE                   128

static void uart_interrupt_handler(const struct device *dev, void *user_data);

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    ringbuf_t rx_ringbuffer;
    ringbuf_t tx_ringbuffer;
    bool tx_complete;
} machine_uart_obj_t;

static const char *_parity_name[] = {"None", "Odd", "Even", "Mark", "Space"};
static const char *_stop_bits_name[] = {"0.5", "1", "1.5", "2"};
static const char *_data_bits_name[] = {"5", "6", "7", "8", "9"};
static const char *_flow_control_name[] = {"None", "RTS/CTS", "DTR/DSR"};

static void mp_machine_uart_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct uart_config config;
    uart_config_get(self->dev, &config);
    mp_printf(print, "UART(\"%s\", baudrate=%u, data_bits=%s, parity_bits=%s, stop_bits=%s, flow_control=%s, timeout=%u, timeout_char=%u)",
        self->dev->name, config.baudrate, _data_bits_name[config.data_bits],
        _parity_name[config.parity], _stop_bits_name[config.stop_bits], _flow_control_name[config.flow_ctrl],
        self->timeout, self->timeout_char);
}

static void mp_machine_uart_init_helper(machine_uart_obj_t *self, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_txbuf, ARG_rxbuf, ARG_timeout, ARG_timeout_char, ARG_flow };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 115200} },
        { MP_QSTR_bits, MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_RX_RING_BUF_DEF_SIZE} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_TX_RING_BUF_DEF_SIZE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_flow, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->timeout = args[ARG_timeout].u_int;
    self->timeout_char = args[ARG_timeout_char].u_int;

    uint8_t data_bits;
    if (args[ARG_bits].u_int == 5) {
        data_bits = UART_CFG_DATA_BITS_5;
    } else if (args[ARG_bits].u_int == 6) {
        data_bits = UART_CFG_DATA_BITS_6;
    } else if (args[ARG_bits].u_int == 7) {
        data_bits = UART_CFG_DATA_BITS_7;
    } else if (args[ARG_bits].u_int == 8) {
        data_bits = UART_CFG_DATA_BITS_8;
    } else if (args[ARG_bits].u_int == 9) {
        data_bits = UART_CFG_DATA_BITS_9;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid data bits"));
    }

    uint8_t parity;
    if (args[ARG_parity].u_obj == mp_const_none) {
        parity = UART_CFG_PARITY_NONE;
    } else if (mp_obj_get_int(args[ARG_parity].u_obj) == 0) {
        parity = UART_CFG_PARITY_EVEN;
    } else if (mp_obj_get_int(args[ARG_parity].u_obj) == 1) {
        parity = UART_CFG_PARITY_ODD;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid parity"));
    }

    uint8_t stop_bits;
    if (args[ARG_stop].u_int == 1) {
        stop_bits = UART_CFG_STOP_BITS_1;
    } else if (args[ARG_stop].u_int == 2) {
        data_bits = UART_CFG_STOP_BITS_2;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid stop bits"));
    }

    uint8_t flow_ctrl;
    if (args[ARG_flow].u_int == 0) {
        flow_ctrl = UART_CFG_FLOW_CTRL_NONE;
    } else if (args[ARG_flow].u_int == (MACHINE_UART_RTS | MACHINE_UART_CTS)) {
        flow_ctrl = UART_CFG_FLOW_CTRL_RTS_CTS;
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("invalid flow control"));
    }

    const struct uart_config cfg = {
        .baudrate = args[ARG_baudrate].u_int,
        .parity = parity,
        .stop_bits = args[ARG_stop].u_int,
        .data_bits = data_bits,
        .flow_ctrl = flow_ctrl
    };

    int ret = uart_configure(self->dev, &cfg);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    ringbuf_alloc(&self->tx_ringbuffer, args[ARG_txbuf].u_int);
    ringbuf_alloc(&self->rx_ringbuffer, args[ARG_rxbuf].u_int);

    uart_irq_callback_user_data_set(self->dev, uart_interrupt_handler, (void *)self);
    uart_irq_rx_enable(self->dev);
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    const struct device *dev = zephyr_device_find(args[0]);
    machine_uart_obj_t *self = mp_obj_malloc_with_finaliser(machine_uart_obj_t, &machine_uart_type);
    self->dev = dev;
    self->tx_complete = true;

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    uart_irq_rx_disable(self->dev);
    uart_irq_tx_disable(self->dev);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return ringbuf_avail(&self->rx_ringbuffer);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    return self->tx_complete && !ringbuf_avail(&self->tx_ringbuffer) ? true : false;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *buffer = (uint8_t *)buf_in;
    mp_uint_t bytes_read = 0;
    size_t elapsed_ms = 0;
    size_t time_to_wait = self->timeout;

    do {
        int _rx_len = MIN(ringbuf_avail(&self->rx_ringbuffer), size - bytes_read);
        if (_rx_len > 0) {
            ringbuf_get_bytes(&self->rx_ringbuffer, &buffer[bytes_read], _rx_len);
            bytes_read += _rx_len;
            elapsed_ms = 0;
            time_to_wait = self->timeout_char;
        } else {
            k_msleep(1);
            elapsed_ms++;
        }
    } while ((elapsed_ms < time_to_wait) && (bytes_read < size));

    return bytes_read;
}

static mp_uint_t mp_machine_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *buffer = (uint8_t *)buf_in;

    // wait for any pending transmission to complete
    while (!mp_machine_uart_txdone(self)) {
        MICROPY_EVENT_POLL_HOOK;
    }

    int _ex_size = 0;
    int _free_space = ringbuf_free(&self->tx_ringbuffer);
    if (size > _free_space) {
        _ex_size = size - _free_space;
    }

    // do a blocking tx of what doesn't fit into the outgoing ring buffer
    for (mp_uint_t i = 0; i < _ex_size; i++) {
        uart_poll_out(self->dev, buffer[i]);
    }

    ringbuf_put_bytes(&self->tx_ringbuffer, &buffer[_ex_size], size - _ex_size);
    self->tx_complete = false;
    uart_irq_tx_enable(self->dev);

    return size;
}

static mp_uint_t mp_machine_uart_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret = 0;

    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        if ((flags & MP_STREAM_POLL_RD) && (mp_machine_uart_any(self) > 0)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && mp_machine_uart_txdone(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else if (request == MP_STREAM_FLUSH) {
        while (!mp_machine_uart_txdone(self)) {
            MICROPY_EVENT_POLL_HOOK;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }

    return ret;
}

static void uart_interrupt_handler(const struct device *dev, void *user_data) {
    machine_uart_obj_t *self = (machine_uart_obj_t *)user_data;

    while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
        if (uart_irq_rx_ready(dev)) {
            uint8_t _rx_buffer[32];
            size_t _free_space = MIN(ringbuf_free(&self->rx_ringbuffer), sizeof(_rx_buffer));

            // empty the uart fifo even if we can't store bytes anymore
            // otherwise we will never exit this interrupt handler
            int rcv_len = uart_fifo_read(dev, _rx_buffer, (_free_space > 0) ? _free_space : 1);
            if ((rcv_len <= 0) || (_free_space == 0)) {
                continue;
            }

            ringbuf_put_bytes(&self->rx_ringbuffer, _rx_buffer, rcv_len);
        }

        int _max_uart_tx_len = uart_irq_tx_ready(dev);
        if (_max_uart_tx_len > 0) {
            uint8_t _tx_buffer[32];
            size_t _buffer_tx_len;

            _max_uart_tx_len = MIN(_max_uart_tx_len, sizeof(_tx_buffer));
            _buffer_tx_len = ringbuf_avail(&self->tx_ringbuffer);
            if (_buffer_tx_len > 0) {
                _buffer_tx_len = MIN(_max_uart_tx_len, _buffer_tx_len);
                ringbuf_get_bytes(&self->tx_ringbuffer, _tx_buffer, _buffer_tx_len);
                uart_fifo_fill(dev, _tx_buffer, _buffer_tx_len);
            } else if (uart_irq_tx_complete(dev)) {
                uart_irq_tx_disable(dev);
                self->tx_complete = true;
            }
        }
    }
}
