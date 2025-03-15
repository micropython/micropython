/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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
#include <string.h>
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK_CYW43

#include "lwip/netif.h"
#include "extmod/network_cyw43.h"
#include "modnetwork.h"

#include "lib/cyw43-driver/src/cyw43.h"
#include "lib/cyw43-driver/src/cyw43_country.h"

// This is the same as cyw43_pm_value but as a macro, to make it a true constant.
#define CYW43_PM_VALUE(pm_mode, pm2_sleep_ret_ms, li_beacon_period, li_dtim_period, li_assoc) \
    ((li_assoc) << 20 \
        | (li_dtim_period) << 16 \
        | (li_beacon_period) << 12 \
        | ((pm2_sleep_ret_ms) / 10) << 4 \
        | (pm_mode))

#define PM_NONE         (CYW43_PM_VALUE(CYW43_NO_POWERSAVE_MODE, 10, 0, 0, 0))
#define PM_PERFORMANCE  (CYW43_PM_VALUE(CYW43_PM2_POWERSAVE_MODE, 200, 1, 1, 10))
#define PM_POWERSAVE    (CYW43_PM_VALUE(CYW43_PM1_POWERSAVE_MODE, 10, 0, 0, 0))

typedef struct _network_cyw43_obj_t {
    mp_obj_base_t base;
    cyw43_t *cyw;
    int itf;
} network_cyw43_obj_t;

static const network_cyw43_obj_t network_cyw43_wl_sta = { { &mp_network_cyw43_type }, &cyw43_state, CYW43_ITF_STA };
static const network_cyw43_obj_t network_cyw43_wl_ap = { { &mp_network_cyw43_type }, &cyw43_state, CYW43_ITF_AP };

// Avoid race conditions with callbacks by tracking the last up or down request
// we have made for each interface.
static bool if_active[2];

static void network_cyw43_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct netif *netif = &self->cyw->netif[self->itf];
    int status = cyw43_tcpip_link_status(self->cyw, self->itf);
    const char *status_str;
    if (status == CYW43_LINK_DOWN) {
        status_str = "down";
    } else if (status == CYW43_LINK_JOIN || status == CYW43_LINK_NOIP) {
        status_str = "join";
    } else if (status == CYW43_LINK_UP) {
        status_str = "up";
    } else if (status == CYW43_LINK_NONET) {
        status_str = "nonet";
    } else if (status == CYW43_LINK_BADAUTH) {
        status_str = "badauth";
    } else {
        status_str = "fail";
    }
    ip4_addr_t *addr = ip_2_ip4(&netif->ip_addr);
    mp_printf(print, "<CYW43 %s %s %u.%u.%u.%u>",
        self->itf == CYW43_ITF_STA ? "STA" : "AP",
        status_str,
        addr->addr & 0xff,
        addr->addr >> 8 & 0xff,
        addr->addr >> 16 & 0xff,
        addr->addr >> 24
        );
}

static mp_obj_t network_cyw43_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    if (n_args == 0 || mp_obj_get_int(args[0]) == MOD_NETWORK_STA_IF) {
        return MP_OBJ_FROM_PTR(&network_cyw43_wl_sta);
    } else {
        return MP_OBJ_FROM_PTR(&network_cyw43_wl_ap);
    }
}

static mp_obj_t network_cyw43_send_ethernet(mp_obj_t self_in, mp_obj_t buf_in) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    int ret = cyw43_send_ethernet(self->cyw, self->itf, buf.len, buf.buf, false);
    if (ret) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(network_cyw43_send_ethernet_obj, network_cyw43_send_ethernet);

static mp_obj_t network_cyw43_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t buf_in) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ | MP_BUFFER_WRITE);
    cyw43_ioctl(self->cyw, mp_obj_get_int(cmd_in), buf.len, buf.buf, self->itf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(network_cyw43_ioctl_obj, network_cyw43_ioctl);

/*******************************************************************************/
// network API

static uint32_t get_country_code(void) {
    return CYW43_COUNTRY(mod_network_country_code[0], mod_network_country_code[1], 0);
}

static mp_obj_t network_cyw43_deinit(mp_obj_t self_in) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyw43_deinit(self->cyw);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_cyw43_deinit_obj, network_cyw43_deinit);

