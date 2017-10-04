/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

//STATIC const mp_obj_type_t opaque_type;
STATIC const mp_obj_type_t ffimod_type;
STATIC const mp_obj_type_t ffifunc_type;
STATIC const mp_obj_type_t fficallback_type;
STATIC const mp_obj_type_t ffivar_type;

STATIC ffi_type *char2ffi_type(char c)
{
    switch (c) {
        case 'b': return &ffi_type_schar;
        case 'B': return &ffi_type_uchar;
        case 'h': return &ffi_type_sshort;
        case 'H': return &ffi_type_ushort;
        case 'i': return &ffi_type_sint;
        case 'I': return &ffi_type_uint;
        case 'l': return &ffi_type_slong;
        case 'L': return &ffi_type_ulong;
        #if MICROPY_PY_BUILTINS_FLOAT
        case 'f': return &ffi_type_float;
        case 'd': return &ffi_type_double;
        #endif
        case 'O': // mp_obj_t
        case 'C': // (*)()
        case 'P': // const void*
        case 'p': // void*
        case 's': return &ffi_type_pointer;
        case 'v': return &ffi_type_void;
        default: return NULL;
    }
}

STATIC ffi_type *get_ffi_type(mp_obj_t o_in)
{
    if (MP_OBJ_IS_STR(o_in)) {
        const char *s = mp_obj_str_get_str(o_in);
        ffi_type *t = char2ffi_type(*s);
        if (t != NULL) {
            return t;
        }
    }
    // TODO: Support actual libffi type objects

    mp_raise_TypeError("Unknown type");
}

STATIC mp_obj_t return_ffi_value(ffi_arg val, char type)
{
    switch (type) {
        case 's': {
            const char *s = (const char *)(intptr_t)val;
            if (!s) {
                return mp_const_none;
            }
            return mp_obj_new_str(s, strlen(s), false);
        }
        case 'v':
            return mp_const_none;
        #if MICROPY_PY_BUILTINS_FLOAT
        case 'f': {
            union { ffi_arg ffi; float flt; } val_union = { .ffi = val };
            return mp_obj_new_float(val_union.flt);
        }
        case 'd': {
            double *p = (double*)&val;
            return mp_obj_new_float(*p);
        }
        #endif
        case 'O':
            return (mp_obj_t)(intptr_t)val;
        default:
            return mp_obj_new_int(val);
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
    mp_obj_ffifunc_t *o = m_new_obj_var(mp_obj_ffifunc_t, ffi_type*, nparams);
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
        mp_raise_ValueError("Error in ffi_prep_cif");
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
    void *addr = (void*)MP_OBJ_TO_PTR(mp_obj_int_get_truncated(addr_in));
    return make_func(rettype, addr, argtypes);
}
MP_DEFINE_CONST_FUN_OBJ_3(mod_ffi_func_obj, mod_ffi_func);

STATIC void call_py_func(ffi_cif *cif, void *ret, void** args, void *func) {
    mp_obj_t pyargs[cif->nargs];
    for (uint i = 0; i < cif->nargs; i++) {
        pyargs[i] = mp_obj_new_int(*(mp_int_t*)args[i]);
    }
    mp_obj_t res = mp_call_function_n_kw(MP_OBJ_FROM_PTR(func), cif->nargs, 0, pyargs);

    if (res != mp_const_none) {
        *(ffi_arg*)ret = mp_obj_int_get_truncated(res);
    }
}

STATIC mp_obj_t mod_ffi_callback(mp_obj_t rettype_in, mp_obj_t func_in, mp_obj_t paramtypes_in) {
    const char *rettype = mp_obj_str_get_str(rettype_in);

    mp_int_t nparams = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(paramtypes_in));
    mp_obj_fficallback_t *o = m_new_obj_var(mp_obj_fficallback_t, ffi_type*, nparams);
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
        mp_raise_ValueError("Error in ffi_prep_cif");
    }

    res = ffi_prep_closure_loc(o->clo, &o->cif, call_py_func, MP_OBJ_TO_PTR(func_in), o->func);
    if (res != FFI_OK) {
        mp_raise_ValueError("ffi_prep_closure_loc");
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

STATIC mp_obj_t ffimod_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)n_args;
    (void)n_kw;

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
    .locals_dict = (mp_obj_dict_t*)&ffimod_locals_dict,
};

