/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#include "bindings/espidf/__init__.h"
#include "lib/utils/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "supervisor/shared/tick.h"

#include "components/lwip/lwip/src/include/lwip/err.h"
#include "components/lwip/lwip/src/include/lwip/sockets.h"
#include "components/lwip/lwip/src/include/lwip/sys.h"
#include "components/lwip/lwip/src/include/lwip/netdb.h"

STATIC socketpool_socket_obj_t * open_socket_handles[CONFIG_LWIP_MAX_SOCKETS]; // 4 on the wrover/wroom

void socket_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_handles); i++) {
        if (open_socket_handles[i]) {
            if (open_socket_handles[i]->num > 0) {
                common_hal_socketpool_socket_close(open_socket_handles[i]);
                open_socket_handles[i] = NULL;
            } else {
                // accidentally got a TCP socket in here, or something.
                open_socket_handles[i] = NULL;
            }
        }
    }
}

bool register_open_socket(socketpool_socket_obj_t* self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_handles); i++) {
        if (open_socket_handles[i] == NULL) {
            open_socket_handles[i] = self;
            return true;
        }
    }
    return false;
}

STATIC void _lazy_init_LWIP(socketpool_socket_obj_t* self) {
    if (self->num != -1) {
        return; //safe to call on existing socket
    }
    if (self->tls != NULL) {
        mp_raise_RuntimeError(translate("Invalid use of TLS Socket"));
    }
    int socknum = -1;
    socknum = lwip_socket(self->family, self->type, self->ipproto);
    if (socknum < 0 || !register_open_socket(self)) {
        mp_raise_RuntimeError(translate("Out of sockets"));
    }
    self->num = socknum;
    lwip_fcntl(socknum, F_SETFL, O_NONBLOCK);
}

STATIC void _lazy_init_TLS(socketpool_socket_obj_t* self) {
    if (self->type != SOCK_STREAM || self->num != -1) {
        mp_raise_RuntimeError(translate("Invalid socket for TLS"));
    }
    esp_tls_t* tls_handle = esp_tls_init();
    if (tls_handle == NULL) {
        mp_raise_espidf_MemoryError();
    }
    self->tls = tls_handle;
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t* self, mp_uint_t timeout_ms) {
    self->timeout_ms = timeout_ms;
}

bool common_hal_socketpool_socket_bind(socketpool_socket_obj_t* self,
                                        const char* host, size_t hostlen, uint8_t port) {
    _lazy_init_LWIP(self);

    struct sockaddr_in bind_addr;
    bind_addr.sin_addr.s_addr = inet_addr(host);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);

    int opt = 1;
    int err = lwip_setsockopt(self->num, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err != 0) {
        mp_raise_RuntimeError(translate("Issue setting SO_REUSEADDR"));
    }
    int result = lwip_bind(self->num, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == 0;
    return result;
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t* self, int backlog) {
    return lwip_listen(self->num, backlog) == 0;
}

