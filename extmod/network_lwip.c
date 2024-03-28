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
#include "py/parsenum.h"

#if MICROPY_PY_NETWORK && MICROPY_PY_LWIP

#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"

#include "lwip/init.h"

#if LWIP_VERSION_MAJOR >= 2

#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/nd6.h"
#include "lwip/dhcp6.h"
#include "lwip/prot/dhcp.h"
#include "lwip/prot/dhcp6.h"
#include <string.h>
#include <stdlib.h>

int mp_mod_network_prefer_dns_use_ip_version = 4;

// Implementations of network methods that can be used by any interface.

// This function provides the implementation of nic.ifconfig, is deprecated and will be removed.
// Use network.ipconfig and nic.ipconfig instead.
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

// This function provides the common implementation of network.ipconfig
mp_obj_t mod_network_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[0])) {
            case MP_QSTR_dns: {
                char addr_str[IPADDR_STRLEN_MAX];
                ipaddr_ntoa_r(dns_getserver(0), addr_str, sizeof(addr_str));
                return mp_obj_new_str(addr_str, strlen(addr_str));
            }
            case MP_QSTR_prefer: {
                return MP_OBJ_NEW_SMALL_INT(mp_mod_network_prefer_dns_use_ip_version);
            }
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
    } else {
        // Set config value(s)
        if (n_args != 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_dns: {
                        ip_addr_t dns;
                        size_t addr_len;
                        const char *addr_str = mp_obj_str_get_data(e->value, &addr_len);
                        if (!ipaddr_aton(addr_str, &dns)) {
                            mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments as dns server"));
                        }
                        dns_setserver(0, &dns);
                        break;
                    }
                    case MP_QSTR_prefer: {
                        int value = mp_obj_get_int(e->value);
                        if (value != 4 && value != 6) {
                            mp_raise_ValueError(MP_ERROR_TEXT("invalid prefer argument"));
                        }
                        mp_mod_network_prefer_dns_use_ip_version = value;
                        break;
                    }
                    default: {
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                    }
                }
            }
        }
    }
    return mp_const_none;
}

