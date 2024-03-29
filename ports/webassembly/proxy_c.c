/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023-2024 Damien P. George
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

#include <stdlib.h>
#include <string.h>

#include "emscripten.h"
#include "py/builtin.h"
#include "py/runtime.h"
#include "proxy_c.h"

// These constants should match the constants in proxy_js.js.

enum {
    PROXY_KIND_MP_EXCEPTION = -1,
    PROXY_KIND_MP_NULL = 0,
    PROXY_KIND_MP_NONE = 1,
    PROXY_KIND_MP_BOOL = 2,
    PROXY_KIND_MP_INT = 3,
    PROXY_KIND_MP_FLOAT = 4,
    PROXY_KIND_MP_STR = 5,
    PROXY_KIND_MP_CALLABLE = 6,
    PROXY_KIND_MP_GENERATOR = 7,
    PROXY_KIND_MP_OBJECT = 8,
    PROXY_KIND_MP_JSPROXY = 9,
};

enum {
    PROXY_KIND_JS_NULL = 1,
    PROXY_KIND_JS_BOOLEAN = 2,
    PROXY_KIND_JS_INTEGER = 3,
    PROXY_KIND_JS_DOUBLE = 4,
    PROXY_KIND_JS_STRING = 5,
    PROXY_KIND_JS_OBJECT = 6,
    PROXY_KIND_JS_PYPROXY = 7,
};

void proxy_c_init(void) {
    MP_STATE_PORT(proxy_c_ref) = mp_obj_new_list(0, NULL);
    mp_obj_list_append(MP_STATE_PORT(proxy_c_ref), MP_OBJ_NULL);
}

MP_REGISTER_ROOT_POINTER(mp_obj_t proxy_c_ref);

static inline mp_obj_t proxy_c_get_obj(uint32_t c_ref) {
    return ((mp_obj_list_t *)MP_OBJ_TO_PTR(MP_STATE_PORT(proxy_c_ref)))->items[c_ref];
}

mp_obj_t proxy_convert_js_to_mp_obj_cside(uint32_t *value) {
    if (value[0] == PROXY_KIND_JS_NULL) {
        return mp_const_none;
    } else if (value[0] == PROXY_KIND_JS_BOOLEAN) {
        return mp_obj_new_bool(value[1]);
    } else if (value[0] == PROXY_KIND_JS_INTEGER) {
        return mp_obj_new_int(value[1]);
    } else if (value[0] == PROXY_KIND_JS_DOUBLE) {
        return mp_obj_new_float_from_d(*(double *)&value[1]);
    } else if (value[0] == PROXY_KIND_JS_STRING) {
        mp_obj_t s = mp_obj_new_str((void *)value[2], value[1]);
        free((void *)value[2]);
        return s;
    } else if (value[0] == PROXY_KIND_JS_PYPROXY) {
        return proxy_c_get_obj(value[1]);
    } else {
        // PROXY_KIND_JS_OBJECT
        return mp_obj_new_jsproxy(value[1]);
    }
}

void proxy_convert_mp_to_js_obj_cside(mp_obj_t obj, uint32_t *out) {
    uint32_t kind;
    if (obj == MP_OBJ_NULL) {
        kind = PROXY_KIND_MP_NULL;
    } else if (obj == mp_const_none) {
        kind = PROXY_KIND_MP_NONE;
    } else if (mp_obj_is_bool(obj)) {
        kind = PROXY_KIND_MP_BOOL;
        out[1] = mp_obj_is_true(obj);
    } else if (mp_obj_is_int(obj)) {
        kind = PROXY_KIND_MP_INT;
        out[1] = mp_obj_get_int_truncated(obj); // TODO support big int
    } else if (mp_obj_is_float(obj)) {
        kind = PROXY_KIND_MP_FLOAT;
        *(double *)&out[1] = mp_obj_get_float(obj);
    } else if (mp_obj_is_str(obj)) {
        kind = PROXY_KIND_MP_STR;
        size_t len;
        const char *str = mp_obj_str_get_data(obj, &len);
        out[1] = len;
        out[2] = (uintptr_t)str;
    } else if (mp_obj_is_jsproxy(obj)) {
        kind = PROXY_KIND_MP_JSPROXY;
        out[1] = mp_obj_jsproxy_get_ref(obj);
    } else {
        if (mp_obj_is_callable(obj)) {
            kind = PROXY_KIND_MP_CALLABLE;
        } else if (mp_obj_is_type(obj, &mp_type_gen_instance)) {
            kind = PROXY_KIND_MP_GENERATOR;
        } else {
            kind = PROXY_KIND_MP_OBJECT;
        }
        size_t id = ((mp_obj_list_t *)MP_OBJ_TO_PTR(MP_STATE_PORT(proxy_c_ref)))->len;
        mp_obj_list_append(MP_STATE_PORT(proxy_c_ref), obj);
        out[1] = id;
    }
    out[0] = kind;
}

