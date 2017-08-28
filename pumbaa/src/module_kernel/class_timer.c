/**
 * @file module_kernel/class_timer.c
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
 * Timer timeout callback. Called from an interrupt.
 */
static void timer_cb_isr(void *self_in)
{
    struct class_timer_t *self_p;

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
 * Print the timer object.
 */
static void class_timer_print(const mp_print_t *print_p,
                              mp_obj_t self_in,
                              mp_print_kind_t kind)
{
    struct class_timer_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    mp_printf(print_p, "<0x%p>", self_p);
}

/**
 * Create a new Timer object associated with the id. If additional
 * arguments are given, they are used to initialise the timer. See
 * `init`.
 */
static mp_obj_t class_timer_make_new(const mp_obj_type_t *type_p,
                                     mp_uint_t n_args,
                                     mp_uint_t n_kw,
                                     const mp_obj_t *args_p)
{
    struct class_timer_t *self_p;
    mp_map_t kwargs;
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_event, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_mask, MP_ARG_INT, { .u_int = 0x1 } },
        { MP_QSTR_callback, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_flags, MP_ARG_INT, { .u_int = 0x0 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    float f_timeout;
    struct time_t timeout;
    mp_uint_t len;
    mp_obj_t *items_p;
    int flags;

    mp_arg_check_num(n_args, n_kw, 0, 5, true);

    /* Parse args. */
    mp_map_init_fixed_table(&kwargs, n_kw, args_p + n_args);
    mp_arg_parse_all(n_args,
                     args_p,
                     &kwargs,
                     MP_ARRAY_SIZE(allowed_args),
                     allowed_args,
                     args);

    /* The timeout can be a tuple or a number. */
    if (MP_OBJ_IS_TYPE(args[0].u_obj, &mp_type_tuple)) {
        mp_obj_tuple_get(args[0].u_obj, &len, &items_p);

        if (len == 2) {
            timeout.seconds = mp_obj_get_int(items_p[0]);
            timeout.nanoseconds = mp_obj_get_int(items_p[1]);
        } else {
            mp_raise_TypeError("expected tuple of length 2");
        }
    } else {
        f_timeout = mp_obj_get_float(args[0].u_obj);
        timeout.seconds = (long)f_timeout;
        timeout.nanoseconds = (f_timeout - timeout.seconds) * 1000000000L;
    }

    /* Second argument must be an event object or None. */
    if (args[1].u_obj != mp_const_none) {
        if (mp_obj_get_type(args[1].u_obj) != &module_sync_class_event) {
            mp_raise_TypeError("expected <class 'Event'>");
        }
    }

    /* Fourth argument must be a callback or None. */
    if (args[3].u_obj != mp_const_none) {
        if (!mp_obj_is_callable(args[3].u_obj)) {
            mp_raise_TypeError("bad callback");
        }
    }

    /* Create a new Timer object. */
    self_p = m_new_obj(struct class_timer_t);
    self_p->base.type = &module_kernel_class_timer;
    self_p->event_obj_p = args[1].u_obj;
    self_p->mask = args[2].u_int;
    self_p->callback = args[3].u_obj;
    flags = args[4].u_int;

    if (timer_init(&self_p->timer,
                   &timeout,
                   timer_cb_isr,
                   self_p,
                   flags) != 0) {
        return (mp_const_none);
    }

    return (self_p);
}

/**
 * def start(self)
 */
static mp_obj_t class_timer_start(mp_obj_t self_in)
{
    struct class_timer_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    timer_start(&self_p->timer);

    return (mp_const_none);
}

/**
 * def stop(self)
 */
static mp_obj_t class_timer_stop(mp_obj_t self_in)
{
    struct class_timer_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    timer_stop(&self_p->timer);

    return (mp_const_none);
}

static MP_DEFINE_CONST_FUN_OBJ_1(class_timer_start_obj, class_timer_start);
static MP_DEFINE_CONST_FUN_OBJ_1(class_timer_stop_obj, class_timer_stop);

static const mp_rom_map_elem_t class_timer_locals_dict_table[] = {
    /* Instance methods. */
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&class_timer_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&class_timer_stop_obj) },

    /* Module constants. */
    { MP_ROM_QSTR(MP_QSTR_PERIODIC), MP_ROM_INT(TIMER_PERIODIC) },
};

static MP_DEFINE_CONST_DICT(class_timer_locals_dict, class_timer_locals_dict_table);

/**
 * Timer class type.
 */
const mp_obj_type_t module_kernel_class_timer = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
    .print = class_timer_print,
    .make_new = class_timer_make_new,
    .locals_dict = (mp_obj_t)&class_timer_locals_dict,
};
