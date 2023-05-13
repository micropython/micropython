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

#pragma once

#include "py/obj.h"

#include "common-hal/socketpool/SocketPool.h"
#include "common-hal/ssl/SSLContext.h"

#include "components/esp-tls/esp_tls.h"

typedef struct ssl_sslsocket_obj ssl_sslsocket_obj_t;

typedef struct {
    mp_obj_base_t base;
    int num;
    int type;
    int family;
    int ipproto;
    bool connected;
    socketpool_socketpool_obj_t *pool;
    ssl_sslsocket_obj_t *ssl_socket;
    mp_uint_t timeout_ms;
} socketpool_socket_obj_t;

void socket_user_reset(void);
// Unblock workflow socket select thread (platform specific)
void socketpool_socket_poll_resume(void);
