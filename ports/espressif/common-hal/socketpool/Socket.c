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
#include "shared/runtime/interrupt_char.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared-bindings/ssl/SSLSocket.h"
#include "common-hal/ssl/SSLSocket.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/workflow.h"

#include "components/lwip/lwip/src/include/lwip/err.h"
#include "components/lwip/lwip/src/include/lwip/sockets.h"
#include "components/lwip/lwip/src/include/lwip/sys.h"
#include "components/lwip/lwip/src/include/lwip/netdb.h"
#include "components/vfs/include/esp_vfs_eventfd.h"

StackType_t socket_select_stack[2 * configMINIMAL_STACK_SIZE];

STATIC int open_socket_fds[CONFIG_LWIP_MAX_SOCKETS];
STATIC socketpool_socket_obj_t *user_socket[CONFIG_LWIP_MAX_SOCKETS];
StaticTask_t socket_select_task_handle;
STATIC int socket_change_fd = -1;

STATIC void socket_select_task(void *arg) {
    uint64_t signal;

    while (true) {
        fd_set readfds;
        fd_set errfds;
        FD_ZERO(&readfds);
        FD_ZERO(&errfds);
        FD_SET(socket_change_fd, &readfds);
        FD_SET(socket_change_fd, &errfds);
        int max_fd = socket_change_fd;
        for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
            int sockfd = open_socket_fds[i];
            if (sockfd < 0) {
                continue;
            }
            max_fd = MAX(max_fd, sockfd);
            FD_SET(sockfd, &readfds);
            FD_SET(sockfd, &errfds);
        }

        int num_triggered = select(max_fd + 1, &readfds, NULL, &errfds, NULL);
        // Check for bad file descriptor and queue up the background task before
        // circling around.
        if (num_triggered == -1 && errno == EBADF) {
            // One for the change fd and one for the closed socket.
            num_triggered = 2;
        }
        // Try and find the bad file and remove it from monitoring.
        for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
            int sockfd = open_socket_fds[i];
            if (sockfd < 0) {
                continue;
            }
            int err;
            int optlen = sizeof(int);
            int ret = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, (socklen_t *)&optlen);
            if (ret < 0) {
                open_socket_fds[i] = -1;
                // Raise num_triggered so that we skip the assert and queue the background task.
                num_triggered = 2;
            }
        }
        assert(num_triggered >= 0);

        if (FD_ISSET(socket_change_fd, &readfds)) {
            read(socket_change_fd, &signal, sizeof(signal));
            num_triggered -= 1;
        }
        if (num_triggered > 0) {
            supervisor_workflow_request_background();

            // Wake up CircuitPython. We know it is asleep because we are lower
            // priority.
            port_wake_main_task();
        }

    }
    close(socket_change_fd);
    vTaskDelete(NULL);
}

void socket_user_reset(void) {
    if (socket_change_fd < 0) {
        esp_vfs_eventfd_config_t config = ESP_VFS_EVENTD_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_vfs_eventfd_register(&config));

        for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
            open_socket_fds[i] = -1;
            user_socket[i] = NULL;
        }
        socket_change_fd = eventfd(0, 0);
        // Run this at the same priority as CP so that the web workflow background task can be
        // queued while CP is running. Both tasks can still sleep and, therefore, sleep overall.
        (void)xTaskCreateStaticPinnedToCore(socket_select_task,
            "socket_select",
            2 * configMINIMAL_STACK_SIZE,
            NULL,
            uxTaskPriorityGet(NULL),
            socket_select_stack,
            &socket_select_task_handle,
            xPortGetCoreID());
    }

    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
        if (open_socket_fds[i] >= 0 && user_socket[i]) {
            common_hal_socketpool_socket_close(user_socket[i]);
            int num = open_socket_fds[i];
            // Close automatically clears socket handle
            lwip_shutdown(num, SHUT_RDWR);
            lwip_close(num);
            open_socket_fds[i] = -1;
            user_socket[i] = NULL;
        }
    }
}

// The writes below send an event to the socket select task so that it redoes the
// select with the new open socket set.

STATIC bool register_open_socket(int fd) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
        if (open_socket_fds[i] == -1) {
            open_socket_fds[i] = fd;
            user_socket[i] = false;
            uint64_t signal = 1;
            write(socket_change_fd, &signal, sizeof(signal));
            return true;
        }
    }
    return false;
}

STATIC void unregister_open_socket(int fd) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
        if (open_socket_fds[i] == fd) {
            open_socket_fds[i] = -1;
            user_socket[i] = false;
            // Write must be 8 bytes for an eventfd.
            uint64_t signal = 1;
            write(socket_change_fd, &signal, sizeof(signal));
            return;
        }
    }
}

