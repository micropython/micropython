/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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

#include "drivers/bus/qspi.h"

#define CS_LOW(self) mp_hal_pin_write(self->cs, 0)
#define CS_HIGH(self) mp_hal_pin_write(self->cs, 1)

#ifdef MICROPY_HW_SOFTQSPI_SCK_LOW

// Use externally provided functions for SCK control and IO reading
#define SCK_LOW(self) MICROPY_HW_SOFTQSPI_SCK_LOW(self)
#define SCK_HIGH(self) MICROPY_HW_SOFTQSPI_SCK_HIGH(self)
#define NIBBLE_READ(self) MICROPY_HW_SOFTQSPI_NIBBLE_READ(self)

#else

// Use generic pin functions for SCK control and IO reading
#define SCK_LOW(self) mp_hal_pin_write(self->clk, 0)
#define SCK_HIGH(self) mp_hal_pin_write(self->clk, 1)
#define NIBBLE_READ(self) ( \
    mp_hal_pin_read(self->io0) \
    | (mp_hal_pin_read(self->io1) << 1) \
    | (mp_hal_pin_read(self->io2) << 2) \
    | (mp_hal_pin_read(self->io3) << 3))

#endif

STATIC void nibble_write(mp_soft_qspi_obj_t *self, uint8_t v) {
    mp_hal_pin_write(self->io0, v & 1);
    mp_hal_pin_write(self->io1, (v >> 1) & 1);
    mp_hal_pin_write(self->io2, (v >> 2) & 1);
    mp_hal_pin_write(self->io3, (v >> 3) & 1);
}

STATIC int mp_soft_qspi_ioctl(void *self_in, uint32_t cmd) {
    mp_soft_qspi_obj_t *self = (mp_soft_qspi_obj_t*)self_in;

    switch (cmd) {
        case MP_QSPI_IOCTL_INIT:
            mp_hal_pin_high(self->cs);
            mp_hal_pin_output(self->cs);

            // Configure pins
            mp_hal_pin_write(self->clk, 0);
            mp_hal_pin_output(self->clk);
            //mp_hal_pin_write(self->clk, 1);
            mp_hal_pin_output(self->io0);
            mp_hal_pin_input(self->io1);
            mp_hal_pin_write(self->io2, 1);
            mp_hal_pin_output(self->io2);
            mp_hal_pin_write(self->io3, 1);
            mp_hal_pin_output(self->io3);
            break;
    }

    return 0; // success
}

STATIC void mp_soft_qspi_transfer(mp_soft_qspi_obj_t *self, size_t len, const uint8_t *src, uint8_t *dest) {
    // Will run as fast as possible, limited only by CPU speed and GPIO time
    mp_hal_pin_input(self->io1);
    mp_hal_pin_output(self->io0);
    if (self->io3) {
        mp_hal_pin_write(self->io2, 1);
        mp_hal_pin_output(self->io2);
        mp_hal_pin_write(self->io3, 1);
        mp_hal_pin_output(self->io3);
    }
    if (src) {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_out = src[i];
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j, data_out <<= 1) {
                mp_hal_pin_write(self->io0, (data_out >> 7) & 1);
                mp_hal_pin_write(self->clk, 1);
                data_in = (data_in << 1) | mp_hal_pin_read(self->io1);
                mp_hal_pin_write(self->clk, 0);
            }
            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
    } else {
        for (size_t i = 0; i < len; ++i) {
            uint8_t data_in = 0;
            for (int j = 0; j < 8; ++j) {
                mp_hal_pin_write(self->clk, 1);
                data_in = (data_in << 1) | mp_hal_pin_read(self->io1);
                mp_hal_pin_write(self->clk, 0);
            }
            if (dest != NULL) {
                dest[i] = data_in;
            }
        }
    }
}

