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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "netutils.h"

#include "wm_include.h"


typedef struct _wlan_if_obj_t {
    mp_obj_base_t base;
    int if_id;
    bool active;
} wlan_if_obj_t;

const mp_obj_type_t wlan_if_type;
STATIC wlan_if_obj_t wlan_sta_obj = {{&wlan_if_type}, IEEE80211_MODE_INFRA, false};
STATIC wlan_if_obj_t wlan_ap_obj = {{&wlan_if_type}, IEEE80211_MODE_AP, false};

// Set to "true" if the STA interface is requested to be connected by the
// user, used for automatic reassociation.
static bool wifi_sta_connect_requested = false;

static bool wifi_sta_connect_failed = false;

// Set to "true" if the STA interface is connected to wifi and has IP address.
static bool wifi_sta_connected = false;

STATIC void require_if(mp_obj_t wlan_if, int if_no) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(wlan_if);
    if (self->if_id != if_no) {
        mp_raise_msg(&mp_type_OSError, if_no == IEEE80211_MODE_INFRA ? "STA mode required" : "SoftAP mode required");
    }
}

STATIC void w600_wifi_status_callback(u8 status) {
    switch(status) {
    case NETIF_IP_NET_UP: {
        wifi_sta_connected = true;
        break;
    }
    case NETIF_WIFI_DISCONNECTED: {
        wifi_sta_connected = false;
        wifi_sta_connect_requested = false;
        break;
    }
    case NETIF_WIFI_JOIN_FAILED: {
        wifi_sta_connected = false;
        wifi_sta_connect_requested = false;
        wifi_sta_connect_failed = true;
        break;
    }
    default: {
        break;
    }
    }

    return;
}

