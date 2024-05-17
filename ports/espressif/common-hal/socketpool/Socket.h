// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "common-hal/socketpool/SocketPool.h"

#include "components/esp-tls/esp_tls.h"
#include "components/lwip/lwip/src/include/lwip/sockets.h"

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
