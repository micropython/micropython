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

// Number of static entries at the start of proxy_c_ref.
#define PROXY_C_REF_NUM_STATIC (1)

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
    PROXY_KIND_JS_UNDEFINED = 0,
    PROXY_KIND_JS_NULL = 1,
    PROXY_KIND_JS_BOOLEAN = 2,
    PROXY_KIND_JS_INTEGER = 3,
    PROXY_KIND_JS_DOUBLE = 4,
    PROXY_KIND_JS_STRING = 5,
    PROXY_KIND_JS_OBJECT = 6,
    PROXY_KIND_JS_PYPROXY = 7,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_undefined,
    MP_QSTR_undefined,
    MP_TYPE_FLAG_NONE
    );

static const mp_obj_base_t mp_const_undefined_obj = {&mp_type_undefined};

#define mp_const_undefined (MP_OBJ_FROM_PTR(&mp_const_undefined_obj))

MP_DEFINE_EXCEPTION(JsException, Exception)

// Index to start searching for the next available slot in proxy_c_ref.
static size_t proxy_c_ref_next;

void proxy_c_init(void) {
    MP_STATE_PORT(proxy_c_ref) = mp_obj_new_list(0, NULL);
    mp_obj_list_append(MP_STATE_PORT(proxy_c_ref), MP_OBJ_NULL);
    proxy_c_ref_next = PROXY_C_REF_NUM_STATIC;
}

MP_REGISTER_ROOT_POINTER(mp_obj_t proxy_c_ref);

// obj cannot be MP_OBJ_NULL.
static inline size_t proxy_c_add_obj(mp_obj_t obj) {
    // Search for the first free slot in proxy_c_ref.
    mp_obj_list_t *l = (mp_obj_list_t *)MP_OBJ_TO_PTR(MP_STATE_PORT(proxy_c_ref));
    while (proxy_c_ref_next < l->len) {
        if (l->items[proxy_c_ref_next] == MP_OBJ_NULL) {
            // Free slot found, reuse it.
            size_t id = proxy_c_ref_next;
            ++proxy_c_ref_next;
            l->items[id] = obj;
            return id;
        }
        ++proxy_c_ref_next;
    }

    // No free slots, so grow proxy_c_ref by one (append at the end of the list).
    size_t id = l->len;
    mp_obj_list_append(MP_STATE_PORT(proxy_c_ref), obj);
    proxy_c_ref_next = l->len;
    return id;
}

static inline mp_obj_t proxy_c_get_obj(uint32_t c_ref) {
    return ((mp_obj_list_t *)MP_OBJ_TO_PTR(MP_STATE_PORT(proxy_c_ref)))->items[c_ref];
}

void proxy_c_free_obj(uint32_t c_ref) {
    if (c_ref >= PROXY_C_REF_NUM_STATIC) {
        ((mp_obj_list_t *)MP_OBJ_TO_PTR(MP_STATE_PORT(proxy_c_ref)))->items[c_ref] = MP_OBJ_NULL;
        proxy_c_ref_next = MIN(proxy_c_ref_next, c_ref);
    }
}

