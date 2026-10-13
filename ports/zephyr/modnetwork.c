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

BUILD_ASSERT(IS_ENABLED(CONFIG_NETWORKING),
    "CONFIG_NETWORKING must be enabled to use MICROPY_PY_ZEPHYR_NETWORK");

#include <zephyr/net/hostname.h>
#include <zephyr/net/dns_resolve.h>

#include "py/runtime.h"
#include "py/objstr.h"

#include "shared/netutils/netutils.h"

#include "modnetwork.h"

#if MICROPY_PY_ZEPHYR_NETWORK_WLAN
extern const struct _mp_obj_type_t mod_network_zephyr_wlan_type;
#endif

/// \module network - network configuration
///
/// This module provides network drivers and routing configuration.

static mp_obj_t mod_network_hostname(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_str_from_cstr(net_hostname_get());
    } else {
        #ifdef CONFIG_NET_HOSTNAME_DYNAMIC
        size_t len;
        const char *str = mp_obj_str_get_data(args[0], &len);
        if (len > NET_HOSTNAME_MAX_LEN) {
            mp_raise_ValueError(NULL);
        }
        int ret = net_hostname_set(str, len);
        if (ret != 0) {
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set hostname: %d"), ret);
        }
        return mp_const_none;
        #else
        mp_raise_msg(&mp_type_NotImplementedError, MP_ERROR_TEXT("setting hostname not support, enable CONFIG_NET_HOSTNAME_DYNAMIC"));
        #endif
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_network_hostname_obj, 0, 1, mod_network_hostname);

static void mod_network_ipconfig_set_dns(const char **addrs) {
    int ret = dns_resolve_close(dns_resolve_get_default());
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set DNS (close): %d"), ret);
    }
    ret = dns_resolve_init(dns_resolve_get_default(), addrs, NULL);
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set DNS (init): %d"), ret);
    }
}

static mp_obj_t mod_network_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (kwargs->used == 0) {
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }
        switch (mp_obj_str_get_qstr(args[0])) {
            #if defined(CONFIG_DNS_RESOLVER)
            case MP_QSTR_dns: {
                struct dns_resolve_context *ctx = dns_resolve_get_default();
                mp_obj_t addrs[DNS_RESOLVER_MAX_POLL];
                size_t addr_cnt = 0;
                for (size_t i = 0; i < DNS_RESOLVER_MAX_POLL; i++) {
                    if (ctx->servers[i].dns_server.sa_family == NET_AF_INET) {
                        struct net_sockaddr_in *sin = net_sin(&ctx->servers[i].dns_server);
                        addrs[addr_cnt] = netutils_format_ipv4_addr(sin->sin_addr.s4_addr, NETUTILS_BIG);
                        addr_cnt++;
                    } else if (ctx->servers[i].dns_server.sa_family == NET_AF_INET6) {
                        struct net_sockaddr_in6 *sin = net_sin6(&ctx->servers[i].dns_server);
                        char buff[NET_IPV6_ADDR_LEN];
                        if (net_addr_ntop(NET_AF_INET6, sin, buff, NET_IPV6_ADDR_LEN) == NULL) {
                            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to convert ip to string"));
                        }
                        addrs[addr_cnt] = mp_obj_new_str_from_cstr(buff);
                        addr_cnt++;
                    }
                }
                return mp_obj_new_tuple(addr_cnt, addrs);
            }
            #endif
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }

        return mp_const_none;
    }

    for (size_t i = 0; i < kwargs->alloc; ++i) {
        if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
            mp_map_elem_t *e = &kwargs->table[i];
            switch (mp_obj_str_get_qstr(e->key)) {
                #if defined(CONFIG_DNS_RESOLVER)
                case MP_QSTR_dns: {
                    if (e->value == mp_const_none) {
                        int ret = dns_resolve_close(dns_resolve_get_default());
                        if (ret != 0) {
                            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to set DNS (close): %d"), ret);
                        }
                        break;
                    }
                    if (mp_obj_is_str_or_bytes(e->value)) {
                        const char *addrs[] = {
                            mp_obj_str_get_str(e->value),
                            NULL,
                        };
                        mod_network_ipconfig_set_dns(addrs);
                        break;
                    }
                    mp_obj_iter_buf_t iter_buf;
                    mp_obj_t iterable = mp_getiter(e->value, &iter_buf);
                    mp_obj_t item;
                    const char *addrs[DNS_RESOLVER_MAX_POLL + 1];
                    size_t addr_cnt = 0;

                    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION
                           && addr_cnt < DNS_RESOLVER_MAX_POLL) {
                        addrs[addr_cnt] = mp_obj_str_get_str(item);
                        addr_cnt++;
                    }

                    if (addr_cnt >= DNS_RESOLVER_MAX_POLL) {
                        mp_raise_ValueError(MP_ERROR_TEXT("too many servers"));
                    }

                    addrs[addr_cnt] = NULL;

                    mod_network_ipconfig_set_dns(addrs);
                    break;
                }
                #endif
                default: {
                    mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                    break;
                }
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mod_network_ipconfig_obj, 0, mod_network_ipconfig);

