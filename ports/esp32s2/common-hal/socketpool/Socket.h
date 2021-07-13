/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ESP32S2_COMMON_HAL_SOCKETPOOL_SOCKET_H
#define MICROPY_INCLUDED_ESP32S2_COMMON_HAL_SOCKETPOOL_SOCKET_H

#include "py/obj.h"

#include "common-hal/socketpool/SocketPool.h"
#include "common-hal/ssl/SSLContext.h"

#include "components/esp-tls/esp_tls.h"

typedef struct {
    mp_obj_base_t base;
    int num;
    int type;
    int family;
    int ipproto;
    bool connected;
    socketpool_socketpool_obj_t *pool;
    mp_uint_t timeout_ms;
} socketpool_socket_obj_t;

void socket_reset(void);
bool register_open_socket(socketpool_socket_obj_t *self);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_SOCKETPOOL_SOCKET_H
