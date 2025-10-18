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
#include "py/runtime.h"

#if MICROPY_PY_TYPING

// Implement roughly the equivalent of the following minimal Python typing module, meant to support the
// typing syntax at runtime but otherwise ignoring any functionality:
//
//  TYPE_CHECKING = False
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

typedef struct _mp_obj_any_call_t
{
    mp_obj_base_t base;
} mp_obj_any_call_t;

static const mp_obj_type_t mp_type_any_call_t;
static const mp_obj_type_t mp_type_typing_alias;

// Lightweight runtime representation for objects such as typing.List[int].
// The alias keeps track of the original builtin type and the tuple of
// parameters so that __origin__ and __args__ can be queried at runtime.
typedef struct _mp_obj_typing_alias_t {
    mp_obj_base_t base;
    mp_obj_t origin;
    mp_obj_t args; // tuple or MP_OBJ_NULL when not parametrised
} mp_obj_typing_alias_t;

// Maps a qstr name to the builtin type that should back the alias.
typedef struct {
    qstr name;
    const mp_obj_type_t *type;
} typing_alias_spec_t;

static mp_obj_t typing_alias_from_spec(const typing_alias_spec_t *spec_table, size_t spec_len, qstr attr);

static mp_obj_t typing_alias_new(mp_obj_t origin, mp_obj_t args) {
    mp_obj_typing_alias_t *self = mp_obj_malloc(mp_obj_typing_alias_t, &mp_type_typing_alias);
    self->origin = origin;
    self->args = args;
    return MP_OBJ_FROM_PTR(self);
}

static void typing_alias_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // Only handle reads that we recognise: __origin__ and __args__.  Anything
    // else is delegated back to the VM where it will fall through to the
    // generic AnyCall behaviour.
    if (dest[0] != MP_OBJ_NULL) {
        return;
    }

    mp_obj_typing_alias_t *self = MP_OBJ_TO_PTR(self_in);
    if (attr == MP_QSTR___origin__) {
        dest[0] = self->origin;
    } else if (attr == MP_QSTR___args__) {
        dest[0] = self->args == MP_OBJ_NULL ? mp_const_empty_tuple : self->args;
    }
}

static mp_obj_t typing_alias_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value != MP_OBJ_SENTINEL) {
        mp_raise_TypeError(MP_ERROR_TEXT("typing alias does not support assignment"));
    }

    mp_obj_typing_alias_t *self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args_obj;
    if (mp_obj_is_type(index_in, &mp_type_tuple)) {
        args_obj = index_in;
    } else {
        mp_obj_t items[1] = { index_in };
        args_obj = mp_obj_new_tuple(1, items);
    }

    return typing_alias_new(self->origin, args_obj);
}

static mp_obj_t typing_alias_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_typing_alias_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_call_function_n_kw(self->origin, n_args, n_kw, args);
}

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_typing_alias,
    MP_QSTR_typing_alias,
    MP_TYPE_FLAG_NONE,
    attr, typing_alias_attr,
    subscr, typing_alias_subscr,
    call, typing_alias_call
    );


// Can be used both for __new__ and __call__: the latter's prototype is
// (mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args)
// so this function works as long as the argument size matches.
static mp_obj_t any_call_new(const mp_obj_type_t *arg0, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    #if MICROPY_OBJ_REPR != MICROPY_OBJ_REPR_D
    MP_STATIC_ASSERT(sizeof(mp_obj_type_t *) == sizeof(mp_obj_t));
    #endif
    // If it's an actual instance we're used for __call__ so return self_in.
    if (mp_obj_get_type(MP_OBJ_FROM_PTR(arg0)) == &mp_type_any_call_t) {
        return MP_OBJ_FROM_PTR(arg0);
    }
    // Could also be we're being called as a function/decorator, return the decorated thing then.
    // TODO obviously a bit of a hack, plus doesn't work for decorators with arguments.
    // Note could test mp_obj_is_fun on the first arg here, then being called as decorator etc that
    // is true, but turns out just returning the last argument works in more cases, like
    // UserId = typing.NewType("UserId", int)
    // assert UserId(1) == 1
    if (n_args != 0) {
        return args[n_args - 1];
    }
    return mp_obj_malloc(mp_obj_any_call_t, arg0);
}

