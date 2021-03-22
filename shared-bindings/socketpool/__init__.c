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
#include "shared-bindings/socketpool/__init__.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/socketpool/SocketPool.h"

//| """
//| The `socketpool` module provides sockets through a pool. The pools themselves
//| act like CPython's `socket` module.
//| """
//|

STATIC const mp_rom_map_elem_t socketpool_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_socketpool) },

    { MP_ROM_QSTR(MP_QSTR_SocketPool), MP_ROM_PTR(&socketpool_socketpool_type) },
    { MP_ROM_QSTR(MP_QSTR_Socket), MP_ROM_PTR(&socketpool_socket_type) },
};

STATIC MP_DEFINE_CONST_DICT(socketpool_globals, socketpool_globals_table);

const mp_obj_module_t socketpool_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&socketpool_globals,
};
