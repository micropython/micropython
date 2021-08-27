/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "py/objexcept.h"
#include "py/objstr.h"
#include "py/parsenum.h"
#include "py/runtime.h"
#include "shared-bindings/ssl/__init__.h"
#include "shared-bindings/ssl/SSLContext.h"

//| """
//| The `ssl` module provides SSL contexts to wrap sockets in.
//| """
//|

//| def create_default_context() -> ssl.SSLContext:
//|     """Return the default SSLContext."""
//|     ...
//|

STATIC mp_obj_t ssl_create_default_context(void) {
    ssl_sslcontext_obj_t *s = m_new_obj(ssl_sslcontext_obj_t);
    s->base.type = &ssl_sslcontext_type;

    common_hal_ssl_create_default_context(s);
    return s;
}
MP_DEFINE_CONST_FUN_OBJ_0(ssl_create_default_context_obj, ssl_create_default_context);

STATIC const mp_rom_map_elem_t ssl_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ssl) },

    { MP_ROM_QSTR(MP_QSTR_create_default_context), MP_ROM_PTR(&ssl_create_default_context_obj) },

    { MP_ROM_QSTR(MP_QSTR_SSLContext), MP_ROM_PTR(&ssl_sslcontext_type) },
};

STATIC MP_DEFINE_CONST_DICT(ssl_globals, ssl_globals_table);

const mp_obj_module_t ssl_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ssl_globals,
};
