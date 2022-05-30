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
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "modnetwork.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "lwip/dns.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 1, 0)
#define DNS_MAIN TCPIP_ADAPTER_DNS_MAIN
#else
#define DNS_MAIN ESP_NETIF_DNS_MAIN
#endif

#define MODNETWORK_INCLUDE_CONSTANTS (1)

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
        case ESP_ERR_TCPIP_ADAPTER_INVALID_PARAMS:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP Invalid Parameters"));
        case ESP_ERR_TCPIP_ADAPTER_IF_NOT_READY:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP IF Not Ready"));
        case ESP_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED:
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP DHCP Client Start Failed"));
        case ESP_ERR_TCPIP_ADAPTER_NO_MEM:
            mp_raise_OSError(MP_ENOMEM);
        default:
            mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Wifi Unknown Error 0x%04x"), e);
    }
}

// This function is called by the system-event task and so runs in a different
// thread to the main MicroPython task.  It must not raise any Python exceptions.
static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        #if MICROPY_PY_NETWORK_WLAN
        case SYSTEM_EVENT_STA_START:
        case SYSTEM_EVENT_STA_CONNECTED:
        case SYSTEM_EVENT_STA_GOT_IP:
        case SYSTEM_EVENT_STA_DISCONNECTED:
            network_wlan_event_handler(event);
            break;
        #endif
        case SYSTEM_EVENT_GOT_IP6:
            ESP_LOGI("network", "Got IPv6");
            break;
        case SYSTEM_EVENT_ETH_START:
            ESP_LOGI("ethernet", "start");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            ESP_LOGI("ethernet", "stop");
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            ESP_LOGI("ethernet", "LAN cable connected");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            ESP_LOGI("ethernet", "LAN cable disconnected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            ESP_LOGI("ethernet", "Got IP");
            break;
        default:
            ESP_LOGI("network", "event %d", event->event_id);
            break;
    }
    return ESP_OK;
}

STATIC mp_obj_t esp_initialize() {
    static int initialized = 0;
    if (!initialized) {
        ESP_LOGD("modnetwork", "Initializing TCP/IP");
        tcpip_adapter_init();
        ESP_LOGD("modnetwork", "Initializing Event Loop");
        esp_exceptions(esp_event_loop_init(event_handler, NULL));
        ESP_LOGD("modnetwork", "esp_event_loop_init done");
        initialized = 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_initialize_obj, esp_initialize);

STATIC mp_obj_t esp_ifconfig(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    tcpip_adapter_ip_info_t info;
    tcpip_adapter_dns_info_t dns_info;
    tcpip_adapter_get_ip_info(self->if_id, &info);
    tcpip_adapter_get_dns_info(self->if_id, DNS_MAIN, &dns_info);
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
                *m = htonl(0xffffffff << (32 - mp_obj_get_int(items[1])));
            } else {
                netutils_parse_ipv4_addr(items[1], (void *)&info.netmask, NETUTILS_BIG);
            }
            netutils_parse_ipv4_addr(items[2], (void *)&info.gw, NETUTILS_BIG);
            netutils_parse_ipv4_addr(items[3], (void *)&dns_info.ip, NETUTILS_BIG);
            // To set a static IP we have to disable DHCP first
            if (self->if_id == WIFI_IF_STA || self->if_id == ESP_IF_ETH) {
                esp_err_t e = tcpip_adapter_dhcpc_stop(self->if_id);
                if (e != ESP_OK && e != ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED) {
                    esp_exceptions_helper(e);
                }
                esp_exceptions(tcpip_adapter_set_ip_info(self->if_id, &info));
                esp_exceptions(tcpip_adapter_set_dns_info(self->if_id, DNS_MAIN, &dns_info));
            } else if (self->if_id == WIFI_IF_AP) {
                esp_err_t e = tcpip_adapter_dhcps_stop(WIFI_IF_AP);
                if (e != ESP_OK && e != ESP_ERR_TCPIP_ADAPTER_DHCP_ALREADY_STOPPED) {
                    esp_exceptions_helper(e);
                }
                esp_exceptions(tcpip_adapter_set_ip_info(WIFI_IF_AP, &info));
                esp_exceptions(tcpip_adapter_set_dns_info(WIFI_IF_AP, DNS_MAIN, &dns_info));
                esp_exceptions(tcpip_adapter_dhcps_start(WIFI_IF_AP));
            }
        } else {
            // check for the correct string
            const char *mode = mp_obj_str_get_str(args[1]);
            if ((self->if_id != WIFI_IF_STA && self->if_id != ESP_IF_ETH) || strcmp("dhcp", mode)) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
            }
            esp_exceptions(tcpip_adapter_dhcpc_start(self->if_id));
        }
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_ifconfig_obj, 1, 2, esp_ifconfig);

STATIC mp_obj_t esp_phy_mode(size_t n_args, const mp_obj_t *args) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_phy_mode_obj, 0, 1, esp_phy_mode);

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(4, 3, 0)
#define TEST_WIFI_AUTH_MAX 9
#else
#define TEST_WIFI_AUTH_MAX 8
#endif
_Static_assert(WIFI_AUTH_MAX == TEST_WIFI_AUTH_MAX, "Synchronize WIFI_AUTH_XXX constants with the ESP-IDF. Look at esp-idf/components/esp_wifi/include/esp_wifi_types.h");

STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&esp_initialize_obj) },

    #if MICROPY_PY_NETWORK_WLAN
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&get_wlan_obj) },
    #endif

    #if MICROPY_PY_NETWORK_LAN
    { MP_ROM_QSTR(MP_QSTR_LAN), MP_ROM_PTR(&get_lan_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_PPP), MP_ROM_PTR(&ppp_make_new_obj) },
    { MP_ROM_QSTR(MP_QSTR_phy_mode), MP_ROM_PTR(&esp_phy_mode_obj) },

    #if MODNETWORK_INCLUDE_CONSTANTS

    #if MICROPY_PY_NETWORK_WLAN
    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(WIFI_IF_STA)},
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(WIFI_IF_AP)},

    { MP_ROM_QSTR(MP_QSTR_MODE_11B), MP_ROM_INT(WIFI_PROTOCOL_11B) },
    { MP_ROM_QSTR(MP_QSTR_MODE_11G), MP_ROM_INT(WIFI_PROTOCOL_11G) },
    { MP_ROM_QSTR(MP_QSTR_MODE_11N), MP_ROM_INT(WIFI_PROTOCOL_11N) },
    { MP_ROM_QSTR(MP_QSTR_MODE_LR), MP_ROM_INT(WIFI_PROTOCOL_LR) },

    { MP_ROM_QSTR(MP_QSTR_AUTH_OPEN), MP_ROM_INT(WIFI_AUTH_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WEP), MP_ROM_INT(WIFI_AUTH_WEP) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_PSK), MP_ROM_INT(WIFI_AUTH_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_PSK), MP_ROM_INT(WIFI_AUTH_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_WPA2_PSK), MP_ROM_INT(WIFI_AUTH_WPA_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_ENTERPRISE), MP_ROM_INT(WIFI_AUTH_WPA2_ENTERPRISE) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA3_PSK), MP_ROM_INT(WIFI_AUTH_WPA3_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_WPA3_PSK), MP_ROM_INT(WIFI_AUTH_WPA2_WPA3_PSK) },
    #if ESP_IDF_VERSION > ESP_IDF_VERSION_VAL(4, 3, 0)
    { MP_ROM_QSTR(MP_QSTR_AUTH_WAPI_PSK), MP_ROM_INT(WIFI_AUTH_WAPI_PSK) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_AUTH_MAX), MP_ROM_INT(WIFI_AUTH_MAX) },
    #endif

    #if MICROPY_PY_NETWORK_LAN
    { MP_ROM_QSTR(MP_QSTR_PHY_LAN8710), MP_ROM_INT(PHY_LAN8710) },
    { MP_ROM_QSTR(MP_QSTR_PHY_LAN8720), MP_ROM_INT(PHY_LAN8720) },
    { MP_ROM_QSTR(MP_QSTR_PHY_IP101), MP_ROM_INT(PHY_IP101) },
    { MP_ROM_QSTR(MP_QSTR_PHY_RTL8201), MP_ROM_INT(PHY_RTL8201) },
    { MP_ROM_QSTR(MP_QSTR_PHY_DP83848), MP_ROM_INT(PHY_DP83848) },
    #if ESP_IDF_VERSION_MINOR >= 3
    // PHY_KSZ8041 is new in ESP-IDF v4.3
    { MP_ROM_QSTR(MP_QSTR_PHY_KSZ8041), MP_ROM_INT(PHY_KSZ8041) },
    #endif
    #if ESP_IDF_VERSION_MINOR >= 4
    // PHY_KSZ8081 is new in ESP-IDF v4.4
    { MP_ROM_QSTR(MP_QSTR_PHY_KSZ8081), MP_ROM_INT(PHY_KSZ8081) },
    #endif

    #if CONFIG_ETH_SPI_ETHERNET_KSZ8851SNL
    { MP_ROM_QSTR(MP_QSTR_PHY_KSZ8851SNL), MP_ROM_INT(PHY_KSZ8851SNL) },
    #endif
    #if CONFIG_ETH_SPI_ETHERNET_DM9051
    { MP_ROM_QSTR(MP_QSTR_PHY_DM9051), MP_ROM_INT(PHY_DM9051) },
    #endif
    #if CONFIG_ETH_SPI_ETHERNET_W5500
    { MP_ROM_QSTR(MP_QSTR_PHY_W5500), MP_ROM_INT(PHY_W5500) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_ETH_INITIALIZED), MP_ROM_INT(ETH_INITIALIZED)},
    { MP_ROM_QSTR(MP_QSTR_ETH_STARTED), MP_ROM_INT(ETH_STARTED)},
    { MP_ROM_QSTR(MP_QSTR_ETH_STOPPED), MP_ROM_INT(ETH_STOPPED)},
    { MP_ROM_QSTR(MP_QSTR_ETH_CONNECTED), MP_ROM_INT(ETH_CONNECTED)},
    { MP_ROM_QSTR(MP_QSTR_ETH_DISCONNECTED), MP_ROM_INT(ETH_DISCONNECTED)},
    { MP_ROM_QSTR(MP_QSTR_ETH_GOT_IP), MP_ROM_INT(ETH_GOT_IP)},
    #endif

    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(STAT_IDLE)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(STAT_CONNECTING)},
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(STAT_GOT_IP)},
    // Errors from the ESP-IDF
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(WIFI_REASON_NO_AP_FOUND)},
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(WIFI_REASON_AUTH_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_BEACON_TIMEOUT), MP_ROM_INT(WIFI_REASON_BEACON_TIMEOUT)},
    { MP_ROM_QSTR(MP_QSTR_STAT_ASSOC_FAIL), MP_ROM_INT(WIFI_REASON_ASSOC_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_HANDSHAKE_TIMEOUT), MP_ROM_INT(WIFI_REASON_HANDSHAKE_TIMEOUT)},
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};

// Note: This port doesn't define MICROPY_PY_NETWORK so this will not conflict
// with the common implementation provided by extmod/modnetwork.c.
MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network);
