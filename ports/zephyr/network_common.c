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

#include "py/mpconfig.h"

#if MICROPY_PY_ZEPHYR_NETWORK

#include <zephyr/kernel.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/dhcpv4_server.h>
#include <zephyr/net/dhcpv6_server.h>

#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/parsenum.h"
#include "py/mperrno.h"

#include "shared/netutils/netutils.h"

#include "modnetwork.h"

/* Atomic bits identifiers */
#define NETWORK_ZEPHYR_STATE_DHCPV4_EN  0
#define NETWORK_ZEPHYR_STATE_DHCPV6_EN  1

static mp_obj_t network_zephyr_ifconfig(size_t n_args, const mp_obj_t *arg) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("deprecated, use ipconfig"));
    return MP_OBJ_NULL;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_ifconfig_obj, 1, 2, network_zephyr_ifconfig);

static mp_obj_t network_zephyr_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_zephyr_t *self = MP_OBJ_TO_PTR(args[0]);

    struct net_if_config *cfg = net_if_get_config(self->net_if);
    if (cfg == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("no interface configuration"));
    }

    if (kwargs->used == 0) {
        if (n_args == 1) {
            /* List supported keys */
            mp_obj_t tobj[] = {
                #if defined(CONFIG_NET_NATIVE_IPV4)
                #if defined(CONFIG_NET_DHCPV4)
                MP_OBJ_NEW_QSTR(MP_QSTR_dhcp4),
                MP_OBJ_NEW_QSTR(MP_QSTR_has_dhcp4),
                #endif /* CONFIG_NET_DHCPV4 */
                MP_OBJ_NEW_QSTR(MP_QSTR_addr4),
                MP_OBJ_NEW_QSTR(MP_QSTR_gw4),
                #endif /* CONFIG_NET_NATIVE_IPV4 */
                #if defined(CONFIG_NET_NATIVE_IPV6)
                #if defined(CONFIG_NET_DHCPV6)
                MP_OBJ_NEW_QSTR(MP_QSTR_dhcp6),
                MP_OBJ_NEW_QSTR(MP_QSTR_has_dhcp6),
                #endif /* CONFIG_NET_DHCPV6 */
                MP_OBJ_NEW_QSTR(MP_QSTR_addr6),
                #endif /* CONFIG_NET_NATIVE_IPV6 */
            };
            return mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj);
        }
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            #if defined(CONFIG_NET_NATIVE_IPV4)
            #if defined(CONFIG_NET_DHCPV4)
            case MP_QSTR_dhcp4: {
                if (cfg->dhcpv4.state == NET_DHCPV4_DISABLED) {
                    return mp_const_false;
                } else {
                    return mp_const_true;
                }
            }
            case MP_QSTR_has_dhcp4: {
                if (cfg->dhcpv4.state == NET_DHCPV4_BOUND) {
                    return mp_const_true;
                } else {
                    return mp_const_false;
                }
            }
            #endif /* CONFIG_NET_DHCPV4 */
            case MP_QSTR_addr4: {
                if (cfg->ip.ipv4 == NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("no IPv4 configuration"));
                }
                /* Can have multiple addresses! */
                mp_obj_t addresses = mp_obj_new_list(0, NULL);
                size_t cnt = 0;
                for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
                    if (cfg->ip.ipv4->unicast[i].ipv4.is_used) {
                        struct in_addr addr = cfg->ip.ipv4->unicast[i].ipv4.address.in_addr;
                        struct in_addr netmask = cfg->ip.ipv4->unicast[i].netmask;
                        mp_obj_t tobj[] = {
                            netutils_format_ipv4_addr((uint8_t *)&addr, NETUTILS_BIG),
                            netutils_format_ipv4_addr((uint8_t *)&netmask, NETUTILS_BIG),
                        };
                        mp_obj_list_append(addresses, mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj));
                        cnt++;
                    }
                }

                if (cnt == 0) {
                    mp_raise_ValueError(MP_ERROR_TEXT("no IPv4 address"));
                }

                if (cnt > 1) {
                    return addresses;
                }

                return mp_obj_subscr(addresses, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL);
            }
            case MP_QSTR_gw4: {
                if (cfg->ip.ipv4 == NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("no IPv4 configuration"));
                }
                return netutils_format_ipv4_addr((uint8_t *)&cfg->ip.ipv4->gw, NETUTILS_BIG);
            }
            #endif /* CONFIG_NET_NATIVE_IPV4 */
            #if defined(CONFIG_NET_NATIVE_IPV6)
            #if defined(CONFIG_NET_DHCPV6)
            case MP_QSTR_dhcp6: {
                if (cfg->dhcpv6.state == NET_DHCPV6_DISABLED) {
                    return mp_const_false;
                } else {
                    return mp_const_true;
                }
            }
            case MP_QSTR_has_dhcp6: {
                if (cfg->dhcpv6.state == NET_DHCPV6_BOUND) {
                    return mp_const_true;
                } else {
                    return mp_const_false;
                }
            }
            #endif /* CONFIG_NET_DHCPV6 */
            case MP_QSTR_addr6: {
                if (cfg->ip.ipv6 == NULL) {
                    mp_raise_ValueError(MP_ERROR_TEXT("no IPv6 configuration"));
                }
                mp_obj_t addresses = mp_obj_new_list(0, NULL);
                size_t cnt = 0;
                for (int i = 0; i < NET_IF_MAX_IPV6_ADDR; i++) {
                    if (cfg->ip.ipv6->unicast[i].is_used) {
                        struct net_in6_addr addr = cfg->ip.ipv6->unicast[0].address.net_in6_addr;
                        char buff[NET_IPV6_ADDR_LEN];
                        if (net_addr_ntop(NET_AF_INET6, &addr, buff, NET_IPV6_ADDR_LEN) == NULL) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to convert ip to string"));
                        }
                        int addr_state = 0x0;
                        if (cfg->ip.ipv6->unicast[0].addr_state == NET_ADDR_TENTATIVE) {
                            addr_state = 0xf;
                        } else if (cfg->ip.ipv6->unicast[0].addr_state == NET_ADDR_PREFERRED) {
                            addr_state = 0x30;
                        } else if (cfg->ip.ipv6->unicast[0].addr_state == NET_ADDR_DEPRECATED) {
                            addr_state = 0x10;
                        }
                        mp_obj_t tobj[] = {
                            mp_obj_new_str_from_cstr(buff),
                            mp_obj_new_int(addr_state),
                            #if defined(CONFIG_NET_IPV6_PE)
                            mp_obj_new_int(cfg->ip.ipv6->unicast[0].address.addr_preferred_lifetime),
                            mp_obj_new_int(cfg->ip.ipv6->unicast[0].address.addr_timeout),
                            #else
                            mp_obj_new_int(0),
                            mp_obj_new_int(0),
                            #endif
                        };
                        mp_obj_list_append(addresses, mp_obj_new_tuple(ARRAY_SIZE(tobj), tobj));
                        cnt++;
                    }
                }

                if (cnt == 0) {
                    mp_raise_ValueError(MP_ERROR_TEXT("no IPv6 address"));
                }

                if (cnt > 1) {
                    return addresses;
                }

                return mp_obj_subscr(addresses, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL);
            }
            #endif /* CONFIG_NET_NATIVE_IPV6 */
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
        return mp_const_none;
    } else {
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }
        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    #if defined(CONFIG_NET_NATIVE_IPV4)
                    #if defined(CONFIG_NET_DHCPV4)
                    case MP_QSTR_dhcp4: {
                        if (mp_obj_is_true(e->value)) {
                            if (cfg->dhcpv4.state == NET_DHCPV4_DISABLED) {
                                net_dhcpv4_start(self->net_if);
                            } else {
                                net_dhcpv4_restart(self->net_if);
                            }
                        } else {
                            net_dhcpv4_stop(self->net_if);
                        }
                        cfg = net_if_get_config(self->net_if);
                        if (mp_obj_is_true(e->value) && cfg->dhcpv4.state == NET_DHCPV4_DISABLED) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to enable DHCPv4"));
                        } else if (!mp_obj_is_true(e->value) && cfg->dhcpv4.state != NET_DHCPV4_DISABLED) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to disable DHCPv4"));
                        }
                        break;
                    }
                    #endif /* CONFIG_NET_DHCPV4 */
                    case MP_QSTR_addr4: {
                        /* Remove other addresses from net_if when using None or when can have only 1 */
                        if (e->value == mp_const_none || NET_IF_MAX_IPV4_ADDR < 2) {
                            if (cfg->ip.ipv4 != NULL) {
                                for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
                                    struct net_if_addr *ifaddr = &cfg->ip.ipv4->unicast[i].ipv4;
                                    if (!ifaddr->is_used) {
                                        continue;
                                    }
                                    /* Copy: rm() invalidates the slot */
                                    struct in_addr addr = ifaddr->address.in_addr;
                                    /* Ignore return if not specifically clearing addresses */
                                    bool success = net_if_ipv4_addr_rm(self->net_if, &addr);
                                    if (!success) {
                                        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to clear IPv4 addresses"));
                                    }
                                }
                            }
                            if (e->value == mp_const_none) {
                                break;
                            }
                        }

                        struct net_in_addr in_addr;
                        struct net_in_addr in_netmask;

                        if (mp_obj_is_str(e->value)) {
                            netutils_parse_ipv4_addr(e->value, in_addr.s4_addr, NETUTILS_BIG);
                            size_t addr_len;
                            const char *input_str = mp_obj_str_get_data(e->value, &addr_len);
                            char *split = strchr(input_str, '/');
                            if (split) {
                                mp_obj_t prefix_obj = mp_parse_num_integer(split + 1, strlen(split + 1), 10, NULL);
                                int prefix_bits = mp_obj_get_int(prefix_obj);
                                uint32_t mask = -(1u << (32 - prefix_bits));
                                in_netmask.s_addr = (prefix_bits == 0 ? 0 : net_htonl(mask));
                            } else {
                                netutils_parse_ipv4_addr(mp_obj_new_str_from_cstr("255.255.255.0"), in_netmask.s4_addr, NETUTILS_BIG);
                            }
                        } else if (e->value != mp_const_none) {
                            mp_obj_t *items;
                            mp_obj_get_array_fixed_n(e->value, 2, &items);
                            netutils_parse_ipv4_addr(items[0], in_addr.s4_addr, NETUTILS_BIG);
                            netutils_parse_ipv4_addr(items[1], in_netmask.s4_addr, NETUTILS_BIG);
                        }

                        struct net_if_addr *addr = net_if_ipv4_addr_add(self->net_if, &in_addr, NET_ADDR_MANUAL, 0);
                        if (addr == NULL) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set IPv4 address"));
                        }

                        bool success = net_if_ipv4_set_netmask_by_addr(self->net_if, &in_addr, &in_netmask);
                        if (!success) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set IPv4 netmask"));
                        }

                        break;
                    }
                    case MP_QSTR_gw4: {
                        struct net_in_addr in_addr;
                        netutils_parse_ipv4_addr(e->value, in_addr.s4_addr, NETUTILS_BIG);
                        net_if_ipv4_set_gw(self->net_if, &in_addr);
                        break;
                    }
                    #endif /* CONFIG_NET_NATIVE_IPV4 */
                    #if defined(CONFIG_NET_NATIVE_IPV6)
                    #if defined(CONFIG_NET_DHCPV6)
                    case MP_QSTR_dhcp6: {
                        if (mp_obj_is_true(e->value)) {
                            if (cfg->dhcpv6.state == NET_DHCPV6_DISABLED) {
                                struct net_dhcpv6_params params = {
                                    .request_addr = true,
                                    .request_prefix = true,
                                };
                                net_dhcpv6_start(self->net_if, &params);
                            } else {
                                net_dhcpv6_restart(self->net_if);
                            }
                        } else {
                            net_dhcpv6_stop(self->net_if);
                        }
                        cfg = net_if_get_config(self->net_if);
                        if (mp_obj_is_true(e->value) && cfg->dhcpv6.state == NET_DHCPV6_DISABLED) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to enable DHCPv6"));
                        } else if (!mp_obj_is_true(e->value) && cfg->dhcpv6.state != NET_DHCPV6_DISABLED) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to disable DHCPv6"));
                        }
                        break;
                    }
                    #endif /* CONFIG_NET_DHCPV6 */
                    case MP_QSTR_addr6: {
                        /* Remove other addresses from net_if when using None or when can have only 1 */
                        if (e->value == mp_const_none || NET_IF_MAX_IPV6_ADDR < 2) {
                            if (cfg->ip.ipv6 != NULL) {
                                for (int i = 0; i < NET_IF_MAX_IPV6_ADDR; i++) {
                                    struct net_if_addr *ifaddr = &cfg->ip.ipv6->unicast[i];
                                    if (!ifaddr->is_used) {
                                        continue;
                                    }
                                    struct net_in6_addr addr = ifaddr->address.net_in6_addr;
                                    bool success = net_if_ipv6_addr_rm(self->net_if, &addr);
                                    if (!success) {
                                        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to clear IPv6 addresses"));
                                    }
                                }
                            }
                            if (e->value == mp_const_none) {
                                break;
                            }
                        }

                        struct net_in6_addr in_addr;

                        int ret = net_addr_pton(NET_AF_INET6, mp_obj_str_get_str(e->value), &in_addr);

                        if (ret != 0) {
                            mp_raise_msg_varg(&mp_type_RuntimeError,
                                MP_ERROR_TEXT("failed to parse IPv6: %d"), ret);
                        }

                        struct net_if_addr *addr = net_if_ipv6_addr_add(self->net_if, &in_addr, NET_ADDR_MANUAL, 0);
                        if (addr == NULL) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set IPv6 address"));
                        }

                        break;
                    }
                    #endif /* CONFIG_NET_NATIVE_IPV6 */
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

