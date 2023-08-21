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

#include "bindings/espnow/__init__.h"
#include "bindings/espnow/ESPNow.h"
#include "bindings/espnow/ESPNowPacket.h"
#include "bindings/espnow/Peer.h"
#include "bindings/espnow/Peers.h"

//| """ESP-NOW Module
//|
//| The `espnow` module provides an interface to the
//| `ESP-NOW <https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/api-reference/network/esp_now.html>`_
//| protocol provided by Espressif on its SoCs.
//|
//| **Sender**
//|
//| .. code-block:: python
//|
//|     import espnow
//|
//|     e = espnow.ESPNow()
//|     peer = espnow.Peer(mac=b'\xaa\xaa\xaa\xaa\xaa\xaa')
//|     e.peers.append(peer)
//|
//|     e.send("Starting...")
//|     for i in range(10):
//|         e.send(str(i)*20)
//|     e.send(b'end')
//|
//| **Receiver**
//|
//| .. code-block:: python
//|
//|     import espnow
//|
//|     e = espnow.ESPNow()
//|     packets = []
//|
//|     while True:
//|         if e:
//|             packet = e.read()
//|             packets.append(packet)
//|             if packet.msg == b'end':
//|                 break
//|
//|     print("packets:", f"length={len(packets)}")
//|     for packet in packets:
//|         print(packet)
//| """
//| ...
//|

STATIC const mp_rom_map_elem_t espnow_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_espnow) },

    // module classes
    { MP_ROM_QSTR(MP_QSTR_ESPNow),      MP_ROM_PTR(&espnow_type) },
    { MP_ROM_QSTR(MP_QSTR_ESPNowPacket), MP_ROM_PTR(&espnow_packet_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_Peer),        MP_ROM_PTR(&espnow_peer_type) },
    { MP_ROM_QSTR(MP_QSTR_Peers),       MP_ROM_PTR(&espnow_peers_type) },
};
STATIC MP_DEFINE_CONST_DICT(espnow_module_globals, espnow_module_globals_table);

const mp_obj_module_t espnow_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_espnow, espnow_module);
