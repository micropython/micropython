/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Paul Sokolovsky
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
#include "queue.h"
#include "user_interface.h"
#include "espconn.h"
#include "spi_flash.h"
#include "ets_alt_task.h"
#include "lwip/dns.h"

#define MODNETWORK_INCLUDE_CONSTANTS (1)

typedef struct _wlan_if_obj_t {
    mp_obj_base_t base;
    int if_id;
} wlan_if_obj_t;

void error_check(bool status, const char *msg);
const mp_obj_type_t wlan_if_type;

STATIC const wlan_if_obj_t wlan_objs[] = {
    {{&wlan_if_type}, STATION_IF},
    {{&wlan_if_type}, SOFTAP_IF},
};

STATIC void require_if(mp_obj_t wlan_if, int if_no) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(wlan_if);
    if (self->if_id != if_no) {
        error_check(false, if_no == STATION_IF ? "STA required" : "AP required");
    }
}

STATIC mp_obj_t get_wlan(size_t n_args, const mp_obj_t *args) {
    int idx = 0;
    if (n_args > 0) {
        idx = mp_obj_get_int(args[0]);
        if (idx < 0 || idx >= sizeof(wlan_objs)) {
            mp_raise_ValueError(NULL);
        }
    }
    return MP_OBJ_FROM_PTR(&wlan_objs[idx]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(get_wlan_obj, 0, 1, get_wlan);

STATIC mp_obj_t esp_active(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t mode = wifi_get_opmode();
    if (n_args > 1) {
        int mask = self->if_id == STATION_IF ? STATION_MODE : SOFTAP_MODE;
        if (mp_obj_get_int(args[1]) != 0) {
            mode |= mask;
        } else {
            mode &= ~mask;
        }
        if (mode != NULL_MODE) {
            wifi_fpm_do_wakeup();
            wifi_fpm_close();
        }
        error_check(wifi_set_opmode(mode), "Cannot update i/f status");
        if (mode == NULL_MODE) {
            // Wait for the interfaces to go down before forcing power management
            while (wifi_get_opmode() != NULL_MODE) {
                ets_loop_iter();
            }
            wifi_fpm_open();
            wifi_fpm_do_sleep(0xfffffff);
        }
        return mp_const_none;
    }

    // Get active status
    if (self->if_id == STATION_IF) {
        return mp_obj_new_bool(mode & STATION_MODE);
    } else {
        return mp_obj_new_bool(mode & SOFTAP_MODE);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_active_obj, 1, 2, esp_active);

STATIC mp_obj_t esp_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    require_if(pos_args[0], STATION_IF);
    struct station_config config = {{0}};
    size_t len;
    const char *p;
    bool set_config = false;

    // set parameters based on given args
    if (args[ARG_ssid].u_obj != mp_const_none) {
        p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
        len = MIN(len, sizeof(config.ssid));
        memcpy(config.ssid, p, len);
        set_config = true;
    }
    if (args[ARG_password].u_obj != mp_const_none) {
        p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
        len = MIN(len, sizeof(config.password));
        memcpy(config.password, p, len);
        set_config = true;
    }
    if (args[ARG_bssid].u_obj != mp_const_none) {
        p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
        if (len != sizeof(config.bssid)) {
            mp_raise_ValueError(NULL);
        }
        config.bssid_set = 1;
        memcpy(config.bssid, p, sizeof(config.bssid));
        set_config = true;
    }

    if (set_config) {
        error_check(wifi_station_set_config(&config), "Cannot set STA config");
    }
    error_check(wifi_station_connect(), "Cannot connect to AP");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp_connect_obj, 1, esp_connect);

STATIC mp_obj_t esp_disconnect(mp_obj_t self_in) {
    require_if(self_in, STATION_IF);
    error_check(wifi_station_disconnect(), "Cannot disconnect from AP");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_disconnect_obj, esp_disconnect);

STATIC mp_obj_t esp_status(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        // Get link status
        if (self->if_id == STATION_IF) {
            return MP_OBJ_NEW_SMALL_INT(wifi_station_get_connect_status());
        }
        return MP_OBJ_NEW_SMALL_INT(-1);
    } else {
        // Get specific status parameter
        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_rssi:
                if (self->if_id == STATION_IF) {
                    return MP_OBJ_NEW_SMALL_INT(wifi_station_get_rssi());
                }
        }
        mp_raise_ValueError("unknown status param");
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_status_obj, 1, 2, esp_status);

STATIC mp_obj_t *esp_scan_list = NULL;

STATIC void esp_scan_cb(void *result, STATUS status) {
    if (esp_scan_list == NULL) {
        // called unexpectedly
        return;
    }
    if (result && status == 0) {
        // we need to catch any memory errors
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            for (struct bss_info *bs = result; bs; bs = STAILQ_NEXT(bs, next)) {
                mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
                #if 1
                // struct bss_info::ssid_len is not documented in SDK API Guide,
                // but is present in SDK headers since 1.4.0
                t->items[0] = mp_obj_new_bytes(bs->ssid, bs->ssid_len);
                #else
                t->items[0] = mp_obj_new_bytes(bs->ssid, strlen((char*)bs->ssid));
                #endif
                t->items[1] = mp_obj_new_bytes(bs->bssid, sizeof(bs->bssid));
                t->items[2] = MP_OBJ_NEW_SMALL_INT(bs->channel);
                t->items[3] = MP_OBJ_NEW_SMALL_INT(bs->rssi);
                t->items[4] = MP_OBJ_NEW_SMALL_INT(bs->authmode);
                t->items[5] = MP_OBJ_NEW_SMALL_INT(bs->is_hidden);
                mp_obj_list_append(*esp_scan_list, MP_OBJ_FROM_PTR(t));
            }
            nlr_pop();
        } else {
            mp_obj_print_exception(&mp_plat_print, MP_OBJ_FROM_PTR(nlr.ret_val));
            // indicate error
            *esp_scan_list = MP_OBJ_NULL;
        }
    } else {
        // indicate error
        *esp_scan_list = MP_OBJ_NULL;
    }
    esp_scan_list = NULL;
}

