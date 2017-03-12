/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/objlist.h"

#if MICROPY_PY_UBLUEPY_CENTRAL

#include "ble_drv.h"

STATIC void ubluepy_scanner_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_scanner_obj_t * self = (ubluepy_scanner_obj_t *)o;
    (void)self;
    mp_printf(print, "Scanner");
}

STATIC mp_obj_t ubluepy_scanner_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    static const mp_arg_t allowed_args[] = {

    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    ubluepy_scanner_obj_t * s = m_new_obj(ubluepy_scanner_obj_t);
    s->base.type = type;

//    s->scan_list = mp_obj_new_list(0, NULL);

    return MP_OBJ_FROM_PTR(s);
}

const mp_obj_type_t ubluepy_scanner_type = {
    { &mp_type_type },
    .name = MP_QSTR_Scanner,
    .print = ubluepy_scanner_print,
    .make_new = ubluepy_scanner_make_new,
#if 0
    .locals_dict = (mp_obj_t)&ubluepy_scanner_locals_dict
#endif
};

#endif // MICROPY_PY_UBLUEPY_CENTRAL
