/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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
#include <errno.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "netutils.h"
#include "queue.h"
#include "user_interface.h"
#include "espconn.h"
#include "ip_addr.h"
#include "spi_flash.h"
#include "utils.h"

#define WLAN_STA    0
#define WLAN_AP     1
#define WLAN_DHCP   "dhcp"
#define NO_DHCP_IP  "0.0.0.0"

void error_check(bool status, const char *msg);
extern const mp_obj_module_t network_module;

STATIC mp_obj_t get_module() {
    return (mp_obj_t)&network_module;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(get_module_obj, get_module);

STATIC mp_obj_t esp_connect(mp_uint_t n_args, const mp_obj_t *args) {
    struct station_config config = {{0}};
    mp_uint_t len;
    const char *p;

    p = mp_obj_str_get_data(args[0], &len);
    memcpy(config.ssid, p, len);
    p = mp_obj_str_get_data(args[1], &len);
    memcpy(config.password, p, len);

    error_check(wifi_station_set_config(&config), "Cannot set STA config");
    error_check(wifi_station_connect(), "Cannot connect to AP");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_connect_obj, 2, 6, esp_connect);

STATIC mp_obj_t esp_disconnect() {
    error_check(wifi_station_disconnect(), "Cannot disconnect from AP");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_disconnect_obj, esp_disconnect);

#define MODNETWORK_INCLUDE_CONSTANTS (1)

STATIC mp_obj_t esp_status() {
    return MP_OBJ_NEW_SMALL_INT(wifi_station_get_connect_status());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_status_obj, esp_status);

STATIC void esp_scan_cb(scaninfo *si, STATUS status) {
    struct bss_info *bs;
    if (si->pbss) {
        STAILQ_FOREACH(bs, si->pbss, next) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
            t->items[0] = mp_obj_new_bytes(bs->ssid, strlen((char*)bs->ssid));
            t->items[1] = mp_obj_new_bytes(bs->bssid, sizeof(bs->bssid));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(bs->channel);
            t->items[3] = MP_OBJ_NEW_SMALL_INT(bs->rssi);
            t->items[4] = MP_OBJ_NEW_SMALL_INT(bs->authmode);
            t->items[5] = MP_OBJ_NEW_SMALL_INT(bs->is_hidden);
            call_function_1_protected(MP_STATE_PORT(scan_cb_obj), t);
        }
    }
}

STATIC mp_obj_t esp_scan(mp_obj_t cb_in) {
    MP_STATE_PORT(scan_cb_obj) = cb_in;
    if (wifi_get_opmode() == SOFTAP_MODE) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, 
            "Scan not supported in AP mode"));
    }
    wifi_station_scan(NULL, (scan_done_cb_t)esp_scan_cb);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_scan_obj, esp_scan);

/// \method isconnected()
/// Return True if connected to an AP and an IP address has been assigned,
///     false otherwise.
STATIC mp_obj_t esp_isconnected() {
    if (wifi_station_get_connect_status() == STATION_GOT_IP) {
        return mp_const_true;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_isconnected_obj, esp_isconnected);

/// \method ifconfig(id = SPA|AP, config = [(ip, netmask, gateway, dns)])
/// Return (ip, netmask, gateway, dns) if config is empty
///     none otherwise.
STATIC mp_obj_t esp_ifconfig(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    struct ip_info ipconfig;

    STATIC const mp_arg_t wlan_ifconfig_args[] = {
        { MP_QSTR_id,               MP_ARG_INT,     {.u_int = 0} },
        { MP_QSTR_config,           MP_ARG_OBJ,     {.u_obj = MP_OBJ_NULL} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(wlan_ifconfig_args)];

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(args), wlan_ifconfig_args, args);
    
    uint8 mode = args[0].u_int;
    
    // translate MicroPython mode to ESP8266 mode
    switch (mode) {
        case WLAN_STA:
            mode = STATION_IF;
            break;

        case WLAN_AP:
            mode = SOFTAP_IF;
            break;
            
        default:
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                "the parameter id has to be STA or AP"));
    }

    // get the configuration
    if (args[1].u_obj == MP_OBJ_NULL) {
        wifi_get_ip_info(mode, &ipconfig);

        mp_obj_t station_array[4] = {
            netutils_format_ipv4_addr((uint8_t*) &ipconfig.ip.addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*) &ipconfig.netmask.addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*) &ipconfig.gw.addr, NETUTILS_BIG),
            mp_obj_new_str(NO_DHCP_IP, strlen(NO_DHCP_IP), false)
        };

        return mp_obj_new_tuple(4, station_array);
    }

    // set the configuration using a tuple
    if (MP_OBJ_IS_TYPE(args[1].u_obj, &mp_type_tuple)) {
        mp_obj_t *items;

        mp_obj_get_array_fixed_n(args[1].u_obj, 4, &items);

        netutils_parse_ipv4_addr(items[0], (uint8_t*) &ipconfig.ip.addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], (uint8_t*) &ipconfig.netmask.addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], (uint8_t*) &ipconfig.gw.addr, NETUTILS_BIG);

        if (STATION_IF == mode) {
            wifi_station_dhcpc_stop();
            wifi_set_ip_info(mode, &ipconfig);
        } else {
            wifi_softap_dhcps_stop();
            wifi_set_ip_info(mode, &ipconfig);
            wifi_softap_dhcps_start();
        }
    } else {
        const char *config_mode = mp_obj_str_get_str(args[1].u_obj);
        
        if (strcmp(WLAN_DHCP, config_mode)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError,
                "The config parameter has to be '" WLAN_DHCP "'"));
        }

        if (STATION_IF == mode) {
            wifi_station_dhcpc_start();
        } 
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp_ifconfig_obj, 0, esp_ifconfig);

STATIC const mp_map_elem_t mp_module_network_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_network) },
    // MicroPython "network" module interface requires it to contains classes
    // to instantiate. But as we have just a static network interace,
    // use module as a "class", and just make all methods module-global
    // functions.
    { MP_OBJ_NEW_QSTR(MP_QSTR_WLAN), (mp_obj_t)&get_module_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&esp_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), (mp_obj_t)&esp_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), (mp_obj_t)&esp_status_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan), (mp_obj_t)&esp_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isconnected), (mp_obj_t)&esp_isconnected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig), (mp_obj_t)&esp_ifconfig_obj },

#if MODNETWORK_INCLUDE_CONSTANTS
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_IDLE),
        MP_OBJ_NEW_SMALL_INT(STATION_IDLE)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_CONNECTING),
        MP_OBJ_NEW_SMALL_INT(STATION_CONNECTING)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_WRONG_PASSWORD),
        MP_OBJ_NEW_SMALL_INT(STATION_WRONG_PASSWORD)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_NO_AP_FOUND),
        MP_OBJ_NEW_SMALL_INT(STATION_NO_AP_FOUND)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_CONNECT_FAIL),
        MP_OBJ_NEW_SMALL_INT(STATION_CONNECT_FAIL)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STAT_GOT_IP),
        MP_OBJ_NEW_SMALL_INT(STATION_GOT_IP)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STA),
        MP_OBJ_NEW_SMALL_INT(WLAN_STA)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_AP),
        MP_OBJ_NEW_SMALL_INT(WLAN_AP)},
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t network_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_network,
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};
