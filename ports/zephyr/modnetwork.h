/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MASSDRIVER EI (massdriver.space)
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

#ifndef MICROPY_INCLUDED_ZEPHYR_MODNETWORK_H
#define MICROPY_INCLUDED_ZEPHYR_MODNETWORK_H

#include <zephyr/net/net_if.h>
#include <zephyr/sys/atomic.h>

#include "py/obj.h"

/* See line 184 of extmod's modnetwork.c */
#define NETWORK_STAT_IDLE               (0)
#define NETWORK_STAT_CONNECTING         (1)
#define NETWORK_STAT_WRONG_PASSWORD     (-3)
#define NETWORK_STAT_NO_AP_FOUND        (-2)
#define NETWORK_STAT_CONNECT_FAIL       (-1)
#define NETWORK_STAT_GOT_IP             (3)

#define MOD_NETWORK_STA_IF              (0)
#define MOD_NETWORK_AP_IF               (1)

typedef struct _network_zephyr_t {
    mp_obj_base_t base;
    struct net_if *net_if;
    atomic_t state;
} network_zephyr_t;

#define NETWORK_ZEPHYR_BASE_FRONT \
    union { \
        mp_obj_base_t base; \
        network_zephyr_t nic; \
    }

/* Ensure struct alignment */
#define NETWORK_ZEPHYR_BASE_FRONT_CHECK(_type, _next) \
    BUILD_ASSERT(offsetof(_type, base) == 0)

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_ifconfig_obj);
MP_DECLARE_CONST_FUN_OBJ_KW(network_zephyr_ipconfig_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_status_obj);

#ifdef CONFIG_NET_DHCPV4_SERVER
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_dhcp4_server_obj);
#endif
#ifdef CONFIG_NET_DHCPV6_SERVER
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_dhcp6_server_obj);
#endif
/* Handle IP shared status */
mp_obj_t network_zephyr_status_handler(size_t n_args, const mp_obj_t *args);

static inline void network_zephyr_make_new(mp_obj_t self_in) {
    network_zephyr_t *self = MP_OBJ_TO_PTR(self_in);

    atomic_clear(&self->state);
}

#endif /* MICROPY_INCLUDED_ZEPHYR_MODNETWORK_H */
