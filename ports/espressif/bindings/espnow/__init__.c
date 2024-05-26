// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

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

static const mp_rom_map_elem_t espnow_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_espnow) },

    // module classes
    { MP_ROM_QSTR(MP_QSTR_ESPNow),      MP_ROM_PTR(&espnow_type) },
    { MP_ROM_QSTR(MP_QSTR_ESPNowPacket), MP_ROM_PTR(&espnow_packet_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_Peer),        MP_ROM_PTR(&espnow_peer_type) },
    { MP_ROM_QSTR(MP_QSTR_Peers),       MP_ROM_PTR(&espnow_peers_type) },
};
static MP_DEFINE_CONST_DICT(espnow_module_globals, espnow_module_globals_table);

const mp_obj_module_t espnow_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&espnow_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_espnow, espnow_module);
