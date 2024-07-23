// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/Socket.h"

#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"
#include "common-hal/socketpool/__init__.h"

#include "components/lwip/lwip/src/include/lwip/netdb.h"

#include "bindings/espidf/__init__.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("SocketPool can only be used with wifi.radio"));
    }
}

// common_hal_socketpool_socket is in socketpool/Socket.c to centralize open socket tracking.

int socketpool_getaddrinfo_common(const char *host, int service, const struct addrinfo *hints, struct addrinfo **res) {
    // As of 2022, the version of lwip in esp-idf does not handle the
    // trailing-dot syntax of domain names, so emulate it.
    // Remove this once https://github.com/espressif/esp-idf/issues/10013 has
    // been implemented
    if (host) {
        size_t strlen_host = strlen(host);
        if (strlen_host && host[strlen_host - 1] == '.') {
            mp_obj_t nodot = mp_obj_new_str(host, strlen_host - 1);
            host = mp_obj_str_get_str(nodot);
        }
    }

    char service_buf[6];
    snprintf(service_buf, sizeof(service_buf), "%d", service);

    return lwip_getaddrinfo(host, service_buf, hints, res);
}

static mp_obj_t format_address(const struct sockaddr *addr, int family) {
    char ip_str[IPADDR_STRLEN_MAX]; // big enough for any supported address type
    const struct sockaddr_in *a = (void *)addr;

    switch (family) {
        #if CIRCUITPY_SOCKETPOOL_IPV6
        case AF_INET6:
            inet_ntop(family, &((const struct sockaddr_in6 *)a)->sin6_addr, ip_str, sizeof(ip_str));
            break;
        #endif
        default:
        case AF_INET:
            inet_ntop(family, &((const struct sockaddr_in *)a)->sin_addr, ip_str, sizeof(ip_str));
            break;
    }
    return mp_obj_new_str(ip_str, strlen(ip_str));
}

static mp_obj_t convert_sockaddr(const struct addrinfo *ai, int port) {
    #if CIRCUITPY_SOCKETPOOL_IPV6
    mp_int_t n_tuple = ai->ai_family == AF_INET6 ? 4 : 2;
    #else
    mp_int_t n_tuple = 2;
    #endif
    mp_obj_tuple_t *result = MP_OBJ_TO_PTR(mp_obj_new_tuple(n_tuple, NULL));
    result->items[0] = format_address(ai->ai_addr, ai->ai_family);
    result->items[1] = MP_OBJ_NEW_SMALL_INT(port);
    #if CIRCUITPY_SOCKETPOOL_IPV6
    if (ai->ai_family == AF_INET6) {
        const struct sockaddr_in6 *ai6 = (void *)ai->ai_addr;
        result->items[2] = MP_OBJ_NEW_SMALL_INT(ai6->sin6_flowinfo);
        result->items[3] = MP_OBJ_NEW_SMALL_INT(ai6->sin6_scope_id);
    }
    #endif
    return result;
}

static mp_obj_t convert_addrinfo(const struct addrinfo *ai, int port) {
    MP_STATIC_ASSERT(AF_INET == SOCKETPOOL_AF_INET);
    #if CIRCUITPY_SOCKETPOOL_IPV6
    MP_STATIC_ASSERT(AF_INET6 == SOCKETPOOL_AF_INET6);
    #endif
    // MP_STATIC_ASSERT(AF_UNSPEC == SOCKETPOOL_AF_UNSPEC);
    mp_obj_tuple_t *result = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    result->items[0] = MP_OBJ_NEW_SMALL_INT(ai->ai_family);
    result->items[1] = MP_OBJ_NEW_SMALL_INT(ai->ai_socktype);
    result->items[2] = MP_OBJ_NEW_SMALL_INT(ai->ai_protocol);
    result->items[3] = ai->ai_canonname ? mp_obj_new_str(ai->ai_canonname, strlen(ai->ai_canonname)) : MP_OBJ_NEW_QSTR(MP_QSTR_);
    result->items[4] = convert_sockaddr(ai, port);
    return result;
}

mp_obj_t common_hal_socketpool_getaddrinfo_raise(socketpool_socketpool_obj_t *self, const char *host, int port, int family, int type, int proto, int flags) {
    const struct addrinfo hints = {
        .ai_flags = flags,
        .ai_family = family,
        .ai_protocol = proto,
        .ai_socktype = type,
    };

    struct addrinfo *res = NULL;
    int err = socketpool_getaddrinfo_common(host, port, &hints, &res);
    if (err != 0 || res == NULL) {
        common_hal_socketpool_socketpool_raise_gaierror_noname();
    }

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t result = mp_obj_new_list(0, NULL);
        for (struct addrinfo *ai = res; ai; ai = ai->ai_next) {
            mp_obj_list_append(result, convert_addrinfo(ai, port));
        }
        nlr_pop();
        lwip_freeaddrinfo(res);
        return result;
    } else {
        lwip_freeaddrinfo(res);
        nlr_raise(MP_OBJ_FROM_PTR(nlr.ret_val));
    }
}
