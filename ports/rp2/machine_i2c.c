/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/machine_i2c.h"
#include "modmachine.h"

#include "hardware/i2c.h"

#define DEFAULT_I2C_FREQ (400000)
#define DEFAULT_I2C_TIMEOUT (50000)

#ifndef MICROPY_HW_I2C0_SCL
#if PICO_DEFAULT_I2C == 0
#define MICROPY_HW_I2C0_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C0_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C0_SCL (9)
#define MICROPY_HW_I2C0_SDA (8)
#endif
#endif

#ifndef MICROPY_HW_I2C1_SCL
#if PICO_DEFAULT_I2C == 1
#define MICROPY_HW_I2C1_SCL (PICO_DEFAULT_I2C_SCL_PIN)
#define MICROPY_HW_I2C1_SDA (PICO_DEFAULT_I2C_SDA_PIN)
#else
#define MICROPY_HW_I2C1_SCL (7)
#define MICROPY_HW_I2C1_SDA (6)
#endif
#endif

// SDA/SCL on even/odd pins, I2C0/I2C1 on even/odd pairs of pins.
#define IS_VALID_SCL(i2c, pin) (((pin) & 1) == 1 && (((pin) & 2) >> 1) == (i2c))
#define IS_VALID_SDA(i2c, pin) (((pin) & 1) == 0 && (((pin) & 2) >> 1) == (i2c))

typedef struct _machine_i2c_obj_t {
    mp_obj_base_t base;
    i2c_inst_t *const i2c_inst;
    uint8_t i2c_id;
    uint8_t scl;
    uint8_t sda;
    uint32_t freq;
    uint32_t timeout;
} machine_i2c_obj_t;

STATIC machine_i2c_obj_t machine_i2c_obj[] = {
    {{&machine_i2c_type}, i2c0, 0, MICROPY_HW_I2C0_SCL, MICROPY_HW_I2C0_SDA, 0},
    {{&machine_i2c_type}, i2c1, 1, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA, 0},
};

STATIC void machine_i2c_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_i2c_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "I2C(%u, freq=%u, scl=%u, sda=%u, timeout=%u)",
        self->i2c_id, self->freq, self->scl, self->sda, self->timeout);
}

mp_obj_t machine_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_freq, ARG_scl, ARG_sda, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_INT, {.u_int = DEFAULT_I2C_FREQ} },
        { MP_QSTR_scl, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_sda, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = DEFAULT_I2C_TIMEOUT} },
    };

    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Get I2C bus.
    int i2c_id = mp_obj_get_int(args[ARG_id].u_obj);
    if (i2c_id < 0 || i2c_id >= MP_ARRAY_SIZE(machine_i2c_obj)) {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("I2C(%d) doesn't exist"), i2c_id);
    }

    // Get static peripheral object.
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)&machine_i2c_obj[i2c_id];

    // Set SCL/SDA pins if configured.
    if (args[ARG_scl].u_obj != mp_const_none) {
        int scl = mp_hal_get_pin_obj(args[ARG_scl].u_obj);
        if (!IS_VALID_SCL(self->i2c_id, scl)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SCL pin"));
        }
        self->scl = scl;
    }
    if (args[ARG_sda].u_obj != mp_const_none) {
        int sda = mp_hal_get_pin_obj(args[ARG_sda].u_obj);
        if (!IS_VALID_SDA(self->i2c_id, sda)) {
            mp_raise_ValueError(MP_ERROR_TEXT("bad SDA pin"));
        }
        self->sda = sda;
    }

    // Initialise the I2C peripheral if any arguments given, or it was not initialised previously.
    if (n_args > 1 || n_kw > 0 || self->freq == 0) {
        self->freq = args[ARG_freq].u_int;
        i2c_init(self->i2c_inst, self->freq);
        self->freq = i2c_set_baudrate(self->i2c_inst, self->freq);
        self->timeout = args[ARG_timeout].u_int;
        gpio_set_function(self->scl, GPIO_FUNC_I2C);
        gpio_set_function(self->sda, GPIO_FUNC_I2C);
        gpio_set_pulls(self->scl, true, 0);
        gpio_set_pulls(self->sda, true, 0);
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC int machine_i2c_transfer_single(mp_obj_base_t *self_in, uint16_t addr, size_t len, uint8_t *buf, unsigned int flags) {
    machine_i2c_obj_t *self = (machine_i2c_obj_t *)self_in;
    int ret;
    bool nostop = !(flags & MP_MACHINE_I2C_FLAG_STOP);
    if (flags & MP_MACHINE_I2C_FLAG_READ) {
        ret = i2c_read_timeout_us(self->i2c_inst, addr, buf, len, nostop, self->timeout);
    } else {
        if (len == 0) {
            // Workaround issue with hardware I2C not accepting zero-length writes.
            mp_machine_soft_i2c_obj_t soft_i2c = {
                .base = { &mp_machine_soft_i2c_type },
                .us_delay = 500000 / self->freq + 1,
                .us_timeout = self->timeout,
                .scl = self->scl,
                .sda = self->sda,
            };
            mp_machine_i2c_buf_t bufs = {
                .len = len,
                .buf = buf,
            };
            mp_hal_pin_open_drain(self->scl);
            mp_hal_pin_open_drain(self->sda);
            ret = mp_machine_soft_i2c_transfer(&soft_i2c.base, addr, 1, &bufs, flags);
            gpio_set_function(self->scl, GPIO_FUNC_I2C);
            gpio_set_function(self->sda, GPIO_FUNC_I2C);
            return ret;
        } else {
            ret = i2c_write_timeout_us(self->i2c_inst, addr, buf, len, nostop, self->timeout);
        }
    }
    if (ret < 0) {
        if (ret == PICO_ERROR_TIMEOUT) {
            return -MP_ETIMEDOUT;
        } else {
            return -MP_EIO;
        }
    } else {
        return ret;
    }
}

STATIC const mp_machine_i2c_p_t machine_i2c_p = {
    .transfer = mp_machine_i2c_transfer_adaptor,
    .transfer_single = machine_i2c_transfer_single,
};

MP_DEFINE_CONST_OBJ_TYPE(
    machine_i2c_type,
    MP_QSTR_I2C,
    MP_TYPE_FLAG_NONE,
    make_new, machine_i2c_make_new,
    print, machine_i2c_print,
    protocol, &machine_i2c_p,
    locals_dict, &mp_machine_i2c_locals_dict
    );