void proxy_convert_mp_to_js_exc_cside(void *exc, uint32_t *out) {
    out[0] = PROXY_KIND_MP_EXCEPTION;
    vstr_t vstr;
    mp_print_t print;
    vstr_init_print(&vstr, 64, &print);
    vstr_add_str(&vstr, qstr_str(mp_obj_get_type(MP_OBJ_FROM_PTR(exc))->name));
    vstr_add_char(&vstr, '\x04');
    mp_obj_print_exception(&print, MP_OBJ_FROM_PTR(exc));
    char *s = malloc(vstr_len(&vstr) + 1);
    memcpy(s, vstr_str(&vstr), vstr_len(&vstr));
    out[1] = vstr_len(&vstr);
    out[2] = (uintptr_t)s;
    vstr_clear(&vstr);
}

void proxy_c_to_js_call(uint32_t c_ref, uint32_t n_args, uint32_t *args_value, uint32_t *out) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t args[4] = { mp_const_none, mp_const_none, mp_const_none, mp_const_none };
        for (size_t i = 0; i < n_args; ++i) {
            args[i] = proxy_convert_js_to_mp_obj_cside(args_value + i * 3);
        }
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        mp_obj_t member = mp_call_function_n_kw(obj, n_args, 0, args);
        nlr_pop();
        proxy_convert_mp_to_js_obj_cside(member, out);
    } else {
        // uncaught exception
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
    }
}

void proxy_c_to_js_dir(uint32_t c_ref, uint32_t *out) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        mp_obj_t dir;
        if (mp_obj_is_dict_or_ordereddict(obj)) {
            mp_map_t *map = mp_obj_dict_get_map(obj);
            dir = mp_obj_new_list(0, NULL);
            for (size_t i = 0; i < map->alloc; i++) {
                if (mp_map_slot_is_filled(map, i)) {
                    mp_obj_list_append(dir, map->table[i].key);
                }
            }
        } else {
            mp_obj_t args[1] = { obj };
            dir = mp_builtin_dir_obj.fun.var(1, args);
        }
        nlr_pop();
        return proxy_convert_mp_to_js_obj_cside(dir, out);
    } else {
        // uncaught exception
        return proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
    }
}

bool proxy_c_to_js_has_attr(uint32_t c_ref, const char *attr_in) {
    mp_obj_t obj = proxy_c_get_obj(c_ref);
    qstr attr = qstr_from_str(attr_in);
    if (mp_obj_is_dict_or_ordereddict(obj)) {
        mp_map_t *map = mp_obj_dict_get_map(obj);
        mp_map_elem_t *elem = mp_map_lookup(map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        return elem != NULL;
    } else {
        mp_obj_t dest[2];
        mp_load_method_protected(obj, attr, dest, true);
        if (dest[0] != MP_OBJ_NULL) {
            return true;
        }
    }
    return false;
}

void proxy_c_to_js_lookup_attr(uint32_t c_ref, const char *attr_in, uint32_t *out) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        qstr attr = qstr_from_str(attr_in);
        mp_obj_t member;
        if (mp_obj_is_dict_or_ordereddict(obj)) {
            member = mp_obj_dict_get(obj, MP_OBJ_NEW_QSTR(attr));
        } else {
            member = mp_load_attr(obj, attr);
        }
        nlr_pop();
        return proxy_convert_mp_to_js_obj_cside(member, out);
    } else {
        // uncaught exception
        return proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
    }
}

static bool proxy_c_to_js_store_helper(uint32_t c_ref, const char *attr_in, mp_obj_t value) {
    mp_obj_t obj = proxy_c_get_obj(c_ref);
    qstr attr = qstr_from_str(attr_in);

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        if (mp_obj_is_dict_or_ordereddict(obj)) {
            if (value == MP_OBJ_NULL) {
                mp_obj_dict_delete(obj, MP_OBJ_NEW_QSTR(attr));
            } else {
                mp_obj_dict_store(obj, MP_OBJ_NEW_QSTR(attr), value);
            }
        } else {
            mp_store_attr(obj, attr, value);
        }
        nlr_pop();
        return true;
    } else {
        // uncaught exception
        return false;
    }
}

bool proxy_c_to_js_store_attr(uint32_t c_ref, const char *attr_in, uint32_t *value_in) {
    mp_obj_t value = proxy_convert_js_to_mp_obj_cside(value_in);
    return proxy_c_to_js_store_helper(c_ref, attr_in, value);
}

bool proxy_c_to_js_delete_attr(uint32_t c_ref, const char *attr_in) {
    return proxy_c_to_js_store_helper(c_ref, attr_in, MP_OBJ_NULL);
}

