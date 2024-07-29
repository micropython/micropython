// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2013-2019 Damien P. George
// SPDX-FileCopyrightText: Copyright (c) 2015 Galen Hazelwood
// SPDX-FileCopyrightText: Copyright (c) 2015-2017 Paul Sokolovsky
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/socketpool/SocketPool.h"

typedef struct _lwip_socket_obj_t {
    mp_obj_base_t base;

    volatile union {
        struct tcp_pcb *ip;
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
    ip_addr_t peer;
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

// Not required for RPi socket positive callbacks
#define socketpool_socket_poll_resume(x)

void socket_user_reset(void);
