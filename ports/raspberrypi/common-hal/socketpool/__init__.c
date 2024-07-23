// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "shared/runtime/interrupt_char.h"

#include "shared-bindings/socketpool/__init__.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared-bindings/wifi/__init__.h"
#include "common-hal/socketpool/__init__.h"

#include "lwip/dns.h"
#include "lwip/inet.h"


void socketpool_user_reset(void) {
    socket_user_reset();
}

typedef struct _getaddrinfo_state_t {
    volatile int status;
    volatile ip_addr_t ipaddr;
} getaddrinfo_state_t;

static void lwip_getaddrinfo_cb(const char *name, const ip_addr_t *ipaddr, void *arg) {
    getaddrinfo_state_t *state = arg;
    if (ipaddr != NULL) {
        state->status = 1;
        state->ipaddr = *ipaddr;
    } else {
        // error
        state->status = -2;
    }
}


static int socketpool_resolve_host(const char *host, ip_addr_t *addr) {

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
    }

    *addr = state.ipaddr;
    return 0;
}

void socketpool_resolve_host_raise(const char *host, ip_addr_t *addr) {
    int result = socketpool_resolve_host(host, addr);
    if (result < 0) {
        printf("socket_resolve_host() returned %d\n", result);
        common_hal_socketpool_socketpool_raise_gaierror_noname();
        mp_raise_OSError(-result);
    }
}
