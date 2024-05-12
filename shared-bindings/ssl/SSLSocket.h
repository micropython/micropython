/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#if CIRCUITPY_SSL_MBEDTLS
#include "shared-module/ssl/SSLSocket.h"
#else
#include "common-hal/ssl/SSLSocket.h"
#endif

extern const mp_obj_type_t ssl_sslsocket_type;

mp_obj_t common_hal_ssl_sslsocket_accept(ssl_sslsocket_obj_t *self);
void common_hal_ssl_sslsocket_bind(ssl_sslsocket_obj_t *self, mp_obj_t addr);
void common_hal_ssl_sslsocket_close(ssl_sslsocket_obj_t *self);
void common_hal_ssl_sslsocket_connect(ssl_sslsocket_obj_t *self, mp_obj_t addr);
bool common_hal_ssl_sslsocket_get_closed(ssl_sslsocket_obj_t *self);
bool common_hal_ssl_sslsocket_get_connected(ssl_sslsocket_obj_t *self);
void common_hal_ssl_sslsocket_listen(ssl_sslsocket_obj_t *self, int backlog);
mp_uint_t common_hal_ssl_sslsocket_recv_into(ssl_sslsocket_obj_t *self, uint8_t *buf, uint32_t len);
mp_uint_t common_hal_ssl_sslsocket_send(ssl_sslsocket_obj_t *self, const uint8_t *buf, uint32_t len);
void common_hal_ssl_sslsocket_settimeout(ssl_sslsocket_obj_t *self, mp_obj_t timeout_obj);
void common_hal_ssl_sslsocket_setsockopt(ssl_sslsocket_obj_t *self, mp_obj_t level, mp_obj_t optname, mp_obj_t optval);
