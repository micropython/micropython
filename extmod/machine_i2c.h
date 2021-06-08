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
#ifndef MICROPY_INCLUDED_EXTMOD_MACHINE_I2C_H
#define MICROPY_INCLUDED_EXTMOD_MACHINE_I2C_H

#include "py/obj.h"
#include "py/mphal.h"

// Temporary support for legacy construction of SoftI2C via I2C type.
#define MP_MACHINE_I2C_CHECK_FOR_LEGACY_SOFTI2C_CONSTRUCTION(n_args, n_kw, all_args) \
    do { \
        if (n_args == 0 || all_args[0] == MP_OBJ_NEW_SMALL_INT(-1)) { \
            mp_print_str(MICROPY_ERROR_PRINTER, "Warning: I2C(-1, ...) is deprecated, use SoftI2C(...) instead\n"); \
            if (n_args != 0) { \
                --n_args; \
                ++all_args; \
            } \
            return mp_machine_soft_i2c_type.make_new(&mp_machine_soft_i2c_type, n_args, n_kw, all_args); \
        } \
    } while (0)

#define MP_MACHINE_I2C_FLAG_READ (0x01) // if not set then it's a write
#define MP_MACHINE_I2C_FLAG_STOP (0x02)

typedef struct _mp_machine_i2c_buf_t {
    size_t len;
    uint8_t *buf;
} mp_machine_i2c_buf_t;

// I2C protocol
// - init must be non-NULL
// - start/stop/read/write can be NULL, meaning operation is not supported
// - transfer must be non-NULL
// - transfer_single only needs to be set if transfer=mp_machine_i2c_transfer_adaptor
typedef struct _mp_machine_i2c_p_t {
    void (*init)(mp_obj_base_t *obj, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args);
    int (*start)(mp_obj_base_t *obj);
    int (*stop)(mp_obj_base_t *obj);
    int (*read)(mp_obj_base_t *obj, uint8_t *dest, size_t len, bool nack);
    int (*write)(mp_obj_base_t *obj, const uint8_t *src, size_t len);
    int (*transfer)(mp_obj_base_t *obj, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags);
    int (*transfer_single)(mp_obj_base_t *obj, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags);
} mp_machine_i2c_p_t;

typedef struct _mp_machine_soft_i2c_obj_t {
    mp_obj_base_t base;
    uint32_t us_delay;
    uint32_t us_timeout;
    mp_hal_pin_obj_t scl;
    mp_hal_pin_obj_t sda;
} mp_machine_soft_i2c_obj_t;

extern const mp_obj_type_t mp_machine_soft_i2c_type;
extern const mp_obj_dict_t mp_machine_i2c_locals_dict;

int mp_machine_i2c_transfer_adaptor(mp_obj_base_t *self, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags);
int mp_machine_soft_i2c_transfer(mp_obj_base_t *self, uint16_t addr, size_t n, mp_machine_i2c_buf_t *bufs, unsigned int flags);

#endif // MICROPY_INCLUDED_EXTMOD_MACHINE_I2C_H
