/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2021 Lucian Copeland for Adafruit Industries
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

#include "shared-bindings/ssl/SSLSocket.h"
#include "shared-bindings/socketpool/Socket.h"
#include "shared-bindings/ssl/SSLContext.h"

#include "bindings/espidf/__init__.h"
#include "lib/utils/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/tick.h"

ssl_sslsocket_obj_t *common_hal_ssl_sslsocket_accept(ssl_sslsocket_obj_t *self,
    uint8_t *ip, uint32_t *port) {
    socketpool_socket_obj_t *sock = common_hal_socketpool_socket_accept(self->sock, ip, port);
    ssl_sslsocket_obj_t *sslsock = common_hal_ssl_sslcontext_wrap_socket(self->ssl_context, sock, false, NULL);
    return sslsock;
}

bool common_hal_ssl_sslsocket_bind(ssl_sslsocket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    return common_hal_socketpool_socket_bind(self->sock, host, hostlen, port);
}

void common_hal_ssl_sslsocket_close(ssl_sslsocket_obj_t *self) {
    common_hal_socketpool_socket_close(self->sock);
    esp_tls_conn_destroy(self->tls);
    self->tls = NULL;
}

void common_hal_ssl_sslsocket_connect(ssl_sslsocket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    esp_tls_cfg_t *tls_config = NULL;
    tls_config = &self->ssl_context->ssl_config;
    int result = esp_tls_conn_new_sync(host, hostlen, port, tls_config, self->tls);
    self->sock->connected = result >= 0;
    if (result < 0) {
        int esp_tls_code;
        int flags;
        esp_err_t err = esp_tls_get_and_clear_last_error(self->tls->error_handle, &esp_tls_code, &flags);

        if (err == ESP_ERR_MBEDTLS_SSL_SETUP_FAILED) {
            mp_raise_espidf_MemoryError();
        } else if (ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED) {
            mp_raise_OSError_msg_varg(translate("Failed SSL handshake"));
        } else {
            mp_raise_OSError_msg_varg(translate("Unhandled ESP TLS error %d %d %x %d"), esp_tls_code, flags, err, result);
        }
    } else {
        // Connection successful, set the timeout on the underlying socket. We can't rely on the IDF
        // to do it because the config structure is only used for TLS connections. Generally, we
        // shouldn't hit this timeout because we try to only read available data. However, there is
        // always a chance that we try to read something that is used internally.
        int fd;
        esp_tls_get_conn_sockfd(self->tls, &fd);
        struct timeval tv;
        tv.tv_sec = 2 * 60; // Two minutes
        tv.tv_usec = 0;
        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    }
}

bool common_hal_ssl_sslsocket_get_closed(ssl_sslsocket_obj_t *self) {
    return self->tls == NULL && self->sock->num < 0;
}

bool common_hal_ssl_sslsocket_get_connected(ssl_sslsocket_obj_t *self) {
    return self->sock->connected;
}

bool common_hal_ssl_sslsocket_listen(ssl_sslsocket_obj_t *self, int backlog) {
    return common_hal_socketpool_socket_listen(self->sock, backlog);
}

mp_uint_t common_hal_ssl_sslsocket_recv_into(ssl_sslsocket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int received = 0;
    bool timed_out = false;
    int status = 0;
    uint64_t start_ticks = supervisor_ticks_ms64();
    int sockfd;
    esp_err_t err = esp_tls_get_conn_sockfd(self->tls, &sockfd);
    if (err != ESP_OK) {
        mp_raise_OSError(MP_EBADF);
    }
    while (received == 0 &&
           status >= 0 &&
           !timed_out &&
           !mp_hal_is_interrupted()) {
        if (self->sock->timeout_ms != (uint)-1 && self->sock->timeout_ms != 0) {
            timed_out = self->sock->timeout_ms == 0 || supervisor_ticks_ms64() - start_ticks >= self->sock->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        size_t available = esp_tls_get_bytes_avail(self->tls);
        if (available == 0) {
            // This reads the raw socket buffer and is used for non-TLS connections
            // and between encrypted TLS blocks.
            status = lwip_ioctl(sockfd, FIONREAD, &available);
        }
        size_t remaining = len - received;
        if (available > remaining) {
            available = remaining;
        }
        if (available > 0) {
            status = esp_tls_conn_read(self->tls, (void *)buf + received, available);
            if (status == 0) {
                // Reading zero when something is available indicates a closed
                // connection. (The available bytes could have been TLS internal.)
                break;
            }
            if (status > 0) {
                received += status;
            }
        }
        // In non-blocking mode, fail instead of timing out
        if (received == 0 && self->sock->timeout_ms == 0) {
            mp_raise_OSError(MP_EAGAIN);
        }
    }

    if (timed_out) {
        mp_raise_OSError(ETIMEDOUT);
    }
    return received;
}

mp_uint_t common_hal_ssl_sslsocket_send(ssl_sslsocket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = -1;
    sent = esp_tls_conn_write(self->tls, buf, len);

    if (sent < 0) {
        int esp_tls_code;
        int flags;
        esp_err_t err = esp_tls_get_and_clear_last_error(self->tls->error_handle, &esp_tls_code, &flags);

        if (err == ESP_ERR_MBEDTLS_SSL_SETUP_FAILED) {
            mp_raise_espidf_MemoryError();
        } else if (ESP_ERR_MBEDTLS_SSL_HANDSHAKE_FAILED) {
            mp_raise_OSError_msg_varg(translate("Failed SSL handshake"));
        } else {
            mp_raise_OSError_msg_varg(translate("Unhandled ESP TLS error %d %d %x %d"), esp_tls_code, flags, err, sent);
        }
    }
    return sent;
}

void common_hal_ssl_sslsocket_settimeout(ssl_sslsocket_obj_t *self, uint32_t timeout_ms) {
    self->sock->timeout_ms = timeout_ms;
}
