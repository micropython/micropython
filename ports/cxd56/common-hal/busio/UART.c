/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright 2019 Sony Semiconductor Solutions Corporation
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

#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <arch/chip/pin.h>
#include <nuttx/serial/tioctl.h>
#include <nuttx/fs/ioctl.h>

#include "py/mperrno.h"
#include "py/stream.h"
#include "py/runtime.h"

#include "shared-bindings/busio/UART.h"

void common_hal_busio_uart_construct(busio_uart_obj_t *self,
    const mcu_pin_obj_t *tx, const mcu_pin_obj_t *rx, uint32_t baudrate,
    uint8_t bits, uart_parity_t parity, uint8_t stop, mp_float_t timeout,
    uint16_t receiver_buffer_size) {
    struct termios tio;

    self->uart_fd = open("/dev/ttyS2", O_RDWR);
    if (self->uart_fd < 0) {
        mp_raise_ValueError(translate("Could not initialize UART"));
    }

    ioctl(self->uart_fd, TCGETS, (long unsigned int)&tio);
    tio.c_speed = baudrate;
    ioctl(self->uart_fd, TCSETS, (long unsigned int)&tio);
    ioctl(self->uart_fd, TCFLSH, (long unsigned int)NULL);

    if (bits != 8) {
        mp_raise_ValueError(translate("Could not initialize UART"));
    }

    if (parity != PARITY_NONE) {
        mp_raise_ValueError(translate("Could not initialize UART"));
    }

    if (stop != 1) {
        mp_raise_ValueError(translate("Could not initialize UART"));
    }

    if (tx->number != PIN_UART2_TXD || rx->number != PIN_UART2_RXD) {
        mp_raise_ValueError(translate("Invalid pins"));
    }

    claim_pin(tx);
    claim_pin(rx);

    self->tx_pin = tx;
    self->rx_pin = rx;
    self->baudrate = baudrate;
    self->timeout = timeout;
}

void common_hal_busio_uart_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        return;
    }

    close(self->uart_fd);
    self->uart_fd = -1;

    reset_pin_number(self->tx_pin->number);
    reset_pin_number(self->rx_pin->number);
}

bool common_hal_busio_uart_deinited(busio_uart_obj_t *self) {
    return self->uart_fd < 0;
}

size_t common_hal_busio_uart_read(busio_uart_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    fd_set rfds;
    struct timeval tv;
    int retval, bytes_read;

    // make sure we want at least 1 char
    if (len == 0) {
        return 0;
    }

    FD_ZERO(&rfds);
    FD_SET(self->uart_fd, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = self->timeout * 1000;

    retval = select(self->uart_fd + 1, &rfds, NULL, NULL, &tv);

    if (retval) {
        bytes_read = read(self->uart_fd, data, len);
    } else {
        *errcode = EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_read;
}

size_t common_hal_busio_uart_write(busio_uart_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    int bytes_written = write(self->uart_fd, data, len);

    if (bytes_written < 0) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    }

    return bytes_written;
}

uint32_t common_hal_busio_uart_get_baudrate(busio_uart_obj_t *self) {
    return self->baudrate;
}

void common_hal_busio_uart_set_baudrate(busio_uart_obj_t *self, uint32_t baudrate) {
    struct termios tio;

    ioctl(self->uart_fd, TCGETS, (long unsigned int)&tio);
    tio.c_speed = baudrate;
    ioctl(self->uart_fd, TCSETS, (long unsigned int)&tio);
    ioctl(self->uart_fd, TCFLSH, (long unsigned int)NULL);
}

uint32_t common_hal_busio_uart_rx_characters_available(busio_uart_obj_t *self) {
    int count = 0;

    ioctl(self->uart_fd, FIONREAD, (long unsigned int)&count);

    return count;
}

void common_hal_busio_uart_clear_rx_buffer(busio_uart_obj_t *self) {
}

bool common_hal_busio_uart_ready_to_tx(busio_uart_obj_t *self) {
    ioctl(self->uart_fd, TCFLSH, (long unsigned int)NULL);
    return true;
}
