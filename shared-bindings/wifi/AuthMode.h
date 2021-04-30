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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_AUTHMODE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_AUTHMODE_H

#include "py/enum.h"

typedef enum {
    AUTHMODE_OPEN,
    AUTHMODE_WEP,
    AUTHMODE_WPA,
    AUTHMODE_WPA2,
    AUTHMODE_WPA3,
    AUTHMODE_PSK,
    AUTHMODE_ENTERPRISE
} wifi_authmode_t;

extern const mp_obj_type_t wifi_authmode_type;
extern const cp_enum_obj_t authmode_OPEN_obj;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_WIFI_AUTHMODE_H