mp_obj_t proxy_convert_js_to_mp_obj_cside(uint32_t *value) {
    if (value[0] == PROXY_KIND_JS_UNDEFINED) {
        return mp_const_undefined;
    } else if (value[0] == PROXY_KIND_JS_NULL) {
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
    } else if (obj == mp_const_undefined) {
        kind = PROXY_KIND_MP_JSPROXY;
        out[1] = 1;
    } else if (mp_obj_is_jsproxy(obj)) {
        kind = PROXY_KIND_MP_JSPROXY;
        out[1] = mp_obj_jsproxy_get_ref(obj);
    } else if (mp_obj_get_type(obj) == &mp_type_JsException) {
        mp_obj_exception_t *exc = MP_OBJ_TO_PTR(obj);
        if (exc->args->len > 0 && mp_obj_is_jsproxy(exc->args->items[0])) {
            kind = PROXY_KIND_MP_JSPROXY;
            out[1] = mp_obj_jsproxy_get_ref(exc->args->items[0]);
        } else {
            kind = PROXY_KIND_MP_OBJECT;
            out[1] = proxy_c_add_obj(obj);
        }
    } else {
        if (mp_obj_is_callable(obj)) {
            kind = PROXY_KIND_MP_CALLABLE;
        } else if (mp_obj_is_type(obj, &mp_type_gen_instance)) {
            kind = PROXY_KIND_MP_GENERATOR;
        } else {
            kind = PROXY_KIND_MP_OBJECT;
        }
        out[1] = proxy_c_add_obj(obj);
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
    external_call_depth_inc();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t args[n_args];
        for (size_t i = 0; i < n_args; ++i) {
            args[i] = proxy_convert_js_to_mp_obj_cside(args_value + i * 3);
        }
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        mp_obj_t member = mp_call_function_n_kw(obj, n_args, 0, args);
        nlr_pop();
        external_call_depth_dec();
        proxy_convert_mp_to_js_obj_cside(member, out);
    } else {
        // uncaught exception
        external_call_depth_dec();
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
    }
}

void proxy_c_to_js_dir(uint32_t c_ref, uint32_t *out) {
    external_call_depth_inc();
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
        external_call_depth_dec();
        proxy_convert_mp_to_js_obj_cside(dir, out);
    } else {
        // uncaught exception
        external_call_depth_dec();
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
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
    external_call_depth_inc();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        qstr attr = qstr_from_str(attr_in);
        mp_obj_t member;
        if (mp_obj_is_dict_or_ordereddict(obj)) {
            // Lookup the requested attribute as a key in the target dict, and
            // return `undefined` if not found (instead of raising `KeyError`).
            mp_obj_dict_t *self = MP_OBJ_TO_PTR(obj);
            mp_map_elem_t *elem = mp_map_lookup(&self->map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
            if (elem == NULL) {
                member = mp_const_undefined;
            } else {
                member = elem->value;
            }
        } else {
            // Lookup the requested attribute as a member/method of the target object.
            member = mp_load_attr(obj, attr);
        }
        nlr_pop();
        external_call_depth_dec();
        proxy_convert_mp_to_js_obj_cside(member, out);
    } else {
        // uncaught exception
        external_call_depth_dec();
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
    }
}

static bool proxy_c_to_js_store_helper(uint32_t c_ref, const char *attr_in, uint32_t *value_in) {
    external_call_depth_inc();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        qstr attr = qstr_from_str(attr_in);

        mp_obj_t value = MP_OBJ_NULL;
        if (value_in != NULL) {
            value = proxy_convert_js_to_mp_obj_cside(value_in);
        }

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
        external_call_depth_dec();
        return true;
    } else {
        // uncaught exception
        external_call_depth_dec();
        return false;
    }
}

bool proxy_c_to_js_store_attr(uint32_t c_ref, const char *attr_in, uint32_t *value_in) {
    return proxy_c_to_js_store_helper(c_ref, attr_in, value_in);
}

