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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SOCKETPOOL_SOCKET_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SOCKETPOOL_SOCKET_H

#include "common-hal/socketpool/Socket.h"

extern const mp_obj_type_t socketpool_socket_type;

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self, uint8_t *ip, uint32_t *port);
bool common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port);
void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self);
bool common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port);
bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self);
bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self);
mp_uint_t common_hal_socketpool_socket_get_timeout(socketpool_socket_obj_t *self);
bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog);
mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, uint8_t *ip, uint32_t *port);
mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len);
mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len);
mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len);
void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_SOCKETPOOL_SOCKET_H
