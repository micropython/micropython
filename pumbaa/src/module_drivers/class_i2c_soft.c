/**
 * @file module_drivers/class_i2c_soft.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

/**
 * Print the i2c soft object.
 */
static void class_i2c_soft_print(const mp_print_t *print_p,
                                 mp_obj_t self_in,
                                 mp_print_kind_t kind)
{
    struct class_i2c_soft_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new I2CSoft object associated with the id.
 */
static mp_obj_t class_i2c_soft_make_new(const mp_obj_type_t *type_p,
                                        mp_uint_t n_args,
                                        mp_uint_t n_kw,
                                        const mp_obj_t *args_p)
{
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_baudrate, MP_ARG_INT, { .u_int = 50000 } },
        { MP_QSTR_max_clock_stretching_us, MP_ARG_INT, { .u_int = 1000000 } },
        { MP_QSTR_clock_stretching_sleep_us, MP_ARG_INT, { .u_int = 10000 } }
    };
    struct class_i2c_soft_t *self_p;
    mp_map_t kwargs;
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    int scl;
    int sda;
    int baudrate;
    int max_clock_stretching_us;
    int clock_stretching_sleep_us;

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    /* Parse args. */
    mp_map_init(&kwargs, 0);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    /* Create a new i2c_soft object. */
    self_p = m_new_obj(struct class_i2c_soft_t);
    self_p->base.type = &module_drivers_class_i2c_soft;

    /* SCL argument. */
    scl = args[0].u_int;

    if ((scl < 0) || (scl > PIN_DEVICE_MAX)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad pin"));
    }

    /* SDA argument. */
    sda = args[1].u_int;

    if ((sda < 0) || (sda > PIN_DEVICE_MAX)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                                           "bad sda"));
    }

    /* Baudrate argument. */
    baudrate = args[2].u_int;

    max_clock_stretching_us = args[3].u_int;

    clock_stretching_sleep_us = args[4].u_int;

    if (i2c_soft_init(&self_p->drv,
                      &pin_device[scl],
                      &pin_device[sda],
                      baudrate,
                      max_clock_stretching_us,
                      clock_stretching_sleep_us) != 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (self_p);
}

/**
 * def start(self)
 */
static mp_obj_t class_i2c_soft_start(mp_obj_t self_in)
{
    struct class_i2c_soft_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    i2c_soft_start(&self_p->drv);

    return (mp_const_none);
}

/**
 * def stop(self)
 */
static mp_obj_t class_i2c_soft_stop(mp_obj_t self_in)
{
    struct class_i2c_soft_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    i2c_soft_stop(&self_p->drv);

    return (mp_const_none);
}

/**
 * def read(self, address, size)
 */
static mp_obj_t class_i2c_soft_read(mp_obj_t self_in,
                                    mp_obj_t address_in,
                                    mp_obj_t size_in)
{
    struct class_i2c_soft_t *self_p;
    vstr_t vstr;
    int address;
    ssize_t size;

    self_p = MP_OBJ_TO_PTR(self_in);
    address = mp_obj_get_int(size_in);
    size = mp_obj_get_int(size_in);

    vstr_init_len(&vstr, size);
    size = i2c_soft_read(&self_p->drv, address, vstr.buf, size);

    if (size < 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    vstr.len = size;

    return (mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr));
}

/**
 * def read_into(self, address, buffer[, size])
 */
static mp_obj_t class_i2c_soft_read_into(mp_uint_t n_args, const mp_obj_t *args_p)
{
    struct class_i2c_soft_t *self_p;
    int address;
    mp_buffer_info_t buffer_info;
    ssize_t size;

    self_p = MP_OBJ_TO_PTR(args_p[0]);
    address = mp_obj_get_int(args_p[1]);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[2]),
                        &buffer_info,
                        MP_BUFFER_WRITE);

    /* Get the size. */
    if (n_args == 4) {
        size = mp_obj_get_int(args_p[3]);

        if (buffer_info.len < size) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    } else {
        size = buffer_info.len;
    }

    size = i2c_soft_read(&self_p->drv, address, buffer_info.buf, size);

    if (size < 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (MP_OBJ_NEW_SMALL_INT(size));
}

/**
 * def write(self, address, buffer[, size])
 */
static mp_obj_t class_i2c_soft_write(mp_uint_t n_args, const mp_obj_t *args_p)
{
    struct class_i2c_soft_t *self_p;
    mp_buffer_info_t buffer_info;
    int address;
    ssize_t size;

    self_p = MP_OBJ_TO_PTR(args_p[0]);
    address = mp_obj_get_int(args_p[1]);
    mp_get_buffer_raise(MP_OBJ_TO_PTR(args_p[2]),
                        &buffer_info,
                        MP_BUFFER_READ);

    /* Get the size. */
    if (n_args == 4) {
        size = mp_obj_get_int(args_p[3]);

        if (buffer_info.len < size) {
            nlr_raise(mp_obj_new_exception(&mp_type_OSError));
        }
    } else {
        size = buffer_info.len;
    }

    size = i2c_soft_write(&self_p->drv, address, buffer_info.buf, size);

    if (size < 0) {
        nlr_raise(mp_obj_new_exception(&mp_type_OSError));
    }

    return (MP_OBJ_NEW_SMALL_INT(size));
}

/**
 * def scan(self)
 */
static mp_obj_t class_i2c_soft_scan(mp_obj_t self_in)
{
    struct class_i2c_soft_t *self_p;
    mp_obj_t list;
    int address;

    self_p = MP_OBJ_TO_PTR(self_in);
    list = mp_obj_new_list(0, NULL);

    for (address = 0; address < 128; address++) {
        if (i2c_soft_scan(&self_p->drv, address) == 1) {
            mp_obj_list_append(list, MP_OBJ_NEW_SMALL_INT(address));
        }
    }

    return (list);
}

static MP_DEFINE_CONST_FUN_OBJ_1(class_i2c_soft_start_obj, class_i2c_soft_start);
static MP_DEFINE_CONST_FUN_OBJ_1(class_i2c_soft_stop_obj, class_i2c_soft_stop);
static MP_DEFINE_CONST_FUN_OBJ_3(class_i2c_soft_read_obj, class_i2c_soft_read);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(class_i2c_soft_read_into_obj, 3, 4, class_i2c_soft_read_into);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(class_i2c_soft_write_obj, 3, 4, class_i2c_soft_write);
static MP_DEFINE_CONST_FUN_OBJ_1(class_i2c_soft_scan_obj, class_i2c_soft_scan);

static const mp_rom_map_elem_t class_i2c_soft_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&class_i2c_soft_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&class_i2c_soft_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&class_i2c_soft_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_read_into), MP_ROM_PTR(&class_i2c_soft_read_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&class_i2c_soft_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&class_i2c_soft_scan_obj) }
};

static MP_DEFINE_CONST_DICT(class_i2c_soft_locals_dict, class_i2c_soft_locals_dict_table);

/**
 * I2c_Soft class type.
 */
const mp_obj_type_t module_drivers_class_i2c_soft = {
    { &mp_type_type },
    .name = MP_QSTR_I2C_Soft,
    .print = class_i2c_soft_print,
    .make_new = class_i2c_soft_make_new,
    .locals_dict = (mp_obj_t)&class_i2c_soft_locals_dict,
};
