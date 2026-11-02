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

#include "psa/crypto.h"

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self) {
    // TLS is built on PSA crypto, which has to be initialized before any of it is
    // used. OK to psa_crypto_init() multiple times. On espressif, ESP-IDF has already done this during
    // its own system init and this call is a no-op.
    if (psa_crypto_init() != PSA_SUCCESS) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q init failed"), MP_QSTR_ssl);
    }
    self->cert_buf.buf = NULL;
    self->cert_buf.len = 0;
    self->key_buf.buf = NULL;
    self->key_buf.len = 0;
    self->check_name = true;
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

void common_hal_ssl_sslcontext_load_cert_chain(ssl_sslcontext_obj_t *self, mp_buffer_info_t *cert_buf, mp_buffer_info_t *key_buf, psa_key_id_t hw_key_id) {
    self->cert_buf = *cert_buf;
    self->hw_key_id = hw_key_id;
    if (hw_key_id == 0) {
        self->key_buf = *key_buf;
    }
}
