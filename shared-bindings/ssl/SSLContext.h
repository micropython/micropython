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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SSL_SSLCONTEXT_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SSL_SSLCONTEXT_H

#include "common-hal/ssl/SSLContext.h"

#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/ssl/SSLSocket.h"

extern const mp_obj_type_t ssl_sslcontext_type;

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self);

ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    socketpool_socket_obj_t *sock, bool server_side, const char *server_hostname);

void common_hal_ssl_sslcontext_load_verify_locations(ssl_sslcontext_obj_t *self,
    const char *cadata);

void common_hal_ssl_sslcontext_set_default_verify_paths(ssl_sslcontext_obj_t *self);

bool common_hal_ssl_sslcontext_get_check_hostname(ssl_sslcontext_obj_t *self);
void common_hal_ssl_sslcontext_set_check_hostname(ssl_sslcontext_obj_t *self, bool value);
void common_hal_ssl_sslcontext_load_cert_chain(ssl_sslcontext_obj_t *self, mp_buffer_info_t *cert_buf, mp_buffer_info_t *key_buf);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_SSL_SSLCONTEXT_H