uint32_t proxy_c_to_js_get_type(uint32_t c_ref) {
    mp_obj_t obj = proxy_c_get_obj(c_ref);
    const mp_obj_type_t *type = mp_obj_get_type(obj);
    if (type == &mp_type_tuple) {
        return 1;
    } else if (type == &mp_type_list) {
        return 2;
    } else if (type == &mp_type_dict) {
        return 3;
    } else {
        return 4;
    }
}

void proxy_c_to_js_get_array(uint32_t c_ref, uint32_t *out) {
    mp_obj_t obj = proxy_c_get_obj(c_ref);
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(obj, &len, &items);
    out[0] = len;
    out[1] = (uintptr_t)items;
}

void proxy_c_to_js_get_dict(uint32_t c_ref, uint32_t *out) {
    mp_obj_t obj = proxy_c_get_obj(c_ref);
    mp_map_t *map = mp_obj_dict_get_map(obj);
    out[0] = map->alloc;
    out[1] = (uintptr_t)map->table;
}

/******************************************************************************/
// Bridge Python generator to JavaScript thenable.

static const mp_obj_fun_builtin_var_t resume_obj;

EM_JS(void, js_then_resolve, (uint32_t * ret_value, uint32_t * resolve, uint32_t * reject), {
    const ret_value_js = proxy_convert_mp_to_js_obj_jsside(ret_value);
    const resolve_js = proxy_convert_mp_to_js_obj_jsside(resolve);
    const reject_js = proxy_convert_mp_to_js_obj_jsside(reject);
    resolve_js(ret_value_js);
});

EM_JS(void, js_then_reject, (uint32_t * resolve, uint32_t * reject), {
    const resolve_js = proxy_convert_mp_to_js_obj_jsside(resolve);
    const reject_js = proxy_convert_mp_to_js_obj_jsside(reject);
    reject_js(null);
});

// *FORMAT-OFF*
EM_JS(void, js_then_continue, (int jsref, uint32_t * py_resume, uint32_t * resolve, uint32_t * reject, uint32_t * out), {
    const py_resume_js = proxy_convert_mp_to_js_obj_jsside(py_resume);
    const resolve_js = proxy_convert_mp_to_js_obj_jsside(resolve);
    const reject_js = proxy_convert_mp_to_js_obj_jsside(reject);
    const ret = proxy_js_ref[jsref].then((x) => {py_resume_js(x, resolve_js, reject_js);}, reject_js);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});
// *FORMAT-ON*

static mp_obj_t proxy_resume_execute(mp_obj_t self_in, mp_obj_t value, mp_obj_t resolve, mp_obj_t reject) {
    mp_obj_t ret_value;
    mp_vm_return_kind_t ret_kind = mp_resume(self_in, value, MP_OBJ_NULL, &ret_value);

    uint32_t out_resolve[PVN];
    uint32_t out_reject[PVN];
    proxy_convert_mp_to_js_obj_cside(resolve, out_resolve);
    proxy_convert_mp_to_js_obj_cside(reject, out_reject);

    if (ret_kind == MP_VM_RETURN_NORMAL) {
        uint32_t out_ret_value[PVN];
        proxy_convert_mp_to_js_obj_cside(ret_value, out_ret_value);
        js_then_resolve(out_ret_value, out_resolve, out_reject);
        return mp_const_none;
    } else if (ret_kind == MP_VM_RETURN_YIELD) {
        // ret_value should be a JS thenable
        mp_obj_t py_resume = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&resume_obj), self_in);
        int ref = mp_obj_jsproxy_get_ref(ret_value);
        uint32_t out_py_resume[PVN];
        proxy_convert_mp_to_js_obj_cside(py_resume, out_py_resume);
        uint32_t out[PVN];
        js_then_continue(ref, out_py_resume, out_resolve, out_reject, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    } else {
        // MP_VM_RETURN_EXCEPTION;
        js_then_reject(out_resolve, out_reject);
        nlr_raise(ret_value);
    }
}

static mp_obj_t resume_fun(size_t n_args, const mp_obj_t *args) {
    return proxy_resume_execute(args[0], args[1], args[2], args[3]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(resume_obj, 4, 4, resume_fun);

void proxy_c_to_js_resume(uint32_t c_ref, uint32_t *args) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        mp_obj_t resolve = proxy_convert_js_to_mp_obj_cside(args + 1 * 3);
        mp_obj_t reject = proxy_convert_js_to_mp_obj_cside(args + 2 * 3);
        mp_obj_t ret = proxy_resume_execute(obj, mp_const_none, resolve, reject);
        nlr_pop();
        return proxy_convert_mp_to_js_obj_cside(ret, args);
    } else {
        // uncaught exception
        return proxy_convert_mp_to_js_exc_cside(nlr.ret_val, args);
    }
}