mp_obj_t mod_network_nic_ipconfig(struct netif *netif, size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[0])) {
            case MP_QSTR_dhcp4: {
                struct dhcp *dhcp = netif_dhcp_data(netif);
                return mp_obj_new_bool(dhcp != NULL && dhcp->state != DHCP_STATE_OFF);
            }
            case MP_QSTR_has_dhcp4: {
                return mp_obj_new_bool(dhcp_supplied_address(netif));
            }
            #if LWIP_IPV6_DHCP6
            case MP_QSTR_dhcp6: {
                struct dhcp6 *dhcp = netif_dhcp6_data(netif);
                return mp_obj_new_bool(dhcp != NULL && dhcp->state != DHCP6_STATE_OFF);
            }
            #endif
            #if LWIP_IPV6_AUTOCONFIG
            case MP_QSTR_autoconf6: {
                return netif->ip6_autoconfig_enabled ?  mp_const_true : mp_const_false;
            }
            case MP_QSTR_has_autoconf6: {
                int found = 0;
                for (int i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
                        !netif_ip6_addr_isstatic(netif, i)) {
                        found = 1;
                        break;
                    }
                }
                if (found) {
                    break;
                }
                return mp_obj_new_bool(found);
            }
            #endif
            case MP_QSTR_addr4: {
                mp_obj_t tuple[2] = {
                    netutils_format_ipv4_addr((uint8_t *)&netif->ip_addr, NETUTILS_BIG),
                    netutils_format_ipv4_addr((uint8_t *)&netif->netmask, NETUTILS_BIG),
                };
                return mp_obj_new_tuple(2, tuple);
            }
            #if LWIP_IPV6
            case MP_QSTR_addr6: {
                mp_obj_t addrs[LWIP_IPV6_NUM_ADDRESSES];
                size_t n_addrs = 0;
                for (int i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                    if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i))) {
                        char addr_str[IPADDR_STRLEN_MAX];
                        ipaddr_ntoa_r(netif_ip_addr6(netif, i), addr_str, sizeof(addr_str));
                        mp_obj_t tuple[4] = {
                            mp_obj_new_str(addr_str, strlen(addr_str)),
                            MP_OBJ_NEW_SMALL_INT(netif_ip6_addr_state(netif, i)),
                            MP_OBJ_NEW_SMALL_INT(netif_ip6_addr_pref_life(netif, i)), // preferred
                            MP_OBJ_NEW_SMALL_INT(netif_ip6_addr_valid_life(netif, i))
                        };
                        addrs[n_addrs++] = mp_obj_new_tuple(4, tuple);
                    }
                }
                return mp_obj_new_list(n_addrs, addrs);
            }
            #endif
            case MP_QSTR_gw4: {
                return netutils_format_ipv4_addr((uint8_t *)&netif->gw, NETUTILS_BIG);
            }
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
        return mp_const_none;
    } else {
        // Set config value(s)
        if (n_args != 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_dhcp4: {
                        if (mp_obj_is_true(e->value)) {
                            if (dhcp_supplied_address(netif)) {
                                dhcp_renew(netif);
                            } else {
                                dhcp_release_and_stop(netif);
                                dhcp_start(netif);
                            }
                        } else {
                            dhcp_release_and_stop(netif);
                        }
                        break;
                    }
                    #if LWIP_IPV6_DHCP6
                    case MP_QSTR_dhcp6: {
                        dhcp6_disable(netif);
                        dhcp6_enable_stateless(netif);
                        break;
                    }
                    #endif
                    #if LWIP_IPV6_AUTOCONFIG
                    case MP_QSTR_autoconf6: {
                        netif_set_ip6_autoconfig_enabled(netif, mp_obj_is_true(e->value));
                        if (mp_obj_is_true(e->value)) {
                            nd6_restart_netif(netif);
                        } else {
                            // Clear out any non-static addresses, skip link-local address in slot 0
                            for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                                if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
                                    !netif_ip6_addr_isstatic(netif, i)) {
                                    netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
                                }
                            }
                        }
                        break;
                    }
                    #endif
                    case MP_QSTR_addr4:
                    case MP_QSTR_addr6: {
                        ip_addr_t ip_addr;
                        ip_addr_t netmask;
                        int prefix_bits = 32;
                        if (e->value != mp_const_none && mp_obj_is_str(e->value)) {
                            size_t addr_len;
                            const char *input_str = mp_obj_str_get_data(e->value, &addr_len);
                            char plain_ip[IPADDR_STRLEN_MAX];
                            char *split = strchr(input_str, '/');
                            const char *addr_str = input_str;
                            int to_copy = MIN(sizeof(plain_ip) - 1, addr_len);
                            memcpy(plain_ip, addr_str, to_copy);
                            if (split) {
                                if (split - addr_str < to_copy) {
                                    to_copy = split - addr_str;
                                }
                                mp_obj_t prefix_obj = mp_parse_num_integer(split + 1, strlen(split + 1), 10, NULL);
                                prefix_bits = mp_obj_get_int(prefix_obj);
                                if (mp_obj_str_get_qstr(args[0]) == MP_QSTR_addr4) {
                                    uint32_t mask = -(1u << (32 - prefix_bits));
                                    ip_addr_set_ip4_u32_val(netmask, ((mask & 0xFF) << 24) | ((mask & 0xFF00) << 8) | ((mask >> 8) & 0xFF00) | ((mask >> 24) & 0xFF));
                                }
                            } else {
                                netmask = netif->netmask;
                            }
                            plain_ip[to_copy] = '\0';
                            if (!ipaddr_aton(plain_ip, &ip_addr)) {
                                mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
                            }
                            if ((mp_obj_str_get_qstr(args[0]) == MP_QSTR_addr6) != IP_IS_V6(&ip_addr)
                                || (mp_obj_str_get_qstr(args[0]) == MP_QSTR_addr4) != IP_IS_V4(&ip_addr)) {
                                mp_raise_ValueError(MP_ERROR_TEXT("invalid address type"));
                            }
                        } else if (e->value != mp_const_none) {
                            mp_obj_t *items;
                            mp_obj_get_array_fixed_n(e->value, 2, &items);
                            size_t addr_len;
                            const char *ip_addr_str = mp_obj_str_get_data(items[0], &addr_len);
                            const char *netmask_str = mp_obj_str_get_data(items[1], &addr_len);
                            if (!ipaddr_aton(ip_addr_str, &ip_addr)) {
                                mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
                            }
                            if (!ipaddr_aton(netmask_str, &netmask)) {
                                mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
                            }
                            if (!IP_IS_V4(&ip_addr) || !IP_IS_V4(&netmask)) {
                                mp_raise_ValueError(MP_ERROR_TEXT("invalid address type"));
                            }
                        }
                        if (mp_obj_str_get_qstr(args[0]) == MP_QSTR_addr4) {
                            if (e->value != mp_const_none) {
                                netif->ip_addr = ip_addr;
                                netif->netmask = netmask;
                            } else {
                                ip4_addr_set_any(ip_2_ip4(&netif->ip_addr));
                                ip4_addr_set_any(ip_2_ip4(&netif->netmask));
                            }
                        #if LWIP_IPV6
                        } else if (mp_obj_str_get_qstr(args[0]) == MP_QSTR_addr6) {
                            // Clear out any existing static addresses. Address 0 comes from autoconf.
                            for (i = 1; i < LWIP_IPV6_NUM_ADDRESSES; i++) {
                                if (ip6_addr_isvalid(netif_ip6_addr_state(netif, i)) &&
                                    netif_ip6_addr_isstatic(netif, i)) {
                                    netif_ip6_addr_set_state(netif, i, IP6_ADDR_INVALID);
                                }
                            }
                            if (e->value != mp_const_none) {
                                s8_t free_idx;
                                netif_add_ip6_address(netif, ip_2_ip6(&ip_addr), &free_idx);
                                netif_ip6_addr_set_valid_life(netif, free_idx, IP6_ADDR_LIFE_STATIC);
                                netif_ip6_addr_set_pref_life(netif, free_idx, IP6_ADDR_LIFE_STATIC);
                                netif_ip6_addr_set_state(netif, free_idx, IP6_ADDR_PREFERRED);
                            }
                        #endif
                        }
                        break;
                    }
                    case MP_QSTR_gw4: {
                        ip_addr_t ip_addr;
                        size_t addr_len;
                        const char *addr_str = mp_obj_str_get_data(e->value, &addr_len);
                        if (!ipaddr_aton(addr_str, &ip_addr)) {
                            mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
                        }
                        if (IP_IS_V4(&ip_addr)) {
                            netif->gw = ip_addr;
                        } else {
                            mp_raise_ValueError(MP_ERROR_TEXT("invalid address type"));
                        }
                        break;
                    }
                    default: {
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                    }
                }
            }
        }
    }
    return mp_const_none;
}

#endif // LWIP_VERSION_MAJOR >= 2

#endif  // MICROPY_PY_NETWORK && MICROPY_PY_LWIP