bool proxy_c_to_js_delete_attr(uint32_t c_ref, const char *attr_in) {
    return proxy_c_to_js_store_helper(c_ref, attr_in, NULL);
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

EM_JS(void, js_get_error_info, (int jsref, uint32_t * out_name, uint32_t * out_message), {
    const error = proxy_js_ref[jsref];
    proxy_convert_js_to_mp_obj_jsside(error.name, out_name);
    proxy_convert_js_to_mp_obj_jsside(error.message, out_message);
});

mp_obj_t mp_obj_jsproxy_make_js_exception(mp_obj_t error) {
    uint32_t out_name[PVN];
    uint32_t out_message[PVN];
    js_get_error_info(mp_obj_jsproxy_get_ref(error), out_name, out_message);
    mp_obj_t args[3] = {
        error,
        proxy_convert_js_to_mp_obj_cside(out_name),
        proxy_convert_js_to_mp_obj_cside(out_message),
    };
    return mp_obj_new_exception_args(&mp_type_JsException, MP_ARRAY_SIZE(args), args);
}

/******************************************************************************/
// Bridge Python iterator to JavaScript iterator protocol.

uint32_t proxy_c_to_js_get_iter(uint32_t c_ref) {
    mp_obj_t obj = proxy_c_get_obj(c_ref);
    mp_obj_t iter = mp_getiter(obj, NULL);
    return proxy_c_add_obj(iter);
}

bool proxy_c_to_js_iternext(uint32_t c_ref, uint32_t *out) {
    external_call_depth_inc();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        mp_obj_t iter = mp_iternext_allow_raise(obj);
        if (iter == MP_OBJ_STOP_ITERATION) {
            external_call_depth_dec();
            nlr_pop();
            return false;
        }
        nlr_pop();
        external_call_depth_dec();
        proxy_convert_mp_to_js_obj_cside(iter, out);
        return true;
    } else {
        external_call_depth_dec();
        if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t *)nlr.ret_val)->type), MP_OBJ_FROM_PTR(&mp_type_StopIteration))) {
            return false;
        } else {
            // uncaught exception
            proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
            return true;
        }
    }
}

/******************************************************************************/
// Bridge Python generator to JavaScript thenable.

static const mp_obj_fun_builtin_var_t resume_obj;

EM_JS(void, js_then_resolve, (uint32_t * ret_value, uint32_t * resolve), {
    const ret_value_js = proxy_convert_mp_to_js_obj_jsside(ret_value);
    const resolve_js = proxy_convert_mp_to_js_obj_jsside(resolve);
    resolve_js(ret_value_js);
});

EM_JS(void, js_then_reject, (uint32_t * ret_value, uint32_t * reject), {
    // The ret_value object should be a Python exception.  Convert it to a
    // JavaScript PythonError and pass it as the reason to reject the promise.
    let ret_value_js;
    try {
        ret_value_js = proxy_convert_mp_to_js_obj_jsside(ret_value);
    } catch(error) {
        ret_value_js = error;
    }
    const reject_js = proxy_convert_mp_to_js_obj_jsside(reject);
    reject_js(ret_value_js);
});

// *FORMAT-OFF*
EM_JS(void, js_then_continue, (int jsref, uint32_t * py_resume, uint32_t * resolve, uint32_t * reject, uint32_t * out), {
    const py_resume_js = proxy_convert_mp_to_js_obj_jsside(py_resume);
    const resolve_js = proxy_convert_mp_to_js_obj_jsside(resolve);
    const reject_js = proxy_convert_mp_to_js_obj_jsside(reject);
    const ret = proxy_js_ref[jsref].then(
        (result) => {
            // The Promise is fulfilled on the JavaScript side.  Take the result and
            // send it to the encapsulating generator on the Python side, so it
            // becomes the result of the "yield from" that deferred to this Promise.
            py_resume_js(result, null, resolve_js, reject_js);
        },
        (reason) => {
            // The Promise is rejected on the JavaScript side.  Take the reason and
            // throw it into the encapsulating generator on the Python side.
            py_resume_js(null, reason, resolve_js, reject_js);
        },
    );
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});
// *FORMAT-ON*

EM_JS(void, create_promise, (uint32_t * out_set, uint32_t * out_promise), {
    const out_set_js = proxy_convert_mp_to_js_obj_jsside(out_set);
    const promise = new Promise(out_set_js);
    proxy_convert_js_to_mp_obj_jsside(promise, out_promise);
});

