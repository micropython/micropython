/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
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
    .base = {
        .base = {
            .type = &mp_type_type
        },
        .flags = MP_TYPE_FLAG_EXTENDED,
        .name = MP_QSTR_ESPNowPacket,
        .print = namedtuple_print,
        .parent = &mp_type_tuple,
        .make_new = namedtuple_make_new,
        .attr = namedtuple_attr,
        MP_TYPE_EXTENDED_FIELDS(
            .unary_op = mp_obj_tuple_unary_op,
            .binary_op = mp_obj_tuple_binary_op,
            .subscr = mp_obj_tuple_subscr,
            .getiter = mp_obj_tuple_getiter,
            ),
    },
    .n_fields = 4,
    .fields = {
        MP_QSTR_mac,
        MP_QSTR_msg,
        MP_QSTR_rssi,
        MP_QSTR_time,
    },
};
