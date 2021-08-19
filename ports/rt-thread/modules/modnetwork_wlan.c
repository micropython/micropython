/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 SummerGift <SummerGift@qq.com>
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
#include "shared/netutils/netutils.h"

#if MICROPY_PY_WLAN
#include <rtthread.h>
#include <wlan_mgnt.h>
#include <wlan_cfg.h>
#include <wlan_prot.h>
#include <arpa/inet.h>
#include <netdev.h>
#include "modnetwork.h"

extern struct netdev *netdev_default;

typedef struct _wlan_if_obj_t
{
    mp_obj_base_t base;
    int if_id;
} wlan_if_obj_t;

enum
{
    STATION_IDLE = 0,
    STATION_CONNECTING,
    STATION_WRONG_PASSWORD,
    STATION_NO_AP_FOUND,
    STATION_CONNECT_FAIL,
    STATION_GOT_IP,
};

const mp_obj_type_t wlan_if_type;
STATIC struct rt_wlan_info _ap_info;
STATIC char _ap_password[RT_WLAN_PASSWORD_MAX_LENGTH];

STATIC void error_check(bool status, const char *msg)
{
    if (!status)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, msg));
    }
}

STATIC const wlan_if_obj_t wlan_objs[] =
{
    {{&wlan_if_type}, STATION_IF},
    {{&wlan_if_type}, SOFTAP_IF},
};

STATIC void require_if(mp_obj_t wlan_if, int if_no)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(wlan_if);
    if (self->if_id != if_no)
    {
        error_check(false, if_no == STATION_IF ? "STA required" : "AP required");
    }
}

STATIC mp_obj_t get_wlan(size_t n_args, const mp_obj_t *args)
{
    int idx = 0;
    if (n_args > 0)
    {
        idx = mp_obj_get_int(args[0]);
        if (idx < 0 || idx >= sizeof(wlan_objs))
        {
            mp_raise_ValueError(NULL);
        }
    }
    return MP_OBJ_FROM_PTR(&wlan_objs[idx]);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(get_wlan_obj, 0, 1, get_wlan);

STATIC mp_obj_t wlan_active(size_t n_args, const mp_obj_t *args)
{

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args > 1)
    {

        if (self->if_id == STATION_IF)
        {
            if (mp_obj_get_int(args[1]) == RT_TRUE)
            {
                error_check(netdev_set_up(netdev_default) == RT_EOK, "Cannot active wlan device");
            }
            else
            {
                error_check(netdev_set_down(netdev_default) == RT_EOK, "Cannot disable wlan device");
            }
        }
        else
        {
            if (mp_obj_get_int(args[1]) == RT_TRUE)
            {
                error_check(rt_wlan_start_ap((char *)&_ap_info.ssid.val, _ap_password) == RT_EOK, "Cannot start AP");
            }
            else
            {
                error_check(rt_wlan_ap_stop() == RT_EOK, "Cannot stop AP");
            }
        }

        return mp_const_none;
    }

    if (self->if_id == STATION_IF)
    {
        return mp_obj_new_bool(rt_wlan_get_mode("wlan0") == RT_WLAN_STATION);
    }
    else
    {
        return mp_obj_new_bool(rt_wlan_get_mode("wlan1") == RT_WLAN_AP);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_active_obj, 1, 2, wlan_active);

STATIC mp_obj_t wlan_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args)
{
    enum { ARG_ssid, ARG_password, ARG_bssid };
    static const mp_arg_t allowed_args[] =
    {
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    require_if(pos_args[0], STATION_IF);

    const char *ssid = RT_NULL;
    const char *key = RT_NULL;
    size_t len;
    const char *p;

    // set parameters based on given args
    if (args[ARG_ssid].u_obj != mp_const_none)
    {
        p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
        ssid = p;
    }

    if (args[ARG_password].u_obj != mp_const_none)
    {
        p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
        key = p;
    }

    error_check(rt_wlan_connect(ssid, key) == RT_EOK, "Cannot connect to AP");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_connect_obj, 1, wlan_connect);

STATIC mp_obj_t wlan_disconnect(mp_obj_t self_in)
{
    require_if(self_in, STATION_IF);
    error_check(rt_wlan_disconnect() == RT_EOK, "Cannot disconnect from AP");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_disconnect_obj, wlan_disconnect);

STATIC mp_obj_t wlan_status(size_t n_args, const mp_obj_t *args)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1)
    {
        // Get link status
        if (self->if_id == STATION_IF)
        {

            if (rt_wlan_is_ready() == RT_EOK)
            {
                return MP_OBJ_NEW_SMALL_INT(STATION_GOT_IP);
            }
            else
            {
                return MP_OBJ_NEW_SMALL_INT(STATION_IDLE);
            }
        }
        return MP_OBJ_NEW_SMALL_INT(-1);
    }
    else
    {
        // Get specific status parameter
        switch (mp_obj_str_get_qstr(args[1]))
        {
        case MP_QSTR_rssi:
            if (self->if_id == STATION_IF)
            {
                return MP_OBJ_NEW_SMALL_INT(rt_wlan_get_rssi());
            }
        }
        mp_raise_ValueError("unknown status param");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_status_obj, 1, 2, wlan_status);

