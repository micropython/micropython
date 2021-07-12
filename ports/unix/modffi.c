/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * SPDX-FileCopyrightText: Copyright (c) 2014-2018 Paul Sokolovsky
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

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <ffi.h>
#include <stdint.h>

#include "py/runtime.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/objint.h"

#include "supervisor/shared/translate.h"

/*
 * modffi uses character codes to encode a value type, based on "struct"
 * module type codes, with some extensions and overridings.
 *
 * Extra/overridden typecodes:
 *      v - void, can be used only as return type
 *      P - const void*, pointer to read-only memory
 *      p - void*, meaning pointer to a writable memory (note that this
 *          clashes with struct's "p" as "Pascal string").
 *      s - as argument, the same as "p", as return value, causes string
 *          to be allocated and returned, instead of pointer value.
 *      O - mp_obj_t, passed as is (mostly useful as a callback param)
 *
 * TODO:
 *      C - callback function
 *
 * Note: all constraint specified by typecode can be not enforced at this time,
 * but may be later.
 */

// This union is large enough to hold any supported argument/return value.
typedef union _ffi_union_t {
    ffi_arg ffi;
    unsigned char B;
    unsigned short int H;
    unsigned int I;
    unsigned long int L;
    unsigned long long int Q;
    float flt;
    double dbl;
} ffi_union_t;

typedef struct _mp_obj_opaque_t {
    mp_obj_base_t base;
    void *val;
} mp_obj_opaque_t;

typedef struct _mp_obj_ffimod_t {
    mp_obj_base_t base;
    void *handle;
} mp_obj_ffimod_t;

typedef struct _mp_obj_ffivar_t {
    mp_obj_base_t base;
    void *var;
    char type;
//    ffi_type *type;
} mp_obj_ffivar_t;

typedef struct _mp_obj_ffifunc_t {
    mp_obj_base_t base;
    void *func;
    char rettype;
    const char *argtypes;
    ffi_cif cif;
    ffi_type *params[];
} mp_obj_ffifunc_t;

typedef struct _mp_obj_fficallback_t {
    mp_obj_base_t base;
    void *func;
    ffi_closure *clo;
    char rettype;
    ffi_cif cif;
    ffi_type *params[];
} mp_obj_fficallback_t;

// STATIC const mp_obj_type_t opaque_type;
STATIC const mp_obj_type_t ffimod_type;
STATIC const mp_obj_type_t ffifunc_type;
STATIC const mp_obj_type_t fficallback_type;
STATIC const mp_obj_type_t ffivar_type;

STATIC ffi_type *char2ffi_type(char c) {
    switch (c) {
        case 'b':
            return &ffi_type_schar;
        case 'B':
            return &ffi_type_uchar;
        case 'h':
            return &ffi_type_sshort;
        case 'H':
            return &ffi_type_ushort;
        case 'i':
            return &ffi_type_sint;
        case 'I':
            return &ffi_type_uint;
        case 'l':
            return &ffi_type_slong;
        case 'L':
            return &ffi_type_ulong;
        case 'q':
            return &ffi_type_sint64;
        case 'Q':
            return &ffi_type_uint64;
        #if MICROPY_PY_BUILTINS_FLOAT
        case 'f':
            return &ffi_type_float;
        case 'd':
            return &ffi_type_double;
        #endif
        case 'O': // mp_obj_t
        case 'C': // (*)()
        case 'P': // const void*
        case 'p': // void*
        case 's':
            return &ffi_type_pointer;
        case 'v':
            return &ffi_type_void;
        default:
            return NULL;
    }
}

STATIC ffi_type *get_ffi_type(mp_obj_t o_in) {
    if (mp_obj_is_str(o_in)) {
        const char *s = mp_obj_str_get_str(o_in);
        ffi_type *t = char2ffi_type(*s);
        if (t != NULL) {
            return t;
        }
    }
    // TODO: Support actual libffi type objects

    mp_raise_TypeError(MP_ERROR_TEXT("unknown type"));
}

STATIC mp_obj_t return_ffi_value(ffi_union_t *val, char type) {
    switch (type) {
        case 's': {
            const char *s = (const char *)(intptr_t)val->ffi;
            if (!s) {
                return mp_const_none;
            }
            return mp_obj_new_str(s, strlen(s));
        }
        case 'v':
            return mp_const_none;
        #if MICROPY_PY_BUILTINS_FLOAT
        case 'f': {
            return mp_obj_new_float_from_f(val->flt);
        }
        case 'd': {
            return mp_obj_new_float_from_d(val->dbl);
        }
        #endif
        case 'b':
        case 'h':
        case 'i':
        case 'l':
            return mp_obj_new_int((signed)val->ffi);
        case 'B':
        case 'H':
        case 'I':
        case 'L':
            return mp_obj_new_int_from_uint(val->ffi);
        case 'q':
            return mp_obj_new_int_from_ll(val->Q);
        case 'Q':
            return mp_obj_new_int_from_ull(val->Q);
        case 'O':
            return (mp_obj_t)(intptr_t)val->ffi;
        default:
            return mp_obj_new_int(val->ffi);
    }
}

