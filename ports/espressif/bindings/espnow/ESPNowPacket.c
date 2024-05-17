// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 MicroDev
//
// SPDX-License-Identifier: MIT

#include "bindings/espnow/ESPNowPacket.h"

//| class ESPNowPacket:
//|     """A packet retrieved from ESP-NOW communication protocol. A namedtuple."""
//|
//|     mac: ReadableBuffer
//|     """The sender's mac address (length = 6 bytes)."""
//|
//|     msg: ReadableBuffer
//|     """The message sent by the peer (length <= 250 bytes)."""
//|
//|     rssi: int
//|     """The received signal strength indication (in dBm from -127 to 0)."""
//|
//|     time: int
//|     """The time in milliseconds since the device last booted when the packet was received."""
//|

const mp_obj_namedtuple_type_t espnow_packet_type_obj = {
    NAMEDTUPLE_TYPE_BASE_AND_SLOTS(MP_QSTR_ESPNowPacket),
    .n_fields = 4,
    .fields = {
        MP_QSTR_mac,
        MP_QSTR_msg,
        MP_QSTR_rssi,
        MP_QSTR_time,
    },
};
