/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 * and Mnemote Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Nick Moore @mnemote
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 *
 * Based on esp8266/modnetwork.c which is Copyright (c) 2015 Paul Sokolovsky
 * And the ESP IDF example code which is Public Domain / CC0
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

#include <string.h>

#include "py/runtime.h"
#include "py/parsenum.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "modnetwork.h"

#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"

NORETURN void esp_exceptions_helper(esp_err_t e) {
    switch (e) {
        case ESP_ERR_WIFI_NOT_INIT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Initialized"));
        case ESP_ERR_WIFI_NOT_STARTED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Started"));
        case ESP_ERR_WIFI_NOT_STOPPED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Stopped"));
        case ESP_ERR_WIFI_IF:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Interface"));
        case ESP_ERR_WIFI_MODE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Mode"));
        case ESP_ERR_WIFI_STATE:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal State Error"));
        case ESP_ERR_WIFI_CONN:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal Error"));
        case ESP_ERR_WIFI_NVS:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal NVS Error"));
        case ESP_ERR_WIFI_MAC:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid MAC Address"));
        case ESP_ERR_WIFI_SSID:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi SSID Invalid"));
        case ESP_ERR_WIFI_PASSWORD:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Password"));
        case ESP_ERR_WIFI_TIMEOUT:
            mp_raise_OSError(MP_ETIMEDOUT);
        case ESP_ERR_WIFI_WAKE_FAIL:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Wakeup Failure"));
        case ESP_ERR_WIFI_WOULD_BLOCK:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Would Block"));
        case ESP_ERR_WIFI_NOT_CONNECT:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Connected"));
        default:
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Wifi Unknown Error 0x%04x"), e);
    }
}

static mp_obj_t esp_initialize() {
    static int initialized = 0;
    if (!initialized) {
        esp_exceptions(esp_netif_init());
        initialized = 1;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(esp_network_initialize_obj, esp_initialize);

static mp_obj_t esp_ifconfig(size_t n_args, const mp_obj_t *args) {
    base_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    esp_netif_ip_info_t info;
    esp_netif_dns_info_t dns_info;
    esp_netif_get_ip_info(self->netif, &info);
    esp_netif_get_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &dns_info);
    if (n_args == 1) {
        // get
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)&info.ip, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&info.netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&info.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&dns_info.ip, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        if (mp_obj_is_type(args[1], &mp_type_tuple) || mp_obj_is_type(args[1], &mp_type_list)) {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(args[1], 4, &items);
            netutils_parse_ipv4_addr(items[0], (void *)&info.ip, NETUTILS_BIG);
            if (mp_obj_is_integer(items[1])) {
                // allow numeric netmask, i.e.:
                // 24 -> 255.255.255.0
                // 16 -> 255.255.0.0
                // etc...
                uint32_t *m = (uint32_t *)&info.netmask;
                *m = esp_netif_htonl(0xffffffff << (32 - mp_obj_get_int(items[1])));
            } else {
                netutils_parse_ipv4_addr(items[1], (void *)&info.netmask, NETUTILS_BIG);
            }
            netutils_parse_ipv4_addr(items[2], (void *)&info.gw, NETUTILS_BIG);
            netutils_parse_ipv4_addr(items[3], (void *)&dns_info.ip, NETUTILS_BIG);
            // To set a static IP we have to disable DHCP first
            if (self->if_id == ESP_IF_WIFI_STA || self->if_id == ESP_IF_ETH) {
                esp_err_t e = esp_netif_dhcpc_stop(self->netif);
                if (e != ESP_OK && e != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
                    esp_exceptions_helper(e);
                }
                esp_exceptions(esp_netif_set_ip_info(self->netif, &info));
                esp_exceptions(esp_netif_set_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &dns_info));
            } else if (self->if_id == ESP_IF_WIFI_AP) {
                esp_err_t e = esp_netif_dhcps_stop(self->netif);
                if (e != ESP_OK && e != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
                    esp_exceptions_helper(e);
                }
                esp_exceptions(esp_netif_set_ip_info(self->netif, &info));
                esp_exceptions(esp_netif_set_dns_info(self->netif, ESP_NETIF_DNS_MAIN, &dns_info));
                esp_exceptions(esp_netif_dhcps_start(self->netif));
            }
        } else {
            // check for the correct string
            const char *mode = mp_obj_str_get_str(args[1]);
            if ((self->if_id != ESP_IF_WIFI_STA && self->if_id != ESP_IF_ETH) || strcmp("dhcp", mode)) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
            }
            esp_exceptions(esp_netif_dhcpc_start(self->netif));
        }
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_network_ifconfig_obj, 1, 2, esp_ifconfig);

static mp_obj_t esp_network_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
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
MP_DEFINE_CONST_FUN_OBJ_KW(esp_network_ipconfig_obj, 0, esp_network_ipconfig);

