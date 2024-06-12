// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
bool common_hal_ssl_sslsocket_readable(ssl_sslsocket_obj_t *self);
bool common_hal_ssl_sslsocket_writable(ssl_sslsocket_obj_t *self);
void common_hal_ssl_sslsocket_listen(ssl_sslsocket_obj_t *self, int backlog);
mp_uint_t common_hal_ssl_sslsocket_recv_into(ssl_sslsocket_obj_t *self, uint8_t *buf, mp_uint_t len);
mp_uint_t common_hal_ssl_sslsocket_send(ssl_sslsocket_obj_t *self, const uint8_t *buf, mp_uint_t len);
void common_hal_ssl_sslsocket_settimeout(ssl_sslsocket_obj_t *self, mp_obj_t timeout_obj);
void common_hal_ssl_sslsocket_setsockopt(ssl_sslsocket_obj_t *self, mp_obj_t level, mp_obj_t optname, mp_obj_t optval);
