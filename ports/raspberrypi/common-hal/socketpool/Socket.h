/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2015 Galen Hazelwood
 * Copyright (c) 2015-2017 Paul Sokolovsky
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

typedef struct _lwip_socket_obj_t {
    mp_obj_base_t base;

    volatile union {
        struct tcp_pcb *tcp;
        struct udp_pcb *udp;
        struct raw_pcb *raw;
    } pcb;
    volatile union {
        struct pbuf *pbuf;
        struct {
            uint8_t alloc;
            uint8_t iget;
            uint8_t iput;
            union {
                struct tcp_pcb *item; // if alloc == 0
                struct tcp_pcb **array; // if alloc != 0
            } tcp;
        } connection;
    } incoming;
    mp_obj_t callback;
    byte peer[4];
    mp_uint_t peer_port;
    mp_uint_t timeout;
    uint16_t recv_offset;

    uint8_t domain;
    uint8_t type;

    #define STATE_NEW 0
    #define STATE_LISTENING 1
    #define STATE_CONNECTING 2
    #define STATE_CONNECTED 3
    #define STATE_PEER_CLOSED 4
    #define STATE_ACTIVE_UDP 5
    // Negative value is lwIP error
    int8_t state;

    socketpool_socketpool_obj_t *pool;
} socketpool_socket_obj_t;

void socket_user_reset(void);