static mp_obj_t esp_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    base_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    esp_netif_ip_info_t info;
    esp_netif_get_ip_info(self->netif, &info);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_dhcp4: {
                if (self->if_id == ESP_IF_WIFI_STA || self->if_id == ESP_IF_ETH) {
                    esp_netif_dhcp_status_t status;
                    esp_exceptions(esp_netif_dhcpc_get_status(self->netif, &status));
                    return mp_obj_new_bool(status == ESP_NETIF_DHCP_STARTED);
                } else {
                    mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                    break;
                }
            }
            case MP_QSTR_addr4: {
                mp_obj_t tuple[2] = {
                    netutils_format_ipv4_addr((uint8_t *)&info.ip, NETUTILS_BIG),
                    netutils_format_ipv4_addr((uint8_t *)&info.netmask, NETUTILS_BIG),
                };
                return mp_obj_new_tuple(2, tuple);
            }
            case MP_QSTR_gw4: {
                return netutils_format_ipv4_addr((uint8_t *)&info.gw, NETUTILS_BIG);
            }
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
        return mp_const_none;
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }
        int touched_ip_info = 0;
        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_dhcp4: {
                        esp_netif_dhcp_status_t status;
                        if (self->if_id == ESP_IF_WIFI_STA || self->if_id == ESP_IF_ETH) {
                            esp_exceptions(esp_netif_dhcpc_get_status(self->netif, &status));
                            if (mp_obj_is_true(e->value) && status != ESP_NETIF_DHCP_STARTED) {
                                esp_exceptions(esp_netif_dhcpc_start(self->netif));
                            } else if (!mp_obj_is_true(e->value) && status == ESP_NETIF_DHCP_STARTED) {
                                esp_exceptions(esp_netif_dhcpc_stop(self->netif));
                            }
                        } else {
                            mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                            break;
                        }
                        break;
                    }
                    case MP_QSTR_addr4: {
                        if (e->value != mp_const_none && mp_obj_is_str(e->value)) {
                            size_t addr_len;
                            const char *input_str = mp_obj_str_get_data(e->value, &addr_len);
                            char *split = strchr(input_str, '/');
                            if (split) {
                                mp_obj_t prefix_obj = mp_parse_num_integer(split + 1, strlen(split + 1), 10, NULL);
                                int prefix_bits = mp_obj_get_int(prefix_obj);
                                uint32_t mask = -(1u << (32 - prefix_bits));
                                uint32_t *m = (uint32_t *)&info.netmask;
                                *m = esp_netif_htonl(mask);
                            }
                            netutils_parse_ipv4_addr(e->value, (void *)&info.ip, NETUTILS_BIG);
                        } else if (e->value != mp_const_none) {
                            mp_obj_t *items;
                            mp_obj_get_array_fixed_n(e->value, 2, &items);
                            netutils_parse_ipv4_addr(items[0], (void *)&info.ip, NETUTILS_BIG);
                            netutils_parse_ipv4_addr(items[1], (void *)&info.netmask, NETUTILS_BIG);
                        }
                        touched_ip_info = 1;
                        break;
                    }
                    case MP_QSTR_gw4: {
                        netutils_parse_ipv4_addr(e->value, (void *)&info.gw, NETUTILS_BIG);
                        touched_ip_info = 1;
                        break;
                    }
                    default: {
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                    }
                }
            }
        }
        if (self->if_id == ESP_IF_WIFI_STA || self->if_id == ESP_IF_ETH) {
            if (touched_ip_info) {
                esp_err_t e = esp_netif_dhcpc_stop(self->netif);
                if (e != ESP_OK && e != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
                    esp_exceptions_helper(e);
                }
                esp_exceptions(esp_netif_set_ip_info(self->netif, &info));
            }
        } else if (self->if_id == ESP_IF_WIFI_AP) {
            esp_err_t e = esp_netif_dhcps_stop(self->netif);
            if (e != ESP_OK && e != ESP_ERR_ESP_NETIF_DHCP_ALREADY_STOPPED) {
                esp_exceptions_helper(e);
            }
            esp_exceptions(esp_netif_set_ip_info(self->netif, &info));
            esp_exceptions(esp_netif_dhcps_start(self->netif));
        }

    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(esp_nic_ipconfig_obj, 1, esp_ipconfig);

mp_obj_t esp_ifname(esp_netif_t *netif) {
    char ifname[NETIF_NAMESIZE + 1] = {0};
    mp_obj_t ret = mp_const_none;
    if (esp_netif_get_netif_impl_name(netif, ifname) == ESP_OK && ifname[0] != 0) {
        ret = mp_obj_new_str((char *)ifname, strlen(ifname));
    }
    return ret;
}

static mp_obj_t esp_phy_mode(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_network_phy_mode_obj, 0, 1, esp_phy_mode);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
_Static_assert(WIFI_AUTH_MAX == 13, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types.h");
#elif ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 5) && ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 1, 0) || ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 2)
_Static_assert(WIFI_AUTH_MAX == 11, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types.h");
#else
_Static_assert(WIFI_AUTH_MAX == 10, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types.h");
#endif
