// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
//|
//| For more information about the `socket` module, see the CPython documentation:
//| https://docs.python.org/3/library/socket.html
//|
//| .. jinja
//|
//| .. raw:: html
//|
//|     <p>
//|     <details>
//|     <summary>AF_INET6 (IPv6) supported on these boards</summary>
//|     <ul>
//|     {% for board in support_matrix_reverse["socketpool.socketpool.AF_INET6"] %}
//|     <li> {{ board }}
//|     {% endfor %}
//|     </ul>
//|     </details>
//|     </p>
//| """

static const mp_rom_map_elem_t socketpool_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_socketpool) },

    { MP_ROM_QSTR(MP_QSTR_SocketPool), MP_ROM_PTR(&socketpool_socketpool_type) },
    { MP_ROM_QSTR(MP_QSTR_Socket), MP_ROM_PTR(&socketpool_socket_type) },
};

static MP_DEFINE_CONST_DICT(socketpool_globals, socketpool_globals_table);

const mp_obj_module_t socketpool_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&socketpool_globals,
};

MP_REGISTER_MODULE(MP_QSTR_socketpool, socketpool_module);
