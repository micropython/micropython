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

// I2C support for Linux using /dev/i2c-* devices
// This provides machine.I2C functionality for Raspberry Pi and other Linux SBCs

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modmachine.h"

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    uint8_t bus_id;
    int fd;
    uint32_t freq;
} machine_i2c_obj_t;

static void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u)", self->bus_id, self->freq);
}

static mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    // Parse arguments
    enum { ARG_id, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    // Create I2C object
    machine_i2c_obj_t *self = mp_obj_malloc(machine_i2c_obj_t, &machine_i2c_type);
    self->bus_id = parsed_args[ARG_id].u_int;
    self->freq = parsed_args[ARG_freq].u_int;

    // Open I2C device
    char device[32];
    snprintf(device, sizeof(device), "/dev/i2c-%d", self->bus_id);
    self->fd = open(device, O_RDWR);
    if (self->fd < 0) {
        mp_raise_OSError(errno);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t machine_i2c_scan(mp_obj_t self_in) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t list = mp_obj_new_list(0, NULL);

    // Scan I2C addresses from 0x08 to 0x77
    for (int addr = 0x08; addr <= 0x77; addr++) {
        if (ioctl(self->fd, I2C_SLAVE, addr) >= 0) {
            // Try to read a byte
            uint8_t buf;
            if (read(self->fd, &buf, 1) >= 0 || errno == EREMOTEIO) {
                // Device responded (or NACK'd which still means it's there)
                mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(addr));
            }
        }
    }

    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_i2c_scan_obj, machine_i2c_scan);

static mp_obj_t machine_i2c_readfrom(size_t n_args, const mp_obj_t *args) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_int_t len = mp_obj_get_int(args[2]);
    (void)n_args; // stop parameter not used in Linux I2C

    // Set slave address
    if (ioctl(self->fd, I2C_SLAVE, addr) < 0) {
        mp_raise_OSError(errno);
    }

    vstr_t vstr;
    vstr_init_len(&vstr, len);

    ssize_t ret = read(self->fd, vstr.buf, len);
    if (ret < 0) {
        vstr_clear(&vstr);
        mp_raise_OSError(errno);
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_readfrom_obj, 3, 4, machine_i2c_readfrom);

static mp_obj_t machine_i2c_readfrom_into(size_t n_args, const mp_obj_t *args) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    (void)n_args; // stop parameter not used in Linux I2C

    // Set slave address
    if (ioctl(self->fd, I2C_SLAVE, addr) < 0) {
        mp_raise_OSError(errno);
    }

    ssize_t ret = read(self->fd, bufinfo.buf, bufinfo.len);
    if (ret < 0) {
        mp_raise_OSError(errno);
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_readfrom_into_obj, 3, 4, machine_i2c_readfrom_into);

static mp_obj_t machine_i2c_writeto(size_t n_args, const mp_obj_t *args) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    (void)n_args; // stop parameter not used in Linux I2C

    // Set slave address
    if (ioctl(self->fd, I2C_SLAVE, addr) < 0) {
        mp_raise_OSError(errno);
    }

    ssize_t ret = write(self->fd, bufinfo.buf, bufinfo.len);
    if (ret < 0) {
        mp_raise_OSError(errno);
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_writeto_obj, 3, 4, machine_i2c_writeto);

static mp_obj_t machine_i2c_readfrom_mem(size_t n_args, const mp_obj_t *args) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_int_t memaddr = mp_obj_get_int(args[2]);
    mp_int_t len = mp_obj_get_int(args[3]);
    mp_int_t addrsize = (n_args == 4) ? 8 : mp_obj_get_int(args[4]);

    // Set slave address
    if (ioctl(self->fd, I2C_SLAVE, addr) < 0) {
        mp_raise_OSError(errno);
    }

    // Write memory address
    uint8_t mem_addr_buf[4];
    int addr_bytes = addrsize / 8;
    for (int i = 0; i < addr_bytes; i++) {
        mem_addr_buf[addr_bytes - 1 - i] = (memaddr >> (i * 8)) & 0xFF;
    }

    if (write(self->fd, mem_addr_buf, addr_bytes) < 0) {
        mp_raise_OSError(errno);
    }

    // Read data
    vstr_t vstr;
    vstr_init_len(&vstr, len);

    ssize_t ret = read(self->fd, vstr.buf, len);
    if (ret < 0) {
        vstr_clear(&vstr);
        mp_raise_OSError(errno);
    }

    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_readfrom_mem_obj, 4, 5, machine_i2c_readfrom_mem);

static mp_obj_t machine_i2c_writeto_mem(size_t n_args, const mp_obj_t *args) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_int_t addr = mp_obj_get_int(args[1]);
    mp_int_t memaddr = mp_obj_get_int(args[2]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
    mp_int_t addrsize = (n_args == 4) ? 8 : mp_obj_get_int(args[4]);

    // Set slave address
    if (ioctl(self->fd, I2C_SLAVE, addr) < 0) {
        mp_raise_OSError(errno);
    }

    // Prepare buffer with memory address + data
    int addr_bytes = addrsize / 8;
    uint8_t *buf = malloc(addr_bytes + bufinfo.len);
    if (buf == NULL) {
        mp_raise_OSError(MP_ENOMEM);
    }

    for (int i = 0; i < addr_bytes; i++) {
        buf[addr_bytes - 1 - i] = (memaddr >> (i * 8)) & 0xFF;
    }
    memcpy(buf + addr_bytes, bufinfo.buf, bufinfo.len);

    ssize_t ret = write(self->fd, buf, addr_bytes + bufinfo.len);
    free(buf);

    if (ret < 0) {
        mp_raise_OSError(errno);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_i2c_writeto_mem_obj, 4, 5, machine_i2c_writeto_mem);

static const mp_rom_map_elem_t machine_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&machine_i2c_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&machine_i2c_readfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_into), MP_ROM_PTR(&machine_i2c_readfrom_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&machine_i2c_writeto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom_mem), MP_ROM_PTR(&machine_i2c_readfrom_mem_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto_mem), MP_ROM_PTR(&machine_i2c_writeto_mem_obj) },
};
static MP_DEFINE_CONST_DICT(machine_i2c_locals_dict, machine_i2c_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    locals_dict, &machine_i2c_locals_dict
);
