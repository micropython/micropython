// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/socketpool/Socket.h"

extern const mp_obj_type_t socketpool_socket_type;

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out);
size_t common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port);
void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self);
void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self, const char *host, size_t hostlen, uint32_t port);
bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self);
bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self);
mp_uint_t common_hal_socketpool_socket_get_timeout(socketpool_socket_obj_t *self);
mp_int_t common_hal_socketpool_socket_get_type(socketpool_socket_obj_t *self);
bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog);
mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, mp_obj_t *peer_out);
mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len);
mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len);
mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len);
void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms);
int common_hal_socketpool_socket_setsockopt(socketpool_socket_obj_t *self, int level, int optname, const void *value, size_t optlen);
bool common_hal_socketpool_readable(socketpool_socket_obj_t *self);
bool common_hal_socketpool_writable(socketpool_socket_obj_t *self);

// Non-allocating versions for internal use.
int socketpool_socket_accept(socketpool_socket_obj_t *self, mp_obj_t *peer_out, socketpool_socket_obj_t *accepted);
void socketpool_socket_close(socketpool_socket_obj_t *self);
int socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len);
int socketpool_socket_recv_into(socketpool_socket_obj_t *self,
    const uint8_t *buf, uint32_t len);

// Moves self to sock without closing the real socket. self will think its closed afterwards.
void socketpool_socket_move(socketpool_socket_obj_t *self, socketpool_socket_obj_t *sock);

// Resets the socket object state so it appears closed and disconnected. This only works on
// uninitialized memory.
void socketpool_socket_reset(socketpool_socket_obj_t *self);