STATIC mp_obj_t *wlan_scan_list = NULL;

void wlan_station_scan(void)
{
    if (wlan_scan_list == NULL)
    {
        // called unexpectedly
        return;
    }

    struct rt_wlan_scan_result *scan_result = RT_NULL;

    /* scan ap info */
    scan_result = rt_wlan_scan_sync();
    if (scan_result)
    {
        int index, num;
        char *security;

        num = scan_result->num;
        for (index = 0; index < num; index ++)
        {
            switch (scan_result->info[index].security)
            {
            case SECURITY_OPEN:
                security = "OPEN";
                break;
            case SECURITY_WEP_PSK:
                security = "WEP_PSK";
                break;
            case SECURITY_WEP_SHARED:
                security = "WEP_SHARED";
                break;
            case SECURITY_WPA_TKIP_PSK:
                security = "WPA_TKIP_PSK";
                break;
            case SECURITY_WPA_AES_PSK:
                security = "WPA_AES_PSK";
                break;
            case SECURITY_WPA2_AES_PSK:
                security = "WPA2_AES_PSK";
                break;
            case SECURITY_WPA2_TKIP_PSK:
                security = "WPA2_TKIP_PSK";
                break;
            case SECURITY_WPA2_MIXED_PSK:
                security = "WPA2_MIXED_PSK";
                break;
            case SECURITY_WPS_OPEN:
                security = "WPS_OPEN";
                break;
            case SECURITY_WPS_SECURE:
                security = "WPS_SECURE";
                break;
            default:
                security = "UNKNOWN";
                break;
            }

            mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
            t->items[0] = mp_obj_new_bytes(&scan_result->info[index].ssid.val[0], strlen((char *)(&scan_result->info[index].ssid.val[0])));
            t->items[1] = mp_obj_new_bytes(&scan_result->info[index].bssid[0], strlen((char *)(&scan_result->info[index].bssid[0])));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(scan_result->info[index].channel);
            t->items[3] = MP_OBJ_NEW_SMALL_INT(scan_result->info[index].rssi);
            t->items[4] = mp_obj_new_bytes((const byte *)security, strlen(security));
            t->items[5] = MP_OBJ_NEW_SMALL_INT(scan_result->info[index].hidden);

            mp_obj_list_append(*wlan_scan_list, MP_OBJ_FROM_PTR(t));

        }
        rt_wlan_scan_result_clean();
    }
    else
    {
        mp_printf(&mp_plat_print, ("wifi scan result is null\n"));
        *wlan_scan_list = MP_OBJ_NULL;
    }
}

STATIC mp_obj_t wlan_scan(mp_obj_t self_in)
{
    require_if(self_in, STATION_IF);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    wlan_scan_list = &list;
    wlan_station_scan();

    if (list == MP_OBJ_NULL)
    {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "scan failed"));
    }
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_scan_obj, wlan_scan);

/// \method isconnected()
/// Return True if connected to an AP and an IP address has been assigned,
/// false otherwise.
STATIC mp_obj_t wlan_isconnected(mp_obj_t self_in)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->if_id == STATION_IF)
    {
        if (rt_wlan_is_connected() == RT_TRUE)
        {
            return mp_const_true;
        }
    }
    else
    {
        if (rt_wlan_ap_get_sta_num() > 0)
        {
            return mp_const_true;
        }
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wlan_isconnected_obj, wlan_isconnected);

