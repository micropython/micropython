/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"
#include "systick.h"
#include "pendsv.h"
#include "modnetwork.h"

#if MICROPY_PY_NETWORK

#if MICROPY_PY_LWIP

#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/apps/mdns.h"
#include "extmod/network_cyw43.h"
#include "drivers/cyw43/cyw43.h"

// Poll lwIP every 128ms
#define LWIP_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & 0x7f) == 0)

u32_t sys_now(void) {
    return mp_hal_ticks_ms();
}

STATIC void pyb_lwip_poll(void) {
    #if MICROPY_PY_WIZNET5K
    // Poll the NIC for incoming data
    wiznet5k_poll();
    #endif

    // Run the lwIP internal updates
    sys_check_timeouts();
}

void mod_network_lwip_poll_wrapper(uint32_t ticks_ms) {
    if (LWIP_TICK(ticks_ms)) {
        pendsv_schedule_dispatch(PENDSV_DISPATCH_LWIP, pyb_lwip_poll);
    }

    #if MICROPY_PY_NETWORK_CYW43
    if (cyw43_poll) {
        if (cyw43_sleep != 0) {
            if (--cyw43_sleep == 0) {
                pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll);
            }
        }
    }
    #endif
}

#endif

/// \module network - network configuration
///
/// This module provides network drivers and routing configuration.

void mod_network_init(void) {
    mp_obj_list_init(&MP_STATE_PORT(mod_network_nic_list), 0);
}

void mod_network_deinit(void) {
}

void mod_network_register_nic(mp_obj_t nic) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        if (MP_STATE_PORT(mod_network_nic_list).items[i] == nic) {
            // nic already registered
            return;
        }
    }
    // nic not registered so add to list
    mp_obj_list_append(MP_OBJ_FROM_PTR(&MP_STATE_PORT(mod_network_nic_list)), nic);
}

mp_obj_t mod_network_find_nic(const uint8_t *ip) {
    // find a NIC that is suited to given IP address
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        // TODO check IP suitability here
        // mod_network_nic_type_t *nic_type = (mod_network_nic_type_t*)mp_obj_get_type(nic);
        return nic;
    }

    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("no available NIC"));
}

STATIC mp_obj_t network_route(void) {
    return MP_OBJ_FROM_PTR(&MP_STATE_PORT(mod_network_nic_list));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(network_route_obj, network_route);

STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },

    #if defined(MICROPY_HW_ETH_MDC)
    { MP_ROM_QSTR(MP_QSTR_LAN), MP_ROM_PTR(&network_lan_type) },
    #endif
    #if MICROPY_PY_NETWORK_CYW43
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mp_network_cyw43_type) },
    #endif

    #if MICROPY_PY_WIZNET5K
    { MP_ROM_QSTR(MP_QSTR_WIZNET5K), MP_ROM_PTR(&mod_network_nic_type_wiznet5k) },
    #endif
    #if MICROPY_PY_CC3K
    { MP_ROM_QSTR(MP_QSTR_CC3K), MP_ROM_PTR(&mod_network_nic_type_cc3k) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_route), MP_ROM_PTR(&network_route_obj) },

    // Constants
    #if MICROPY_PY_NETWORK_CYW43
    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(CYW43_ITF_STA)},
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(CYW43_ITF_AP)},
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};

/*******************************************************************************/
// Implementations of network methods that can be used by any interface

#if MICROPY_PY_LWIP

mp_obj_t mod_network_nic_ifconfig(struct netif *netif, size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        // Get IP addresses
        const ip_addr_t *dns = dns_getserver(0);
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)&netif->ip_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&netif->netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&netif->gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else if (args[0] == MP_OBJ_NEW_QSTR(MP_QSTR_dhcp)) {
        // Start the DHCP client
        if (dhcp_supplied_address(netif)) {
            dhcp_renew(netif);
        } else {
            dhcp_stop(netif);
            dhcp_start(netif);
        }

        // Wait for DHCP to get IP address
        uint32_t start = mp_hal_ticks_ms();
        while (!dhcp_supplied_address(netif)) {
            if (mp_hal_ticks_ms() - start > 10000) {
                mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("timeout waiting for DHCP to get IP address"));
            }
            mp_hal_delay_ms(100);
        }

        return mp_const_none;
    } else {
        // Release and stop any existing DHCP
        dhcp_release(netif);
        dhcp_stop(netif);
        // Set static IP addresses
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[0], 4, &items);
        netutils_parse_ipv4_addr(items[0], (uint8_t *)&netif->ip_addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], (uint8_t *)&netif->netmask, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], (uint8_t *)&netif->gw, NETUTILS_BIG);
        ip_addr_t dns;
        netutils_parse_ipv4_addr(items[3], (uint8_t *)&dns, NETUTILS_BIG);
        dns_setserver(0, &dns);
        return mp_const_none;
    }
}

#endif

#endif  // MICROPY_PY_NETWORK
