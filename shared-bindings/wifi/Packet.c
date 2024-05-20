// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"

#include "shared-bindings/wifi/Packet.h"

MAKE_ENUM_VALUE(wifi_packet_type, packet, CH, PACKET_CH);
MAKE_ENUM_VALUE(wifi_packet_type, packet, LEN, PACKET_LEN);
MAKE_ENUM_VALUE(wifi_packet_type, packet, RAW, PACKET_RAW);
MAKE_ENUM_VALUE(wifi_packet_type, packet, RSSI, PACKET_RSSI);

//| class Packet:
//|     """The packet parameters."""
//|
//|     CH: object
//|     """The packet's channel."""
//|
//|     LEN: object
//|     """The packet's length."""
//|
//|     RAW: object
//|     """The packet's payload."""
//|
//|     RSSI: object
//|     """The packet's rssi."""
//|
MAKE_ENUM_MAP(wifi_packet) {
    MAKE_ENUM_MAP_ENTRY(packet, CH),
    MAKE_ENUM_MAP_ENTRY(packet, LEN),
    MAKE_ENUM_MAP_ENTRY(packet, RAW),
    MAKE_ENUM_MAP_ENTRY(packet, RSSI),
};
static MP_DEFINE_CONST_DICT(wifi_packet_locals_dict, wifi_packet_locals_table);

MAKE_PRINTER(wifi, wifi_packet);

MP_DEFINE_CONST_OBJ_TYPE(
    wifi_packet_type,
    MP_QSTR_Packet,
    MP_TYPE_FLAG_NONE,
    print, wifi_packet_print,
    locals_dict, &wifi_packet_locals_dict
    );
