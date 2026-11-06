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

#include "emscripten.h"
#include "py/gc.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "proxy_c.h"

#if MICROPY_PY_JSFFI

/******************************************************************************/
// jsffi module

EM_JS(void, proxy_convert_mp_to_js_then_js_to_mp_obj_jsside, (uint32_t * out), {
    const ret = proxy_convert_mp_to_js_obj_jsside(out);
    proxy_convert_js_to_mp_obj_jsside_force_double_proxy(ret, out);
});

static mp_obj_t mp_jsffi_create_proxy(mp_obj_t arg) {
    uint32_t out[3];
    proxy_convert_mp_to_js_obj_cside(arg, out);
    proxy_convert_mp_to_js_then_js_to_mp_obj_jsside(out);
    return proxy_convert_js_to_mp_obj_cside(out);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_jsffi_create_proxy_obj, mp_jsffi_create_proxy);


EM_JS(void, proxy_convert_mp_to_js_then_js_to_js_then_js_to_mp_obj_jsside, (uint32_t * out), {
    const ret = proxy_convert_mp_to_js_obj_jsside(out);
    const js_obj = PyProxy.toJs(ret);
    proxy_convert_js_to_mp_obj_jsside(js_obj, out);
});

static mp_obj_t mp_jsffi_to_js(mp_obj_t arg) {
    uint32_t out[3];
    proxy_convert_mp_to_js_obj_cside(arg, out);
    proxy_convert_mp_to_js_then_js_to_js_then_js_to_mp_obj_jsside(out);
    return proxy_convert_js_to_mp_obj_cside(out);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_jsffi_to_js_obj, mp_jsffi_to_js);

// *FORMAT-OFF*
EM_JS(void, js_get_proxy_js_ref_info, (uint32_t * out), {
    let used = 0;
    for (const elem of proxy_js_ref) {
        if (elem !== undefined) {
            ++used;
        }
    }
    Module.setValue(out, proxy_js_ref.length, "i32");
    Module.setValue(out + 4, used, "i32");
});
// *FORMAT-ON*

static mp_obj_t mp_jsffi_mem_info(void) {
    mp_obj_list_t *l = (mp_obj_list_t *)MP_OBJ_TO_PTR(MP_STATE_PORT(proxy_c_ref));
    mp_int_t used = 0;
    for (size_t i = 0; i < l->len; ++i) {
        if (l->items[i] != MP_OBJ_NULL) {
            ++used;
        }
    }
    uint32_t proxy_js_ref_info[2];
    js_get_proxy_js_ref_info(proxy_js_ref_info);
    gc_info_t info;
    gc_info(&info);
    mp_obj_t elems[] = {
        MP_OBJ_NEW_SMALL_INT(info.total), // GC heap total bytes
        MP_OBJ_NEW_SMALL_INT(info.used), // GC heap used bytes
        MP_OBJ_NEW_SMALL_INT(info.free), // GC heap free bytes
        MP_OBJ_NEW_SMALL_INT(l->len), // proxy_c_ref allocated size
        MP_OBJ_NEW_SMALL_INT(used), // proxy_c_ref used
        MP_OBJ_NEW_SMALL_INT(proxy_js_ref_info[0]), // proxy_js_ref allocated size
        MP_OBJ_NEW_SMALL_INT(proxy_js_ref_info[1]), // proxy_js_ref used
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(elems), elems);
}
static MP_DEFINE_CONST_FUN_OBJ_0(mp_jsffi_mem_info_obj, mp_jsffi_mem_info);

/******************************************************************************/
// jsffi.run_sync / jsffi.can_run_sync
//
// Block synchronous-looking Python on a JS promise (or any awaitable)
// by suspending the promising entry via JSPI, mirroring Pyodide's
// pyodide.ffi.run_sync / can_run_sync API.  Both functions exist on
// every build so that feature-detection code is portable; without
// JSPI, run_sync() raises RuntimeError and can_run_sync() is False.

#if MICROPY_JSPI

// Outcome of js_run_sync_wait(), below.
enum {
    RUN_SYNC_RESOLVED = 0,
    RUN_SYNC_REJECTED = 1,
    RUN_SYNC_NOT_AWAITABLE = 2,
    RUN_SYNC_INTERRUPTED = 3,
    RUN_SYNC_ERROR = 4,
};

// *FORMAT-OFF*