STATIC mp_obj_t wlan_ifconfig(size_t n_args, const mp_obj_t *args)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    struct netdev *netdev = netdev_default;
    if (netdev == RT_NULL)
    {
        mp_printf(&mp_plat_print, ("not find wlan interface device.\n"));
        return MP_OBJ_NEW_SMALL_INT(-1);
    }

    if (n_args == 1)
    {
        // get
        mp_obj_t tuple[4] =
        {
            mp_obj_new_str((const char *)inet_ntoa(netdev->ip_addr), strlen((char *)(inet_ntoa(netdev->ip_addr)))),
            mp_obj_new_str((const char *)inet_ntoa(netdev->netmask), strlen((char *)(inet_ntoa(netdev->netmask)))),
            mp_obj_new_str((const char *)inet_ntoa(netdev->gw), strlen((char *)(inet_ntoa(netdev->gw)))),
            mp_obj_new_str((const char *)inet_ntoa(netdev->dns_servers), strlen((char *)(inet_ntoa(netdev->dns_servers)))),
        };
        return mp_obj_new_tuple(4, tuple);
    }
    else
    {
        // set
        mp_obj_t *items;
        uint8_t ip_addr[4];
        uint8_t netmask[4];
        uint8_t gw[4];
        uint8_t dns_server[4];

        mp_obj_get_array_fixed_n(args[1], 4, &items);

        netutils_parse_ipv4_addr(items[0], (uint8_t *)ip_addr,    NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], (uint8_t *)netmask,    NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], (uint8_t *)gw,    NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[3], (uint8_t *)dns_server, NETUTILS_BIG);

        // To set a static IP we have to disable DHCP first
        if (self->if_id == STATION_IF)
        {
            if (netdev_dhcp_enabled(netdev, 0) == RT_EOK)
            {
                if (netdev_set_ipaddr(netdev, (const ip_addr_t *)ip_addr) != RT_EOK)
                {
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "netdev_set_ipaddr() failed"));
                }
            }
            else
            {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "netdev_dhcp_enabled() failed"));
            }
        }
        else
        {
            // TODO modify IP netmask gw under AP mode
            netdev_set_dns_server(netdev, 0, (const ip_addr_t *)dns_server);
            return mp_const_none;
        }

        netdev_set_netmask(netdev, (const ip_addr_t *)netmask);
        netdev_set_gw(netdev, (const ip_addr_t *)gw);
        netdev_set_dns_server(netdev, 0, (const ip_addr_t *)dns_server);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wlan_ifconfig_obj, 1, 2, wlan_ifconfig);

