/**
 * @file module_drivers/class_exti.c
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
 * Enternal interrupt callback. Called from an interrupt.
 */
static void exti_cb_isr(void *self_in)
{
    struct class_exti_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);

    if (self_p->callback != mp_const_none) {
        mp_call_function_0(self_p->callback);
    }

    if (self_p->event_obj_p != mp_const_none) {
        event_write_isr(&self_p->event_obj_p->event,
                        &self_p->mask,
                        sizeof(self_p->mask));
    }
}

/**
 * Print the exti object.
 */
static void class_exti_print(const mp_print_t *print_p,
                              mp_obj_t self_in,
                              mp_print_kind_t kind)
{
    struct class_exti_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Exti object. If additional arguments are given, they
 * are used to initialise the exti.
 */
static mp_obj_t class_exti_make_new(const mp_obj_type_t *type_p,
                                     mp_uint_t n_args,
                                     mp_uint_t n_kw,
                                     const mp_obj_t *args_p)
{
    struct class_exti_t *self_p;
    mp_map_t kwargs;
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_device, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_trigger, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_event, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_mask, MP_ARG_INT, { .u_int = 0x1 } },
        { MP_QSTR_callback, MP_ARG_OBJ, { .u_obj = mp_const_none } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    int device;
    int trigger;

    mp_arg_check_num(n_args, n_kw, 0, 5, true);

    /* Parse args. */
    mp_map_init_fixed_table(&kwargs, n_kw, args_p + n_args);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    /* Validate the device. */
    device = args[0].u_int;

    if ((device < 0) || (device >= EXTI_DEVICE_MAX)) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "bad exti device %d",
                                                device));
    }

    /* Validate the trigger. */
    trigger = args[1].u_int;

    if (!((trigger == EXTI_TRIGGER_RISING_EDGE)
          || (trigger == EXTI_TRIGGER_FALLING_EDGE)
          || (trigger == EXTI_TRIGGER_BOTH_EDGES))) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "bad trigger %d",
                                                trigger));
    }

    /* Third argument must be an event object or None. */
    if (mp_obj_get_type(args[2].u_obj) != mp_const_none) {
        if (mp_obj_get_type(args[2].u_obj) != &module_sync_class_event) {
            mp_raise_TypeError("expected <class 'Event'>");
        }
    }

    /* Fifth argument must be a callback or None. */
    if (args[4].u_obj != mp_const_none) {
        if (!mp_obj_is_callable(args[4].u_obj)) {
            mp_raise_TypeError("bad callback");
        }
    }

    /* Create a new Exti object. */
    self_p = m_new_obj(struct class_exti_t);
    self_p->base.type = &module_drivers_class_exti;
    self_p->event_obj_p = args[2].u_obj;
    self_p->mask = args[3].u_int;
    self_p->callback = args[4].u_obj;

    if (exti_init(&self_p->exti,
                  &exti_device[device],
                  trigger,
                  exti_cb_isr,
                  self_p) != 0) {
        return (mp_const_none);
    }

    return (self_p);
}

/**
 * def start(self)
 */
static mp_obj_t class_exti_start(mp_obj_t self_in)
{
    struct class_exti_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    exti_start(&self_p->exti);

    return (mp_const_none);
}

/**
 * def stop(self)
 */
static mp_obj_t class_exti_stop(mp_obj_t self_in)
{
    struct class_exti_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    exti_stop(&self_p->exti);

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_1(class_exti_start_obj, class_exti_start);
static MP_DEFINE_CONST_FUN_OBJ_1(class_exti_stop_obj, class_exti_stop);

static const mp_rom_map_elem_t class_exti_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&class_exti_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&class_exti_stop_obj) },

    /* Module constants. */
    { MP_ROM_QSTR(MP_QSTR_RISING), MP_ROM_INT(EXTI_TRIGGER_RISING_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_FALLING), MP_ROM_INT(EXTI_TRIGGER_FALLING_EDGE) },
    { MP_ROM_QSTR(MP_QSTR_BOTH), MP_ROM_INT(EXTI_TRIGGER_BOTH_EDGES) }
};

static MP_DEFINE_CONST_DICT(class_exti_locals_dict, class_exti_locals_dict_table);

/**
 * Exti class type.
 */
const mp_obj_type_t module_drivers_class_exti = {
    { &mp_type_type },
    .name = MP_QSTR_Exti,
    .print = class_exti_print,
    .make_new = class_exti_make_new,
    .locals_dict = (mp_obj_t)&class_exti_locals_dict,
};