#ifdef CONFIG_WIFI_NM_WPA_SUPPLICANT

#include <zephyr/net/wifi.h>
#include "utils/common.h"
#include "sha1.h"

/* Encoder function for passwords as pbkdf2 for WiFi */
static mp_obj_t mod_network_encode_pbkdf2(size_t n_args, const mp_obj_t *args) {
    uint8_t buf[WIFI_PSK_PBKDF2_KEY_LEN];

    /* Self document */
    if (n_args < 2) {
        mp_raise_TypeError(MP_ERROR_TEXT("secure_key = encode_pbkdf2(\"ssid\", \"key\")"));
    }

    if (!mp_obj_is_str_or_bytes(args[0]) || !mp_obj_is_str_or_bytes(args[0])) {
        mp_raise_TypeError(MP_ERROR_TEXT("inputs must be bytes or str"));
    }

    GET_STR_DATA_LEN(args[0], ssid_str, ssid_len);

    int ret = pbkdf2_sha1(mp_obj_str_get_str(args[1]), ssid_str, ssid_len, 4096, buf, sizeof(buf));
    if (ret != 0) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("failed to compute pbkdf2: %d"), ret);
    }

    return mp_obj_new_bytes(buf, WIFI_PSK_PBKDF2_KEY_LEN);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_network_encode_pbkdf2_obj, 0, 2, mod_network_encode_pbkdf2);

#endif

static const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR_hostname),            MP_ROM_PTR(&mod_network_hostname_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig),            MP_ROM_PTR(&mod_network_ipconfig_obj) },
    /* Types */
    #if MICROPY_PY_ZEPHYR_NETWORK_WLAN
    { MP_ROM_QSTR(MP_QSTR_WLAN),                MP_ROM_PTR(&mod_network_zephyr_wlan_type) },
    #endif

    #ifdef CONFIG_WIFI_NM_WPA_SUPPLICANT
    { MP_ROM_QSTR(MP_QSTR_encode_pbkdf2),       MP_ROM_PTR(&mod_network_encode_pbkdf2_obj) },
    #endif

    /* Constants */
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE),           MP_ROM_INT(NETWORK_STAT_IDLE) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING),     MP_ROM_INT(NETWORK_STAT_CONNECTING) },
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(NETWORK_STAT_WRONG_PASSWORD) },
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND),    MP_ROM_INT(NETWORK_STAT_NO_AP_FOUND) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL),   MP_ROM_INT(NETWORK_STAT_CONNECT_FAIL) },
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP),         MP_ROM_INT(NETWORK_STAT_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_STA_IF),              MP_ROM_INT(MOD_NETWORK_STA_IF) },
    { MP_ROM_QSTR(MP_QSTR_AP_IF),               MP_ROM_INT(MOD_NETWORK_AP_IF) },
};

static MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};

MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network);

#endif  // MICROPY_PY_ZEPHYR_NETWORK