STATIC mp_obj_t esp_scan(mp_obj_t self_in) {
    require_if(self_in, STATION_IF);
    if ((wifi_get_opmode() & STATION_MODE) == 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "STA must be active"));
    }
    mp_obj_t list = mp_obj_new_list(0, NULL);
    esp_scan_list = &list;
    wifi_station_scan(NULL, (scan_done_cb_t)esp_scan_cb);
    while (esp_scan_list != NULL) {
        // our esp_scan_cb is called via ets_loop_iter so it's safe to set the
        // esp_scan_list variable to NULL without disabling interrupts
        if (MP_STATE_VM(mp_pending_exception) != NULL) {
            esp_scan_list = NULL;
            mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            nlr_raise(obj);
        }
        ets_loop_iter();
    }
    if (list == MP_OBJ_NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "scan failed"));
    }
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_scan_obj, esp_scan);

/// \method isconnected()
/// Return True if connected to an AP and an IP address has been assigned,
///     false otherwise.
STATIC mp_obj_t esp_isconnected(mp_obj_t self_in) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->if_id == STATION_IF) {
        if (wifi_station_get_connect_status() == STATION_GOT_IP) {
            return mp_const_true;
        }
    } else {
        if (wifi_softap_get_station_num() > 0) {
            return mp_const_true;
        }
    }
    return mp_const_false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_isconnected_obj, esp_isconnected);

