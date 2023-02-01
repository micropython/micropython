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

#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK && MICROPY_PY_LWIP

#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"

#include "lwip/init.h"

#if LWIP_VERSION_MAJOR >= 2

#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"

// Implementations of network methods that can be used by any interface.

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

#endif // LWIP_VERSION_MAJOR >= 2

#endif  // MICROPY_PY_NETWORK && MICROPY_PY_LWIP
