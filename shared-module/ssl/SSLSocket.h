// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-module/ssl/SSLContext.h"

#include "mbedtls/platform.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/pk.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

typedef struct ssl_sslsocket_obj {
    mp_obj_base_t base;
    mp_obj_t sock_obj;
    ssl_sslcontext_obj_t *ssl_context;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt cert;
    mbedtls_pk_context pkey;
    uintptr_t poll_mask;
    bool closed;
    mp_obj_t accept_args[2];
    mp_obj_t bind_args[3];
    mp_obj_t close_args[2];
    mp_obj_t connect_args[3];
    mp_obj_t listen_args[3];
    mp_obj_t recv_into_args[3];
    mp_obj_t send_args[3];
    mp_obj_t setsockopt_args[5];
    mp_obj_t settimeout_args[3];
} ssl_sslsocket_obj_t;