socketpool_socket_obj_t* common_hal_socketpool_socket_accept(socketpool_socket_obj_t* self,
                                        uint8_t* ip, uint *port) {
    struct sockaddr_in accept_addr;
    socklen_t socklen = sizeof(accept_addr);
    int newsoc = -1;
    bool timed_out = false;
    uint64_t start_ticks = supervisor_ticks_ms64();

    if (self->timeout_ms != (uint)-1) {
        mp_printf(&mp_plat_print, "will timeout");
    } else {
        mp_printf(&mp_plat_print, "won't timeout");
    }

    // Allow timeouts and interrupts
    while (newsoc == -1 &&
           !timed_out &&
           !mp_hal_is_interrupted()) {
        if (self->timeout_ms != (uint)-1) {
            timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        newsoc = lwip_accept(self->num, (struct sockaddr *)&accept_addr, &socklen);
        // In non-blocking mode, fail instead of looping
        if (newsoc == -1 && self->timeout_ms == 0) {
            mp_raise_OSError(MP_EAGAIN);
        }
    }

    if (!timed_out) {
        // harmless on failure but avoiding memcpy is faster
        memcpy((void *)ip, (void*)&accept_addr.sin_addr.s_addr, sizeof(accept_addr.sin_addr.s_addr));
        *port = accept_addr.sin_port;
    } else {
        mp_raise_OSError(ETIMEDOUT);
    }

    if (newsoc > 0) {
        // Create the socket
        socketpool_socket_obj_t *sock = m_new_obj_with_finaliser(socketpool_socket_obj_t);
        sock->base.type = &socketpool_socket_type;
        sock->num = newsoc;
        sock->tls = NULL;
        sock->ssl_context = NULL;
        sock->pool = self->pool;

        if (!register_open_socket(sock)) {
            mp_raise_OSError(MP_EBADF);
        }

        lwip_fcntl(newsoc, F_SETFL, O_NONBLOCK);
        return sock;
    } else {
        mp_raise_OSError(MP_EBADF);
        return NULL;
    }
}

bool common_hal_socketpool_socket_connect(socketpool_socket_obj_t* self,
                                            const char* host, mp_uint_t hostlen, mp_int_t port) {
    // For simplicity we use esp_tls for all TCP connections. If it's not SSL, ssl_context will be
    // NULL and should still work. This makes regular TCP connections more memory expensive but TLS
    // should become more and more common. Therefore, we optimize for the TLS case.

    // Todo: move to SSL Wrapper and add lwip_connect()
    _lazy_init_TLS(self);

    esp_tls_cfg_t* tls_config = NULL;
    if (self->ssl_context != NULL) {
        tls_config = &self->ssl_context->ssl_config;
    }
    int result = esp_tls_conn_new_sync(host, hostlen, port, tls_config, self->tls);
    self->connected = result >= 0;
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

    return self->connected;
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t* self) {
    return self->connected;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t* self, const uint8_t* buf, mp_uint_t len) {
    int sent = -1;
    if (self->num != -1) {
        // LWIP Socket
        // TODO: deal with potential failure/add timeout?
        sent = lwip_send(self->num, buf, len, 0);
    } else if (self->tls != NULL) {
        // TLS Socket
        sent = esp_tls_conn_write(self->tls, buf, len);
    }

    if (sent < 0) {
        mp_raise_OSError(MP_ENOTCONN);
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t* self, const uint8_t* buf, mp_uint_t len) {
    int received = 0;
    bool timed_out = false;

    if (self->num != -1) {
        // LWIP Socket
        uint64_t start_ticks = supervisor_ticks_ms64();
        received = -1;
        while (received == -1 &&
                !timed_out &&
                !mp_hal_is_interrupted()) {
            if (self->timeout_ms != (uint)-1) {
                timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
            }
            RUN_BACKGROUND_TASKS;
            received = lwip_recv(self->num, (void*) buf, len - 1, 0);

            // In non-blocking mode, fail instead of looping
            if (received == -1 && self->timeout_ms == 0) {
                mp_raise_OSError(MP_EAGAIN);
            }
        }
    } else if (self->tls != NULL) {
        // TLS Socket
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
            if (self->timeout_ms != (uint)-1) {
                timed_out = self->timeout_ms == 0 || supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
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
                status = esp_tls_conn_read(self->tls, (void*) buf + received, available);
                if (status == 0) {
                    // Reading zero when something is available indicates a closed
                    // connection. (The available bytes could have been TLS internal.)
                    break;
                }
                if (status > 0) {
                    received += status;
                }
            }
        }
    } else {
        // Socket does not have a valid descriptor of either type
        mp_raise_OSError(MP_EBADF);
    }

    if (timed_out) {
        mp_raise_OSError(ETIMEDOUT);
    }
    return received;
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t* self,
    const char* host, size_t hostlen, uint8_t port, const uint8_t* buf, mp_uint_t len) {

    _lazy_init_LWIP(self);

    // Get the IP address string
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *result;
    int error = lwip_getaddrinfo(host, NULL, &hints, &result);
    if (error != 0 || result == NULL) {
        return 0;
    }

    // Set parameters
    struct sockaddr_in dest_addr;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    dest_addr.sin_addr.s_addr = ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
    #pragma GCC diagnostic pop
    freeaddrinfo(result);

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    int bytes_sent = lwip_sendto(self->num, buf, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (bytes_sent < 0) {
        mp_raise_BrokenPipeError();
        return 0;
    }
    return bytes_sent;
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t* self,
    uint8_t* buf, mp_uint_t len, uint8_t* ip, uint *port) {

    _lazy_init_LWIP(self);

    struct sockaddr_in source_addr;
    socklen_t socklen = sizeof(source_addr);

    // LWIP Socket
    uint64_t start_ticks = supervisor_ticks_ms64();
    int received = -1;
    bool timed_out = false;
    while (received == -1 &&
            !timed_out &&
            !mp_hal_is_interrupted()) {
        if (self->timeout_ms != (uint)-1) {
            timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        received = lwip_recvfrom(self->num, buf, len, 0, (struct sockaddr *)&source_addr, &socklen);

        // In non-blocking mode, fail instead of looping
        if (received == -1 && self->timeout_ms == 0) {
            mp_raise_OSError(MP_EAGAIN);
        }
    }

    if (!timed_out) {
        memcpy((void *)ip, (void*)&source_addr.sin_addr.s_addr, sizeof(source_addr.sin_addr.s_addr));
        *port = source_addr.sin_port;
    } else {
        mp_raise_OSError(ETIMEDOUT);
    }

    if (received < 0) {
        mp_raise_BrokenPipeError();
        return 0;
    }

    return received;
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t* self) {
    self->connected = false;
    if (self->tls != NULL) {
        esp_tls_conn_destroy(self->tls);
        self->tls = NULL;
    }
    if (self->num >= 0) {
        lwip_shutdown(self->num, 0);
        lwip_close(self->num);
        self->num = -1;
    }
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t* self) {
    return self->tls == NULL && self->num < 0;
}


mp_uint_t common_hal_socketpool_socket_get_hash(socketpool_socket_obj_t* self) {
    return self->num;
}