MP_DEFINE_CONST_FUN_OBJ_KW(network_zephyr_ipconfig_obj, 1, network_zephyr_ipconfig);

/* Handle IP status */
mp_obj_t network_zephyr_status_handler(size_t n_args, const mp_obj_t *args) {
    network_zephyr_t *self = MP_OBJ_TO_PTR(args[0]);

    struct net_if_config *cfg = net_if_get_config(self->net_if);

    /* Nothing, so we must be idle */
    if (cfg == NULL) {
        return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_IDLE);
    }

    #if defined(CONFIG_NET_DHCPV4) && defined(CONFIG_NET_NATIVE_IPV4)
    /* DHCP ip, so we must be fully setup */
    if (cfg->dhcpv4.state == NET_DHCPV4_BOUND) {
        return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_GOT_IP);
    }
    /* DHCP is disabled but we have an IP, we are fully setup */
    if (cfg->dhcpv4.state == NET_DHCPV4_DISABLED) {
        for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
            if (cfg->ip.ipv4->unicast[i].ipv4.is_used) {
                return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_GOT_IP);
            }
        }
    }
    #endif

    #if defined(CONFIG_NET_DHCPV6) && defined(CONFIG_NET_NATIVE_IPV6)
    if (cfg->dhcpv6.state == NET_DHCPV6_BOUND) {
        return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_GOT_IP);
    }
    if (cfg->dhcpv6.state == NET_DHCPV6_DISABLED) {
        for (int i = 0; i < NET_IF_MAX_IPV6_ADDR; i++) {
            if (cfg->ip.ipv6->unicast[i].is_used) {
                return MP_OBJ_NEW_SMALL_INT(NETWORK_STAT_GOT_IP);
            }
        }
    }
    #endif

    return mp_const_none;
}

