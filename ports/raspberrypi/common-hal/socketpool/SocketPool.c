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
#include "shared/runtime/interrupt_char.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"

#include "lwip/dns.h"
#include "lwip/inet.h"

void common_hal_socketpool_socketpool_construct(socketpool_socketpool_obj_t *self, mp_obj_t radio) {
    if (radio != MP_OBJ_FROM_PTR(&common_hal_wifi_radio_obj)) {
        mp_raise_ValueError(MP_ERROR_TEXT("SocketPool can only be used with wifi.radio"));
    }
}

// common_hal_socketpool_socket is in socketpool/Socket.c to centralize open socket tracking.

typedef struct _getaddrinfo_state_t {
    volatile int status;
    volatile ip_addr_t ipaddr;
} getaddrinfo_state_t;

STATIC void lwip_getaddrinfo_cb(const char *name, const ip_addr_t *ipaddr, void *arg) {
    getaddrinfo_state_t *state = arg;
    if (ipaddr != NULL) {
        state->status = 1;
        state->ipaddr = *ipaddr;
    } else {
        // error
        state->status = -2;
    }
}

STATIC int socketpool_resolve_host(socketpool_socketpool_obj_t *self, const char *host, ip_addr_t *addr) {

    getaddrinfo_state_t state;
    state.status = 0;

    MICROPY_PY_LWIP_ENTER
    err_t ret = dns_gethostbyname(host, (ip_addr_t *)&state.ipaddr, lwip_getaddrinfo_cb, &state);
    MICROPY_PY_LWIP_EXIT

    switch (ret) {
        case ERR_OK:
            // cached
            state.status = 1;
            break;
        case ERR_INPROGRESS:
            while (state.status == 0) {
                RUN_BACKGROUND_TASKS;
                if (mp_hal_is_interrupted()) {
                    break;
                }
            }
            break;
        default:
            state.status = ret;
    }

    if (state.status < 0) {
        return state.status;
        // TODO: CPython raises gaierror, we raise with native lwIP negative error
        // values, to differentiate from normal errno's at least in such way.
        mp_raise_OSError(state.status);
    }

    *addr = state.ipaddr;
    return 0;
}

void socketpool_resolve_host_raise(socketpool_socketpool_obj_t *self, const char *host, ip_addr_t *addr) {
    int result = socketpool_resolve_host(self, host, addr);
    if (result < 0) {
        printf("socket_resolve_host() returned %d\n", result);
        common_hal_socketpool_socketpool_raise_gaierror_noname();
        mp_raise_OSError(-result);
    }
}

mp_obj_t common_hal_socketpool_socketpool_gethostbyname(socketpool_socketpool_obj_t *self, const char *host) {

    ip_addr_t addr;
    socketpool_resolve_host_raise(self, host, &addr);

    char ip_str[IP4ADDR_STRLEN_MAX];
    inet_ntoa_r(addr, ip_str, IP4ADDR_STRLEN_MAX);
    mp_obj_t ip_obj = mp_obj_new_str(ip_str, strlen(ip_str));
    return ip_obj;
}

mp_obj_t common_hal_socketpool_socketpool_gethostbyname_raise(socketpool_socketpool_obj_t *self, const char *host) {
    return common_hal_socketpool_socketpool_gethostbyname(self, host);
}
