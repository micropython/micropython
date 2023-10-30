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

#include "bindings/espidf/__init__.h"

#include "components/mbedtls/esp_crt_bundle/include/esp_crt_bundle.h"

#include "py/runtime.h"
#include "lwip/sockets.h"

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self) {

}

ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    socketpool_socket_obj_t *socket, bool server_side, const char *server_hostname) {

    if (socket->type != SOCK_STREAM) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Invalid socket for TLS"));
    }

    ssl_sslsocket_obj_t *sock = m_new_obj_with_finaliser(ssl_sslsocket_obj_t);
    sock->base.type = &ssl_sslsocket_type;
    sock->ssl_context = self;
    sock->sock = socket;
    socket->ssl_socket = sock;

    // Create a copy of the ESP-TLS config object and store the server hostname
    // Note that ESP-TLS will use common_name for both SNI and verification
    memcpy(&sock->ssl_config, &self->ssl_config, sizeof(self->ssl_config));
    sock->ssl_config.common_name = server_hostname;

    esp_tls_t *tls_handle = esp_tls_init();
    if (tls_handle == NULL) {
        mp_raise_espidf_MemoryError();
    }
    sock->tls = tls_handle;

    // TODO: do something with the original socket? Don't call a close on the internal LWIP.

    return sock;
}

void common_hal_ssl_sslcontext_load_verify_locations(ssl_sslcontext_obj_t *self,
    const char *cadata) {
    self->ssl_config.crt_bundle_attach = NULL;
    self->ssl_config.use_global_ca_store = false;
    self->ssl_config.cacert_buf = (const unsigned char *)cadata;
    self->ssl_config.cacert_bytes = strlen(cadata) + 1;
}

void common_hal_ssl_sslcontext_set_default_verify_paths(ssl_sslcontext_obj_t *self) {
    self->ssl_config.crt_bundle_attach = esp_crt_bundle_attach;
    self->ssl_config.use_global_ca_store = true;
    self->ssl_config.cacert_buf = NULL;
    self->ssl_config.cacert_bytes = 0;
}

bool common_hal_ssl_sslcontext_get_check_hostname(ssl_sslcontext_obj_t *self) {
    return !self->ssl_config.skip_common_name;
}

void common_hal_ssl_sslcontext_set_check_hostname(ssl_sslcontext_obj_t *self, bool value) {
    self->ssl_config.skip_common_name = !value;
}

void common_hal_ssl_sslcontext_load_cert_chain(ssl_sslcontext_obj_t *self, mp_buffer_info_t *cert_buf, mp_buffer_info_t *key_buf) {
    self->ssl_config.clientcert_buf = cert_buf->buf;
    self->ssl_config.clientcert_bytes = cert_buf->len + 1;
    self->ssl_config.clientkey_buf = key_buf->buf;
    self->ssl_config.clientkey_bytes = key_buf->len + 1;
}