static mp_obj_t proxy_resume_execute(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t resolve, mp_obj_t reject) {
    if (throw_value != MP_OBJ_NULL && throw_value != mp_const_none) {
        if (send_value == mp_const_none) {
            send_value = MP_OBJ_NULL;
        }
        // Ensure that the `throw_value` is a proper Python exception instance.
        if (mp_obj_is_jsproxy(throw_value)) {
            throw_value = mp_obj_jsproxy_make_js_exception(throw_value);
        } else {
            throw_value = mp_make_raise_obj(throw_value);
        }
    } else {
        throw_value = MP_OBJ_NULL;
        if (send_value == mp_const_undefined) {
            send_value = mp_const_none;
        }
    }

    mp_obj_t ret_value;
    mp_vm_return_kind_t ret_kind = mp_resume(self_in, send_value, throw_value, &ret_value);

    if (ret_kind == MP_VM_RETURN_NORMAL) {
        uint32_t out_ret_value[PVN];
        uint32_t out_resolve[PVN];
        proxy_convert_mp_to_js_obj_cside(ret_value, out_ret_value);
        proxy_convert_mp_to_js_obj_cside(resolve, out_resolve);
        js_then_resolve(out_ret_value, out_resolve);
        return mp_const_none;
    } else if (ret_kind == MP_VM_RETURN_YIELD) {
        // If ret_value is None then there has been a top-level await of an asyncio primitive.
        // Otherwise, ret_value should be a JS thenable.

        if (ret_value == mp_const_none) {
            // Waiting on an asyncio primitive to complete, eg a Task or Event.
            //
            // Completion of this primitive will occur when the asyncio.core._top_level_task
            // Task is made runable and its coroutine's send() method is called.  Need to
            // construct a Promise that resolves when that send() method is called, because
            // that will resume the top-level await from the JavaScript side.
            //
            // This is accomplished via the asyncio.core.TopLevelCoro class and its methods.
            mp_obj_t asyncio = mp_import_name(MP_QSTR_asyncio_dot_core, mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
            mp_obj_t asyncio_core = mp_load_attr(asyncio, MP_QSTR_core);
            mp_obj_t top_level_coro = mp_load_attr(asyncio_core, MP_QSTR_TopLevelCoro);
            mp_obj_t top_level_coro_set = mp_load_attr(top_level_coro, MP_QSTR_set);
            uint32_t out_set[PVN];
            proxy_convert_mp_to_js_obj_cside(top_level_coro_set, out_set);
            uint32_t out_promise[PVN];
            create_promise(out_set, out_promise);
            ret_value = proxy_convert_js_to_mp_obj_cside(out_promise);
        }

        mp_obj_t py_resume = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&resume_obj), self_in);
        int ref = mp_obj_jsproxy_get_ref(ret_value);
        uint32_t out_py_resume[PVN];
        uint32_t out_resolve[PVN];
        uint32_t out_reject[PVN];
        proxy_convert_mp_to_js_obj_cside(py_resume, out_py_resume);
        proxy_convert_mp_to_js_obj_cside(resolve, out_resolve);
        proxy_convert_mp_to_js_obj_cside(reject, out_reject);
        uint32_t out[PVN];
        js_then_continue(ref, out_py_resume, out_resolve, out_reject, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    } else { // ret_kind == MP_VM_RETURN_EXCEPTION;
        // Pass the exception through as an object to reject the promise (don't raise/throw it).
        uint32_t out_ret_value[PVN];
        uint32_t out_reject[PVN];
        proxy_convert_mp_to_js_exc_cside(ret_value, out_ret_value);
        proxy_convert_mp_to_js_obj_cside(reject, out_reject);
        js_then_reject(out_ret_value, out_reject);
        return mp_const_none;
    }
}

static mp_obj_t resume_fun(size_t n_args, const mp_obj_t *args) {
    return proxy_resume_execute(args[0], args[1], args[2], args[3], args[4]);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(resume_obj, 5, 5, resume_fun);

void proxy_c_to_js_resume(uint32_t c_ref, uint32_t *args) {
    external_call_depth_inc();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t obj = proxy_c_get_obj(c_ref);
        mp_obj_t resolve = proxy_convert_js_to_mp_obj_cside(args + 1 * 3);
        mp_obj_t reject = proxy_convert_js_to_mp_obj_cside(args + 2 * 3);
        mp_obj_t ret = proxy_resume_execute(obj, mp_const_none, mp_const_none, resolve, reject);
        nlr_pop();
        external_call_depth_dec();
        proxy_convert_mp_to_js_obj_cside(ret, args);
    } else {
        // uncaught exception
        external_call_depth_dec();
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, args);
    }
}