static mp_obj_t network_cyw43_active(size_t n_args, const mp_obj_t *args) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(if_active[self->itf]);
    } else {
        bool value = mp_obj_is_true(args[1]);
        cyw43_wifi_set_up(self->cyw, self->itf, value, get_country_code());
        if_active[self->itf] = value;
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_cyw43_active_obj, 1, 2, network_cyw43_active);

static int network_cyw43_scan_cb(void *env, const cyw43_ev_scan_result_t *res) {
    mp_obj_t list = MP_OBJ_FROM_PTR(env);

    // Search for existing BSSID to remove duplicates
    bool found = false;
    size_t len;
    mp_obj_t *items;
    mp_obj_get_array(list, &len, &items);
    for (size_t i = 0; i < len; ++i) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(items[i]);
        if (memcmp(res->bssid, ((mp_obj_str_t *)MP_OBJ_TO_PTR(t->items[1]))->data, sizeof(res->bssid)) == 0) {
            if (res->rssi > MP_OBJ_SMALL_INT_VALUE(t->items[3])) {
                t->items[3] = MP_OBJ_NEW_SMALL_INT(res->rssi);
            }
            t->items[5] = MP_OBJ_NEW_SMALL_INT(MP_OBJ_SMALL_INT_VALUE(t->items[5]) + 1);
            found = true;
            break;
        }
    }

    // Add to list of results if wanted
    if (!found) {
        mp_obj_t tuple[6] = {
            mp_obj_new_bytes(res->ssid, res->ssid_len),
            mp_obj_new_bytes(res->bssid, sizeof(res->bssid)),
            MP_OBJ_NEW_SMALL_INT(res->channel),
            MP_OBJ_NEW_SMALL_INT(res->rssi),
            MP_OBJ_NEW_SMALL_INT(res->auth_mode),
            // mp_const_false, // hidden
            MP_OBJ_NEW_SMALL_INT(1), // N
        };
        mp_obj_list_append(list, mp_obj_new_tuple(6, tuple));
    }

    return 0; // continue scan
}

static mp_obj_t network_cyw43_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_passive, ARG_ssid, ARG_essid, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_passive, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_ssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_essid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Deprecated kwarg
    if (args[ARG_essid].u_obj != mp_const_none) {
        args[ARG_ssid].u_obj = args[ARG_essid].u_obj;
    }

    cyw43_wifi_scan_options_t opts;
    opts.scan_type = args[ARG_passive].u_bool ? 1 : 0;
    if (args[ARG_ssid].u_obj == mp_const_none) {
        opts.ssid_len = 0;
    } else {
        mp_buffer_info_t ssid;
        mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);
        opts.ssid_len = MIN(ssid.len, sizeof(opts.ssid));
        memcpy(opts.ssid, ssid.buf, opts.ssid_len);
    }
    if (args[ARG_bssid].u_obj == mp_const_none) {
        memset(opts.bssid, 0xff, sizeof(opts.bssid));
    } else {
        mp_buffer_info_t bssid;
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        memcpy(opts.bssid, bssid.buf, sizeof(opts.bssid));
    }

    mp_obj_t res = mp_obj_new_list(0, NULL);
    int scan_res = cyw43_wifi_scan(self->cyw, &opts, MP_OBJ_TO_PTR(res), network_cyw43_scan_cb);

    if (scan_res < 0) {
        mp_raise_OSError(-scan_res);
    }

    // Wait for scan to finish, with a 10s timeout
    uint32_t start = mp_hal_ticks_ms();
    const uint32_t TIMEOUT = 10000;
    while (cyw43_wifi_scan_active(self->cyw)) {
        uint32_t elapsed = mp_hal_ticks_ms() - start;
        if (elapsed >= TIMEOUT) {
            break;
        }
        mp_event_wait_ms(TIMEOUT - elapsed);
    }

    return res;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_cyw43_scan_obj, 1, network_cyw43_scan);