// FFI function

STATIC void ffifunc_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_ffifunc_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<ffifunc %p>", self->func);
}

STATIC mp_obj_t ffifunc_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_ffifunc_t *self = MP_OBJ_TO_PTR(self_in);
    assert(n_kw == 0);
    assert(n_args == self->cif.nargs);

    ffi_arg values[n_args];
    void *valueptrs[n_args];
    const char *argtype = self->argtypes;
    for (uint i = 0; i < n_args; i++, argtype++) {
        mp_obj_t a = args[i];
        if (*argtype == 'O') {
            values[i] = (ffi_arg)(intptr_t)a;
        #if MICROPY_PY_BUILTINS_FLOAT
        } else if (*argtype == 'f') {
            float *p = (float*)&values[i];
            *p = mp_obj_get_float(a);
        } else if (*argtype == 'd') {
            double *p = (double*)&values[i];
            *p = mp_obj_get_float(a);
        #endif
        } else if (a == mp_const_none) {
            values[i] = 0;
        } else if (MP_OBJ_IS_INT(a)) {
            values[i] = mp_obj_int_get_truncated(a);
        } else if (MP_OBJ_IS_STR(a)) {
            const char *s = mp_obj_str_get_str(a);
            values[i] = (ffi_arg)(intptr_t)s;
        } else if (((mp_obj_base_t*)MP_OBJ_TO_PTR(a))->type->buffer_p.get_buffer != NULL) {
            mp_obj_base_t *o = (mp_obj_base_t*)MP_OBJ_TO_PTR(a);
            mp_buffer_info_t bufinfo;
            int ret = o->type->buffer_p.get_buffer(MP_OBJ_FROM_PTR(o), &bufinfo, MP_BUFFER_READ); // TODO: MP_BUFFER_READ?
            if (ret != 0) {
                goto error;
            }
            values[i] = (ffi_arg)(intptr_t)bufinfo.buf;
        } else if (MP_OBJ_IS_TYPE(a, &fficallback_type)) {
            mp_obj_fficallback_t *p = MP_OBJ_TO_PTR(a);
            values[i] = (ffi_arg)(intptr_t)p->func;
        } else {
            goto error;
        }
        valueptrs[i] = &values[i];
    }

    // If ffi_arg is not big enough to hold a double, then we must pass along a
    // pointer to a memory location of the correct size.
    // TODO check if this needs to be done for other types which don't fit into
    // ffi_arg.
    #if MICROPY_PY_BUILTINS_FLOAT
    if (sizeof(ffi_arg) == 4 && self->rettype == 'd') {
        double retval;
        ffi_call(&self->cif, self->func, &retval, valueptrs);
        return mp_obj_new_float(retval);
    } else
    #endif
    {
        ffi_arg retval;
        ffi_call(&self->cif, self->func, &retval, valueptrs);
        return return_ffi_value(retval, self->rettype);
    }

error:
    mp_raise_TypeError("Don't know how to pass object to native function");
}

STATIC const mp_obj_type_t ffifunc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ffifunc,
    .print = ffifunc_print,
    .call = ffifunc_call,
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
    mp_printf(print, "<ffivar @%p: 0x%x>", self->var, *(int*)self->var);
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
    .locals_dict = (mp_obj_dict_t*)&ffivar_locals_dict,
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
    return ffimod_make_new(&ffimod_type, n_args, 0, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_ffi_open_obj, 1, 2, mod_ffi_open);

STATIC mp_obj_t mod_ffi_as_bytearray(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytearray_by_ref(mp_obj_int_get_truncated(size), (void*)(uintptr_t)mp_obj_int_get_truncated(ptr));
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
    .globals = (mp_obj_dict_t*)&mp_module_ffi_globals,
};
