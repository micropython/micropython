/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

#ifndef MICROPY_INCLUDED_ZEPHYR_MODSOCKET_H
#define MICROPY_INCLUDED_ZEPHYR_MODSOCKET_H

/* TLS for sockets on zephyr is very tightly coupled to the sockets themselves,
 * This header shares the socket structure to modtls
 */

typedef enum _socket_obj_state_t {
    SOCK_STATE_NEW = 0,
    SOCK_STATE_CONNECTING = 1,
    SOCK_STATE_LISTENING = 2,
    SOCK_STATE_CONNECTED = 3,
    SOCK_STATE_ACCEPTED = 4,
    SOCK_STATE_PEER_CLOSED = 5,
} socket_obj_state_t;

typedef struct _socket_obj_t {
    mp_obj_base_t base;
    int ctx;
    enum net_sock_type type;
    net_sa_family_t family;
    int proto;
    bool bound;
    int timeout;
    socket_obj_state_t state;
} socket_obj_t;

/* Import the socket type and module */
extern const mp_obj_type_t zephyr_socket_type;
extern const mp_obj_module_t mp_module_socket;

#endif
