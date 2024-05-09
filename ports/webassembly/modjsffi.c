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
EM_JS(void, promise_with_timeout_ms, (double ms, uint32_t * out), {
    const ret = new Promise((resolve) => setTimeout(resolve, ms));
    proxy_convert_js_to_mp_obj_jsside(ret, out);
});
// *FORMAT-ON*

static mp_obj_t mp_jsffi_async_timeout_ms(mp_obj_t arg) {
    uint32_t out[PVN];
    promise_with_timeout_ms(mp_obj_get_float_to_d(arg), out);
    return proxy_convert_js_to_mp_obj_cside(out);
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_jsffi_async_timeout_ms_obj, mp_jsffi_async_timeout_ms);

static const mp_rom_map_elem_t mp_module_jsffi_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jsffi) },

    { MP_ROM_QSTR(MP_QSTR_JsProxy), MP_ROM_PTR(&mp_type_jsproxy) },
    { MP_ROM_QSTR(MP_QSTR_JsException), MP_ROM_PTR(&mp_type_JsException) },
    { MP_ROM_QSTR(MP_QSTR_create_proxy), MP_ROM_PTR(&mp_jsffi_create_proxy_obj) },
    { MP_ROM_QSTR(MP_QSTR_to_js), MP_ROM_PTR(&mp_jsffi_to_js_obj) },
    { MP_ROM_QSTR(MP_QSTR_async_timeout_ms), MP_ROM_PTR(&mp_jsffi_async_timeout_ms_obj) },
};
static MP_DEFINE_CONST_DICT(mp_module_jsffi_globals, mp_module_jsffi_globals_table);

const mp_obj_module_t mp_module_jsffi = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_jsffi_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jsffi, mp_module_jsffi);

#endif // MICROPY_PY_JSFFI
