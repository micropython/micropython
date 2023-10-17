/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 microDev
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
STATIC MP_DEFINE_CONST_DICT(wifi_packet_locals_dict, wifi_packet_locals_table);

MAKE_PRINTER(wifi, wifi_packet);

MP_DEFINE_CONST_OBJ_TYPE(
    wifi_packet_type,
    MP_QSTR_Packet,
    MP_TYPE_FLAG_NONE,
    print, wifi_packet_print,
    locals_dict, &wifi_packet_locals_dict,
    unary_op, mp_generic_unary_op
    );