// FFI module

STATIC void ffimod_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ffimod_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ffimod %p>", self->handle);
}

STATIC mp_obj_t ffimod_close(mp_obj_t self_in) {
    mp_obj_ffimod_t *self = MP_OBJ_TO_PTR(self_in);
    dlclose(self->handle);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ffimod_close_obj, ffimod_close);

STATIC mp_obj_t make_func(mp_obj_t rettype_in, void *func, mp_obj_t argtypes_in) {
    const char *rettype = mp_obj_str_get_str(rettype_in);
    const char *argtypes = mp_obj_str_get_str(argtypes_in);

    mp_int_t nparams = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(argtypes_in));
    mp_obj_ffifunc_t *o = m_new_obj_var(mp_obj_ffifunc_t, ffi_type *, nparams);
    o->base.type = &ffifunc_type;

    o->func = func;
    o->rettype = *rettype;
    o->argtypes = argtypes;

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(argtypes_in, &iter_buf);
    mp_obj_t item;
    int i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        o->params[i++] = get_ffi_type(item);
    }

    int res = ffi_prep_cif(&o->cif, FFI_DEFAULT_ABI, nparams, char2ffi_type(*rettype), o->params);
    if (res != FFI_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("Error in ffi_prep_cif"));
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t ffimod_func(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // always 4
    mp_obj_ffimod_t *self = MP_OBJ_TO_PTR(args[0]);
    const char *symname = mp_obj_str_get_str(args[2]);

    void *sym = dlsym(self->handle, symname);
    if (sym == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    return make_func(args[1], sym, args[3]);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ffimod_func_obj, 4, 4, ffimod_func);

STATIC mp_obj_t mod_ffi_func(mp_obj_t rettype, mp_obj_t addr_in, mp_obj_t argtypes) {
    void *addr = (void *)MP_OBJ_TO_PTR(mp_obj_int_get_truncated(addr_in));
    return make_func(rettype, addr, argtypes);
}
MP_DEFINE_CONST_FUN_OBJ_3(mod_ffi_func_obj, mod_ffi_func);

STATIC void call_py_func(ffi_cif *cif, void *ret, void **args, void *func) {
    mp_obj_t pyargs[cif->nargs];
    for (uint i = 0; i < cif->nargs; i++) {
        pyargs[i] = mp_obj_new_int(*(mp_int_t *)args[i]);
    }
    mp_obj_t res = mp_call_function_n_kw(MP_OBJ_FROM_PTR(func), cif->nargs, 0, pyargs);

    if (res != mp_const_none) {
        *(ffi_arg *)ret = mp_obj_int_get_truncated(res);
    }
}

STATIC mp_obj_t mod_ffi_callback(mp_obj_t rettype_in, mp_obj_t func_in, mp_obj_t paramtypes_in) {
    const char *rettype = mp_obj_str_get_str(rettype_in);

    mp_int_t nparams = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(paramtypes_in));
    mp_obj_fficallback_t *o = m_new_obj_var(mp_obj_fficallback_t, ffi_type *, nparams);
    o->base.type = &fficallback_type;

    o->clo = ffi_closure_alloc(sizeof(ffi_closure), &o->func);

    o->rettype = *rettype;

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(paramtypes_in, &iter_buf);
    mp_obj_t item;
    int i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        o->params[i++] = get_ffi_type(item);
    }

    int res = ffi_prep_cif(&o->cif, FFI_DEFAULT_ABI, nparams, char2ffi_type(*rettype), o->params);
    if (res != FFI_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("Error in ffi_prep_cif"));
    }

    res = ffi_prep_closure_loc(o->clo, &o->cif, call_py_func, MP_OBJ_TO_PTR(func_in), o->func);
    if (res != FFI_OK) {
        mp_raise_ValueError(MP_ERROR_TEXT("ffi_prep_closure_loc"));
    }

    return MP_OBJ_FROM_PTR(o);
}
MP_DEFINE_CONST_FUN_OBJ_3(mod_ffi_callback_obj, mod_ffi_callback);

