// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"

typedef enum {
    PACKET_CH,
    PACKET_LEN,
    PACKET_RAW,
    PACKET_RSSI,
} wifi_packet_t;

extern const mp_obj_type_t wifi_packet_type;
