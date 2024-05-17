// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_PACKET_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_PACKET_H

#include "py/enum.h"

typedef enum {
    PACKET_CH,
    PACKET_LEN,
    PACKET_RAW,
    PACKET_RSSI,
} wifi_packet_t;

extern const mp_obj_type_t wifi_packet_type;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_PACKET_H
