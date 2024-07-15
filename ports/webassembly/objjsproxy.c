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

#include <stdio.h>
#include <stdlib.h>

#include "emscripten.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "proxy_c.h"

EM_JS(bool, has_attr, (int jsref, const char *str), {
    const base = proxy_js_ref[jsref];
    const attr = UTF8ToString(str);
    if (attr in base) {
        return true;
    } else {
        return false;
    }
});

// *FORMAT-OFF*
EM_JS(bool, lookup_attr, (int jsref, const char *str, uint32_t * out), {
    const base = proxy_js_ref[jsref];
    const attr = UTF8ToString(str);

    // Attempt to lookup the requested attribute from the base object:
    // - If the value is not `undefined` then the attribute exists with that value.
    // - Otherwise if the value is `undefined` and the `in` operator returns true, then
    //   that attribute does exist and is intended to have a value of `undefined`.
    // - Otherwise, the attribute does not exist.
    let value = base[attr];
    if (value !== undefined || attr in base) {
        if (typeof value === "function") {
            if (base !== globalThis) {
                if ("_ref" in value) {
                    // This is a proxy of a Python function, it doesn't need
                    // binding.  And not binding it means if it's passed back
                    // to Python then it can be extracted from the proxy as a
                    // true Python function.
                } else {
                    // A function that is not a Python function.  Bind it.
                    value = value.bind(base);
                }
            }
        }
        proxy_convert_js_to_mp_obj_jsside(value, out);
        return true;
    } else {
        return false;
    }
});
// *FORMAT-ON*

EM_JS(void, store_attr, (int jsref, const char *attr_ptr, uint32_t * value_ref), {
    const attr = UTF8ToString(attr_ptr);
    const value = proxy_convert_mp_to_js_obj_jsside(value_ref);
    proxy_js_ref[jsref][attr] = value;
});