static mp_obj_t network_cyw43_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_auth, ARG_security, ARG_bssid, ARG_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_auth, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = CYW43_CHANNEL_NONE} },
    };

    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Deprecated kwarg
    if (args[ARG_auth].u_int != -1) {
        args[ARG_security] = args[ARG_auth];
    }

    // Extract the SSID.
    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);

    // Extract the key, if given.
    mp_buffer_info_t key;
    key.buf = NULL;
    if (args[ARG_key].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_key].u_obj, &key, MP_BUFFER_READ);
    }

    // Extract the BSSID, if given.
    mp_buffer_info_t bssid;
    bssid.buf = NULL;
    if (args[ARG_bssid].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        if (bssid.len != 6) {
            mp_raise_ValueError(NULL);
        }
    }

    // Extract the security type, if given.
    uint32_t auth_type;
    if (args[ARG_security].u_int == -1) {
        if (key.buf == NULL || key.len == 0) {
            // Default to open when no password set.
            auth_type = CYW43_AUTH_OPEN;
        } else {
            // Default to WPA2 otherwise. All other modes require the security
            // kwarg to be set explicitly.
            auth_type = CYW43_AUTH_WPA2_MIXED_PSK;
        }
    } else {
        auth_type = args[ARG_security].u_int;
    }

    // Start the WiFi join procedure.  It will run in the background.
    int ret = cyw43_wifi_join(self->cyw, ssid.len, ssid.buf, key.len, key.buf,
        auth_type, bssid.buf, args[ARG_channel].u_int);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_cyw43_connect_obj, 1, network_cyw43_connect);

static mp_obj_t network_cyw43_disconnect(mp_obj_t self_in) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(self_in);
    cyw43_wifi_leave(self->cyw, self->itf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_cyw43_disconnect_obj, network_cyw43_disconnect);

static mp_obj_t network_cyw43_isconnected(mp_obj_t self_in) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool result = (cyw43_tcpip_link_status(self->cyw, self->itf) == CYW43_LINK_UP);

    if (result && self->itf == CYW43_ITF_AP) {
        // For AP we need to not only know if the link is up, but also if any stations
        // have associated.
        uint8_t mac_buf[6];
        int num_stas = 1;
        cyw43_wifi_ap_get_stas(self->cyw, &num_stas, mac_buf);
        result = num_stas > 0;
    }
    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_cyw43_isconnected_obj, network_cyw43_isconnected);

static mp_obj_t network_cyw43_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->cyw->netif[self->itf], n_args - 1, args + 1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_cyw43_ifconfig_obj, 1, 2, network_cyw43_ifconfig);

static mp_obj_t network_cyw43_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(&self->cyw->netif[self->itf], n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_cyw43_ipconfig_obj, 1, network_cyw43_ipconfig);

static mp_obj_t network_cyw43_status(size_t n_args, const mp_obj_t *args) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;

    if (n_args == 1) {
        // no arguments: return link status
        return MP_OBJ_NEW_SMALL_INT(cyw43_tcpip_link_status(self->cyw, self->itf));
    }

    // one argument: return status based on query parameter
    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_rssi: {
            if (self->itf != CYW43_ITF_STA) {
                mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
            }
            int32_t rssi;
            cyw43_wifi_get_rssi(self->cyw, &rssi);
            return mp_obj_new_int(rssi);
        }
        case MP_QSTR_stations: {
            // return list of connected stations
            if (self->itf != CYW43_ITF_AP) {
                mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
            }
            static const unsigned mac_len = 6;
            static const unsigned max_stas = 32;
            int num_stas = max_stas;
            uint8_t macs[max_stas * mac_len];
            cyw43_wifi_ap_get_stas(self->cyw, &num_stas, macs);
            mp_obj_t list = mp_obj_new_list(num_stas, NULL);
            for (int i = 0; i < num_stas; ++i) {
                mp_obj_t tuple[1] = {
                    mp_obj_new_bytes(&macs[i * mac_len], mac_len),
                };
                ((mp_obj_list_t *)MP_OBJ_TO_PTR(list))->items[i] = mp_obj_new_tuple(1, tuple);
            }
            return list;
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_cyw43_status_obj, 1, 2, network_cyw43_status);

