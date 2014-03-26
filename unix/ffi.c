#include <assert.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <ffi.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "map.h"
#include "runtime.h"
#include "binary.h"

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

//static const mp_obj_type_t opaque_type;
static const mp_obj_type_t ffimod_type;
static const mp_obj_type_t ffifunc_type;
static const mp_obj_type_t fficallback_type;
static const mp_obj_type_t ffivar_type;

static ffi_type *char2ffi_type(char c)
{
    switch (c) {
        case 'b': return &ffi_type_schar;
        case 'B': return &ffi_type_uchar;
        case 'i': return &ffi_type_sint;
        case 'I': return &ffi_type_uint;
        case 'l': return &ffi_type_slong;
        case 'L': return &ffi_type_ulong;
        case 'p':
        case 's': return &ffi_type_pointer;
        case 'v': return &ffi_type_void;
        default: return NULL;
    }
}

static ffi_type *get_ffi_type(mp_obj_t o_in)
{
    if (MP_OBJ_IS_STR(o_in)) {
        uint len;
        const char *s = mp_obj_str_get_data(o_in, &len);
        ffi_type *t = char2ffi_type(*s);
        if (t != NULL) {
            return t;
        }
    }
    // TODO: Support actual libffi type objects

    nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Unknown type"));
}

static mp_obj_t return_ffi_value(ffi_arg val, char type)
{
    switch (type) {
        case 's': {
            const char *s = (const char *)val;
            return mp_obj_new_str((const byte *)s, strlen(s), false);
        }
        case 'v':
            return mp_const_none;
        default:
            return mp_obj_new_int(val);
    }
}

// FFI module

static void ffimod_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_ffimod_t *self = self_in;
    print(env, "<ffimod %p>", self->handle);
}

static mp_obj_t ffimod_close(mp_obj_t self_in) {
    mp_obj_ffimod_t *self = self_in;
    dlclose(self->handle);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(ffimod_close_obj, ffimod_close);

static mp_obj_t ffimod_func(uint n_args, const mp_obj_t *args) {
    mp_obj_ffimod_t *self = args[0];
    const char *rettype = mp_obj_str_get_str(args[1]);
    const char *symname = mp_obj_str_get_str(args[2]);

    void *sym = dlsym(self->handle, symname);
    if (sym == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", errno));
    }
    int nparams = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(args[3]));
    mp_obj_ffifunc_t *o = m_new_obj_var(mp_obj_ffifunc_t, ffi_type*, nparams);
    o->base.type = &ffifunc_type;

    o->func = sym;
    o->rettype = *rettype;

    mp_obj_t iterable = rt_getiter(args[3]);
    mp_obj_t item;
    int i = 0;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        o->params[i++] = get_ffi_type(item);
    }

    int res = ffi_prep_cif(&o->cif, FFI_DEFAULT_ABI, nparams, char2ffi_type(*rettype), o->params);
    if (res != FFI_OK) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error in ffi_prep_cif"));
    }

    return o;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ffimod_func_obj, 4, 4, ffimod_func);

static void call_py_func(ffi_cif *cif, void *ret, void** args, mp_obj_t func) {
    mp_obj_t pyargs[cif->nargs];
    for (int i = 0; i < cif->nargs; i++) {
        pyargs[i] = mp_obj_new_int(*(int*)args[i]);
    }
    mp_obj_t res = rt_call_function_n_kw(func, cif->nargs, 0, pyargs);

    *(ffi_arg*)ret = mp_obj_int_get(res);
}

static mp_obj_t mod_ffi_callback(mp_obj_t rettype_in, mp_obj_t func_in, mp_obj_t paramtypes_in) {
    const char *rettype = mp_obj_str_get_str(rettype_in);

    int nparams = MP_OBJ_SMALL_INT_VALUE(mp_obj_len_maybe(paramtypes_in));
    mp_obj_fficallback_t *o = m_new_obj_var(mp_obj_fficallback_t, ffi_type*, nparams);
    o->base.type = &fficallback_type;

    o->clo = ffi_closure_alloc(sizeof(ffi_closure), &o->func);

    o->rettype = *rettype;

    mp_obj_t iterable = rt_getiter(paramtypes_in);
    mp_obj_t item;
    int i = 0;
    while ((item = rt_iternext(iterable)) != mp_const_stop_iteration) {
        o->params[i++] = get_ffi_type(item);
    }

    int res = ffi_prep_cif(&o->cif, FFI_DEFAULT_ABI, nparams, char2ffi_type(*rettype), o->params);
    if (res != FFI_OK) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error in ffi_prep_cif"));
    }

    res = ffi_prep_closure_loc(o->clo, &o->cif, call_py_func, func_in, o->func);
    if (res != FFI_OK) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "ffi_prep_closure_loc"));
    }

    return o;
}
MP_DEFINE_CONST_FUN_OBJ_3(mod_ffi_callback_obj, mod_ffi_callback);

static mp_obj_t ffimod_var(mp_obj_t self_in, mp_obj_t vartype_in, mp_obj_t symname_in) {
    mp_obj_ffimod_t *self = self_in;
    const char *rettype = mp_obj_str_get_str(vartype_in);
    const char *symname = mp_obj_str_get_str(symname_in);

    void *sym = dlsym(self->handle, symname);
    if (sym == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", errno));
    }
    mp_obj_ffivar_t *o = m_new_obj(mp_obj_ffivar_t);
    o->base.type = &ffivar_type;

    o->var = sym;
    o->type = *rettype;
    return o;
}
MP_DEFINE_CONST_FUN_OBJ_3(ffimod_var_obj, ffimod_var);

