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

#include "shared-bindings/wifi/AuthMode.h"

MAKE_ENUM_VALUE(wifi_authmode_type, authmode, OPEN, AUTHMODE_OPEN);
MAKE_ENUM_VALUE(wifi_authmode_type, authmode, WEP, AUTHMODE_WEP);
MAKE_ENUM_VALUE(wifi_authmode_type, authmode, WPA, AUTHMODE_WPA);
MAKE_ENUM_VALUE(wifi_authmode_type, authmode, WPA2, AUTHMODE_WPA2);
MAKE_ENUM_VALUE(wifi_authmode_type, authmode, WPA3, AUTHMODE_WPA3);
MAKE_ENUM_VALUE(wifi_authmode_type, authmode, PSK, AUTHMODE_PSK);
MAKE_ENUM_VALUE(wifi_authmode_type, authmode, ENTERPRISE, AUTHMODE_ENTERPRISE);

//| class AuthMode:
//|     """The authentication protocols used by WiFi."""
//|
//|     OPEN: object
//|     """Open network. No authentication required."""
//|
//|     WEP: object
//|     """Wired Equivalent Privacy."""
//|
//|     WPA: object
//|     """Wireless Protected Access."""
//|
//|     WPA2: object
//|     """Wireless Protected Access 2."""
//|
//|     WPA3: object
//|     """Wireless Protected Access 3."""
//|
//|     PSK: object
//|     """Pre-shared Key. (password)"""
//|
//|     ENTERPRISE: object
//|     """Each user has a unique credential."""
//|
MAKE_ENUM_MAP(wifi_authmode) {
    MAKE_ENUM_MAP_ENTRY(authmode, OPEN),
    MAKE_ENUM_MAP_ENTRY(authmode, WEP),
    MAKE_ENUM_MAP_ENTRY(authmode, WPA),
    MAKE_ENUM_MAP_ENTRY(authmode, WPA2),
    MAKE_ENUM_MAP_ENTRY(authmode, WPA3),
    MAKE_ENUM_MAP_ENTRY(authmode, PSK),
    MAKE_ENUM_MAP_ENTRY(authmode, ENTERPRISE),
};
STATIC MP_DEFINE_CONST_DICT(wifi_authmode_locals_dict, wifi_authmode_locals_table);

MAKE_PRINTER(wifi, wifi_authmode);

MAKE_ENUM_TYPE(wifi, AuthMode, wifi_authmode);