#if MICROPY_OBJ_REPR == MICROPY_OBJ_REPR_D
static mp_obj_t any_call_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    if (mp_obj_get_type(self_in) == &mp_type_any_call_t || n_args == 0) {
        return self_in;
    }
    return args[n_args - 1];
}
#else
#define any_call_call any_call_new
#endif

#if defined(MICROPY_UNIX_COVERAGE)
void any_call_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest);
#endif

static mp_obj_t any_call_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    return self_in;
}

// TODO could probably apply same trick as in any_call_new here and merge any_call_module_attr,
// but still have to test if that's worth it code-size wise.
static void any_call_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // Only loading is supported.
    if (dest[0] == MP_OBJ_NULL) {
        if (attr != MP_QSTR___str__ && attr != MP_QSTR___repr__) {
            dest[0] = self_in;
        }
    }
}

// Only a small subset of typing.* names need concrete runtime behaviour.  The
// table below lists those names together with the builtin type that should be
// wrapped in a typing alias.  Everything else continues to use the extremely
// small AnyCall shim.
static const typing_alias_spec_t typing_container_specs[] = {
    { MP_QSTR_type, &mp_type_type },
    { MP_QSTR_Type, &mp_type_type },
    { MP_QSTR_List, &mp_type_list },
    { MP_QSTR_Dict, &mp_type_dict },
    { MP_QSTR_Tuple, &mp_type_tuple },
    { MP_QSTR_Literal, &mp_type_any_call_t },
    #if MICROPY_PY_BUILTINS_SET
    { MP_QSTR_Set, &mp_type_set },
    #endif
    #if MICROPY_PY_BUILTINS_FROZENSET
    { MP_QSTR_FrozenSet, &mp_type_frozenset },
    #endif
};

void any_call_module_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // Only loading is supported.
    if (dest[0] == MP_OBJ_NULL) {
        // First see if this attribute corresponds to a container alias that
        // needs a proper __getitem__ implementation.
        mp_obj_t alias = typing_alias_from_spec(typing_container_specs, MP_ARRAY_SIZE(typing_container_specs), attr);
        if (alias != MP_OBJ_NULL) {
            dest[0] = alias;
        } else {
            // Otherwise fall back to returning the singleton AnyCall object,
            // preserving the "typing ignores everything" behaviour used for
            // the majority of names.
            dest[0] = MP_OBJ_FROM_PTR(&mp_type_any_call_t);
        }
    }
}

static MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_any_call_t,
    MP_QSTR_any_call,
    MP_TYPE_FLAG_NONE,
    make_new, any_call_new,
    attr, any_call_attr,
    subscr, any_call_subscr,
    call, any_call_call
    );

// Helper to look up a qstr in the alias specification table and lazily create
// the corresponding typing alias object when a match is found.
static mp_obj_t typing_alias_from_spec(const typing_alias_spec_t *spec_table, size_t spec_len, qstr attr) {
    for (size_t i = 0; i < spec_len; ++i) {
        if (spec_table[i].name == attr) {
            mp_obj_t origin = MP_OBJ_FROM_PTR(spec_table[i].type);
            return typing_alias_new(origin, MP_OBJ_NULL);
        }
    }
    return MP_OBJ_NULL;
}

static const mp_rom_map_elem_t mp_module_typing_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_typing) },
    { MP_ROM_QSTR(MP_QSTR_TYPE_CHECKING), MP_ROM_FALSE },
};

static MP_DEFINE_CONST_DICT(mp_module_typing_globals, mp_module_typing_globals_table);

const mp_obj_module_t mp_module_typing = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_typing_globals,
};

// Extensible such that a typing module implemented in Python still has priority.
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_typing, mp_module_typing);
MP_REGISTER_MODULE_DELEGATION(mp_module_typing, any_call_module_attr);


#if MICROPY_PY_TYPING_EXTRA_MODULES
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_abc, mp_module_typing);
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR___future__, mp_module_typing);
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_typing_extensions, mp_module_typing);
#endif

#endif // MICROPY_PY_TYPING