STATIC void mark_user_socket(int fd, socketpool_socket_obj_t *obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_fds); i++) {
        if (open_socket_fds[i] == fd) {
            user_socket[i] = obj;
            return;
        }
    }
}

bool socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    socketpool_socket_obj_t *sock) {
    int addr_family;
    int ipproto;
    if (family == SOCKETPOOL_AF_INET) {
        addr_family = AF_INET;
        ipproto = IPPROTO_IP;
    } else { // INET6
        addr_family = AF_INET6;
        ipproto = IPPROTO_IPV6;
    }

    int socket_type;
    if (type == SOCKETPOOL_SOCK_STREAM) {
        socket_type = SOCK_STREAM;
    } else if (type == SOCKETPOOL_SOCK_DGRAM) {
        socket_type = SOCK_DGRAM;
    } else { // SOCKETPOOL_SOCK_RAW
        socket_type = SOCK_RAW;
    }
    sock->type = socket_type;
    sock->family = addr_family;
    sock->ipproto = ipproto;
    sock->pool = self;
    sock->timeout_ms = (uint)-1;

    // Create LWIP socket
    int socknum = -1;
    socknum = lwip_socket(sock->family, sock->type, sock->ipproto);
    if (socknum < 0) {
        return false;
    }
    // This shouldn't happen since we have room for the same number of sockets as LWIP.
    if (!register_open_socket(socknum)) {
        lwip_close(socknum);
        return false;
    }
    sock->num = socknum;
    // Sockets should be nonblocking in most cases
    lwip_fcntl(socknum, F_SETFL, O_NONBLOCK);
    return true;
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type) {
    if (family != SOCKETPOOL_AF_INET) {
        mp_raise_NotImplementedError(translate("Only IPv4 sockets supported"));
    }

    socketpool_socket_obj_t *sock = m_new_obj_with_finaliser(socketpool_socket_obj_t);
    sock->base.type = &socketpool_socket_type;

    if (!socketpool_socket(self, family, type, sock)) {
        mp_raise_RuntimeError(translate("Out of sockets"));
    }
    mark_user_socket(sock->num, sock);
    return sock;
}

int socketpool_socket_accept(socketpool_socket_obj_t *self, uint8_t *ip, uint32_t *port, socketpool_socket_obj_t *accepted) {
    struct sockaddr_in accept_addr;
    socklen_t socklen = sizeof(accept_addr);
    int newsoc = -1;
    bool timed_out = false;
    uint64_t start_ticks = supervisor_ticks_ms64();

    // Allow timeouts and interrupts
    while (newsoc == -1 && !timed_out) {
        if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
            timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
        }
        RUN_BACKGROUND_TASKS;
        newsoc = lwip_accept(self->num, (struct sockaddr *)&accept_addr, &socklen);
        // In non-blocking mode, fail instead of timing out
        if (newsoc == -1 && (self->timeout_ms == 0 || mp_hal_is_interrupted())) {
            return -MP_EAGAIN;
        }
    }

    if (!timed_out) {
        // harmless on failure but avoiding memcpy is faster
        memcpy((void *)ip, (void *)&accept_addr.sin_addr.s_addr, sizeof(accept_addr.sin_addr.s_addr));
        *port = accept_addr.sin_port;
    } else {
        return -ETIMEDOUT;
    }
    if (newsoc < 0) {
        return -MP_EBADF;
    }

    // We got a socket. New client socket will not be non-blocking by default, so make it non-blocking.
    lwip_fcntl(newsoc, F_SETFL, O_NONBLOCK);

    if (!register_open_socket(newsoc)) {
        lwip_close(newsoc);
        return -MP_EBADF;
    }


    if (accepted != NULL) {
        // Close the active socket because we have another we accepted.
        if (!common_hal_socketpool_socket_get_closed(accepted)) {
            common_hal_socketpool_socket_close(accepted);
        }
        // Replace the old accepted socket with the new one.
        accepted->num = newsoc;
        accepted->pool = self->pool;
        accepted->connected = true;
    }

    return newsoc;
}

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *self,
    uint8_t *ip, uint32_t *port) {
    socketpool_socket_obj_t *sock = m_new_obj_with_finaliser(socketpool_socket_obj_t);
    int newsoc = socketpool_socket_accept(self, ip, port, NULL);

    if (newsoc > 0) {
        // Create the socket
        mark_user_socket(newsoc, sock);
        sock->base.type = &socketpool_socket_type;
        sock->num = newsoc;
        sock->pool = self->pool;
        sock->connected = true;

        return sock;
    } else {
        mp_raise_OSError(-newsoc);
        return NULL;
    }
}

