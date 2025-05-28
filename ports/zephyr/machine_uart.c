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
#include <zephyr/sys/ring_buffer.h>

#include "py/mperrno.h"
#include "zephyr_device.h"

// The UART class doesn't have any constants for this port.
#define MICROPY_PY_MACHINE_UART_CLASS_CONSTANTS

#define UART_RX_RING_BUF_DEF_SIZE                  128
#define UART_TX_RING_BUF_DEF_SIZE                  128

static void uart_interrupt_handler(const struct device *dev, void *user_data);

typedef struct _machine_uart_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
    uint16_t timeout;       // timeout waiting for first char (in ms)
    uint16_t timeout_char;  // timeout waiting between chars (in ms)
    uint16_t txbuf_size;
    uint16_t rxbuf_size;
    uint8_t *rx_buffer;
    struct ring_buf rx_ringbuffer;
    uint8_t *tx_buffer;
    struct ring_buf tx_ringbuffer;
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
    enum { ARG_txbuf, ARG_rxbuf, ARG_timeout, ARG_timeout_char };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_txbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_RX_RING_BUF_DEF_SIZE} },
        { MP_QSTR_rxbuf, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = UART_TX_RING_BUF_DEF_SIZE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout_char, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    self->txbuf_size = args[ARG_txbuf].u_int;
    self->rxbuf_size = args[ARG_rxbuf].u_int;
    self->timeout = args[ARG_timeout].u_int;
    self->timeout_char = args[ARG_timeout_char].u_int;

    self->tx_buffer = m_tracked_calloc(self->txbuf_size, sizeof(uint8_t));
    ring_buf_init(&self->tx_ringbuffer, self->txbuf_size, self->tx_buffer);

    self->rx_buffer = m_tracked_calloc(self->rxbuf_size, sizeof(uint8_t));
    ring_buf_init(&self->rx_ringbuffer, self->rxbuf_size, self->rx_buffer);

    uart_irq_callback_user_data_set(self->dev, uart_interrupt_handler, (void *)self);
    uart_irq_rx_enable(self->dev);
}

static mp_obj_t mp_machine_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    machine_uart_obj_t *self = mp_obj_malloc(machine_uart_obj_t, &machine_uart_type);
    self->dev = zephyr_device_find(args[0]);

    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    mp_machine_uart_init_helper(self, n_args - 1, args + 1, &kw_args);

    return MP_OBJ_FROM_PTR(self);
}

static void mp_machine_uart_deinit(machine_uart_obj_t *self) {
    m_tracked_free(self->tx_buffer);
    m_tracked_free(self->rx_buffer);
}

static mp_int_t mp_machine_uart_any(machine_uart_obj_t *self) {
    return ring_buf_size_get(&self->rx_ringbuffer);
}

static bool mp_machine_uart_txdone(machine_uart_obj_t *self) {
    if (ring_buf_is_empty(&self->tx_ringbuffer)) {
        return true;
    }
    return false;
}

static mp_uint_t mp_machine_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    machine_uart_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t *buffer = (uint8_t *)buf_in;
    mp_uint_t bytes_read = 0;
    size_t elapsed_ms = 0;
    size_t time_to_wait = self->timeout;

    do {
        int _rx_len = ring_buf_get(&self->rx_ringbuffer, &buffer[bytes_read], size - bytes_read);
        if (_rx_len > 0) {
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
    int _free_space = ring_buf_space_get(&self->tx_ringbuffer);
    if (size > _free_space) {
        _ex_size = size - _free_space;
    }

    // do a blocking tx of what doesn't fit into the outgoing ring buffer
    for (mp_uint_t i = 0; i < _ex_size; i++) {
        uart_poll_out(self->dev, buffer[i]);
    }

    ring_buf_put(&self->tx_ringbuffer, &buffer[_ex_size], size - _ex_size);
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
            size_t _free_space = MIN(ring_buf_space_get(&self->rx_ringbuffer), sizeof(_rx_buffer));

            // empty the uart fifo even if we can't store bytes anymore
            // otherwise we will never exit this interrupt handler
            int rcv_len = uart_fifo_read(dev, _rx_buffer, (_free_space > 0) ? _free_space : 1);
            if ((rcv_len <= 0) || (_free_space == 0)) {
                continue;
            }

            ring_buf_put(&self->rx_ringbuffer, _rx_buffer, rcv_len);
        }

        int _max_tx_len = uart_irq_tx_ready(dev);
        if (_max_tx_len > 0) {
            uint8_t _tx_buffer[32];
            size_t _tx_len = MIN(_max_tx_len, sizeof(_tx_buffer));

            _tx_len = ring_buf_get(&self->tx_ringbuffer, _tx_buffer, _tx_len);
            if (_tx_len > 0) {
                uart_fifo_fill(dev, _tx_buffer, _tx_len);
            } else {
                uart_irq_tx_disable(dev);
            }
        }
    }
}