STATIC mp_obj_t w600_initialize() {
    static int initialized = 0;
    if (!initialized) {
        tls_netif_add_status_event(w600_wifi_status_callback);
        initialized = 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(w600_initialize_obj, w600_initialize);

STATIC mp_obj_t set_wlan(size_t n_args, const mp_obj_t *args) {
    static int initialized = 0;
    if (!initialized) {
        w600_initialize();
        initialized = 1;
    }

    int idx = (n_args > 0) ? mp_obj_get_int(args[0]) : IEEE80211_MODE_INFRA;
    if (idx == IEEE80211_MODE_INFRA) {
        return MP_OBJ_FROM_PTR(&wlan_sta_obj);
    } else if (idx == IEEE80211_MODE_AP) {
        return MP_OBJ_FROM_PTR(&wlan_ap_obj);
    } else {
        mp_raise_ValueError("invalid WLAN interface identifier");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(set_wlan_obj, 0, 1, set_wlan);

STATIC mp_obj_t w600_active(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args > 1) {
        bool active = mp_obj_is_true(args[1]);
        self->active = active;
        if (!active) {
            if (self->if_id == IEEE80211_MODE_INFRA) {
                tls_wifi_disconnect();
            } else if (self->if_id == IEEE80211_MODE_AP) {
                tls_wifi_softap_destroy();
            }
        }
    }

    return self->active ? mp_const_true : mp_const_false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(w600_active_obj, 1, 2, w600_active);

STATIC mp_obj_t w600_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    u8 apssid[33] = {0};
    u8 apkey[65] = {0};
    u8 apbssid[6] = {0};
    u8 bssidset = 0;

    // configure any parameters that are given
    if (n_args > 1) {
        mp_uint_t len;
        const char *p;
        if (args[ARG_ssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
            memcpy(apssid, p, MIN(len, sizeof(apssid) - 1));
        }
        if (args[ARG_password].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
            memcpy(apkey, p, MIN(len, sizeof(apkey) - 1));
        }
        if (args[ARG_bssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
            if (len != sizeof(apbssid)) {
                mp_raise_ValueError(NULL);
            }
            bssidset = 1;
            memcpy(apbssid, p, sizeof(apbssid));
        }
    }

    tls_wifi_disconnect();
    tls_wifi_softap_destroy();

    u8 wireless_protocol;
    tls_param_get(TLS_PARAM_ID_WPROTOCOL, (void *) &wireless_protocol, FALSE);
    if (IEEE80211_MODE_INFRA != wireless_protocol) {
        wireless_protocol = IEEE80211_MODE_INFRA;
        tls_param_set(TLS_PARAM_ID_WPROTOCOL, (void *) &wireless_protocol, TRUE);
    }

    // connect to the WiFi AP
    MP_THREAD_GIL_EXIT();
    if (bssidset) {
        tls_wifi_connect_by_ssid_bssid(apssid, strlen(apssid), apbssid, apkey, strlen(apkey));
    } else {
        tls_wifi_connect(apssid, strlen(apssid), apkey, strlen(apkey));
    }
    MP_THREAD_GIL_ENTER();
    wifi_sta_connect_requested = true;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(w600_connect_obj, 1, w600_connect);

STATIC mp_obj_t w600_disconnect(mp_obj_t self_in) {
    wifi_sta_connect_requested = false;
    tls_wifi_disconnect();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(w600_disconnect_obj, w600_disconnect);

enum {
    STAT_IDLE               = 0,
    STAT_CONNECTING         = 1,
    STAT_WRONG_PASSWORD     = 2,
    STAT_NO_AP_FOUND        = 3,
    STAT_CONNECT_FAIL       = 4,
    STAT_GOT_IP             = 5,
};

STATIC mp_obj_t w600_status(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        if (self->if_id == IEEE80211_MODE_INFRA) {
            // Case of no arg is only for the STA interface
            if (wifi_sta_connected) {
                // Happy path, connected with IP
                return MP_OBJ_NEW_SMALL_INT(STAT_GOT_IP);
            } else if (wifi_sta_connect_requested) {
                // No connection or error, but is requested = Still connecting
                return MP_OBJ_NEW_SMALL_INT(STAT_CONNECTING);
            } else if (wifi_sta_connect_failed) {
                if (WM_WIFI_EKEY == tls_wifi_get_errno())
                    return MP_OBJ_NEW_SMALL_INT(STAT_WRONG_PASSWORD);
                else if (WM_WIFI_ENOAP == tls_wifi_get_errno())
                    return MP_OBJ_NEW_SMALL_INT(STAT_NO_AP_FOUND);
                else
                    return MP_OBJ_NEW_SMALL_INT(STAT_CONNECT_FAIL);
            } else {
                // No activity, No error = Idle
                return MP_OBJ_NEW_SMALL_INT(STAT_IDLE);
            }
        }
        return mp_const_none;
    }

    // one argument: return status based on query parameter
    switch ((uintptr_t)args[1]) {
    case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_stations): {
        // return list of connected stations, only if in soft-AP mode
        require_if(args[0], IEEE80211_MODE_AP);
        u32 stanum = 0;
        u8 stalist[64] = {0};
        struct tls_sta_info_t *sta;
        tls_wifi_get_authed_sta_info(&stanum, stalist, sizeof(stalist));
        sta = (struct tls_sta_info_t *)stalist;
        mp_obj_t list = mp_obj_new_list(0, NULL);
        for (int i = 0; i < stanum; ++i) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(1, NULL);
            t->items[0] = mp_obj_new_bytes(sta[i].mac_addr, sizeof(sta[i].mac_addr));
            mp_obj_list_append(list, t);
        }
        return list;
    }
    case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_rssi): {
        struct tls_curr_bss_t currbss;
        memset(&currbss, 0, sizeof(currbss));
        tls_wifi_get_current_bss(&currbss);
        return MP_OBJ_NEW_SMALL_INT(-currbss.rssi);
    }
    default:
        mp_raise_ValueError("unknown status param");
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(w600_status_obj, 1, 2, w600_status);

STATIC tls_os_sem_t *w600_scan_finish_sem = NULL;

STATIC void w600_scan_finish_callback(void) {
    if (w600_scan_finish_sem) {
        tls_os_sem_release(w600_scan_finish_sem);
    }

    return;
}

STATIC mp_obj_t w600_scan(mp_obj_t self_in) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    MP_THREAD_GIL_EXIT();
    int ret = tls_os_sem_create(&w600_scan_finish_sem, 0);
    tls_wifi_scan_result_cb_register(w600_scan_finish_callback);
    tls_wifi_scan();
    MP_THREAD_GIL_ENTER();
    tls_os_sem_acquire(w600_scan_finish_sem, 0);
    tls_wifi_scan_result_cb_register(NULL);
    tls_os_sem_delete(w600_scan_finish_sem);
    w600_scan_finish_sem = NULL;

    u8 *buf = tls_mem_alloc(2000);
    if (!buf) {
        return list;
    }

    memset(buf, 0, 2000);
    ret = tls_wifi_get_scan_rslt(buf, 2000);
    if (ret) {
        tls_mem_free(buf);
        return list;
    }

    struct tls_scan_bss_t *scan_res = NULL;
    struct tls_bss_info_t *bss_info = NULL;
    scan_res = (struct tls_scan_bss_t *)buf;
    bss_info = scan_res->bss;

    for (u8 i = 0; i < scan_res->count; i++) {
        mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
        t->items[0] = mp_obj_new_bytes(bss_info->ssid, bss_info->ssid_len);
        t->items[1] = mp_obj_new_bytes(bss_info->bssid, sizeof(bss_info->bssid));
        t->items[2] = MP_OBJ_NEW_SMALL_INT(bss_info->channel);
        t->items[3] = MP_OBJ_NEW_SMALL_INT(-(0x100 - bss_info->rssi));
        t->items[4] = MP_OBJ_NEW_SMALL_INT(bss_info->privacy);//only open and not open!!!
        t->items[5] = (0 == bss_info->ssid_len) ? mp_const_true : mp_const_false;
        mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
        bss_info++;
    }

    tls_mem_free(buf);
    return list;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(w600_scan_obj, w600_scan);

STATIC mp_obj_t w600_isconnected(mp_obj_t self_in) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->if_id == IEEE80211_MODE_INFRA) {
        return mp_obj_new_bool(wifi_sta_connected);
    } else {
        u32 stanum = 0;
        u8 stalist[64] = {0};
        tls_wifi_get_authed_sta_info(&stanum, stalist, sizeof(stalist));
        return mp_obj_new_bool(stanum > 0);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(w600_isconnected_obj, w600_isconnected);

STATIC mp_obj_t w600_ifconfig(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct tls_ethif *ni = tls_netif_get_ethif();
    if (n_args == 1) {
        // get
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)&ni->ip_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&ni->netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&ni->gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&ni->dns1, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        if (MP_OBJ_IS_TYPE(args[1], &mp_type_tuple) || MP_OBJ_IS_TYPE(args[1], &mp_type_list)) {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(args[1], 4, &items);
            struct tls_param_ip param_ip;
            memset(&param_ip, 0, sizeof(param_ip));
            netutils_parse_ipv4_addr(items[0], (void *)&param_ip.ip, NETUTILS_BIG);
            if (mp_obj_is_integer(items[1])) {
                // allow numeric netmask, i.e.:
                // 24 -> 255.255.255.0
                // 16 -> 255.255.0.0
                // etc...
                uint32_t *m = (uint32_t *)&param_ip.netmask;
                *m = htonl(0xffffffff << (32 - mp_obj_get_int(items[1])));
            } else {
                netutils_parse_ipv4_addr(items[1], (void *)&param_ip.netmask, NETUTILS_BIG);
            }
            netutils_parse_ipv4_addr(items[2], (void *)&param_ip.gateway, NETUTILS_BIG);
            netutils_parse_ipv4_addr(items[3], (void *)&param_ip.dns1, NETUTILS_BIG);
            // To set a static IP we have to disable DHCP first
            if (self->if_id == IEEE80211_MODE_INFRA) {
                tls_dhcp_stop();
                struct tls_ethif *ethif = tls_netif_get_ethif();
                MEMCPY((char *)ip_2_ip4(&ethif->ip_addr), &param_ip.ip, 4);
                MEMCPY((char *)ip_2_ip4(&ethif->dns1), &param_ip.dns1, 4);
                MEMCPY((char *)ip_2_ip4(&ethif->netmask), &param_ip.netmask, 4);
                MEMCPY((char *)ip_2_ip4(&ethif->gw), &param_ip.gateway, 4);
                tls_netif_set_addr(&ethif->ip_addr, &ethif->netmask, &ethif->gw);
            } else if (self->if_id == IEEE80211_MODE_AP) {
                struct tls_param_ip param_ip2;
                tls_param_get(TLS_PARAM_ID_SOFTAP_IP, &param_ip2, FALSE);
                param_ip.dhcp_enable = param_ip2.dhcp_enable;
                tls_param_set(TLS_PARAM_ID_SOFTAP_IP, (void *)&param_ip, (bool)TRUE);
            }
        } else {
            // check for the correct string
            const char *mode = mp_obj_str_get_str(args[1]);
            if ((self->if_id != IEEE80211_MODE_INFRA) || strcmp("dhcp", mode)) {
                mp_raise_ValueError("invalid arguments");
            }
            tls_dhcp_start();
        }
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(w600_ifconfig_obj, 1, 2, w600_ifconfig);

extern u8 *wpa_supplicant_get_mac(void);
extern void wpa_supplicant_set_mac(u8 *mac);
STATIC mp_obj_t w600_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args != 1 && kwargs->used != 0) {
        mp_raise_TypeError("either pos or kw args are allowed");
    }

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used != 0) {
        struct tls_softap_info_t apinfo;
        memset(&apinfo, 0, sizeof(apinfo));
        for (size_t i = 0; i < kwargs->alloc; i++) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                int req_if = -1;

#define QS(x) (uintptr_t)MP_OBJ_NEW_QSTR(x)
                switch ((uintptr_t)kwargs->table[i].key) {
                case QS(MP_QSTR_mac): {
                    mp_buffer_info_t bufinfo;
                    mp_get_buffer_raise(kwargs->table[i].value, &bufinfo, MP_BUFFER_READ);
                    if (bufinfo.len != 6) {
                        mp_raise_ValueError("invalid buffer length");
                    }
                    wpa_supplicant_set_mac(bufinfo.buf);
                    tls_set_mac_addr(bufinfo.buf);
                    break;
                }
                case QS(MP_QSTR_essid): {
                    mp_uint_t len;
                    req_if = IEEE80211_MODE_AP;
                    const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                    len = MIN(len, sizeof(apinfo.ssid) - 1);
                    memcpy(apinfo.ssid, s, len);
                    apinfo.ssid[len] = '\0';
                    break;
                }
                case QS(MP_QSTR_authmode): {
                    req_if = IEEE80211_MODE_AP;
                    apinfo.encrypt = mp_obj_get_int(kwargs->table[i].value);
                    break;
                }
                case QS(MP_QSTR_password): {
                    mp_uint_t len;
                    req_if = IEEE80211_MODE_AP;
                    const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                    len = MIN(len, sizeof(apinfo.keyinfo.key));
                    memcpy(apinfo.keyinfo.key, s, len);
                    apinfo.keyinfo.key_len = len;
                    apinfo.keyinfo.format = 1;//hex???
                    apinfo.keyinfo.index = 1;
                    break;
                }
                case QS(MP_QSTR_channel): {
                    req_if = IEEE80211_MODE_AP;
                    apinfo.channel = mp_obj_get_int(kwargs->table[i].value);
                    break;
                }
                default:
                    goto unknown;
                }
#undef QS

                // We post-check interface requirements to save on code size
                if (req_if >= 0) {
                    require_if(args[0], req_if);
                }
            }
        }

        struct tls_ip_info_t ipinfo;
        ipinfo.ip_addr[0] = 192;
        ipinfo.ip_addr[1] = 168;
        ipinfo.ip_addr[2] = 43;
        ipinfo.ip_addr[3] = 1;
        ipinfo.netmask[0] = 255;
        ipinfo.netmask[1] = 255;
        ipinfo.netmask[2] = 255;
        ipinfo.netmask[3] = 0;
        memcpy(ipinfo.dnsname, "localmpy.wm", sizeof("localmpy.wm"));
        tls_wifi_softap_create(&apinfo, &ipinfo);

        return mp_const_none;
    }

    // Get config

    if (n_args != 2) {
        mp_raise_TypeError("can query only one param");
    }

    int req_if = -1;
    mp_obj_t val;

#define QS(x) (uintptr_t)MP_OBJ_NEW_QSTR(x)
    switch ((uintptr_t)args[1]) {
    case QS(MP_QSTR_mac): {
        uint8_t *mac;
        mac = wpa_supplicant_get_mac();
        return mp_obj_new_bytes(mac, 6);
    }
    case QS(MP_QSTR_essid): {
        struct tls_param_ssid params_ssid;
        if (self->if_id == IEEE80211_MODE_INFRA) {
            tls_param_get(TLS_PARAM_ID_SSID, (void *)&params_ssid, 1);
            val = mp_obj_new_str((char *)&params_ssid.ssid, params_ssid.ssid_len);
        } else {
            tls_param_get(TLS_PARAM_ID_SOFTAP_SSID, (void *)&params_ssid, 1);
            val = mp_obj_new_str((char *)&params_ssid.ssid, params_ssid.ssid_len);
        }
    }
    break;
    case QS(MP_QSTR_authmode):
        req_if = IEEE80211_MODE_AP;
        u8 encrypt;
        tls_param_get(TLS_PARAM_ID_SOFTAP_ENCRY, (void *)&encrypt, 1);
        val = MP_OBJ_NEW_SMALL_INT(encrypt);
        break;
    case QS(MP_QSTR_channel):
        req_if = IEEE80211_MODE_AP;
        u8 channel;
        tls_param_get(TLS_PARAM_ID_SOFTAP_CHANNEL, (void *)&channel, 1);
        val = MP_OBJ_NEW_SMALL_INT(channel);
        break;
    default:
        goto unknown;
    }
#undef QS

    // We post-check interface requirements to save on code size
    if (req_if >= 0) {
        require_if(args[0], req_if);
    }

    return val;

unknown:
    mp_raise_ValueError("unknown config param");
}

STATIC MP_DEFINE_CONST_FUN_OBJ_KW(w600_config_obj, 1, w600_config);

STATIC mp_obj_t w600_oneshot(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (self->if_id != IEEE80211_MODE_INFRA) {
        mp_raise_TypeError("Support only in sta mode");
    }

    if (n_args > 1) {
        tls_wifi_set_oneshot_flag(mp_obj_is_true(args[1]) ? 1 : 0);
    } else {
        return tls_wifi_get_oneshot_flag() ? mp_const_true : mp_const_false;
    }

    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(w600_oneshot_obj, 1, 2, w600_oneshot);

STATIC const mp_rom_map_elem_t wlan_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&w600_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&w600_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&w600_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&w600_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&w600_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&w600_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&w600_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&w600_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_oneshot), MP_ROM_PTR(&w600_oneshot_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wlan_if_locals_dict, wlan_if_locals_dict_table);

const mp_obj_type_t wlan_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_WLAN,
    .locals_dict = (mp_obj_t) &wlan_if_locals_dict,
};


STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&w600_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&set_wlan_obj) },

    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(IEEE80211_MODE_INFRA) },
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(IEEE80211_MODE_AP) },

    { MP_ROM_QSTR(MP_QSTR_AUTH_OPEN), MP_ROM_INT(IEEE80211_ENCRYT_NONE) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WEP40), MP_ROM_INT(IEEE80211_ENCRYT_WEP40) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WEP104), MP_ROM_INT(IEEE80211_ENCRYT_WEP104) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_PSK_TKIP), MP_ROM_INT(IEEE80211_ENCRYT_TKIP_WPA) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_PSK_AES), MP_ROM_INT(IEEE80211_ENCRYT_CCMP_WPA) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_PSK_TKIP), MP_ROM_INT(IEEE80211_ENCRYT_TKIP_WPA2) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_PSK_AES), MP_ROM_INT(IEEE80211_ENCRYT_CCMP_WPA2) },

    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(STAT_IDLE)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(STAT_CONNECTING)},
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(STAT_WRONG_PASSWORD)},
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(STAT_NO_AP_FOUND)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(STAT_CONNECT_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(STAT_GOT_IP)},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *) &mp_module_network_globals,
};

