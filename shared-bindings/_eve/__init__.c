/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 James Bowman
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
#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/binary.h"

#include "shared-module/_eve/__init__.h"

//| :mod:`_eve` --- low-level BridgeTek EVE bindings
//| ================================================
//|
//| .. module:: _eve
//|   :synopsis: low-level BridgeTek EVE bindings
//|   :platform: SAMD21/SAMD51
//|
//| The `_eve` module provides a class _EVE which
//| contains methods for constructing EVE command 
//| buffers and appending basic graphics commands.
//|

typedef struct _mp_obj__EVE_t {
    mp_obj_base_t base;
    common_hal__eve_t _eve;
} mp_obj__EVE_t;

STATIC const mp_obj_type_t _EVE_type;

#define EVEHAL(s) \
  (&((mp_obj__EVE_t*)mp_instance_cast_to_native_base((s), &_EVE_type))->_eve)

STATIC mp_obj_t _register(mp_obj_t self, mp_obj_t o) {
    common_hal__eve_t *eve = EVEHAL(self);
    mp_load_method(o, MP_QSTR_write, eve->dest);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(register_obj, _register);

STATIC mp_obj_t _flush(mp_obj_t self) {
    common_hal__eve_flush(EVEHAL(self));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(flush_obj, _flush);

STATIC mp_obj_t _cc(mp_obj_t self, mp_obj_t b) {
    mp_buffer_info_t buffer_info;
    mp_get_buffer_raise(b, &buffer_info, MP_BUFFER_READ);
    common_hal__eve_add(EVEHAL(self), buffer_info.len, buffer_info.buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc_obj, _cc);

#include "mod_eve-gen.h"

// Hand-written functions {

#define ADD_X(self, x) \
    common_hal__eve_add(EVEHAL(self), sizeof(x), &(x));

STATIC mp_obj_t _cmd0(mp_obj_t self, mp_obj_t n) {
    uint32_t code = 0xffffff00 | mp_obj_get_int_truncated(n);
    ADD_X(self, code);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cmd0_obj, _cmd0);

STATIC mp_obj_t _vertex2f(mp_obj_t self, mp_obj_t a0, mp_obj_t a1) {
    mp_float_t x = mp_obj_get_float(a0);
    mp_float_t y = mp_obj_get_float(a1);
    common_hal__eve_Vertex2f(EVEHAL(self), x, y);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vertex2f_obj, _vertex2f);

STATIC mp_obj_t _cmd(size_t n_args, const mp_obj_t *args) {
    mp_obj_t self = args[0];
    mp_obj_t num = args[1];
    mp_buffer_info_t fmt;
    mp_get_buffer_raise(args[2], &fmt, MP_BUFFER_READ);
    size_t len;
    mp_obj_t *items;
    mp_obj_tuple_get(args[3], &len, &items);

    // Count how many 32-bit words required
    size_t n = 0;
    for (size_t i = 0; i < fmt.len; n++) {
        switch (((char*)fmt.buf)[i]) {
            case 'I':
            case 'i':
                i += 1;
                break;
            case 'H':
            case 'h':
                i += 2;
                break;
            default:
                break;
        }
    }

    uint32_t buf[16];
    uint32_t *p = buf;
    *p++ = 0xffffff00 | mp_obj_get_int_truncated(num);
    mp_obj_t *a = items;
    uint32_t lo;

    for (size_t i = 0; i < fmt.len; ) {
        switch (((char*)fmt.buf)[i]) {
            case 'I':
            case 'i':
                *p++ = mp_obj_get_int_truncated(*a++);
                i += 1;
                break;
            case 'H':
            case 'h':
                lo = mp_obj_get_int_truncated(*a++) & 0xffff;
                *p++ = lo | (mp_obj_get_int_truncated(*a++) << 16);
                i += 2;
                break;
            default:
                break;
        }
    }

    common_hal__eve_add(EVEHAL(self), sizeof(uint32_t) * (1 + n), buf);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(cmd_obj, 4, 4, _cmd);

STATIC const mp_rom_map_elem_t _EVE_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&register_obj) },
    { MP_ROM_QSTR(MP_QSTR_cc), MP_ROM_PTR(&cc_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_Vertex2f), MP_ROM_PTR(&vertex2f_obj) },
    { MP_ROM_QSTR(MP_QSTR_cmd), MP_ROM_PTR(&cmd_obj) },
    { MP_ROM_QSTR(MP_QSTR_cmd0), MP_ROM_PTR(&cmd0_obj) },
    ROM_DECLS
};
STATIC MP_DEFINE_CONST_DICT(_EVE_locals_dict, _EVE_locals_dict_table);

STATIC void _EVE_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    (void)kind;
    mp_printf(print, "<_EVE>");
}

STATIC mp_obj_t _EVE_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // mp_arg_check_num(n_args, kw_args, 1, 1, false);
    mp_obj__EVE_t *o = m_new_obj(mp_obj__EVE_t);
    o->base.type = &_EVE_type;
    o->_eve.n = 0;
    o->_eve.vscale = 16;
    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_obj_type_t _EVE_type = {
    { &mp_type_type },
    // Save on qstr's, reuse same as for module
    .name = MP_QSTR__EVE,
    .print = _EVE_print,
    .make_new = _EVE_make_new,
    // .attr = _EVE_attr,
    .locals_dict = (void*)&_EVE_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module__eve_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__eve) },
    { MP_ROM_QSTR(MP_QSTR__EVE), MP_OBJ_FROM_PTR(&_EVE_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module__eve_globals, mp_module__eve_globals_table);

const mp_obj_module_t _eve_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module__eve_globals,
};
