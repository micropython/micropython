/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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

#include "py/obj.h"

#if MICROPY_PY_TYPING

// Implement roughly the equivalent of the following minimal Python typing module, meant to support the
// typing syntax at runtime but otherwise ignoring any functionality:
//
//  class _AnyCall:
//    def __init__(*args, **kwargs):
//        pass
//
//    def __call__(self, *args, **kwargs):
//        return self
//
//    def __getitem__(self, attr):
//        return self
//
//  _typing_obj = _AnyCall()
//
//  def __getattr__(attr):
//      return _typing_obj
//
// Note this works together with the micropython compiler itself ignoring type hints, i.e. when encountering
//
// def hello(name: str) -> None:
//    pass
//
// both str and None hints are simply ignored.


// The singleton object returned everywhere, both for module attr/subscr as well as for its own
// attr/subscr calls.
extern mp_obj_base_t typing_obj;

static mp_obj_t any_call_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    return self_in;
}

// Can be used both for __new__ and __call__: the latter's prototype is
// (mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args)
// so this function works as long as the argument size matches.
static mp_obj_t any_call_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    #if MICROPY_OBJ_REPR != MICROPY_OBJ_REPR_D
    MP_STATIC_ASSERT(sizeof(mp_obj_type_t *) == sizeof(mp_obj_t));
    #endif
    return MP_OBJ_FROM_PTR(&typing_obj);
}

#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
static mp_obj_t any_call_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    return MP_OBJ_FROM_PTR(&typing_obj);
}
#else
#define any_call_call any_call_new
#endif

#if defined(MICROPY_UNIX_COVERAGE)
void any_call_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
#endif

// Not static because it's also used for the module's attr delegation.
void any_call_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // Only loading is supported.
    if (dest[ 0 ] == MP_OBJ_NULL) {
        dest[ 0 ] = MP_OBJ_FROM_PTR(&typing_obj);
    }
}


static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_any_call_t,
    MP_QSTR_any_call,
    MP_TYPE_FLAG_NONE,
    make_new,
    any_call_new,
    attr,
    any_call_attr,
    subscr,
    any_call_subscr,
    call,
    any_call_call
    );

mp_obj_base_t typing_obj = {&mp_type_any_call_t};


static const mp_rom_map_elem_t mp_module_typing_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_typing) },
};

static MP_DEFINE_CONST_DICT(mp_module_typing_globals, mp_module_typing_globals_table);

const mp_obj_module_t mp_module_typing = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_typing_globals,
};

// Extensible such that a typing module implemented in Python still has priority.
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_typing, mp_module_typing);
MP_REGISTER_MODULE_DELEGATION(mp_module_typing, any_call_attr);


#if MICROPY_PY_TYPING_EXTRA_MODULES
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_abc, mp_module_typing);
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR___future__, mp_module_typing);
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_typing_extensions, mp_module_typing);
#endif

#endif // MICROPY_PY_TYPING
