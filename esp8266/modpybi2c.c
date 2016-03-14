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

#include "ets_sys.h"
#include "etshal.h"
#include "osapi.h"
#include "gpio.h"

#include "py/runtime.h"
#include "modpyb.h"

typedef struct _pyb_i2c_obj_t {
    mp_obj_base_t base;
    pyb_pin_obj_t *scl;
    pyb_pin_obj_t *sda;
} pyb_i2c_obj_t;

// these set the frequency of SCL
#define mphal_i2c_wait_a() os_delay_us(2)
#define mphal_i2c_wait_b() os_delay_us(1)

STATIC void mphal_i2c_set_sda(pyb_i2c_obj_t *self, uint8_t sda) {
    uint32_t port = self->sda->phys_port;
    sda &= 0x01;
    gpio_output_set(sda << port, (1 - sda) << port, 1 << port, 0);
}

STATIC void mphal_i2c_set_scl(pyb_i2c_obj_t *self, uint8_t scl) {
    uint32_t port = self->scl->phys_port;
    scl &= 0x01;
    gpio_output_set(scl << port, (1 - scl) << port, 1 << port, 0);
}

STATIC int mphal_i2c_get_sda(pyb_i2c_obj_t *self) {
    return GPIO_INPUT_GET(GPIO_ID_PIN(self->sda->phys_port));
}

STATIC void mphal_i2c_start(pyb_i2c_obj_t *self) {
    mphal_i2c_set_sda(self, 1);
    mphal_i2c_wait_a();
    mphal_i2c_set_scl(self, 1);
    mphal_i2c_wait_a();
    mphal_i2c_set_sda(self, 0);
    mphal_i2c_wait_a();
}

STATIC void mphal_i2c_stop(pyb_i2c_obj_t *self) {
    mphal_i2c_wait_a();
    mphal_i2c_set_sda(self, 0);
    mphal_i2c_wait_a();
    mphal_i2c_set_scl(self, 1);
    mphal_i2c_wait_a();
    mphal_i2c_set_sda(self, 1);
    mphal_i2c_wait_a();
}

STATIC void mphal_i2c_init(pyb_i2c_obj_t *self, uint32_t freq) {
    pyb_pin_obj_t *scl = self->scl;
    pyb_pin_obj_t *sda = self->sda;

    ETS_GPIO_INTR_DISABLE();
    //ETS_INTR_LOCK();

    PIN_FUNC_SELECT(sda->periph, sda->func);
    PIN_FUNC_SELECT(scl->periph, scl->func);

    GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(sda->phys_port)),
        GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(sda->phys_port)))
        | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); // open drain
    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS,
        GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << sda->phys_port));
    GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(scl->phys_port)),
        GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(scl->phys_port)))
        | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)); // open drain
    GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS,
        GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << scl->phys_port));

    mphal_i2c_set_scl(self, 1);
    mphal_i2c_set_sda(self, 1);

    ETS_GPIO_INTR_ENABLE();
    //ETS_INTR_UNLOCK();

    mphal_i2c_set_scl(self, 0);
    mphal_i2c_wait_a();

    // when SCL = 0, toggle SDA to clear up
    mphal_i2c_set_sda(self, 0);
    mphal_i2c_wait_a();
    mphal_i2c_set_sda(self, 1);
    mphal_i2c_wait_a();

    // set data_cnt to max value
    for (uint8_t i = 0; i < 28; i++) {
        mphal_i2c_set_scl(self, 0);
        mphal_i2c_wait_a();
        mphal_i2c_set_scl(self, 1);
        mphal_i2c_wait_a();
    }

    // reset all
    mphal_i2c_stop(self);
}

STATIC int mphal_i2c_write_byte(pyb_i2c_obj_t *self, uint8_t val) {
    uint8_t dat;
    sint8 i;

    mphal_i2c_wait_a();

    mphal_i2c_set_scl(self, 0);
    mphal_i2c_wait_a();

    for (i = 7; i >= 0; i--) {
        dat = val >> i;
        mphal_i2c_set_sda(self, dat);
        mphal_i2c_wait_a();
        mphal_i2c_set_scl(self, 1);
        mphal_i2c_wait_a();

        if (i == 0) {
            mphal_i2c_wait_b();
        }

        mphal_i2c_set_scl(self, 0);
        mphal_i2c_wait_a();
    }

    mphal_i2c_set_sda(self, 1);
    mphal_i2c_wait_a();
    mphal_i2c_set_scl(self, 1);
    mphal_i2c_wait_a();

    int ret = mphal_i2c_get_sda(self);
    mphal_i2c_wait_a();
    mphal_i2c_set_scl(self, 0);
    mphal_i2c_wait_a();

    return !ret;
}

