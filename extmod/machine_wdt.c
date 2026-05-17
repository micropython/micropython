/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2023 Damien P. George
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

#if MICROPY_PY_MACHINE_WDT

#include "extmod/modmachine.h"

// The port must provide implementations of these low-level WDT functions.
static machine_wdt_obj_t *mp_machine_wdt_make_new_instance(mp_int_t id, mp_int_t timeout_ms);
static void mp_machine_wdt_feed(machine_wdt_obj_t *self);
#if MICROPY_PY_MACHINE_WDT_TIMEOUT_MS
static void mp_machine_wdt_timeout_ms_set(machine_wdt_obj_t *self_in, mp_int_t timeout_ms);
#endif

// The port provides implementations of the above in this file.
#include MICROPY_PY_MACHINE_WDT_INCLUDEFILE

static mp_obj_t machine_wdt_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_timeout, MP_ARG_INT, {.u_int = 5000} },
    };

    // Parse the arguments.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Create WDT instance.
    machine_wdt_obj_t *self = mp_machine_wdt_make_new_instance(args[ARG_id].u_int, args[ARG_timeout].u_int);

    return MP_OBJ_FROM_PTR(self);
}

// WDT.feed()
static mp_obj_t machine_wdt_feed(mp_obj_t self_in) {
    machine_wdt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_machine_wdt_feed(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(machine_wdt_feed_obj, machine_wdt_feed);

#if MICROPY_PY_MACHINE_WDT_TIMEOUT_MS
// WDT.timeout_ms(timeout)
static mp_obj_t machine_wdt_timeout_ms(mp_obj_t self_in, mp_obj_t timeout_in) {
    machine_wdt_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t timeout_ms = mp_obj_get_int(timeout_in);
    mp_machine_wdt_timeout_ms_set(self, timeout_ms);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(machine_wdt_timeout_ms_obj, machine_wdt_timeout_ms);
#endif

static const mp_rom_map_elem_t machine_wdt_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_feed), MP_ROM_PTR(&machine_wdt_feed_obj) },
    #if MICROPY_PY_MACHINE_WDT_TIMEOUT_MS
    { MP_ROM_QSTR(MP_QSTR_timeout_ms), MP_ROM_PTR(&machine_wdt_timeout_ms_obj) },
    #endif
};
static MP_DEFINE_CONST_DICT(machine_wdt_locals_dict, machine_wdt_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    machine_wdt_type,
    MP_QSTR_WDT,
    MP_TYPE_FLAG_NONE,
    make_new, machine_wdt_make_new,
    locals_dict, &machine_wdt_locals_dict
    );

#endif // MICROPY_PY_MACHINE_WDT