// Suspend until the object in "value" is fulfilled or rejected, or a
// KeyboardInterrupt arrives.  Under -sJSPI an EM_ASYNC_JS function is a
// WebAssembly.Suspending import: the engine parks this frame and resumes
// it when the awaited promise is done, so nothing polls in between.  The
// interrupt arrives through Module.jspiInterrupt, a promise created here
// and fulfilled by api.js when a KeyboardInterrupt arrives.  Returns a
// RUN_SYNC_* code.  When the object was fulfilled or rejected, its value
// or rejection reason is written back into "value".
EM_ASYNC_JS(int, js_run_sync_wait, (uint32_t * value), {
    // Everything is inside try/catch: a JS exception thrown from here
    // would pass straight through the C frames, skipping their clean-up,
    // so it is reported as a status code instead.
    try {
        const obj = proxy_convert_mp_to_js_obj_jsside(value);
        if (!obj || typeof obj.then !== "function") {
            return 2; // RUN_SYNC_NOT_AWAITABLE
        }
        // Create a promise to be resolved if a KeyboardInterrupt arrives.
        const interrupt = {};
        interrupt.promise = new Promise((resolve) => {
            interrupt.resolve = resolve;
        });
        Module.jspiInterrupt = interrupt;
        // Promise.race takes the outcome of whichever promise finishes
        // first and ignores the rest.  Each input is wrapped with .then
        // so a win is always a plain { status, result } object: the
        // awaited object's rejection becomes status 1 rather than a
        // throw, and the interrupt gives status 3 with no result.  If
        // the interrupt wins, the awaited promise carries on in the
        // background and its eventual outcome is dropped.
        const outcome = await Promise.race([
            Promise.resolve(obj).then(
                (result) => ({ status: 0, result }), // RUN_SYNC_RESOLVED
                (error) => ({ status: 1, result: error }), // RUN_SYNC_REJECTED
            ),
            interrupt.promise.then(() => ({ status: 3 })), // RUN_SYNC_INTERRUPTED
        ]);
        if (outcome.status !== 3) {
            proxy_convert_js_to_mp_obj_jsside(outcome.result, value);
        }
        return outcome.status;
    } catch (error) {
        proxy_convert_js_to_mp_obj_jsside(error, value);
        return 4; // RUN_SYNC_ERROR
    } finally {
        // Clear the interrupt promise.
        Module.jspiInterrupt = null;
    }
});

// *FORMAT-ON*

static mp_obj_t mp_jsffi_run_sync(mp_obj_t arg) {
    if (!mp_js_can_suspend()) {
        if (mp_js_suspension_active()) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("run_sync: another run_sync is already pending"));
        }
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("run_sync: no suspender in this context"));
    }

    // Convert the argument to its JS form: a JsProxy unwraps to the
    // underlying JS object, and a Python generator/coroutine becomes a
    // PyProxyThenable, so both are adopted uniformly by Promise.resolve
    // on the JS side.
    uint32_t value[PVN];
    proxy_convert_mp_to_js_obj_cside(arg, value);

    // Suspend here until the promise is fulfilled or rejected, or a
    // KeyboardInterrupt arrives.  js_run_sync_wait() catches every JS
    // exception and reports it as a status code, and the other two
    // calls cannot raise, so suspension_end() always runs and the
    // "already pending" flag can never be left set.
    mp_js_suspension_begin();
    int status = js_run_sync_wait(value);
    mp_js_suspension_end();

    if (status == RUN_SYNC_NOT_AWAITABLE) {
        mp_raise_TypeError(MP_ERROR_TEXT("object is not awaitable"));
    }
    if (status == RUN_SYNC_INTERRUPTED) {
        // A KeyboardInterrupt arrived while suspended.  In that case
        // api.js only wakes the wait and does not queue the
        // KeyboardInterrupt, so queue and raise it here.
        mp_sched_keyboard_interrupt();
        mp_handle_pending(true);
    }

    mp_obj_t result = proxy_convert_js_to_mp_obj_cside(value);
    if (status == RUN_SYNC_REJECTED || status == RUN_SYNC_ERROR) {
        // The promise rejected, or the wait itself threw a JS exception:
        // raise the reason as Python sees it.
        if (mp_obj_get_type(result) == &mp_type_jsproxy) {
            nlr_raise(mp_obj_jsproxy_make_js_exception(result));
        }
        if (mp_obj_is_exception_instance(result)) {
            nlr_raise(result);
        }
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("run_sync: promise rejected"));
    }
    return result;
}

#else // !MICROPY_JSPI

static mp_obj_t mp_jsffi_run_sync(mp_obj_t arg) {
    (void)arg;
    mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("WebAssembly stack switching not supported in this build"));
}

#endif // MICROPY_JSPI

static MP_DEFINE_CONST_FUN_OBJ_1(mp_jsffi_run_sync_obj, mp_jsffi_run_sync);

static mp_obj_t mp_jsffi_can_run_sync(void) {
    #if MICROPY_JSPI
    return mp_obj_new_bool(mp_js_can_suspend());
    #else
    return mp_const_false;
    #endif
}
static MP_DEFINE_CONST_FUN_OBJ_0(mp_jsffi_can_run_sync_obj, mp_jsffi_can_run_sync);

static const mp_rom_map_elem_t mp_module_jsffi_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jsffi) },

    { MP_ROM_QSTR(MP_QSTR_JsProxy), MP_ROM_PTR(&mp_type_jsproxy) },
    { MP_ROM_QSTR(MP_QSTR_JsException), MP_ROM_PTR(&mp_type_JsException) },
    { MP_ROM_QSTR(MP_QSTR_create_proxy), MP_ROM_PTR(&mp_jsffi_create_proxy_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_js), MP_ROM_PTR(&mp_jsffi_to_js_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem_info), MP_ROM_PTR(&mp_jsffi_mem_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_run_sync), MP_ROM_PTR(&mp_jsffi_run_sync_obj) },
    { MP_ROM_QSTR(MP_QSTR_can_run_sync), MP_ROM_PTR(&mp_jsffi_can_run_sync_obj) },
};
static MP_DEFINE_CONST_DICT(mp_module_jsffi_globals, mp_module_jsffi_globals_table);

const mp_obj_module_t mp_module_jsffi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_jsffi_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jsffi, mp_module_jsffi);

#endif // MICROPY_PY_JSFFI