STATIC mp_obj_t ffimod_var(mp_obj_t self_in, mp_obj_t vartype_in, mp_obj_t symname_in) {
    mp_obj_ffimod_t *self = MP_OBJ_TO_PTR(self_in);
    const char *rettype = mp_obj_str_get_str(vartype_in);
    const char *symname = mp_obj_str_get_str(symname_in);

    void *sym = dlsym(self->handle, symname);
    if (sym == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    mp_obj_ffivar_t *o = m_new_obj(mp_obj_ffivar_t);
    o->base.type = &ffivar_type;

    o->var = sym;
    o->type = *rettype;
    return MP_OBJ_FROM_PTR(o);
}
MP_DEFINE_CONST_FUN_OBJ_3(ffimod_var_obj, ffimod_var);

STATIC mp_obj_t ffimod_addr(mp_obj_t self_in, mp_obj_t symname_in) {
    mp_obj_ffimod_t *self = MP_OBJ_TO_PTR(self_in);
    const char *symname = mp_obj_str_get_str(symname_in);

    void *sym = dlsym(self->handle, symname);
    if (sym == NULL) {
        mp_raise_OSError(MP_ENOENT);
    }
    return mp_obj_new_int((uintptr_t)sym);
}
MP_DEFINE_CONST_FUN_OBJ_2(ffimod_addr_obj, ffimod_addr);

STATIC mp_obj_t ffimod_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    (void)n_args;
    (void)kw_args;

    const char *fname = NULL;
    if (args[0] != mp_const_none) {
        fname = mp_obj_str_get_str(args[0]);
    }
    void *mod = dlopen(fname, RTLD_NOW | RTLD_LOCAL);

    if (mod == NULL) {
        mp_raise_OSError(errno);
    }
    mp_obj_ffimod_t *o = m_new_obj(mp_obj_ffimod_t);
    o->base.type = type;
    o->handle = mod;
    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_rom_map_elem_t ffimod_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_func), MP_ROM_PTR(&ffimod_func_obj) },
    { MP_ROM_QSTR(MP_QSTR_var), MP_ROM_PTR(&ffimod_var_obj) },
    { MP_ROM_QSTR(MP_QSTR_addr), MP_ROM_PTR(&ffimod_addr_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ffimod_close_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ffimod_locals_dict, ffimod_locals_dict_table);

STATIC const mp_obj_type_t ffimod_type = {
    { &mp_type_type },
    .name = MP_QSTR_ffimod,
    .print = ffimod_print,
    .make_new = ffimod_make_new,
    .locals_dict = (mp_obj_dict_t *)&ffimod_locals_dict,
};

// FFI function

STATIC void ffifunc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ffifunc_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ffifunc %p>", self->func);
}

STATIC unsigned long long ffi_get_int_value(mp_obj_t o) {
    if (mp_obj_is_small_int(o)) {
        return MP_OBJ_SMALL_INT_VALUE(o);
    } else {
        unsigned long long res;
        mp_obj_int_to_bytes_impl(o, MP_ENDIANNESS_BIG, sizeof(res), (byte *)&res);
        return res;
    }
}

STATIC ffi_union_t ffi_int_obj_to_ffi_union(mp_obj_t o, const char argtype) {
    ffi_union_t ret;
    if ((argtype | 0x20) == 'q') {
        ret.Q = ffi_get_int_value(o);
        return ret;
    } else {
        mp_uint_t val = mp_obj_int_get_truncated(o);
        switch (argtype) {
            case 'b':
            case 'B':
                ret.B = val;
                break;
            case 'h':
            case 'H':
                ret.H = val;
                break;
            case 'i':
            case 'I':
                ret.I = val;
                break;
            case 'l':
            case 'L':
                ret.L = val;
                break;
            default:
                ret.ffi = val;
                break;
        }
    }
    return ret;
}

