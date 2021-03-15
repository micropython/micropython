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

#include "py/runtime.h"

void common_hal_ssl_sslcontext_construct(ssl_sslcontext_obj_t *self) {

}

ssl_sslsocket_obj_t *common_hal_ssl_sslcontext_wrap_socket(ssl_sslcontext_obj_t *self,
    socketpool_socket_obj_t *socket, bool server_side, const char *server_hostname) {

    if (socket->type != SOCK_STREAM) {
        mp_raise_RuntimeError(translate("Invalid socket for TLS"));
    }

    ssl_sslsocket_obj_t *sock = m_new_obj_with_finaliser(ssl_sslsocket_obj_t);
    sock->base.type = &ssl_sslsocket_type;
    sock->ssl_context = self;
    sock->sock = socket;

    esp_tls_t *tls_handle = esp_tls_init();
    if (tls_handle == NULL) {
        mp_raise_espidf_MemoryError();
    }
    sock->tls = tls_handle;

    // TODO: do something with the original socket? Don't call a close on the internal LWIP.

    // Should we store server hostname on the socket in case connect is called with an ip?
    return sock;
}