STATIC mp_obj_t esp_ifconfig(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct ip_info info;
    ip_addr_t dns_addr;
    wifi_get_ip_info(self->if_id, &info);
    if (n_args == 1) {
        // get
        dns_addr = dns_getserver(0);
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t*)&info.ip, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&info.netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&info.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&dns_addr, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        mp_obj_t *items;
        bool restart_dhcp_server = false;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[0], (void*)&info.ip, NETUTILS_BIG);
        if (mp_obj_is_integer(items[1])) {
            // allow numeric netmask, i.e.:
            // 24 -> 255.255.255.0
            // 16 -> 255.255.0.0
            // etc...
            uint32_t* m = (uint32_t*)&info.netmask;
            *m = htonl(0xffffffff << (32 - mp_obj_get_int(items[1])));
        } else {
            netutils_parse_ipv4_addr(items[1], (void*)&info.netmask, NETUTILS_BIG);
        }
        netutils_parse_ipv4_addr(items[2], (void*)&info.gw, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[3], (void*)&dns_addr, NETUTILS_BIG);
        // To set a static IP we have to disable DHCP first
        if (self->if_id == STATION_IF) {
            wifi_station_dhcpc_stop();
        } else {
            restart_dhcp_server = wifi_softap_dhcps_status();
            wifi_softap_dhcps_stop();
        }
        if (!wifi_set_ip_info(self->if_id, &info)) {
          nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "wifi_set_ip_info() failed"));
        }
        dns_setserver(0, &dns_addr);
        if (restart_dhcp_server) {
            wifi_softap_dhcps_start();
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_ifconfig_obj, 1, 2, esp_ifconfig);

STATIC mp_obj_t esp_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args != 1 && kwargs->used != 0) {
        mp_raise_TypeError("either pos or kw args are allowed");
    }

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    union {
        struct station_config sta;
        struct softap_config ap;
    } cfg;

    if (self->if_id == STATION_IF) {
        error_check(wifi_station_get_config(&cfg.sta), "can't get STA config");
    } else {
        error_check(wifi_softap_get_config(&cfg.ap), "can't get AP config");
    }

    int req_if = -1;

    if (kwargs->used != 0) {

        for (mp_uint_t i = 0; i < kwargs->alloc; i++) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                #define QS(x) (uintptr_t)MP_OBJ_NEW_QSTR(x)
                switch ((uintptr_t)kwargs->table[i].key) {
                    case QS(MP_QSTR_mac): {
                        mp_buffer_info_t bufinfo;
                        mp_get_buffer_raise(kwargs->table[i].value, &bufinfo, MP_BUFFER_READ);
                        if (bufinfo.len != 6) {
                            mp_raise_ValueError("invalid buffer length");
                        }
                        wifi_set_macaddr(self->if_id, bufinfo.buf);
                        break;
                    }
                    case QS(MP_QSTR_essid): {
                        req_if = SOFTAP_IF;
                        size_t len;
                        const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                        len = MIN(len, sizeof(cfg.ap.ssid));
                        memcpy(cfg.ap.ssid, s, len);
                        cfg.ap.ssid_len = len;
                        break;
                    }
                    case QS(MP_QSTR_hidden): {
                        req_if = SOFTAP_IF;
                        cfg.ap.ssid_hidden = mp_obj_is_true(kwargs->table[i].value);
                        break;
                    }
                    case QS(MP_QSTR_authmode): {
                        req_if = SOFTAP_IF;
                        cfg.ap.authmode = mp_obj_get_int(kwargs->table[i].value);
                        break;
                    }
                    case QS(MP_QSTR_password): {
                        req_if = SOFTAP_IF;
                        size_t len;
                        const char *s = mp_obj_str_get_data(kwargs->table[i].value, &len);
                        len = MIN(len, sizeof(cfg.ap.password) - 1);
                        memcpy(cfg.ap.password, s, len);
                        cfg.ap.password[len] = 0;
                        break;
                    }
                    case QS(MP_QSTR_channel): {
                        req_if = SOFTAP_IF;
                        cfg.ap.channel = mp_obj_get_int(kwargs->table[i].value);
                        break;
                    }
                    case QS(MP_QSTR_dhcp_hostname): {
                        req_if = STATION_IF;
                        if (self->if_id == STATION_IF) {
                            const char *s = mp_obj_str_get_str(kwargs->table[i].value);
                            wifi_station_set_hostname((char*)s);
                        }
                        break;
                    }
                    default:
                        goto unknown;
                }
                #undef QS
            }
        }

        // We post-check interface requirements to save on code size
        if (req_if >= 0) {
            require_if(args[0], req_if);
        }

        if (self->if_id == STATION_IF) {
            error_check(wifi_station_set_config(&cfg.sta), "can't set STA config");
        } else {
            error_check(wifi_softap_set_config(&cfg.ap), "can't set AP config");
        }

        return mp_const_none;
    }

    // Get config

    if (n_args != 2) {
        mp_raise_TypeError("can query only one param");
    }

    mp_obj_t val;

    qstr key = mp_obj_str_get_qstr(args[1]);
    switch (key) {
        case MP_QSTR_mac: {
            uint8_t mac[6];
            wifi_get_macaddr(self->if_id, mac);
            return mp_obj_new_bytes(mac, sizeof(mac));
        }
        case MP_QSTR_essid:
            if (self->if_id == STATION_IF) {
                val = mp_obj_new_str((char*)cfg.sta.ssid, strlen((char*)cfg.sta.ssid));
            } else {
                val = mp_obj_new_str((char*)cfg.ap.ssid, cfg.ap.ssid_len);
            }
            break;
        case MP_QSTR_hidden:
            req_if = SOFTAP_IF;
            val = mp_obj_new_bool(cfg.ap.ssid_hidden);
            break;
        case MP_QSTR_authmode:
            req_if = SOFTAP_IF;
            val = MP_OBJ_NEW_SMALL_INT(cfg.ap.authmode);
            break;
        case MP_QSTR_channel:
            req_if = SOFTAP_IF;
            val = MP_OBJ_NEW_SMALL_INT(cfg.ap.channel);
            break;
        case MP_QSTR_dhcp_hostname: {
            req_if = STATION_IF;
            char* s = wifi_station_get_hostname();
            if (s == NULL) {
                val = MP_OBJ_NEW_QSTR(MP_QSTR_);
            } else {
                val = mp_obj_new_str(s, strlen(s));
            }
            break;
        }
        default:
            goto unknown;
    }

    // We post-check interface requirements to save on code size
    if (req_if >= 0) {
        require_if(args[0], req_if);
    }

    return val;

