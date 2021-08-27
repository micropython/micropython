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

#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/Socket.h"

#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"

#include "components/lwip/lwip/src/include/lwip/netdb.h"

#include "bindings/espidf/__init__.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(translate("SocketPool can only be used with wifi.radio"));
    }
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type) {

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

    if (addr_family == AF_INET6 || ipproto == IPPROTO_IPV6) {
        mp_raise_NotImplementedError(translate("Only IPv4 sockets supported"));
    }

    socketpool_socket_obj_t *sock = m_new_obj_with_finaliser(socketpool_socket_obj_t);
    sock->base.type = &socketpool_socket_type;
    sock->type = socket_type;
    sock->family = addr_family;
    sock->ipproto = ipproto;
    sock->pool = self;
    sock->timeout_ms = (uint)-1;

    // Create LWIP socket
    int socknum = -1;
    socknum = lwip_socket(sock->family, sock->type, sock->ipproto);
    if (socknum < 0 || !register_open_socket(sock)) {
        mp_raise_RuntimeError(translate("Out of sockets"));
    }
    sock->num = socknum;
    // Sockets should be nonblocking in most cases
    lwip_fcntl(socknum, F_SETFL, O_NONBLOCK);
    return sock;
}


mp_obj_t common_hal_socketpool_socketpool_gethostbyname(socketpool_socketpool_obj_t *self,
    const char *host) {

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    int err = getaddrinfo(host, NULL, &hints, &res);
    if (err != 0 || res == NULL) {
        return mp_const_none;
    }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-align"
    struct in_addr *addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    #pragma GCC diagnostic pop
    char ip_str[IP4ADDR_STRLEN_MAX];
    inet_ntoa_r(*addr, ip_str, IP4ADDR_STRLEN_MAX);
    mp_obj_t ip_obj = mp_obj_new_str(ip_str, strlen(ip_str));
    freeaddrinfo(res);

    return ip_obj;
}
