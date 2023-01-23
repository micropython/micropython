/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 MicroDev
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

#include "shared-bindings/util.h"

#include "bindings/espnow/__init__.h"
#include "bindings/espnow/ESPNow.h"

//| """ESP-NOW Module
//|
//| The `espnow` module provides an interface to the
//| `ESP-NOW <https://www.espressif.com/en/products/software/esp-now/overview>`_
//| protocol provided by Espressif on its SoCs
//| (`API docs <https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/network/esp_now.html>`_).
//|
//| **Sender**
//|
//| .. code-block:: python
//|
//|     import espnow
//|
//|     e = espnow.ESPNow()
//|     e.active(True)
//|     peer = b'\xbb\xbb\xbb\xbb\xbb\xbb'   # MAC address of peer's wifi interface
//|     e.add_peer(peer)
//|
//|     e.send("Starting...")   # Send to all peers
//|     for i in range(100):
//|         e.send(peer, str(i)*20, True)
//|         e.send(b'end')
//|
//| **Receiver**
//|
//| .. code-block:: python
//|
//|     import espnow
//|
//|     e = espnow.ESPNow()
//|     e.active(True)
//|     peer = b'\xaa\xaa\xaa\xaa\xaa\xaa'   # MAC address of peer's wifi interface
//|     e.add_peer(peer)
//|
//|     while True:
//|         host, msg = e.recv()
//|         if msg:             # msg == None if timeout in recv()
//|             print(host, msg)
//|             if msg == b'end':
//|                 break
//| """
//| ...
//|

STATIC const mp_rom_map_elem_t espnow_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_espnow) },
    // module classes
    { MP_ROM_QSTR(MP_QSTR_ESPNow), MP_ROM_PTR(&espnow_type) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_module_globals, espnow_module_globals_table);

const mp_obj_module_t espnow_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_espnow, espnow_module, CIRCUITPY_ESPNOW);
