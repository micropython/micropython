/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

STATIC socketpool_socket_obj_t *open_socket_handles[CONFIG_LWIP_MAX_SOCKETS];

void socket_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_handles); i++) {
        if (open_socket_handles[i]) {
            if (open_socket_handles[i]->num > 0) {
                // Close automatically clears socket handle
                common_hal_socketpool_socket_close(open_socket_handles[i]);
            } else {
                open_socket_handles[i] = NULL;
            }
        }
    }
}

bool register_open_socket(socketpool_socket_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_handles); i++) {
        if (open_socket_handles[i] == NULL) {
            open_socket_handles[i] = self;
            return true;
        }
    }
    return false;
}

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self,
    uint8_t *ip, uint32_t *port) {
    struct sockaddr_in accept_addr;
    socklen_t socklen = sizeof(accept_addr);
    int newsoc = -1;
    bool timed_out = false;
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Allow timeouts and interrupts
    while (newsoc == -1 &&
           !timed_out &&
           !mp_hal_is_interrupted()) {
        if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
            timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        newsoc = lwip_accept(self->num, (struct sockaddr *)&accept_addr, &socklen);
        // In non-blocking mode, fail instead of timing out
        if (newsoc == -1 && self->timeout_ms == 0) {
            mp_raise_OSError(MP_EAGAIN);
        }
    }

    if (!timed_out) {
        // harmless on failure but avoiding memcpy is faster
        memcpy((void *)ip, (void *)&accept_addr.sin_addr.s_addr, sizeof(accept_addr.sin_addr.s_addr));
        *port = accept_addr.sin_port;
    } else {
        mp_raise_OSError(ETIMEDOUT);
    }

    if (newsoc > 0) {
        // Create the socket
        socketpool_socket_obj_t *sock = m_new_obj_with_finaliser(socketpool_socket_obj_t);
        sock->base.type = &socketpool_socket_type;
        sock->num = newsoc;
        sock->pool = self->pool;
        sock->connected = true;

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

bool common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    struct sockaddr_in bind_addr;
    bind_addr.sin_addr.s_addr = inet_addr(host);
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);

    int opt = 1;
    int err = lwip_setsockopt(self->num, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err != 0) {
        mp_raise_RuntimeError(translate("Cannot set socket options"));
    }
    int result = lwip_bind(self->num, (struct sockaddr *)&bind_addr, sizeof(bind_addr)) == 0;
    return result;
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self) {
    self->connected = false;
    if (self->num >= 0) {
        lwip_shutdown(self->num, 0);
        lwip_close(self->num);
        self->num = -1;
    }
    // Remove socket record
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_handles); i++) {
        if (open_socket_handles[i] == self) {
            open_socket_handles[i] = NULL;
        }
    }
}

void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *result_i;
    int error = lwip_getaddrinfo(host, NULL, &hints, &result_i);
    if (error != 0 || result_i == NULL) {
        mp_raise_OSError(EHOSTUNREACH);
    }

    // Set parameters
    struct sockaddr_in dest_addr;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    dest_addr.sin_addr.s_addr = ((struct sockaddr_in *)result_i->ai_addr)->sin_addr.s_addr;
    #pragma GCC diagnostic pop
    freeaddrinfo(result_i);

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    // Replace above with function call -----

    // Switch to blocking mode for this one call
    int opts;
    opts = lwip_fcntl(self->num,F_GETFL,0);
    opts = opts & (~O_NONBLOCK);
    lwip_fcntl(self->num, F_SETFL, opts);

    int result = -1;
    result = lwip_connect(self->num, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in));

    // Switch back once complete
    opts = opts | O_NONBLOCK;
    lwip_fcntl(self->num, F_SETFL, opts);

    if (result >= 0) {
        self->connected = true;
        return;
    } else {
        mp_raise_OSError(errno);
    }
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *self) {
    return self->num < 0;
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *self) {
    return self->connected;
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *self, int backlog) {
    return lwip_listen(self->num, backlog) == 0;
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, uint8_t *ip, uint *port) {

    struct sockaddr_in source_addr;
    socklen_t socklen = sizeof(source_addr);

    // LWIP Socket
    uint64_t start_ticks = supervisor_ticks_ms64();
    int received = -1;
    bool timed_out = false;
    while (received == -1 &&
           !timed_out &&
           !mp_hal_is_interrupted()) {
        if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
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
        memcpy((void *)ip, (void *)&source_addr.sin_addr.s_addr, sizeof(source_addr.sin_addr.s_addr));
        *port = htons(source_addr.sin_port);
    } else {
        mp_raise_OSError(ETIMEDOUT);
    }

    if (received < 0) {
        mp_raise_BrokenPipeError();
        return 0;
    }

    return received;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int received = 0;
    bool timed_out = false;

    if (self->num != -1) {
        // LWIP Socket
        uint64_t start_ticks = supervisor_ticks_ms64();
        received = -1;
        while (received == -1 &&
               !timed_out &&
               !mp_hal_is_interrupted()) {
            if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
                timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
            }
            RUN_BACKGROUND_TASKS;
            received = lwip_recv(self->num, (void *)buf, len, 0);

            // In non-blocking mode, fail instead of looping
            if (received == -1 && self->timeout_ms == 0) {
                mp_raise_OSError(MP_EAGAIN);
            }
        }
    } else {
        mp_raise_OSError(MP_EBADF);
    }

    if (timed_out) {
        mp_raise_OSError(ETIMEDOUT);
    }
    return received;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = -1;
    if (self->num != -1) {
        // LWIP Socket
        // TODO: deal with potential failure/add timeout?
        sent = lwip_send(self->num, buf, len, 0);
    } else {
        mp_raise_OSError(MP_EBADF);
    }

    if (sent < 0) {
        mp_raise_OSError(errno);
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len) {

    // Set parameters
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *result_i;
    int error = lwip_getaddrinfo(host, NULL, &hints, &result_i);
    if (error != 0 || result_i == NULL) {
        mp_raise_OSError(EHOSTUNREACH);
    }

    // Set parameters
    struct sockaddr_in dest_addr;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    dest_addr.sin_addr.s_addr = ((struct sockaddr_in *)result_i->ai_addr)->sin_addr.s_addr;
    #pragma GCC diagnostic pop
    freeaddrinfo(result_i);

    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    int bytes_sent = lwip_sendto(self->num, buf, len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (bytes_sent < 0) {
        mp_raise_BrokenPipeError();
        return 0;
    }
    return bytes_sent;
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms) {
    self->timeout_ms = timeout_ms;
}
