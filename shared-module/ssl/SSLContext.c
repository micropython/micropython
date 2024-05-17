// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/ssl/SSLContext.h"
#include "shared-bindings/ssl/SSLSocket.h"

#include "py/runtime.h"
#include "py/stream.h"

#include "lib/mbedtls_config/crt_bundle.h"

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self) {
    common_hal_ssl_sslcontext_set_default_verify_paths(self);
}

void common_hal_ssl_sslcontext_load_verify_locations(ssl_sslcontext_obj_t *self,
    const char *cadata) {
    self->crt_bundle_attach = NULL;
    self->use_global_ca_store = false;
    self->cacert_buf = (const unsigned char *)cadata;
    self->cacert_bytes = *cadata ? strlen(cadata) + 1 : 0;
}

void common_hal_ssl_sslcontext_set_default_verify_paths(ssl_sslcontext_obj_t *self) {
    self->crt_bundle_attach = crt_bundle_attach;
    self->use_global_ca_store = true;
    self->cacert_buf = NULL;
    self->cacert_bytes = 0;
}

bool common_hal_ssl_sslcontext_get_check_hostname(ssl_sslcontext_obj_t *self) {
    return self->check_name;
}

void common_hal_ssl_sslcontext_set_check_hostname(ssl_sslcontext_obj_t *self, bool value) {
    self->check_name = value;
}

void common_hal_ssl_sslcontext_load_cert_chain(ssl_sslcontext_obj_t *self, mp_buffer_info_t *cert_buf, mp_buffer_info_t *key_buf) {
    self->cert_buf = *cert_buf;
    self->key_buf = *key_buf;
}