EM_JS(void, call0, (int f_ref, uint32_t * out), {
    // Because of JavaScript "this" semantics, we must extract the target function
    // to a variable before calling it, so "this" is bound to the correct value.
    //
    // In detail:
    // In JavaScript, proxy_js_ref[f_ref] acts like a function call
    // proxy_js_ref.at(f_ref), and "this" will be bound to proxy_js_ref if
    // there is a chain of calls, such as proxy_js_ref.at(f_ref)().
    // But proxy_js_ref is not "this" in the context of the call, so we
    // must extract the function to an independent variable and then call
    // that variable, so that "this" is correct (it will be "undefined").

    const f = proxy_js_ref[f_ref];
    const ret = f();
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(int, call1, (int f_ref, uint32_t * a0, uint32_t * out), {
    const a0_js = proxy_convert_mp_to_js_obj_jsside(a0);
    const f = proxy_js_ref[f_ref];
    const ret = f(a0_js);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(int, call2, (int f_ref, uint32_t * a0, uint32_t * a1, uint32_t * out), {
    const a0_js = proxy_convert_mp_to_js_obj_jsside(a0);
    const a1_js = proxy_convert_mp_to_js_obj_jsside(a1);
    const f = proxy_js_ref[f_ref];
    const ret = f(a0_js, a1_js);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(int, calln, (int f_ref, uint32_t n_args, uint32_t * value, uint32_t * out), {
    const f = proxy_js_ref[f_ref];
    const a = [];
    for (let i = 0; i < n_args; ++i) {
        const v = proxy_convert_mp_to_js_obj_jsside(value + i * 3 * 4);
        a.push(v);
    }
    const ret = f(... a);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(void, call0_kwarg, (int f_ref, uint32_t n_kw, uint32_t * key, uint32_t * value, uint32_t * out), {
    const f = proxy_js_ref[f_ref];
    const a = {};
    for (let i = 0; i < n_kw; ++i) {
        const k = UTF8ToString(getValue(key + i * 4, "i32"));
        const v = proxy_convert_mp_to_js_obj_jsside(value + i * 3 * 4);
        a[k] = v;
    }
    const ret = f(a);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(void, call1_kwarg, (int f_ref, uint32_t * arg0, uint32_t n_kw, uint32_t * key, uint32_t * value, uint32_t * out), {
    const f = proxy_js_ref[f_ref];
    const a0 = proxy_convert_mp_to_js_obj_jsside(arg0);
    const a = {};
    for (let i = 0; i < n_kw; ++i) {
        const k = UTF8ToString(getValue(key + i * 4, "i32"));
        const v = proxy_convert_mp_to_js_obj_jsside(value + i * 3 * 4);
        a[k] = v;
    }
    const ret = f(a0, a);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(void, js_reflect_construct, (int f_ref, uint32_t n_args, uint32_t * args, uint32_t * out), {
    const f = proxy_js_ref[f_ref];
    const as = [];
    for (let i = 0; i < n_args; ++i) {
        as.push(proxy_convert_mp_to_js_obj_jsside(args + i * 3 * 4));
    }
    const ret = Reflect.construct(f, as);
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(void, js_get_iter, (int f_ref, uint32_t * out), {
    const f = proxy_js_ref[f_ref];
    const ret = f[Symbol.iterator]();
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(bool, js_iter_next, (int f_ref, uint32_t * out), {
    const f = proxy_js_ref[f_ref];
    const ret = f.next();
    if (ret.done) {
        return false;
    } else {
        proxy_convert_js_to_mp_obj_jsside(ret.value, out);
        return true;
    }
});

EM_JS(void, js_subscr_load, (int f_ref, uint32_t * index_ref, uint32_t * out), {
    const target = proxy_js_ref[f_ref];
    const index = python_index_semantics(target, proxy_convert_mp_to_js_obj_jsside(index_ref));
    const ret = target[index];
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});

EM_JS(void, js_subscr_store, (int f_ref, uint32_t * idx, uint32_t * value), {
    const f = proxy_js_ref[f_ref];
    f[proxy_convert_mp_to_js_obj_jsside(idx)] = proxy_convert_mp_to_js_obj_jsside(value);
});

static void jsproxy_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<JsProxy %d>", self->ref);
}

static mp_obj_t jsproxy_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);

    if (n_kw == 0) {
        mp_arg_check_num(n_args, n_kw, 0, MP_OBJ_FUN_ARGS_MAX, false);
    } else {
        mp_arg_check_num(n_args, n_kw, 0, 1, true);
        uint32_t key[n_kw];
        uint32_t value[PVN * n_kw];
        for (int i = 0; i < n_kw; ++i) {
            key[i] = (uintptr_t)mp_obj_str_get_str(args[n_args + i * 2]);
            proxy_convert_mp_to_js_obj_cside(args[n_args + i * 2 + 1], &value[i * PVN]);
        }
        uint32_t out[3];
        if (n_args == 0) {
            call0_kwarg(self->ref, n_kw, key, value, out);
        } else {
            // n_args == 1
            uint32_t arg0[PVN];
            proxy_convert_mp_to_js_obj_cside(args[0], arg0);
            call1_kwarg(self->ref, arg0, n_kw, key, value, out);
        }
        return proxy_convert_js_to_mp_obj_cside(out);
    }

    if (n_args == 0) {
        uint32_t out[3];
        call0(self->ref, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    } else if (n_args == 1) {
        uint32_t arg0[PVN];
        uint32_t out[PVN];
        proxy_convert_mp_to_js_obj_cside(args[0], arg0);
        call1(self->ref, arg0, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    } else if (n_args == 2) {
        uint32_t arg0[PVN];
        proxy_convert_mp_to_js_obj_cside(args[0], arg0);
        uint32_t arg1[PVN];
        proxy_convert_mp_to_js_obj_cside(args[1], arg1);
        uint32_t out[3];
        call2(self->ref, arg0, arg1, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    } else {
        uint32_t value[PVN * n_args];
        for (int i = 0; i < n_args; ++i) {
            proxy_convert_mp_to_js_obj_cside(args[i], &value[i * PVN]);
        }
        uint32_t out[3];
        calln(self->ref, n_args, value, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    }
}

EM_JS(void, proxy_js_free_obj, (int js_ref), {
    if (js_ref >= PROXY_JS_REF_NUM_STATIC) {
        proxy_js_ref[js_ref] = undefined;
        if (js_ref < proxy_js_ref_next) {
            proxy_js_ref_next = js_ref;
        }
    }
});

static mp_obj_t jsproxy___del__(mp_obj_t self_in) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);
    proxy_js_free_obj(self->ref);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(jsproxy___del___obj, jsproxy___del__);

static mp_obj_t jsproxy_reflect_construct(size_t n_args, const mp_obj_t *args) {
    int arg0 = mp_obj_jsproxy_get_ref(args[0]);
    n_args -= 1;
    args += 1;
    uint32_t args_conv[n_args];
    for (unsigned int i = 0; i < n_args; ++i) {
        proxy_convert_mp_to_js_obj_cside(args[i], &args_conv[i * PVN]);
    }
    uint32_t out[PVN];
    js_reflect_construct(arg0, n_args, args_conv, out);
    return proxy_convert_js_to_mp_obj_cside(out);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(jsproxy_reflect_construct_obj, 1, jsproxy_reflect_construct);

static mp_obj_t jsproxy_subscr(mp_obj_t self_in, mp_obj_t index, mp_obj_t value) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == MP_OBJ_SENTINEL) {
        // Load subscript.
        uint32_t idx[PVN], out[PVN];
        proxy_convert_mp_to_js_obj_cside(index, idx);
        js_subscr_load(self->ref, idx, out);
        return proxy_convert_js_to_mp_obj_cside(out);
    } else if (value == MP_OBJ_NULL) {
        // Delete subscript.
        return MP_OBJ_NULL; // not supported
    } else {
        // Store subscript.
        uint32_t idx[PVN], val[PVN];
        proxy_convert_mp_to_js_obj_cside(index, idx);
        proxy_convert_mp_to_js_obj_cside(value, val);
        js_subscr_store(self->ref, idx, val);
        return mp_const_none;
    }
}

void mp_obj_jsproxy_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute.
        uint32_t out[PVN];
        if (attr == MP_QSTR___del__) {
            // For finaliser.
            dest[0] = MP_OBJ_FROM_PTR(&jsproxy___del___obj);
            dest[1] = self_in;
        } else if (lookup_attr(self->ref, qstr_str(attr), out)) {
            dest[0] = proxy_convert_js_to_mp_obj_cside(out);
        } else if (attr == MP_QSTR_new) {
            // Special case to handle construction of JS objects.
            // JS objects don't have a ".new" attribute, doing "Obj.new" is a Pyodide idiom for "new Obj".
            // It translates to the JavaScript "Reflect.construct(Obj, Array(...args))".
            dest[0] = MP_OBJ_FROM_PTR(&jsproxy_reflect_construct_obj);
            dest[1] = self_in;
        }
    } else if (dest[1] == MP_OBJ_NULL) {
        // Delete attribute.
    } else {
        // Store attribute.
        uint32_t value[PVN];
        proxy_convert_mp_to_js_obj_cside(dest[1], value);
        store_attr(self->ref, qstr_str(attr), value);
        dest[0] = MP_OBJ_NULL;
    }
}

/******************************************************************************/
// jsproxy iterator

typedef struct _jsproxy_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_jsproxy_t *iter;
} jsproxy_it_t;

static mp_obj_t jsproxy_it_iternext(mp_obj_t self_in) {
    jsproxy_it_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t out[3];
    if (js_iter_next(self->iter->ref, out)) {
        return proxy_convert_js_to_mp_obj_cside(out);
    } else {
        return MP_OBJ_STOP_ITERATION;
    }
}

static mp_obj_t jsproxy_new_it(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(jsproxy_it_t) <= sizeof(mp_obj_iter_buf_t));
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);
    jsproxy_it_t *o = (jsproxy_it_t *)iter_buf;
    o->base.type = &mp_type_polymorph_iter;
    o->iternext = jsproxy_it_iternext;
    uint32_t out[3];
    js_get_iter(self->ref, out);
    o->iter = proxy_convert_js_to_mp_obj_cside(out);
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
// jsproxy generator

enum {
    JSOBJ_GEN_STATE_WAITING,
    JSOBJ_GEN_STATE_COMPLETED,
    JSOBJ_GEN_STATE_EXHAUSTED,
};

typedef struct _jsproxy_gen_t {
    mp_obj_base_t base;
    mp_obj_t thenable;
    int state;
} jsproxy_gen_t;

mp_vm_return_kind_t jsproxy_gen_resume(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, mp_obj_t *ret_val) {
    jsproxy_gen_t *self = MP_OBJ_TO_PTR(self_in);

    if (throw_value) {
        *ret_val = throw_value;
        return MP_VM_RETURN_EXCEPTION;
    }

    switch (self->state) {
        case JSOBJ_GEN_STATE_WAITING:
            self->state = JSOBJ_GEN_STATE_COMPLETED;
            *ret_val = self->thenable;
            return MP_VM_RETURN_YIELD;

        case JSOBJ_GEN_STATE_COMPLETED:
            self->state = JSOBJ_GEN_STATE_EXHAUSTED;
            *ret_val = send_value;
            return MP_VM_RETURN_NORMAL;

        case JSOBJ_GEN_STATE_EXHAUSTED:
        default:
            // Trying to resume an already stopped generator.
            // This is an optimised "raise StopIteration(None)".
            *ret_val = mp_const_none;
            return MP_VM_RETURN_NORMAL;
    }
}

static mp_obj_t jsproxy_gen_resume_and_raise(mp_obj_t self_in, mp_obj_t send_value, mp_obj_t throw_value, bool raise_stop_iteration) {
    mp_obj_t ret;
    switch (jsproxy_gen_resume(self_in, send_value, throw_value, &ret)) {
        case MP_VM_RETURN_NORMAL:
        default:
            // A normal return is a StopIteration, either raise it or return
            // MP_OBJ_STOP_ITERATION as an optimisation.
            if (ret == mp_const_none) {
                ret = MP_OBJ_NULL;
            }
            if (raise_stop_iteration) {
                mp_raise_StopIteration(ret);
            } else {
                return mp_make_stop_iteration(ret);
            }

        case MP_VM_RETURN_YIELD:
            return ret;

        case MP_VM_RETURN_EXCEPTION:
            nlr_raise(ret);
    }
}

static mp_obj_t jsproxy_gen_instance_iternext(mp_obj_t self_in) {
    return jsproxy_gen_resume_and_raise(self_in, mp_const_none, MP_OBJ_NULL, false);
}

static mp_obj_t jsproxy_gen_instance_send(mp_obj_t self_in, mp_obj_t send_value) {
    return jsproxy_gen_resume_and_raise(self_in, send_value, MP_OBJ_NULL, true);
}
static MP_DEFINE_CONST_FUN_OBJ_2(jsproxy_gen_instance_send_obj, jsproxy_gen_instance_send);

static mp_obj_t jsproxy_gen_instance_throw(size_t n_args, const mp_obj_t *args) {
    // The signature of this function is: throw(type[, value[, traceback]])
    // CPython will pass all given arguments through the call chain and process them
    // at the point they are used (native generators will handle them differently to
    // user-defined generators with a throw() method).  To save passing multiple
    // values, MicroPython instead does partial processing here to reduce it down to
    // one argument and passes that through:
    // - if only args[1] is given, or args[2] is given but is None, args[1] is
    //   passed through (in the standard case it is an exception class or instance)
    // - if args[2] is given and not None it is passed through (in the standard
    //   case it would be an exception instance and args[1] its corresponding class)
    // - args[3] is always ignored

    mp_obj_t exc = args[1];
    if (n_args > 2 && args[2] != mp_const_none) {
        exc = args[2];
    }

    return jsproxy_gen_resume_and_raise(args[0], mp_const_none, exc, true);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(jsproxy_gen_instance_throw_obj, 2, 4, jsproxy_gen_instance_throw);

static mp_obj_t jsproxy_gen_instance_close(mp_obj_t self_in) {
    mp_obj_t ret;
    switch (jsproxy_gen_resume(self_in, mp_const_none, MP_OBJ_FROM_PTR(&mp_const_GeneratorExit_obj), &ret)) {
        case MP_VM_RETURN_YIELD:
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("generator ignored GeneratorExit"));

        // Swallow GeneratorExit (== successful close), and re-raise any other
        case MP_VM_RETURN_EXCEPTION:
            // ret should always be an instance of an exception class
            if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(mp_obj_get_type(ret)), MP_OBJ_FROM_PTR(&mp_type_GeneratorExit))) {
                return mp_const_none;
            }
            nlr_raise(ret);

        default:
            // The only choice left is MP_VM_RETURN_NORMAL which is successful close
            return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(jsproxy_gen_instance_close_obj, jsproxy_gen_instance_close);

static const mp_rom_map_elem_t jsproxy_gen_instance_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&jsproxy_gen_instance_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&jsproxy_gen_instance_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_throw), MP_ROM_PTR(&jsproxy_gen_instance_throw_obj) },
};
static MP_DEFINE_CONST_DICT(jsproxy_gen_instance_locals_dict, jsproxy_gen_instance_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_jsproxy_gen,
    MP_QSTR_generator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, jsproxy_gen_instance_iternext,
    locals_dict, &jsproxy_gen_instance_locals_dict
    );

static mp_obj_t jsproxy_new_gen(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    assert(sizeof(jsproxy_gen_t) <= sizeof(mp_obj_iter_buf_t));
    jsproxy_gen_t *o = (jsproxy_gen_t *)iter_buf;
    o->base.type = &mp_type_jsproxy_gen;
    o->thenable = self_in;
    o->state = JSOBJ_GEN_STATE_WAITING;
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/

#if MICROPY_PY_ASYNCIO
extern mp_obj_t mp_asyncio_context;
#endif

static mp_obj_t jsproxy_getiter(mp_obj_t self_in, mp_obj_iter_buf_t *iter_buf) {
    mp_obj_jsproxy_t *self = MP_OBJ_TO_PTR(self_in);
    if (has_attr(self->ref, "then")) {
        #if MICROPY_PY_ASYNCIO
        // When asyncio is running and the caller here is a task, wrap the JavaScript
        // thenable in a ThenableEvent, and get the task to wait on that event.  This
        // decouples the task from the thenable and allows cancelling the task.
        if (mp_asyncio_context != MP_OBJ_NULL) {
            mp_obj_t cur_task = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_cur_task));
            if (cur_task != mp_const_none) {
                mp_obj_t thenable_event_class = mp_obj_dict_get(mp_asyncio_context, MP_OBJ_NEW_QSTR(MP_QSTR_ThenableEvent));
                mp_obj_t thenable_event = mp_call_function_1(thenable_event_class, self_in);
                mp_obj_t dest[2];
                mp_load_method(thenable_event, MP_QSTR_wait, dest);
                mp_obj_t wait_gen = mp_call_method_n_kw(0, 0, dest);
                return mp_getiter(wait_gen, iter_buf);
            }
        }
        #endif
        return jsproxy_new_gen(self_in, iter_buf);
    } else {
        return jsproxy_new_it(self_in, iter_buf);
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_jsproxy,
    MP_QSTR_JsProxy,
    MP_TYPE_FLAG_ITER_IS_GETITER,
    print, jsproxy_print,
    call, jsproxy_call,
    attr, mp_obj_jsproxy_attr,
    subscr, jsproxy_subscr,
    iter, jsproxy_getiter
    );

mp_obj_t mp_obj_new_jsproxy(int ref) {
    mp_obj_jsproxy_t *o = mp_obj_malloc_with_finaliser(mp_obj_jsproxy_t, &mp_type_jsproxy);
    o->ref = ref;
    return MP_OBJ_FROM_PTR(o);
}