STATIC mp_obj_t ffifunc_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)n_kw;
    mp_obj_ffifunc_t *self = MP_OBJ_TO_PTR(self_in);
    assert(n_kw == 0);
    assert(n_args == self->cif.nargs);

    ffi_union_t values[n_args];
    void *valueptrs[n_args];
    const char *argtype = self->argtypes;
    for (uint i = 0; i < n_args; i++, argtype++) {
        mp_obj_t a = args[i];
        if (*argtype == 'O') {
            values[i].ffi = (ffi_arg)(intptr_t)a;
        #if MICROPY_PY_BUILTINS_FLOAT
        } else if (*argtype == 'f') {
            values[i].flt = mp_obj_get_float_to_f(a);
        } else if (*argtype == 'd') {
            values[i].dbl = mp_obj_get_float_to_d(a);
        #endif
        } else if (a == mp_const_none) {
            values[i].ffi = 0;
        } else if (mp_obj_is_int(a)) {
            values[i] = ffi_int_obj_to_ffi_union(a, *argtype);
        } else if (mp_obj_is_str(a)) {
            const char *s = mp_obj_str_get_str(a);
            values[i].ffi = (ffi_arg)(intptr_t)s;
        } else {
            mp_getbuffer_fun_t get_buffer = mp_type_get_getbuffer_slot(((mp_obj_base_t *)MP_OBJ_TO_PTR(a))->type);
            if (get_buffer != NULL) {
                mp_obj_base_t *o = (mp_obj_base_t *)MP_OBJ_TO_PTR(a);
                mp_buffer_info_t bufinfo;
                int ret = get_buffer(MP_OBJ_FROM_PTR(o), &bufinfo, MP_BUFFER_READ); // TODO: MP_BUFFER_READ?
                if (ret != 0) {
                    goto error;
                }
                values[i].ffi = (ffi_arg)(intptr_t)bufinfo.buf;
            } else if (mp_obj_is_type(a, &fficallback_type)) {
                mp_obj_fficallback_t *p = MP_OBJ_TO_PTR(a);
                values[i].ffi = (ffi_arg)(intptr_t)p->func;
            } else {
                goto error;
            }
        }
        valueptrs[i] = &values[i];
    }

    ffi_union_t retval;
    ffi_call(&self->cif, self->func, &retval, valueptrs);
    return return_ffi_value(&retval, self->rettype);

error:
    mp_raise_TypeError(MP_ERROR_TEXT("Don't know how to pass object to native function"));
}

STATIC const mp_obj_type_t ffifunc_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_ffifunc,
    .print = ffifunc_print,
    MP_TYPE_EXTENDED_FIELDS(
        .call = ffifunc_call,
        ),
};

// FFI callback for Python function

STATIC void fficallback_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_fficallback_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<fficallback %p>", self->func);
}

STATIC const mp_obj_type_t fficallback_type = {
    { &mp_type_type },
    .name = MP_QSTR_fficallback,
    .print = fficallback_print,
};

// FFI variable

STATIC void ffivar_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ffivar_t *self = MP_OBJ_TO_PTR(self_in);
    // Variable value printed as cast to int
    mp_printf(print, "<ffivar @%p: 0x%x>", self->var, *(int *)self->var);
}

STATIC mp_obj_t ffivar_get(mp_obj_t self_in) {
    mp_obj_ffivar_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_binary_get_val_array(self->type, self->var, 0);
}
MP_DEFINE_CONST_FUN_OBJ_1(ffivar_get_obj, ffivar_get);

STATIC mp_obj_t ffivar_set(mp_obj_t self_in, mp_obj_t val_in) {
    mp_obj_ffivar_t *self = MP_OBJ_TO_PTR(self_in);
    mp_binary_set_val_array(self->type, self->var, 0, val_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(ffivar_set_obj, ffivar_set);

STATIC const mp_rom_map_elem_t ffivar_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&ffivar_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&ffivar_set_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ffivar_locals_dict, ffivar_locals_dict_table);

STATIC const mp_obj_type_t ffivar_type = {
    { &mp_type_type },
    .name = MP_QSTR_ffivar,
    .print = ffivar_print,
    .locals_dict = (mp_obj_dict_t *)&ffivar_locals_dict,
};

// Generic opaque storage object (unused)

/*
STATIC const mp_obj_type_t opaque_type = {
    { &mp_type_type },
    .name = MP_QSTR_opaqueval,
//    .print = opaque_print,
};
*/

STATIC mp_obj_t mod_ffi_open(size_t n_args, const mp_obj_t *args) {
    return ffimod_make_new(&ffimod_type, n_args, args, NULL);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_ffi_open_obj, 1, 2, mod_ffi_open);

STATIC mp_obj_t mod_ffi_as_bytearray(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytearray_by_ref(mp_obj_int_get_truncated(size), (void *)(uintptr_t)mp_obj_int_get_truncated(ptr));
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_ffi_as_bytearray_obj, mod_ffi_as_bytearray);

STATIC const mp_rom_map_elem_t mp_module_ffi_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ffi) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mod_ffi_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&mod_ffi_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_func), MP_ROM_PTR(&mod_ffi_func_obj) },
    { MP_ROM_QSTR(MP_QSTR_as_bytearray), MP_ROM_PTR(&mod_ffi_as_bytearray_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_ffi_globals, mp_module_ffi_globals_table);

const mp_obj_module_t mp_module_ffi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_ffi_globals,
};