static mp_obj_t ffimod_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    const char *fname = mp_obj_str_get_str(args[0]);
    void *mod = dlopen(fname, RTLD_NOW | RTLD_LOCAL);

    if (mod == NULL) {
        nlr_jump(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", errno));
    }
    mp_obj_ffimod_t *o = m_new_obj(mp_obj_ffimod_t);
    o->base.type = type_in;
    o->handle = mod;
    return o;
}

static const mp_map_elem_t ffimod_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_func), (mp_obj_t) &ffimod_func_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_var), (mp_obj_t) &ffimod_var_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t) &ffimod_close_obj },
};

STATIC MP_DEFINE_CONST_DICT(ffimod_locals_dict, ffimod_locals_dict_table);

static const mp_obj_type_t ffimod_type = {
    { &mp_type_type },
    .name = MP_QSTR_ffimod,
    .print = ffimod_print,
    .make_new = ffimod_make_new,
    .locals_dict = (mp_obj_t)&ffimod_locals_dict,
};

// FFI function

static void ffifunc_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_ffifunc_t *self = self_in;
    print(env, "<ffifunc %p>", self->func);
}

mp_obj_t ffifunc_call(mp_obj_t self_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_ffifunc_t *self = self_in;
    assert(n_kw == 0);
    assert(n_args == self->cif.nargs);

    ffi_arg values[n_args];
    void *valueptrs[n_args];
    int i;
    for (i = 0; i < n_args; i++) {
        mp_obj_t a = args[i];
        if (a == mp_const_none) {
            values[i] = 0;
        } else if (MP_OBJ_IS_INT(a)) {
            values[i] = mp_obj_int_get(a);
        } else if (MP_OBJ_IS_STR(a) || MP_OBJ_IS_TYPE(a, &bytes_type)) {
            const char *s = mp_obj_str_get_str(a);
            values[i] = (ffi_arg)s;
        } else if (MP_OBJ_IS_TYPE(a, &fficallback_type)) {
            mp_obj_fficallback_t *p = a;
            values[i] = (ffi_arg)p->func;
        } else {
            assert(0);
        }
        valueptrs[i] = &values[i];
    }

    ffi_arg retval;
    ffi_call(&self->cif, self->func, &retval, valueptrs);
    return return_ffi_value(retval, self->rettype);
}

static const mp_obj_type_t ffifunc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ffifunc,
    .print = ffifunc_print,
    .call = ffifunc_call,
};

// FFI callback for Python function

static void fficallback_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_fficallback_t *self = self_in;
    print(env, "<fficallback %p>", self->func);
}

static const mp_obj_type_t fficallback_type = {
    { &mp_type_type },
    .name = MP_QSTR_fficallback,
    .print = fficallback_print,
};

// FFI variable

static void ffivar_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_ffivar_t *self = self_in;
    print(env, "<ffivar @%p: 0x%x>", self->var, *(int*)self->var);
}

static mp_obj_t ffivar_get(mp_obj_t self_in) {
    mp_obj_ffivar_t *self = self_in;
    return mp_binary_get_val(self->type, self->var, 0);
}
MP_DEFINE_CONST_FUN_OBJ_1(ffivar_get_obj, ffivar_get);

static mp_obj_t ffivar_set(mp_obj_t self_in, mp_obj_t val_in) {
    mp_obj_ffivar_t *self = self_in;
    mp_binary_set_val(self->type, self->var, 0, val_in);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(ffivar_set_obj, ffivar_set);

static const mp_method_t ffivar_type_methods[] = {
        { "get", &ffivar_get_obj },
        { "set", &ffivar_set_obj },
        { NULL, NULL },
};

static const mp_obj_type_t ffivar_type = {
    { &mp_type_type },
    .name = MP_QSTR_ffivar,
    .print = ffivar_print,
    .methods = ffivar_type_methods,
};

// Generic opaque storage object (unused)

/*
static const mp_obj_type_t opaque_type = {
    { &mp_type_type },
    .name = MP_QSTR_opaqueval,
//    .print = opaque_print,
};
*/

mp_obj_t mod_ffi_open(uint n_args, const mp_obj_t *args) {
    return ffimod_make_new((mp_obj_t)&ffimod_type, n_args, 0, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_ffi_open_obj, 1, 2, mod_ffi_open);

mp_obj_t mod_ffi_as_bytearray(mp_obj_t ptr, mp_obj_t size) {
    return mp_obj_new_bytearray_by_ref(mp_obj_int_get(size), (void*)mp_obj_int_get(ptr));
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_ffi_as_bytearray_obj, mod_ffi_as_bytearray);


void ffi_init() {
    mp_obj_t m = mp_obj_new_module(QSTR_FROM_STR_STATIC("ffi"));
    rt_store_attr(m, MP_QSTR_open, (mp_obj_t)&mod_ffi_open_obj);
    rt_store_attr(m, QSTR_FROM_STR_STATIC("callback"), (mp_obj_t)&mod_ffi_callback_obj);
    // there would be as_bytes, but bytes currently is value, not reference type!
    rt_store_attr(m, QSTR_FROM_STR_STATIC("as_bytearray"), (mp_obj_t)&mod_ffi_as_bytearray_obj);
}
