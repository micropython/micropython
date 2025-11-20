/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 MicroPython contributors
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

// SPI support for Linux using /dev/spidev* devices
// This provides machine.SPI functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"

#define SPI_POLARITY_LOW  (0)
#define SPI_POLARITY_HIGH (1)
#define SPI_PHASE_1EDGE   (0)
#define SPI_PHASE_2EDGE   (1)

typedef struct _machine_spi_obj_t {
    mp_obj_base_t base;
    uint8_t bus_id;
    uint8_t cs;
    int fd;
    uint32_t baudrate;
    uint8_t polarity;
    uint8_t phase;
    uint8_t bits;
    uint8_t firstbit;
} machine_spi_obj_t;

static void machine_spi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "SPI(%u, baudrate=%u, polarity=%u, phase=%u, bits=%u)",
        self->bus_id, self->baudrate, self->polarity, self->phase, self->bits);
}

static mp_obj_t machine_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_id, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1000000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SPI_POLARITY_LOW} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = SPI_PHASE_1EDGE} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },  // MSB first
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create SPI object
    machine_spi_obj_t *self = mp_obj_malloc(machine_spi_obj_t, &machine_spi_type);
    self->bus_id = parsed_args[ARG_id].u_int;
    self->cs = 0;  // Default to CS0
    self->baudrate = parsed_args[ARG_baudrate].u_int;
    self->polarity = parsed_args[ARG_polarity].u_int;
    self->phase = parsed_args[ARG_phase].u_int;
    self->bits = parsed_args[ARG_bits].u_int;
    self->firstbit = parsed_args[ARG_firstbit].u_int;

    // Open SPI device
    char device[32];
    snprintf(device, sizeof(device), "/dev/spidev%d.%d", self->bus_id, self->cs);
    self->fd = open(device, O_RDWR);
    if (self->fd < 0) {
        mp_raise_OSError(errno);
    }

    // Configure SPI mode
    uint8_t mode = (self->polarity << 1) | self->phase;
    if (ioctl(self->fd, SPI_IOC_WR_MODE, &mode) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    // Configure bits per word
    if (ioctl(self->fd, SPI_IOC_WR_BITS_PER_WORD, &self->bits) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    // Configure speed
    if (ioctl(self->fd, SPI_IOC_WR_MAX_SPEED_HZ, &self->baudrate) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    // Configure bit order (LSB first if firstbit == 1)
    uint8_t lsb = self->firstbit;
    if (ioctl(self->fd, SPI_IOC_WR_LSB_FIRST, &lsb) < 0) {
        close(self->fd);
        mp_raise_OSError(errno);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_spi_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_firstbit, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_baudrate].u_int != (uint32_t)-1) {
        self->baudrate = args[ARG_baudrate].u_int;
        ioctl(self->fd, SPI_IOC_WR_MAX_SPEED_HZ, &self->baudrate);
    }

    if (args[ARG_polarity].u_int != (uint32_t)-1 || args[ARG_phase].u_int != (uint32_t)-1) {
        if (args[ARG_polarity].u_int != (uint32_t)-1) {
            self->polarity = args[ARG_polarity].u_int;
        }
        if (args[ARG_phase].u_int != (uint32_t)-1) {
            self->phase = args[ARG_phase].u_int;
        }
        uint8_t mode = (self->polarity << 1) | self->phase;
        ioctl(self->fd, SPI_IOC_WR_MODE, &mode);
    }

    if (args[ARG_bits].u_int != (uint32_t)-1) {
        self->bits = args[ARG_bits].u_int;
        ioctl(self->fd, SPI_IOC_WR_BITS_PER_WORD, &self->bits);
    }

    if (args[ARG_firstbit].u_int != (uint32_t)-1) {
        self->firstbit = args[ARG_firstbit].u_int;
        uint8_t lsb = self->firstbit;
        ioctl(self->fd, SPI_IOC_WR_LSB_FIRST, &lsb);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(machine_spi_init_obj, 1, machine_spi_init);

static mp_obj_t machine_spi_read(size_t n_args, const mp_obj_t *args) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t len = mp_obj_get_int(args[1]);
    uint8_t write_byte = (n_args == 3) ? mp_obj_get_int(args[2]) : 0xFF;

    vstr_t vstr;
    vstr_init_len(&vstr, len);
    memset(vstr.buf, write_byte, len);

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)vstr.buf,
        .rx_buf = (unsigned long)vstr.buf,
        .len = len,
        .speed_hz = self->baudrate,
        .bits_per_word = self->bits,
    };

    if (ioctl(self->fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        vstr_clear(&vstr);
        mp_raise_OSError(errno);
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_spi_read_obj, 2, 3, machine_spi_read);

static mp_obj_t machine_spi_readinto(size_t n_args, const mp_obj_t *args) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    uint8_t write_byte = (n_args == 3) ? mp_obj_get_int(args[2]) : 0xFF;

    // Fill buffer with write_byte
    memset(bufinfo.buf, write_byte, bufinfo.len);

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)bufinfo.buf,
        .rx_buf = (unsigned long)bufinfo.buf,
        .len = bufinfo.len,
        .speed_hz = self->baudrate,
        .bits_per_word = self->bits,
    };

    if (ioctl(self->fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_spi_readinto_obj, 2, 3, machine_spi_readinto);

static mp_obj_t machine_spi_write(mp_obj_t self_in, mp_obj_t buf_in) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)bufinfo.buf,
        .rx_buf = 0,
        .len = bufinfo.len,
        .speed_hz = self->baudrate,
        .bits_per_word = self->bits,
    };

    if (ioctl(self->fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_spi_write_obj, machine_spi_write);

static mp_obj_t machine_spi_write_readinto(mp_obj_t self_in, mp_obj_t write_buf, mp_obj_t read_buf) {
    machine_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t write_bufinfo;
    mp_get_buffer_raise(write_buf, &write_bufinfo, MP_BUFFER_READ);
    mp_buffer_info_t read_bufinfo;
    mp_get_buffer_raise(read_buf, &read_bufinfo, MP_BUFFER_WRITE);

    if (write_bufinfo.len != read_bufinfo.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffers must be the same length"));
    }

    struct spi_ioc_transfer transfer = {
        .tx_buf = (unsigned long)write_bufinfo.buf,
        .rx_buf = (unsigned long)read_bufinfo.buf,
        .len = write_bufinfo.len,
        .speed_hz = self->baudrate,
        .bits_per_word = self->bits,
    };

    if (ioctl(self->fd, SPI_IOC_MESSAGE(1), &transfer) < 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(machine_spi_write_readinto_obj, machine_spi_write_readinto);

static const mp_rom_map_elem_t machine_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&machine_spi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&machine_spi_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&machine_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&machine_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&machine_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_MSB), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_LSB), MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(machine_spi_locals_dict, machine_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, machine_spi_make_new,
    print, machine_spi_print,
    locals_dict, &machine_spi_locals_dict
);