unknown:
    mp_raise_ValueError("unknown config param");
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(esp_config_obj, 1, esp_config);

STATIC const mp_rom_map_elem_t wlan_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&esp_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&esp_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&esp_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&esp_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&esp_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&esp_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&esp_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&esp_ifconfig_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wlan_if_locals_dict, wlan_if_locals_dict_table);

const mp_obj_type_t wlan_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_WLAN,
    .locals_dict = (mp_obj_dict_t*)&wlan_if_locals_dict,
};

STATIC mp_obj_t esp_phy_mode(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int(wifi_get_phy_mode());
    } else {
        wifi_set_phy_mode(mp_obj_get_int(args[0]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_phy_mode_obj, 0, 1, esp_phy_mode);

STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&get_wlan_obj) },
    { MP_ROM_QSTR(MP_QSTR_phy_mode), MP_ROM_PTR(&esp_phy_mode_obj) },

#if MODNETWORK_INCLUDE_CONSTANTS
    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(STATION_IF)},
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(SOFTAP_IF)},

    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(STATION_IDLE)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(STATION_CONNECTING)},
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(STATION_WRONG_PASSWORD)},
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(STATION_NO_AP_FOUND)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(STATION_CONNECT_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(STATION_GOT_IP)},

    { MP_ROM_QSTR(MP_QSTR_MODE_11B), MP_ROM_INT(PHY_MODE_11B) },
    { MP_ROM_QSTR(MP_QSTR_MODE_11G), MP_ROM_INT(PHY_MODE_11G) },
    { MP_ROM_QSTR(MP_QSTR_MODE_11N), MP_ROM_INT(PHY_MODE_11N) },

    { MP_ROM_QSTR(MP_QSTR_AUTH_OPEN), MP_ROM_INT(AUTH_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WEP), MP_ROM_INT(AUTH_WEP) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_PSK), MP_ROM_INT(AUTH_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_PSK), MP_ROM_INT(AUTH_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_WPA2_PSK), MP_ROM_INT(AUTH_WPA_WPA2_PSK) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t network_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};
