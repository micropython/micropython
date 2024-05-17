// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_AUTHMODE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_AUTHMODE_H

#include "py/enum.h"

typedef enum {
    AUTHMODE_OPEN = 1 << 0,
    AUTHMODE_WEP = 1 << 1,
    AUTHMODE_WPA = 1 << 2,
    AUTHMODE_WPA2 = 1 << 3,
    AUTHMODE_WPA3 = 1 << 4,
    AUTHMODE_PSK = 1 << 5,
    AUTHMODE_ENTERPRISE = 1 << 6,
} wifi_authmode_t;

extern const mp_obj_type_t wifi_authmode_type;
extern const cp_enum_obj_t authmode_OPEN_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_AUTHMODE_H
