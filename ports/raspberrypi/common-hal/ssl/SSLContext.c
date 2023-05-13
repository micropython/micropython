/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/ssl/SSLContext.h"
#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-bindings/socketpool/SocketPool.h"

#include "py/runtime.h"
#include "py/stream.h"

#include "mbedtls/crt_bundle.h"

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