STATIC void mp_soft_qspi_qread(mp_soft_qspi_obj_t *self, size_t len, uint8_t *buf) {
    // Make all IO lines input
    mp_hal_pin_input(self->io2);
    mp_hal_pin_input(self->io3);
    mp_hal_pin_input(self->io0);
    mp_hal_pin_input(self->io1);

    // Will run as fast as possible, limited only by CPU speed and GPIO time
    while (len--) {
        SCK_HIGH(self);
        uint8_t data_in = NIBBLE_READ(self);
        SCK_LOW(self);
        SCK_HIGH(self);
        *buf++ = (data_in << 4) | NIBBLE_READ(self);
        SCK_LOW(self);
    }
}

STATIC void mp_soft_qspi_qwrite(mp_soft_qspi_obj_t *self, size_t len, const uint8_t *buf) {
    // Make all IO lines output
    mp_hal_pin_output(self->io2);
    mp_hal_pin_output(self->io3);
    mp_hal_pin_output(self->io0);
    mp_hal_pin_output(self->io1);

    // Will run as fast as possible, limited only by CPU speed and GPIO time
    for (size_t i = 0; i < len; ++i) {
        nibble_write(self, buf[i] >> 4);
        SCK_HIGH(self);
        SCK_LOW(self);

        nibble_write(self, buf[i]);
        SCK_HIGH(self);
        SCK_LOW(self);
    }

    //mp_hal_pin_input(self->io1);
}

STATIC void mp_soft_qspi_write_cmd_data(void *self_in, uint8_t cmd, size_t len, uint32_t data) {
    mp_soft_qspi_obj_t *self = (mp_soft_qspi_obj_t*)self_in;
    uint32_t cmd_buf = cmd | data << 8;
    CS_LOW(self);
    mp_soft_qspi_transfer(self, 1 + len, (uint8_t*)&cmd_buf, NULL);
    CS_HIGH(self);
}

STATIC void mp_soft_qspi_write_cmd_addr_data(void *self_in, uint8_t cmd, uint32_t addr, size_t len, const uint8_t *src) {
    mp_soft_qspi_obj_t *self = (mp_soft_qspi_obj_t*)self_in;
    uint8_t cmd_buf[5] = {cmd};
    uint8_t addr_len = mp_spi_set_addr_buff(&cmd_buf[1], addr);
    CS_LOW(self);
    mp_soft_qspi_transfer(self, addr_len + 1, cmd_buf, NULL);
    mp_soft_qspi_transfer(self, len, src, NULL);
    CS_HIGH(self);
}

STATIC uint32_t mp_soft_qspi_read_cmd(void *self_in, uint8_t cmd, size_t len) {
    mp_soft_qspi_obj_t *self = (mp_soft_qspi_obj_t*)self_in;
    uint32_t cmd_buf = cmd;
    CS_LOW(self);
    mp_soft_qspi_transfer(self, 1 + len, (uint8_t*)&cmd_buf, (uint8_t*)&cmd_buf);
    CS_HIGH(self);
    return cmd_buf >> 8;
}

STATIC void mp_soft_qspi_read_cmd_qaddr_qdata(void *self_in, uint8_t cmd, uint32_t addr, size_t len, uint8_t *dest) {
    mp_soft_qspi_obj_t *self = (mp_soft_qspi_obj_t*)self_in;
    uint8_t cmd_buf[7] = {cmd};
    uint8_t addr_len = mp_spi_set_addr_buff(&cmd_buf[1], addr);
    CS_LOW(self);
    mp_soft_qspi_transfer(self, 1, cmd_buf, NULL);
    mp_soft_qspi_qwrite(self, addr_len + 3, &cmd_buf[1]); // 3/4 addr bytes, 1 extra byte (0), 2 dummy bytes (4 dummy cycles)
    mp_soft_qspi_qread(self, len, dest);
    CS_HIGH(self);
}

const mp_qspi_proto_t mp_soft_qspi_proto = {
    .ioctl = mp_soft_qspi_ioctl,
    .write_cmd_data = mp_soft_qspi_write_cmd_data,
    .write_cmd_addr_data = mp_soft_qspi_write_cmd_addr_data,
    .read_cmd = mp_soft_qspi_read_cmd,
    .read_cmd_qaddr_qdata = mp_soft_qspi_read_cmd_qaddr_qdata,
};
