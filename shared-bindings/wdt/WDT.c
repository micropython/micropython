/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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

#include <string.h>

#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/wdt/__init__.h"
#include "shared-bindings/wdt/WDT.h"

static wdt_wdt_obj_t *wdt_singleton;

//| class WDT:
//|     """Watchdog Timer"""
//|
//|     def __init__(self, ):
//|         """This class represents the system's Watchdog Timer. It is a
//|         singleton and will always return the same instance."""
//|         ...
//|
STATIC mp_obj_t wdt_wdt_make_new(const mp_obj_type_t *type, size_t n_args,
                                 const mp_obj_t *pos_args,
                                 mp_map_t *kw_args) {
    enum { ARG_timeout_ms, ARG_sleep };
    static const mp_arg_t allowed_args[] = {
        {MP_QSTR_timeout_ms, MP_ARG_INT | MP_ARG_REQUIRED},
        {MP_QSTR_sleep, MP_ARG_BOOL, {.u_bool = false}},
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];

    if (wdt_singleton)
        return wdt_singleton;

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args),
                     allowed_args, args);

    if (args[ARG_timeout_ms].u_int <= 0) {
        mp_raise_ValueError(translate("watchdog timeout must be greater than 0"));
    }

    wdt_wdt_obj_t *self = m_new_obj(wdt_wdt_obj_t);
    self->base.type = &wdt_wdt_type;
    self->timeout = args[ARG_timeout_ms].u_int;
    self->sleep = args[ARG_sleep].u_bool;

    common_hal_wdt_init(self->timeout, self->sleep);
    wdt_singleton = self;
    return MP_OBJ_FROM_PTR(self);
}

//|     def feed(self):
//|         """Feed the watchdog timer. This must be called regularly, otherwise
//|         the system will reset."""
//|         ...
//|
STATIC mp_obj_t wdt_wdt_feed(mp_obj_t self_in) {
    (void)self_in;
    common_hal_wdt_feed();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wdt_wdt_feed_obj, wdt_wdt_feed);

//|     def deinit(self):
//|         """Stop the watchdog timer. This may raise an error if the watchdog
//|         timer cannot be disabled on this platform."""
//|         ...
//|
STATIC mp_obj_t wdt_wdt_deinit(mp_obj_t self_in) {
    (void)self_in;
    common_hal_wdt_disable();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wdt_wdt_deinit_obj, wdt_wdt_deinit);

//|     timeout: int = ...
//|     """The maximum number of milliseconds that can elapse between calls
//|     to feed()"""
//|
STATIC mp_obj_t wdt_wdt_obj_get_timeout(mp_obj_t self_in) {
    wdt_wdt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->timeout);
}
MP_DEFINE_CONST_FUN_OBJ_1(wdt_wdt_obj_get_timeout_obj, wdt_wdt_obj_get_timeout);

const mp_obj_property_t wdt_wdt_timeout_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&wdt_wdt_obj_get_timeout_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC const mp_rom_map_elem_t wdt_wdt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&wdt_wdt_feed_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&wdt_wdt_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout), MP_ROM_PTR(&wdt_wdt_timeout_obj) },
};
STATIC MP_DEFINE_CONST_DICT(wdt_wdt_locals_dict, wdt_wdt_locals_dict_table);

const mp_obj_type_t wdt_wdt_type = {
    { &mp_type_type },
    .name = MP_QSTR_WDT,
    .make_new = wdt_wdt_make_new,
    .locals_dict = (mp_obj_dict_t*)&wdt_wdt_locals_dict,
};
