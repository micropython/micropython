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

#include "shared-bindings/socketpool/Socket.h"

#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/workflow.h"

#include "lwip/err.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

#define MAX_SOCKETS (8)

STATIC mp_obj_t open_socket_objs[MAX_SOCKETS];
STATIC bool user_socket[MAX_SOCKETS];

void socket_user_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (open_socket_objs[i] && user_socket[i]) {
            // shut 'er down
        }
    }
}

// The writes below send an event to the socket select task so that it redoes the
// select with the new open socket set.

STATIC bool register_open_socket(mp_obj_t obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (!open_socket_objs[i]) {
            open_socket_objs[i] = obj;
            user_socket[i] = false;
            return true;
        }
    }
    return false;
}

STATIC void unregister_open_socket(mp_obj_t obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (open_socket_objs[i] == obj) {
            open_socket_objs[i] = NULL;
            user_socket[i] = false;
        }
    }
}

STATIC void mark_user_socket(mp_obj_t obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (open_socket_objs[i] == obj) {
            user_socket[i] = true;
            return;
        }
    }
}

bool socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    socketpool_socket_obj_t *sock) {
    mp_raise_NotImplementedError(NULL);
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type) {
    mp_raise_NotImplementedError(NULL);
}

int socketpool_socket_accept(socketpool_socket_obj_t *self, uint8_t *ip, uint32_t *port) {
    mp_raise_NotImplementedError(NULL);
}

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self,
    uint8_t *ip, uint32_t *port) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    mp_raise_NotImplementedError(NULL);
}

void socketpool_socket_close(socketpool_socket_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog) {
    mp_raise_NotImplementedError(NULL);
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, uint8_t *ip, uint32_t *port) {
    mp_raise_NotImplementedError(NULL);
}

int socketpool_socket_recv_into(socketpool_socket_obj_t *self,
    const uint8_t *buf, uint32_t len) {
    mp_raise_NotImplementedError(NULL);
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    mp_raise_NotImplementedError(NULL);
}

int socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    mp_raise_NotImplementedError(NULL);
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    mp_raise_NotImplementedError(NULL);
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms) {
    mp_raise_NotImplementedError(NULL);
}