#ifdef CONFIG_NET_DHCPV4_SERVER
static mp_obj_t network_zephyr_dhcp4_server(size_t n_args, const mp_obj_t *args) {
    network_zephyr_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        if (atomic_test_bit(&self->state, NETWORK_ZEPHYR_STATE_DHCPV4_EN)) {
            return mp_const_true;
        } else {
            return mp_const_false;
        }
    }

    struct net_if_config *cfg = net_if_get_config(self->net_if);

    if (mp_obj_is_true(args[1])) {
        if (cfg->ip.ipv4 == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("no IPv4 address to use for DHCP server"));
        }

        struct in_addr *in_addr = NULL;
        for (int i = 0; i < NET_IF_MAX_IPV4_ADDR; i++) {
            struct net_if_addr *ifaddr = &cfg->ip.ipv4->unicast[i].ipv4;
            if (!ifaddr->is_used) {
                continue;
            } else {
                in_addr = &ifaddr->address.in_addr;
                break;
            }
        }

        if (in_addr == NULL) {
            mp_raise_ValueError(MP_ERROR_TEXT("no IPv4 address to use for DHCP server"));
        }

        /* Copy to not overwrite */
        struct in_addr addr = *in_addr;

        net_if_ipv4_set_gw(self->net_if, &addr);
        addr.s4_addr[3] += 10; /* Starting IPv4 address for DHCPv4 address pool. */

        int ret = net_dhcpv4_server_start(self->net_if, &addr);
        if (ret == -EALREADY) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPv4 DHCP server is already started"));
        } else if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("failed to start IPv4 DHCP server: %d"), ret);
        }
        atomic_set_bit(&self->state, NETWORK_ZEPHYR_STATE_DHCPV4_EN);
    } else {
        int ret = net_dhcpv4_server_stop(self->net_if);
        if (ret != 0 && ret != -ENOENT) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("failed to stop IPv4 DHCP server: %d"), ret);
        }
        atomic_clear_bit(&self->state, NETWORK_ZEPHYR_STATE_DHCPV4_EN);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_dhcp4_server_obj, 1, 2, network_zephyr_dhcp4_server);
