// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#if CIRCUITPY_SSL_MBEDTLS
#include "shared-module/ssl/SSLContext.h"
#else
#include "common-hal/ssl/SSLContext.h"
#endif

#include "shared-bindings/ssl/SSLSocket.h"

extern const mp_obj_type_t ssl_sslcontext_type;

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self);

ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    mp_obj_t socket, bool server_side, const char *server_hostname);

void common_hal_ssl_sslcontext_load_verify_locations(ssl_sslcontext_obj_t *self,
    const char *cadata);

void common_hal_ssl_sslcontext_set_default_verify_paths(ssl_sslcontext_obj_t *self);

bool common_hal_ssl_sslcontext_get_check_hostname(ssl_sslcontext_obj_t *self);
void common_hal_ssl_sslcontext_set_check_hostname(ssl_sslcontext_obj_t *self, bool value);
void common_hal_ssl_sslcontext_load_cert_chain(ssl_sslcontext_obj_t *self, mp_buffer_info_t *cert_buf, mp_buffer_info_t *key_buf);
