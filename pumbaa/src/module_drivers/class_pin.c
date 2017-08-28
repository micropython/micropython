/**
 * @file module_drivers/class_pin.c
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
 * Print the pin object.
 */
static void class_pin_print(const mp_print_t *print_p,
                            mp_obj_t self_in,
                            mp_print_kind_t kind)
{
    struct class_pin_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Pin object associated with the id. If additional
 * arguments are given, they are used to initialise the pin. See
 * `init`.
 */
static mp_obj_t class_pin_make_new(const mp_obj_type_t *type_p,
                                   mp_uint_t n_args,
                                   mp_uint_t n_kw,
                                   const mp_obj_t *args_p)
{
    struct class_pin_t *self_p;
    mp_map_t kwargs;
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_mode, MP_ARG_REQUIRED | MP_ARG_INT }
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    int device;
    int mode;

    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    /* Parse args. */
    mp_map_init(&kwargs, 0);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    device = args[0].u_int;
    mode = args[1].u_int;

    if ((device < 0) || (device >= PIN_DEVICE_MAX)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "bad pin device %d",
                                                device));
    }

    if ((mode != PIN_INPUT) && (mode != PIN_OUTPUT)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "bad pin mode %d",
                                                mode));
    }

    /* Create a new Pin object. */
    self_p = m_new0(struct class_pin_t, 1);
    self_p->base.type = &module_drivers_class_pin;

    if (pin_init((struct pin_driver_t *)&self_p->drv,
                 &pin_device[device],
                 mode) != 0) {
        return (mp_const_none);
    }

    return (self_p);
}

/**
 * def read(self)
 */
static mp_obj_t class_pin_read(mp_obj_t self_in)
{
    struct class_pin_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    return (MP_OBJ_NEW_SMALL_INT(pin_read(&self_p->drv)));
}

/**
 * def write(self, value)
 */
static mp_obj_t class_pin_write(mp_obj_t self_in, mp_obj_t value_in)
{
    struct class_pin_t *self_p;
    int value;

    value = mp_obj_get_int(value_in);

    if ((value != 0) && (value != 1)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "bad pin value %d",
                                                value));
    }

    self_p = MP_OBJ_TO_PTR(self_in);
    pin_write(&self_p->drv, value);

    return (mp_const_none);
}

/**
 * def toggle(self)
 */
static mp_obj_t class_pin_toggle(mp_obj_t self_in)
{
    struct class_pin_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    pin_toggle(&self_p->drv);

    return (mp_const_none);
}

/**
 * def set_mode(self, mode)
 */
static mp_obj_t class_pin_set_mode(mp_obj_t self_in, mp_obj_t mode_in)
{
    struct class_pin_t *self_p;
    int mode;

    mode = mp_obj_get_int(mode_in);

    if ((mode != 0) && (mode != 1)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "bad pin mode %d",
                                                mode));

    }

    self_p = MP_OBJ_TO_PTR(self_in);
    pin_set_mode(&self_p->drv, mode);

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_1(class_pin_read_obj, class_pin_read);
static MP_DEFINE_CONST_FUN_OBJ_2(class_pin_write_obj, class_pin_write);
static MP_DEFINE_CONST_FUN_OBJ_1(class_pin_toggle_obj, class_pin_toggle);
static MP_DEFINE_CONST_FUN_OBJ_2(class_pin_set_mode_obj, class_pin_set_mode);

static const mp_rom_map_elem_t class_pin_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&class_pin_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&class_pin_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&class_pin_toggle_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_mode), MP_ROM_PTR(&class_pin_set_mode_obj) },

    /* Class constants. */
    { MP_ROM_QSTR(MP_QSTR_INPUT), MP_ROM_INT(PIN_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_OUTPUT), MP_ROM_INT(PIN_OUTPUT) },
};

static MP_DEFINE_CONST_DICT(class_pin_locals_dict, class_pin_locals_dict_table);

/**
 * Pin class type.
 */
const mp_obj_type_t module_drivers_class_pin = {
    { &mp_type_type },
    .name = MP_QSTR_Pin,
    .print = class_pin_print,
    .make_new = class_pin_make_new,
    .locals_dict = (mp_obj_t)&class_pin_locals_dict,
};
