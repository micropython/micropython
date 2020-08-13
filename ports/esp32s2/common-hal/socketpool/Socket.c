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

#include "shared-bindings/socketpool/Socket.h"

#include "esp_log.h"
static const char *TAG = "socket";

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t* self, mp_uint_t timeout_ms) {
    self->timeout_ms = timeout_ms;
}

bool common_hal_socketpool_socket_connect(socketpool_socket_obj_t* self, const char* host, mp_uint_t hostlen, mp_int_t port) {
    // For simplicity we use esp_tls for all TCP connections. If it's not SSL, ssl_context will be
    // NULL and should still work. This makes regular TCP connections more memory expensive but TLS
    // should become more and more common. Therefore, we optimize for the TLS case.

    ESP_LOGI(TAG, "connecting to %s:%d %p", host, port, self->ssl_context);
    esp_tls_cfg_t* tls_config = NULL;
    if (self->ssl_context != NULL) {
        tls_config = &self->ssl_context->ssl_config;
    }
    int result = esp_tls_conn_new_sync(host, hostlen, port, tls_config, self->tcp);
    ESP_LOGI(TAG, "result %d", result);
    return result >= 0;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t* self, const uint8_t* buf, mp_uint_t len) {
    size_t sent = esp_tls_conn_write(self->tcp, buf, len);

    ESP_LOGI(TAG, "sent %d bytes", sent);
    if (sent < 0) {
        // raise an error
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t* self, const uint8_t* buf, mp_uint_t len) {
    size_t received = esp_tls_conn_read(self->tcp, (void*) buf, len);

    ESP_LOGI(TAG, "received %d bytes", received);
    if (received == 0) {
        // socket closed
    }
    if (received < 0) {
        // raise an error
    }
    return received;
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t* self) {
    if (self->tcp != NULL) {
        int status = esp_tls_conn_destroy(self->tcp);

        if (status < 0) {
            // raise an error
        }
        self->tcp = NULL;
    }
}