#endif

#ifdef CONFIG_NET_DHCPV6_SERVER
static mp_obj_t network_zephyr_dhcp6_server(size_t n_args, const mp_obj_t *args) {
    network_zephyr_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        if (atomic_test_bit(&self->state, NETWORK_ZEPHYR_STATE_DHCPV6_EN)) {
            return mp_const_true;
        } else {
            return mp_const_false;
        }
    }

    if (mp_obj_is_true(args[1])) {
        /* From dhcp6_pd sample */
        struct net_dhcpv6_server_params params = {
            /* 2001:db8:beef::/56, 2001:db8:beef:100::/56, ... */
            .prefix = { { { 0x20, 0x01, 0x0d, 0xb8, 0xbe, 0xef, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0 } } },
            .prefix_len = 56,
            /* 2001:db8:abcd::1, 2001:db8:abcd::2, ... */
            .addr = { { { 0x20, 0x01, 0x0d, 0xb8, 0xab, 0xcd, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0 } } },
            .offer_addr = true,
            .offer_prefix = true,
        };
        int ret = net_dhcpv6_server_start(self->net_if, &params);
        if (ret == -EALREADY) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("IPv6 DHCP server is already started"));
        } else if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("failed to start IPv6 DHCP server: %d"), ret);
        }
        atomic_set_bit(&self->state, NETWORK_ZEPHYR_STATE_DHCPV6_EN);
    } else {
        int ret = net_dhcpv6_server_stop(self->net_if);
        if (ret != 0 && ret != -ENOENT) {
            mp_raise_msg_varg(&mp_type_RuntimeError,
                MP_ERROR_TEXT("failed to stop IPv6 DHCP server: %d"), ret);
        }
        atomic_clear_bit(&self->state, NETWORK_ZEPHYR_STATE_DHCPV6_EN);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_zephyr_dhcp6_server_obj, 1, 2, network_zephyr_dhcp6_server);
#endif

#endif /* MICROPY_PY_NETWORK */