STATIC mp_obj_t wlan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs)
{
    if (n_args != 1 && kwargs->used != 0)
    {
        mp_raise_TypeError("either pos or kw args are allowed");
    }

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct rt_wlan_info cfg = {0};

    if (self->if_id == STATION_IF)
    {
        error_check(rt_wlan_get_info(&cfg) == RT_EOK, "can't get STA config");
    }
    else
    {
        error_check(rt_wlan_ap_get_info(&cfg) == RT_EOK, "can't get AP config");
    }

    int req_if = -1;

    if (kwargs->used != 0)
    {

        for (mp_uint_t i = 0; i < kwargs->alloc; i++)
        {
            if (mp_map_slot_is_filled(kwargs, i))
            {
#define QS(x) (uintptr_t)MP_OBJ_NEW_QSTR(x)
                switch ((uintptr_t)kwargs->table[i].key)
                {
                case QS(MP_QSTR_mac):
                {
                    mp_buffer_info_t bufinfo;
                    mp_get_buffer_raise(kwargs->table[i].value, &bufinfo, MP_BUFFER_READ);
                    if (bufinfo.len != 6)
                    {
                        mp_raise_ValueError("invalid buffer length");
                    }
                    error_check(rt_wlan_set_mac((rt_uint8_t *)bufinfo.buf) == RT_EOK, "can't set MAC");

                    break;
                }
                case QS(MP_QSTR_essid):
                {
                    req_if = SOFTAP_IF;
                    size_t len;
                    const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                    len = MIN(len, sizeof(_ap_info.ssid.val));
                    memcpy(_ap_info.ssid.val, s, len);
                    _ap_info.ssid.len = len;
                    break;
                }
                case QS(MP_QSTR_hidden):
                {
                    req_if = SOFTAP_IF;
                    _ap_info.hidden = mp_obj_is_true(kwargs->table[i].value);
                    break;
                }
//                    case QS(MP_QSTR_authmode): {
//                        req_if = SOFTAP_IF;
//                        cfg.ap.authmode = mp_obj_get_int(kwargs->table[i].value);
//                        break;
//                    }
                case QS(MP_QSTR_password):
                {
                    req_if = SOFTAP_IF;
                    size_t len;
                    const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                    len = MIN(len, sizeof(_ap_password) - 1);
                    memcpy(_ap_password, s, len);
                    _ap_password[len] = 0;
                    break;
                }
                case QS(MP_QSTR_channel):
                {
                    req_if = SOFTAP_IF;
                    _ap_info.channel = mp_obj_get_int(kwargs->table[i].value);
                    break;
                }
//                    case QS(MP_QSTR_dhcp_hostname): {
//                        req_if = STATION_IF;
//                        if (self->if_id == STATION_IF) {
//                            const char *s = mp_obj_str_get_str(kwargs->table[i].value);
//                            wifi_station_set_hostname((char*)s);
//                        }
//                        break;
//                    }
                default:
                    goto unknown;
                }
#undef QS
            }
        }

        // We post-check interface requirements to save on code size
        if (req_if >= 0)
        {
            require_if(args[0], req_if);
        }

        return mp_const_none;
    }

    // Get config
    if (n_args != 2)
    {
        mp_raise_TypeError("can query only one param");
    }

    mp_obj_t val;

    qstr key = mp_obj_str_get_qstr(args[1]);
    switch (key)
    {
    case MP_QSTR_mac:
    {
        uint8_t mac[6];
        error_check(rt_wlan_get_mac(mac) == RT_EOK, "can't get mac config");
        return mp_obj_new_bytes(mac, sizeof(mac));
    }
    case MP_QSTR_essid:
        if (self->if_id == STATION_IF)
        {
            val = mp_obj_new_str((char *)cfg.ssid.val, strlen((char *)cfg.ssid.val));
        }
        else
        {
            val = mp_obj_new_str((char *)_ap_info.ssid.val, _ap_info.ssid.len);
        }
        break;
    case MP_QSTR_hidden:
        req_if = SOFTAP_IF;
        val = mp_obj_new_bool(cfg.hidden);
        break;
//        case MP_QSTR_authmode:
//            req_if = SOFTAP_IF;
//            val = MP_OBJ_NEW_SMALL_INT(cfg.ap.authmode);
//            break;
    case MP_QSTR_channel:
        req_if = SOFTAP_IF;
        val = MP_OBJ_NEW_SMALL_INT(cfg.channel);
        break;
//        case MP_QSTR_dhcp_hostname: {
//            req_if = STATION_IF;
//            char* s = wifi_station_get_hostname();
//            if (s == NULL) {
//                val = MP_OBJ_NEW_QSTR(MP_QSTR_);
//            } else {
//                val = mp_obj_new_str(s, strlen(s));
//            }
//            break;
//        }
    default:
        goto unknown;
    }

    // We post-check interface requirements to save on code size
    if (req_if >= 0)
    {
        require_if(args[0], req_if);
    }

    return val;

unknown:
    mp_raise_ValueError("unknown config param");
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wlan_config_obj, 1, wlan_config);

STATIC const mp_rom_map_elem_t wlan_if_locals_dict_table[] =
{
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&wlan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&wlan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&wlan_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&wlan_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&wlan_ifconfig_obj) },

#if MODNETWORK_INCLUDE_CONSTANTS
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(STATION_IDLE)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(STATION_CONNECTING)},
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(STATION_WRONG_PASSWORD)},
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(STATION_NO_AP_FOUND)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(STATION_CONNECT_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(STATION_GOT_IP)},

//    { MP_ROM_QSTR(MP_QSTR_MODE_11B), MP_ROM_INT(PHY_MODE_11B) },
//    { MP_ROM_QSTR(MP_QSTR_MODE_11G), MP_ROM_INT(PHY_MODE_11G) },
//    { MP_ROM_QSTR(MP_QSTR_MODE_11N), MP_ROM_INT(PHY_MODE_11N) },

//    { MP_ROM_QSTR(MP_QSTR_AUTH_OPEN), MP_ROM_INT(AUTH_OPEN) },
//    { MP_ROM_QSTR(MP_QSTR_AUTH_WEP), MP_ROM_INT(AUTH_WEP) },
//    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_PSK), MP_ROM_INT(AUTH_WPA_PSK) },
//    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_PSK), MP_ROM_INT(AUTH_WPA2_PSK) },
//    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_WPA2_PSK), MP_ROM_INT(AUTH_WPA_WPA2_PSK) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(wlan_if_locals_dict, wlan_if_locals_dict_table);

const mp_obj_type_t wlan_if_type =
{
    { &mp_type_type },
    .name = MP_QSTR_WLAN,
    .locals_dict = (mp_obj_dict_t *) &wlan_if_locals_dict,
};

#endif
