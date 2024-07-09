// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/Socket.h"

#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"

#include "components/lwip/lwip/src/include/lwip/netdb.h"

#include "bindings/espidf/__init__.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("SocketPool can only be used with wifi.radio"));
    }
}

// common_hal_socketpool_socket is in socketpool/Socket.c to centralize open socket tracking.

mp_obj_t common_hal_socketpool_socketpool_gethostbyname(socketpool_socketpool_obj_t *self,
    const char *host) {

    // As of 2022, the version of lwip in esp-idf does not handle the
    // trailing-dot syntax of domain names, so emulate it.
    // Remove this once https://github.com/espressif/esp-idf/issues/10013 has
    // been implemented
    size_t strlen_host = strlen(host);
    if (strlen_host && host[strlen_host - 1] == '.') {
        mp_obj_t nodot = mp_obj_new_str(host, strlen_host - 1);
        host = mp_obj_str_get_str(nodot);
    }

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    int err = lwip_getaddrinfo(host, NULL, &hints, &res);
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
    lwip_freeaddrinfo(res);

    return ip_obj;
}
