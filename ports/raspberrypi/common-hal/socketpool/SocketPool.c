// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/socketpool/SocketPool.h"
#include "common-hal/socketpool/__init__.h"
#include "common-hal/socketpool/Socket.h"
#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/ipaddress/__init__.h"

#include "lwip/dns.h"
#include "lwip/inet.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("SocketPool can only be used with wifi.radio"));
    }
}

// common_hal_socketpool_socket is in socketpool/Socket.c to centralize open socket tracking.

static mp_obj_t common_hal_socketpool_socketpool_gethostbyname_raise(socketpool_socketpool_obj_t *self, const char *host) {

    ip_addr_t addr;
    socketpool_resolve_host_raise(host, &addr);

    char ip_str[IP4ADDR_STRLEN_MAX];
    inet_ntoa_r(addr, ip_str, IP4ADDR_STRLEN_MAX);
    mp_obj_t ip_obj = mp_obj_new_str(ip_str, strlen(ip_str));
    return ip_obj;
}

mp_obj_t common_hal_socketpool_getaddrinfo_raise(socketpool_socketpool_obj_t *self, const char *host, int port, int family, int type, int proto, int flags) {
    mp_obj_t ip_str;

    if (strlen(host) > 0 && ipaddress_parse_ipv4address(host, strlen(host), NULL)) {
        ip_str = mp_obj_new_str(host, strlen(host));
    } else {
        ip_str = common_hal_socketpool_socketpool_gethostbyname_raise(self, host);
    }

    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_AF_INET);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(SOCKETPOOL_SOCK_STREAM);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    mp_obj_tuple_t *sockaddr = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    sockaddr->items[0] = ip_str;
    sockaddr->items[1] = MP_OBJ_NEW_SMALL_INT(port);
    tuple->items[4] = MP_OBJ_FROM_PTR(sockaddr);
    return mp_obj_new_list(1, (mp_obj_t *)&tuple);
}
