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

#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"

#include "components/lwip/lwip/src/include/lwip/netdb.h"

#include "bindings/espidf/__init__.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t* self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(translate("SocketPool can only be used with wifi.radio"));
    }
}

socketpool_socket_obj_t* common_hal_socketpool_socket(socketpool_socketpool_obj_t* self,
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

    // Consider LWIP and MbedTLS "variant" sockets to be incompatible (for now)
    // The variant of the socket is determined by whether the socket is wrapped
    // by SSL. If no TLS handle is set in sslcontext_wrap_socket, the first call
    // of bind() or connect() will create a LWIP socket with a corresponding
    // socketnum.
    // TODO: move MbedTLS to its own duplicate Socket or Server API, maybe?
    socketpool_socket_obj_t *sock = m_new_obj_with_finaliser(socketpool_socket_obj_t);
    sock->base.type = &socketpool_socket_type;
    sock->num = -1;
    sock->type = socket_type;
    sock->family = addr_family;
    sock->ipproto = ipproto;
    sock->timeout_ms = (uint)-1;

    sock->tls = NULL;
    sock->ssl_context = NULL;
    sock->pool = self;
    return sock;
}


mp_obj_t common_hal_socketpool_socketpool_gethostbyname(socketpool_socketpool_obj_t* self,
    const char* host) {

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