STATIC void mphal_i2c_write(pyb_i2c_obj_t *self, uint8_t addr, uint8_t *data, size_t len, bool stop) {
    mphal_i2c_start(self);
    if (!mphal_i2c_write_byte(self, addr << 1)) {
        goto er;
    }
    while (len--) {
        if (!mphal_i2c_write_byte(self, *data++)) {
            goto er;
        }
    }
    if (stop) {
        mphal_i2c_stop(self);
    }
    return;

er:
    mphal_i2c_stop(self);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
}

STATIC int mphal_i2c_read_byte(pyb_i2c_obj_t *self, uint8_t *val) {
    mphal_i2c_wait_a();
    mphal_i2c_set_scl(self, 0);
    mphal_i2c_wait_a();

    uint8_t dat = 0;
    for (int i = 7; i >= 0; i--) {
        mphal_i2c_set_scl(self, 1);
        mphal_i2c_wait_a();
        dat = (dat << 1) | mphal_i2c_get_sda(self);
        mphal_i2c_wait_a();
        mphal_i2c_set_scl(self, 0);
        mphal_i2c_wait_a();
    }
    *val = dat;

    mphal_i2c_wait_a();
    mphal_i2c_set_scl(self, 1);
    mphal_i2c_wait_a();
    mphal_i2c_wait_b();
    mphal_i2c_set_scl(self, 0);
    mphal_i2c_wait_a();

    return 1; // success
}

STATIC void mphal_i2c_read(pyb_i2c_obj_t *self, uint8_t addr, uint8_t *data, size_t len, bool stop) {
    mphal_i2c_start(self);
    if (!mphal_i2c_write_byte(self, (addr << 1) | 1)) {
        goto er;
    }
    while (len--) {
        if (!mphal_i2c_read_byte(self, data++)) {
            goto er;
        }
    }
    if (stop) {
        mphal_i2c_stop(self);
    }
    return;

er:
    mphal_i2c_stop(self);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "I2C bus error"));
}

/******************************************************************************/
// MicroPython bindings for I2C

STATIC void pyb_i2c_obj_init_helper(pyb_i2c_obj_t *self, mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_scl, ARG_sda, ARG_freq };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_freq, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 400000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    self->scl = mp_obj_get_pin_obj(args[ARG_scl].u_obj);
    self->sda = mp_obj_get_pin_obj(args[ARG_sda].u_obj);
    mphal_i2c_init(self, args[ARG_freq].u_int);
}

STATIC mp_obj_t pyb_i2c_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, true);
    pyb_i2c_obj_t *self = m_new_obj(pyb_i2c_obj_t);
    self->base.type = &pyb_i2c_type;
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, args + n_args);
    pyb_i2c_obj_init_helper(self, n_args, args, &kw_args);
    return (mp_obj_t)self;
}

STATIC mp_obj_t pyb_i2c_obj_init(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    pyb_i2c_obj_init_helper(args[0], n_args - 1, args + 1, kw_args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_init_obj, 1, pyb_i2c_obj_init);

STATIC mp_obj_t pyb_i2c_readfrom(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_n, ARG_stop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_n,    MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    pyb_i2c_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // do the I2C transfer
    vstr_t vstr;
    vstr_init_len(&vstr, args[ARG_n].u_int);
    mphal_i2c_read(self, args[ARG_addr].u_int, (uint8_t*)vstr.buf, vstr.len, args[ARG_stop].u_bool);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_readfrom_obj, 1, pyb_i2c_readfrom);

STATIC mp_obj_t pyb_i2c_writeto(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_addr, ARG_buf, ARG_stop };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_addr, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_buf,  MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    pyb_i2c_obj_t *self = pos_args[0];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the buffer to write from
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &bufinfo, MP_BUFFER_READ);

    // do the I2C transfer
    mphal_i2c_write(self, args[ARG_addr].u_int, bufinfo.buf, bufinfo.len, args[ARG_stop].u_bool);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(pyb_i2c_writeto_obj, 1, pyb_i2c_writeto);

STATIC const mp_rom_map_elem_t pyb_i2c_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&pyb_i2c_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_readfrom), MP_ROM_PTR(&pyb_i2c_readfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeto), MP_ROM_PTR(&pyb_i2c_writeto_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_i2c_locals_dict, pyb_i2c_locals_dict_table);

const mp_obj_type_t pyb_i2c_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2C,
    .make_new = pyb_i2c_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_i2c_locals_dict,
};