static inline uint32_t nw_get_le32(const uint8_t *buf) {
    return buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
}

static inline void nw_put_le32(uint8_t *buf, uint32_t x) {
    buf[0] = x;
    buf[1] = x >> 8;
    buf[2] = x >> 16;
    buf[3] = x >> 24;
}

static mp_obj_t network_cyw43_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_cyw43_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_antenna: {
                uint8_t buf[4];
                cyw43_ioctl(self->cyw, CYW43_IOCTL_GET_ANTDIV, 4, buf, self->itf);
                return MP_OBJ_NEW_SMALL_INT(nw_get_le32(buf));
            }
            case MP_QSTR_channel: {
                uint8_t buf[4];
                cyw43_ioctl(self->cyw, CYW43_IOCTL_GET_CHANNEL, 4, buf, self->itf);
                return MP_OBJ_NEW_SMALL_INT(nw_get_le32(buf));
            }
            case MP_QSTR_ssid:
            case MP_QSTR_essid: {
                if (self->itf == CYW43_ITF_STA) {
                    uint8_t buf[36];
                    cyw43_ioctl(self->cyw, CYW43_IOCTL_GET_SSID, 36, buf, self->itf);
                    return mp_obj_new_str((const char *)buf + 4, nw_get_le32(buf));
                } else {
                    size_t len;
                    const uint8_t *buf;
                    cyw43_wifi_ap_get_ssid(self->cyw, &len, &buf);
                    return mp_obj_new_str((const char *)buf, len);
                }
            }
            case MP_QSTR_security: {
                return MP_OBJ_NEW_SMALL_INT(cyw43_wifi_ap_get_auth(self->cyw));
            }
            case MP_QSTR_mac: {
                uint8_t buf[6];
                cyw43_wifi_get_mac(self->cyw, self->itf, buf);
                return mp_obj_new_bytes(buf, 6);
            }
            case MP_QSTR_pm: {
                uint32_t pm;
                cyw43_wifi_get_pm(self->cyw, &pm);
                return MP_OBJ_NEW_SMALL_INT(pm);
            }
            case MP_QSTR_txpower: {
                uint8_t buf[13];
                memcpy(buf, "qtxpower\x00\x00\x00\x00\x00", 13);
                cyw43_ioctl(self->cyw, CYW43_IOCTL_GET_VAR, 13, buf, self->itf);
                return MP_OBJ_NEW_SMALL_INT(nw_get_le32(buf) / 4);
            }
            case MP_QSTR_hostname: {
                // TODO: Deprecated. Use network.hostname() instead.
                return mod_network_hostname(0, NULL);
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        // A number of these options only update buffers in memory, and
        // won't do anything until the interface is cycled down and back up
        bool cycle_active = false;

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_antenna: {
                        uint8_t buf[4];
                        nw_put_le32(buf, mp_obj_get_int(e->value));
                        cyw43_ioctl(self->cyw, CYW43_IOCTL_SET_ANTDIV, 4, buf, self->itf);
                        break;
                    }
                    case MP_QSTR_channel: {
                        cyw43_wifi_ap_set_channel(self->cyw, mp_obj_get_int(e->value));
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_ssid:
                    case MP_QSTR_essid: {
                        size_t len;
                        const char *str = mp_obj_str_get_data(e->value, &len);
                        cyw43_wifi_ap_set_ssid(self->cyw, len, (const uint8_t *)str);
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_monitor: {
                        mp_int_t value = mp_obj_get_int(e->value);
                        uint8_t buf[9 + 4];
                        memcpy(buf, "allmulti\x00", 9);
                        nw_put_le32(buf + 9, value);
                        cyw43_ioctl(self->cyw, CYW43_IOCTL_SET_VAR, 9 + 4, buf, self->itf);
                        nw_put_le32(buf, value);
                        cyw43_ioctl(self->cyw, CYW43_IOCTL_SET_MONITOR, 4, buf, self->itf);
                        if (value) {
                            self->cyw->trace_flags |= CYW43_TRACE_MAC;
                        } else {
                            self->cyw->trace_flags &= ~CYW43_TRACE_MAC;
                        }
                        break;
                    }
                    case MP_QSTR_security: {
                        cyw43_wifi_ap_set_auth(self->cyw, mp_obj_get_int(e->value));
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_key:
                    case MP_QSTR_password: {
                        size_t len;
                        const char *str = mp_obj_str_get_data(e->value, &len);
                        cyw43_wifi_ap_set_password(self->cyw, len, (const uint8_t *)str);
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_pm: {
                        cyw43_wifi_pm(self->cyw, mp_obj_get_int(e->value));
                        break;
                    }
                    case MP_QSTR_trace: {
                        self->cyw->trace_flags = mp_obj_get_int(e->value);
                        break;
                    }
                    case MP_QSTR_txpower: {
                        mp_int_t dbm = mp_obj_get_int(e->value);
                        uint8_t buf[9 + 4];
                        memcpy(buf, "qtxpower\x00", 9);
                        nw_put_le32(buf + 9, dbm * 4);
                        cyw43_ioctl(self->cyw, CYW43_IOCTL_SET_VAR, 9 + 4, buf, self->itf);
                        break;
                    }
                    case MP_QSTR_hostname: {
                        // TODO: Deprecated. Use network.hostname(name) instead.
                        mod_network_hostname(1, &e->value);
                        break;
                    }
                    default:
                        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
                }
            }
        }

        // If the interface is already active, cycle it down and up
        if (cycle_active && if_active[self->itf]) {
            uint32_t country = get_country_code();
            cyw43_wifi_set_up(self->cyw, self->itf, false, country);
            cyw43_wifi_set_up(self->cyw, self->itf, true, country);
        }

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_cyw43_config_obj, 1, network_cyw43_config);

/*******************************************************************************/
// class bindings

static const mp_rom_map_elem_t network_cyw43_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_ethernet), MP_ROM_PTR(&network_cyw43_send_ethernet_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&network_cyw43_ioctl_obj) },

    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&network_cyw43_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_cyw43_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&network_cyw43_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_cyw43_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_cyw43_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_cyw43_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_cyw43_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&network_cyw43_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_cyw43_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&network_cyw43_config_obj) },

    // Class constants.
    { MP_ROM_QSTR(MP_QSTR_IF_STA), MP_ROM_INT(MOD_NETWORK_STA_IF) },
    { MP_ROM_QSTR(MP_QSTR_IF_AP), MP_ROM_INT(MOD_NETWORK_AP_IF) },
    { MP_ROM_QSTR(MP_QSTR_SEC_OPEN), MP_ROM_INT(CYW43_AUTH_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_WPA2), MP_ROM_INT(CYW43_AUTH_WPA2_MIXED_PSK) },

    { MP_ROM_QSTR(MP_QSTR_PM_NONE), MP_ROM_INT(PM_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PM_PERFORMANCE), MP_ROM_INT(PM_PERFORMANCE) },
    { MP_ROM_QSTR(MP_QSTR_PM_POWERSAVE), MP_ROM_INT(PM_POWERSAVE) },
};
static MP_DEFINE_CONST_DICT(network_cyw43_locals_dict, network_cyw43_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_cyw43_type,
    MP_QSTR_CYW43,
    MP_TYPE_FLAG_NONE,
    make_new, network_cyw43_make_new,
    print, network_cyw43_print,
    locals_dict, &network_cyw43_locals_dict
    );

#endif // MICROPY_PY_NETWORK_CYW43