bool common_hal_socketpool_socket_bind(socketpool_socket_obj_t *self,
    const char *host, size_t hostlen, uint32_t port) {
    struct sockaddr_in bind_addr;
    const char *broadcast = "<broadcast>";
    uint32_t ip;
    if (hostlen == 0) {
        ip = IPADDR_ANY;
    } else if (hostlen == strlen(broadcast) &&
               memcmp(host, broadcast, strlen(broadcast)) == 0) {
        ip = IPADDR_BROADCAST;
    } else {
        ip = inet_addr(host);
    }
    bind_addr.sin_addr.s_addr = ip;
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_port = htons(port);

    int opt = 1;
    int err = lwip_setsockopt(self->num, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (err != 0) {
        mp_raise_RuntimeError(translate("Cannot set socket options"));
    }
    int result = lwip_bind(self->num, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
    return result == 0;
}

void socketpool_socket_close(socketpool_socket_obj_t *self) {
    if (self->ssl_socket) {
        ssl_sslsocket_obj_t *ssl_socket = self->ssl_socket;
        self->ssl_socket = NULL;
        common_hal_ssl_sslsocket_close(ssl_socket);
        return;
    }
    self->connected = false;
    if (self->num >= 0) {
        lwip_shutdown(self->num, SHUT_RDWR);
        lwip_close(self->num);
        unregister_open_socket(self->num);
        self->num = -1;
    }
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *self) {
    socketpool_socket_close(self);
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
        common_hal_socketpool_socketpool_raise_gaierror_noname();
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
    return lwip_listen(self->num, backlog);
}

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *self,
    uint8_t *buf, uint32_t len, uint8_t *ip, uint32_t *port) {

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

int socketpool_socket_recv_into(socketpool_socket_obj_t *self,
    const uint8_t *buf, uint32_t len) {
    int received = 0;
    bool timed_out = false;

    if (self->num != -1) {
        // LWIP Socket
        uint64_t start_ticks = supervisor_ticks_ms64();
        received = -1;
        while (received == -1 &&
               !timed_out) {
            if (self->timeout_ms != (uint)-1 && self->timeout_ms != 0) {
                timed_out = supervisor_ticks_ms64() - start_ticks >= self->timeout_ms;
            }
            RUN_BACKGROUND_TASKS;
            received = lwip_recv(self->num, (void *)buf, len, 0);

            // In non-blocking mode, fail instead of looping
            if (received == -1 && self->timeout_ms == 0) {
                if (errno == ENOTCONN) {
                    self->connected = false;
                    return -MP_ENOTCONN;
                }
                return -MP_EAGAIN;
            }
            // Check this after going through the loop once so it can make
            // progress while interrupted.
            if (mp_hal_is_interrupted()) {
                if (received == -1) {
                    return -MP_EAGAIN;
                }
                break;
            }
        }
    } else {
        return -MP_EBADF;
    }

    if (timed_out) {
        return -ETIMEDOUT;
    }
    return received;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int received = socketpool_socket_recv_into(self, buf, len);
    if (received < 0) {
        mp_raise_OSError(-received);
    }
    return received;
}

int socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = -1;
    if (self->num != -1) {
        // LWIP Socket
        // TODO: deal with potential failure/add timeout?
        sent = lwip_send(self->num, buf, len, 0);
    } else {
        sent = -MP_EBADF;
    }

    if (sent < 0) {
        if (errno == ECONNRESET || errno == ENOTCONN) {
            self->connected = false;
        }
        return -errno;
    }

    return sent;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = socketpool_socket_send(self, buf, len);

    if (sent < 0) {
        mp_raise_OSError(-sent);
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
        common_hal_socketpool_socketpool_raise_gaierror_noname();
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


int common_hal_socketpool_socket_setsockopt(socketpool_socket_obj_t *self, int level, int optname, const void *value, size_t optlen) {
    int err = lwip_setsockopt(self->num, level, optname, value, optlen);
    if (err != 0) {
        return -errno;
    }
    return 0;
}

bool common_hal_socketpool_readable(socketpool_socket_obj_t *self) {
    struct timeval immediate = {0, 0};

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(self->num, &fds);
    int num_triggered = select(self->num + 1, &fds, NULL, &fds, &immediate);

    // including returning true in the error case
    return num_triggered != 0;
}

bool common_hal_socketpool_writable(socketpool_socket_obj_t *self) {
    struct timeval immediate = {0, 0};

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(self->num, &fds);
    int num_triggered = select(self->num + 1, NULL, &fds, &fds, &immediate);

    // including returning true in the error case
    return num_triggered != 0;
}

void socketpool_socket_move(socketpool_socket_obj_t *self, socketpool_socket_obj_t *sock) {
    *sock = *self;
    self->connected = false;
    self->num = -1;
}

void socketpool_socket_reset(socketpool_socket_obj_t *self) {
    if (self->base.type == &socketpool_socket_type) {
        return;
    }
    self->base.type = &socketpool_socket_type;
    self->connected = false;
    self->num = -1;
}
