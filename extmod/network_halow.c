/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include "lwip/netif.h"
#include "extmod/network_halow.h"
#include "modnetwork.h"

#include "drivers/halow/halow.h"
#include "drivers/halow/halow_sched.h"

typedef struct _network_halow_obj_t {
    mp_obj_base_t base;
    halow_t *halow;
    int itf;
} network_halow_obj_t;

static const network_halow_obj_t network_halow_wl_sta = { { &mp_network_halow_type }, &halow_state, HALOW_ITF_STA };
#if MICROPY_PY_NETWORK_HALOW_AP
static const network_halow_obj_t network_halow_wl_ap = { { &mp_network_halow_type }, &halow_state, HALOW_ITF_AP };
#endif

// Tracks the last up or down request made for each interface, to avoid races
// with the callbacks.
static bool if_active[2];

// if_active[] is a static that would otherwise survive into the next run.
void network_halow_deinit_all(void) {
    halow_deinit(&halow_state);
    if_active[0] = false;
    if_active[1] = false;
}

static void network_halow_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct netif *netif = &self->halow->netif[self->itf];
    int status = halow_tcpip_link_status(self->halow, self->itf);
    const char *status_str;
    if (status == HALOW_LINK_DOWN) {
        status_str = "down";
    } else if (status == HALOW_LINK_JOIN || status == HALOW_LINK_NOIP) {
        status_str = "join";
    } else if (status == HALOW_LINK_UP) {
        status_str = "up";
    } else if (status == HALOW_LINK_NONET) {
        status_str = "nonet";
    } else if (status == HALOW_LINK_BADAUTH) {
        status_str = "badauth";
    } else {
        status_str = "fail";
    }
    ip4_addr_t *addr = ip_2_ip4(&netif->ip_addr);
    mp_printf(print, "<HALOW %s %s %u.%u.%u.%u>",
        self->itf == HALOW_ITF_STA ? "STA" : "AP",
        status_str,
        addr->addr & 0xff,
        addr->addr >> 8 & 0xff,
        addr->addr >> 16 & 0xff,
        addr->addr >> 24
        );
}

static mp_obj_t network_halow_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    if (n_args == 0 || mp_obj_get_int(args[0]) == MOD_NETWORK_STA_IF) {
        return MP_OBJ_FROM_PTR(&network_halow_wl_sta);
    }
    #if MICROPY_PY_NETWORK_HALOW_AP
    return MP_OBJ_FROM_PTR(&network_halow_wl_ap);
    #else
    mp_raise_ValueError(MP_ERROR_TEXT("AP not supported"));
    #endif
}

static mp_obj_t network_halow_send_ethernet(mp_obj_t self_in, mp_obj_t buf_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    int ret = halow_send_ethernet(self->halow, self->itf, buf.len, buf.buf, false);
    if (ret) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(network_halow_send_ethernet_obj, network_halow_send_ethernet);

// A vendor command passthrough, in the shape of cyw43's ioctl(): the buffer
// carries the command in and the response out.  morselib's test interface has no
// command code of its own, so both ioctl(buf) and cyw43's ioctl(cmd, buf) are
// accepted, and in the second form the command is ignored.
static mp_obj_t network_halow_ioctl(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(args[n_args - 1], &buf, MP_BUFFER_READ | MP_BUFFER_WRITE);

    // The command is copied out first: morselib may modify the buffer it is
    // given, and the response is written back over the same one.
    vstr_t vstr;
    vstr_init_len(&vstr, buf.len);
    memcpy(vstr.buf, buf.buf, buf.len);

    size_t rsp_len = buf.len;
    int ret = halow_wifi_ate_command(self->halow, (uint8_t *)vstr.buf, buf.len,
        buf.buf, &rsp_len);
    vstr_clear(&vstr);
    if (ret) {
        mp_raise_OSError(-ret);
    }
    return MP_OBJ_NEW_SMALL_INT(rsp_len);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_halow_ioctl_obj, 2, 3, network_halow_ioctl);

/*******************************************************************************/
// network API

static const char *get_country_code(void) {
    return mod_network_country_code;
}

static mp_obj_t network_halow_deinit(mp_obj_t self_in) {
    if (halow_sched_in_wait()) {
        // Reached from a scheduled callback that ran while the driver was
        // waiting inside morselib.  Giving the memory pool back here would free
        // it out from under the frames that are still standing on it.
        mp_raise_OSError(MP_EBUSY);
    }
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    halow_deinit(self->halow);
    if_active[HALOW_ITF_STA] = false;
    if_active[HALOW_ITF_AP] = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_deinit_obj, network_halow_deinit);

static mp_obj_t network_halow_active(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(if_active[self->itf]);
    } else {
        bool value = mp_obj_is_true(args[1]);
        if (value && !halow_country_supported(get_country_code())) {
            // 802.11ah channel plans are country specific and there is no
            // worldwide default, so refuse to come up until one is chosen.
            mp_raise_ValueError(MP_ERROR_TEXT("country not set"));
        }
        if (!value && self->itf == HALOW_ITF_STA) {
            halow_wifi_leave(self->halow, self->itf);
        }
        int ret = halow_wifi_set_up(self->halow, self->itf, value, get_country_code());
        if (ret) {
            mp_raise_OSError(-ret);
        }
        if_active[self->itf] = value;
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_halow_active_obj, 1, 2, network_halow_active);


static mp_obj_t network_halow_scan(mp_obj_t self_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->itf != HALOW_ITF_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
    }

    uint32_t pm;
    halow_wifi_get_pm(self->halow, &pm);
    if (pm != HALOW_PM_NONE) {
        // Power saving is a transmit-only mode; the transceiver is not
        // listening, so it cannot see probe responses.
        mp_raise_OSError(MP_EPERM);
    }

    // Off the heap rather than the stack: the sweep can report a good number of
    // networks and each result is not small.
    halow_ev_scan_result_t *results =
        m_new(halow_ev_scan_result_t, HALOW_SCAN_CACHE_MAX);
    size_t n = halow_wifi_scan_cached(self->halow, results, HALOW_SCAN_CACHE_MAX);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (size_t i = 0; i < n; i++) {
        const halow_ev_scan_result_t *r = &results[i];
        mp_obj_t tuple[6] = {
            mp_obj_new_bytes(r->ssid, r->ssid_len),
            mp_obj_new_bytes(r->bssid, sizeof(r->bssid)),
            // The S1G channel number, as config("channel") reports it.  Zero
            // for an access point outside the local regulatory plan.
            MP_OBJ_NEW_SMALL_INT(r->chan_num),
            MP_OBJ_NEW_SMALL_INT(r->rssi),
            MP_OBJ_NEW_SMALL_INT(r->security),
            // The transceiver does not report whether the network is hidden,
            // and an empty SSID is the only sign of it in a beacon.
            mp_obj_new_bool(r->ssid_len == 0),
        };
        mp_obj_list_append(list, mp_obj_new_tuple(6, tuple));
    }
    m_del(halow_ev_scan_result_t, results, HALOW_SCAN_CACHE_MAX);
    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_scan_obj, network_halow_scan);

static mp_obj_t network_halow_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_security, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    network_halow_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);

    mp_buffer_info_t key;
    key.buf = NULL;
    key.len = 0;
    if (args[ARG_key].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_key].u_obj, &key, MP_BUFFER_READ);
    }

    mp_buffer_info_t bssid;
    bssid.buf = NULL;
    if (args[ARG_bssid].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        if (bssid.len != 6) {
            mp_raise_ValueError(NULL);
        }
    }

    uint32_t auth_type;
    if (args[ARG_security].u_obj == mp_const_none) {
        if (key.buf == NULL || key.len == 0) {
            // Default to open when no password set.
            auth_type = HALOW_SEC_OPEN;
        } else {
            // 802.11ah has no WPA2-PSK, so a passphrase always means SAE.
            auth_type = HALOW_SEC_SAE;
        }
    } else {
        auth_type = mp_obj_get_int(args[ARG_security].u_obj);
    }

    int ret = halow_wifi_join(self->halow, ssid.len, ssid.buf, key.len, key.buf,
        auth_type, bssid.buf);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_halow_connect_obj, 1, network_halow_connect);

static mp_obj_t network_halow_disconnect(mp_obj_t self_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    halow_wifi_leave(self->halow, self->itf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_disconnect_obj, network_halow_disconnect);

static mp_obj_t network_halow_isconnected(mp_obj_t self_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool result = (halow_tcpip_link_status(self->halow, self->itf) == HALOW_LINK_UP);

    #if MICROPY_PY_NETWORK_HALOW_AP
    if (result && self->itf == HALOW_ITF_AP) {
        // For AP we need to not only know if the link is up, but also if any stations
        // have associated.
        uint8_t mac_buf[6];
        int num_stas = 1;
        halow_wifi_ap_get_stas(self->halow, &num_stas, mac_buf);
        result = num_stas > 0;
    }
    #endif
    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_isconnected_obj, network_halow_isconnected);

static mp_obj_t network_halow_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->halow->netif[self->itf], n_args - 1, args + 1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_halow_ifconfig_obj, 1, 2, network_halow_ifconfig);

static mp_obj_t network_halow_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(&self->halow->netif[self->itf], n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_halow_ipconfig_obj, 1, network_halow_ipconfig);

static mp_obj_t network_halow_status(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // no arguments: return link status
        return MP_OBJ_NEW_SMALL_INT(halow_tcpip_link_status(self->halow, self->itf));
    }

    // one argument: return status based on query parameter
    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_rssi: {
            if (self->itf != HALOW_ITF_STA) {
                mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
            }
            int32_t rssi;
            halow_wifi_get_rssi(self->halow, &rssi);
            return mp_obj_new_int(rssi);
        }
        case MP_QSTR_rates: {
            // (mcs, bandwidth_mhz, gi, sent, success) per rate table entry that
            // has been used, so the list stays about the link rather than the
            // whole table.
            // Room for the copy is taken first: the vendor's block is not
            // garbage collected, so an allocation that raises after asking for
            // it would strand it for good.
            uint32_t *info = m_new(uint32_t, HALOW_RC_STATS_MAX * 3);

            struct mmwlan_rc_stats *st;
            int ret = halow_wifi_get_rc_stats(self->halow, &st);
            if (ret) {
                m_del(uint32_t, info, HALOW_RC_STATS_MAX * 3);
                mp_raise_OSError(-ret);
            }
            if (st == NULL) {
                m_del(uint32_t, info, HALOW_RC_STATS_MAX * 3);
                return mp_obj_new_list(0, NULL);
            }
            uint32_t n = MIN(st->n_entries, (uint32_t)HALOW_RC_STATS_MAX);
            for (uint32_t i = 0; i < n; i++) {
                info[i * 3] = st->rate_info[i];
                info[i * 3 + 1] = st->total_sent[i];
                info[i * 3 + 2] = st->total_success[i];
            }
            halow_wifi_free_rc_stats(st);

            mp_obj_t list = mp_obj_new_list(0, NULL);
            for (uint32_t i = 0; i < n; i++) {
                if (info[i * 3 + 1] == 0) {
                    continue;
                }
                uint32_t rate = info[i * 3];
                // The bandwidth field is an index, not a width: 0 is 1MHz.
                uint32_t bw = (rate >> HALOW_RC_BW_SHIFT) & HALOW_RC_FIELD_MASK;
                mp_obj_t entry[5] = {
                    MP_OBJ_NEW_SMALL_INT((rate >> HALOW_RC_RATE_SHIFT) & HALOW_RC_FIELD_MASK),
                    MP_OBJ_NEW_SMALL_INT(1 << bw),
                    MP_OBJ_NEW_SMALL_INT((rate >> HALOW_RC_GI_SHIFT) & HALOW_RC_GI_MASK),
                    mp_obj_new_int_from_uint(info[i * 3 + 1]),
                    mp_obj_new_int_from_uint(info[i * 3 + 2]),
                };
                mp_obj_list_append(list, mp_obj_new_tuple(5, entry));
            }
            m_del(uint32_t, info, HALOW_RC_STATS_MAX * 3);
            return list;
        }
        case MP_QSTR_duty_cycle: {
            struct mmwlan_duty_cycle_stats st;
            int ret = halow_wifi_get_duty_cycle(self->halow, &st);
            if (ret) {
                mp_raise_OSError(-ret);
            }
            mp_obj_t items[4] = {
                mp_obj_new_int_from_uint(st.duty_cycle),
                MP_OBJ_NEW_SMALL_INT(st.mode),
                mp_obj_new_int_from_uint(st.burst_airtime_remaining_us),
                mp_obj_new_int_from_uint(st.burst_window_duration_us),
            };
            return mp_obj_new_tuple(4, items);
        }
        case MP_QSTR_bssid: {
            if (self->itf != HALOW_ITF_STA) {
                mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
            }
            uint8_t bssid[6];
            int ret = halow_wifi_get_bssid(self->halow, bssid);
            if (ret) {
                mp_raise_OSError(-ret);
            }
            return mp_obj_new_bytes(bssid, sizeof(bssid));
        }
        #if MICROPY_PY_NETWORK_HALOW_AP
        case MP_QSTR_stations: {
            // return list of connected stations
            if (self->itf != HALOW_ITF_AP) {
                mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
            }
            static const unsigned mac_len = 6;
            static const unsigned max_stas = 32;
            int num_stas = max_stas;
            uint8_t macs[max_stas * mac_len];
            halow_wifi_ap_get_stas(self->halow, &num_stas, macs);
            mp_obj_t list = mp_obj_new_list(num_stas, NULL);
            for (int i = 0; i < num_stas; ++i) {
                mp_obj_t tuple[1] = {
                    mp_obj_new_bytes(&macs[i * mac_len], mac_len),
                };
                ((mp_obj_list_t *)MP_OBJ_TO_PTR(list))->items[i] = mp_obj_new_tuple(1, tuple);
            }
            return list;
        }
        #endif
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_halow_status_obj, 1, 2, network_halow_status);

// Settings that reach a narrow field.  Without this a negative or oversized
// value wraps: txpower=-5 became 65531, which the transceiver reads as "no
// limit" -- the opposite of what was asked for.
static uint32_t halow_arg_range(mp_obj_t obj, uint32_t max) {
    mp_int_t value = mp_obj_get_int(obj);
    if (value < 0 || (uint32_t)value > max) {
        mp_raise_ValueError(MP_ERROR_TEXT("value out of range"));
    }
    return (uint32_t)value;
}

static void halow_set_radio(network_halow_obj_t *self, int what, uint32_t value) {
    int ret = halow_wifi_set_radio(self->halow, what, value);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
}

static mp_obj_t network_halow_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("can query only one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_channel: {
                // The S1G channel number, as used by 802.11ah operating classes.
                uint16_t chan_num;
                uint8_t bw_mhz;
                int ret = halow_wifi_get_channel(self->halow, self->itf, &chan_num, &bw_mhz);
                if (ret) {
                    mp_raise_OSError(-ret);
                }
                return MP_OBJ_NEW_SMALL_INT(chan_num);
            }
            case MP_QSTR_bandwidth: {
                // Width of the primary channel in MHz.  A frequency on its own
                // does not identify an S1G channel; the width is needed too.
                uint16_t chan_num;
                uint8_t bw_mhz;
                int ret = halow_wifi_get_channel(self->halow, self->itf, &chan_num, &bw_mhz);
                if (ret) {
                    mp_raise_OSError(-ret);
                }
                return MP_OBJ_NEW_SMALL_INT(bw_mhz);
            }
            case MP_QSTR_ssid:
            case MP_QSTR_essid: {
                #if MICROPY_PY_NETWORK_HALOW_AP
                if (self->itf == HALOW_ITF_AP) {
                    size_t len;
                    const uint8_t *buf;
                    halow_wifi_ap_get_ssid(self->halow, &len, &buf);
                    return mp_obj_new_str((const char *)buf, len);
                }
                #endif
                return mp_obj_new_str((const char *)self->halow->sta_ssid, self->halow->sta_ssid_len);
            }
            #if MICROPY_PY_NETWORK_HALOW_AP
            case MP_QSTR_security: {
                return MP_OBJ_NEW_SMALL_INT(halow_wifi_ap_get_auth(self->halow));
            }
            #endif
            case MP_QSTR_version: {
                struct mmwlan_version v;
                int ret = halow_wifi_get_version(self->halow, &v);
                if (ret) {
                    mp_raise_OSError(-ret);
                }
                mp_obj_t items[3] = {
                    mp_obj_new_str_from_cstr(v.morselib_version),
                    mp_obj_new_str_from_cstr(v.morse_fw_version),
                    mp_obj_new_str_from_cstr(v.morse_chip_id_string),
                };
                return mp_obj_new_tuple(3, items);
            }
            case MP_QSTR_ampdu:
                return mp_obj_new_bool(halow_wifi_get_radio(self->halow, HALOW_RADIO_AMPDU));
            case MP_QSTR_sgi:
                return mp_obj_new_bool(halow_wifi_get_radio(self->halow, HALOW_RADIO_SGI));
            case MP_QSTR_subbands:
                return mp_obj_new_bool(halow_wifi_get_radio(self->halow, HALOW_RADIO_SUBBANDS));
            case MP_QSTR_rts_threshold:
                return mp_obj_new_int_from_uint(halow_wifi_get_radio(self->halow, HALOW_RADIO_RTS));
            case MP_QSTR_fragment_threshold:
                return mp_obj_new_int_from_uint(halow_wifi_get_radio(self->halow, HALOW_RADIO_FRAG));
            case MP_QSTR_wnm_powerdown:
                return mp_obj_new_bool(halow_wifi_get_radio(self->halow, HALOW_RADIO_WNM_PD));
            case MP_QSTR_listen_interval:
                return MP_OBJ_NEW_SMALL_INT(halow_wifi_get_radio(self->halow, HALOW_RADIO_LISTEN));
            case MP_QSTR_txpower:
                return MP_OBJ_NEW_SMALL_INT(halow_wifi_get_radio(self->halow, HALOW_RADIO_TXPOWER));
            case MP_QSTR_duty_cycle:
                return MP_OBJ_NEW_SMALL_INT(self->halow->duty_cycle_mode);
            case MP_QSTR_mac: {
                uint8_t buf[6];
                halow_wifi_get_mac(self->halow, self->itf, buf);
                return mp_obj_new_bytes(buf, 6);
            }
            case MP_QSTR_pm: {
                uint32_t pm;
                halow_wifi_get_pm(self->halow, &pm);
                return MP_OBJ_NEW_SMALL_INT(pm);
            }
            case MP_QSTR_ps_timeout: {
                uint32_t ms;
                halow_wifi_get_ps_timeout(self->halow, &ms);
                return mp_obj_new_int_from_uint(ms);
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
                    #if MICROPY_PY_NETWORK_HALOW_AP
                    case MP_QSTR_ssid:
                    case MP_QSTR_essid:
                    case MP_QSTR_security:
                    case MP_QSTR_key:
                    case MP_QSTR_password:
                        // These describe the network this interface hands out,
                        // so they only mean anything on the access point.  On
                        // the station they would write the AP's settings and
                        // then cycle the station, dropping its association.
                        if (self->itf != HALOW_ITF_AP) {
                            mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
                        }
                        break;
                    #endif
                    default:
                        break;
                }

                switch (mp_obj_str_get_qstr(e->key)) {
                    #if MICROPY_PY_NETWORK_HALOW_AP
                    case MP_QSTR_ssid:
                    case MP_QSTR_essid: {
                        size_t len;
                        const char *str = mp_obj_str_get_data(e->value, &len);
                        halow_wifi_ap_set_ssid(self->halow, len, (const uint8_t *)str);
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_security: {
                        halow_wifi_ap_set_auth(self->halow, mp_obj_get_int(e->value));
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_channel: {
                        if (self->itf != HALOW_ITF_AP) {
                            mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
                        }
                        halow_wifi_ap_set_channel(self->halow, mp_obj_get_int(e->value));
                        cycle_active = true;
                        break;
                    }
                    case MP_QSTR_key:
                    case MP_QSTR_password: {
                        size_t len;
                        const char *str = mp_obj_str_get_data(e->value, &len);
                        halow_wifi_ap_set_password(self->halow, len, (const uint8_t *)str);
                        cycle_active = true;
                        break;
                    }
                    #endif
                    case MP_QSTR_ampdu: {
                        halow_set_radio(self, HALOW_RADIO_AMPDU, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_sgi: {
                        halow_set_radio(self, HALOW_RADIO_SGI, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_subbands: {
                        halow_set_radio(self, HALOW_RADIO_SUBBANDS, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_rts_threshold: {
                        halow_set_radio(self, HALOW_RADIO_RTS, halow_arg_range(e->value, UINT16_MAX));
                        break;
                    }
                    case MP_QSTR_fragment_threshold: {
                        halow_set_radio(self, HALOW_RADIO_FRAG, halow_arg_range(e->value, UINT16_MAX));
                        break;
                    }
                    case MP_QSTR_listen_interval: {
                        halow_set_radio(self, HALOW_RADIO_LISTEN,
                            halow_arg_range(e->value, UINT16_MAX));
                        break;
                    }
                    case MP_QSTR_wnm_sleep: {
                        int ret = halow_wifi_wnm_sleep(self->halow,
                            mp_obj_is_true(e->value),
                            halow_wifi_get_radio(self->halow, HALOW_RADIO_WNM_PD));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_wnm_powerdown: {
                        halow_set_radio(self, HALOW_RADIO_WNM_PD, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_twt: {
                        // (interval_us, duration_us[, setup])
                        size_t n;
                        mp_obj_t *items;
                        mp_obj_get_array(e->value, &n, &items);
                        if (n < 2 || n > 3) {
                            mp_raise_ValueError(MP_ERROR_TEXT("bad twt"));
                        }
                        int ret = halow_wifi_twt(self->halow,
                            mp_obj_get_ll(items[0]),
                            halow_arg_range(items[1], UINT32_MAX),
                            n == 3 ? mp_obj_get_int(items[2]) : HALOW_TWT_REQUEST);
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_fixed_rate: {
                        // (mcs, bandwidth, gi), -1 for automatic
                        size_t n;
                        mp_obj_t *items;
                        mp_obj_get_array(e->value, &n, &items);
                        if (n != 3) {
                            mp_raise_ValueError(MP_ERROR_TEXT("bad fixed_rate"));
                        }
                        int ret = halow_wifi_fixed_rate(self->halow,
                            mp_obj_get_int(items[0]), mp_obj_get_int(items[1]),
                            mp_obj_get_int(items[2]));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_health_check: {
                        // (min_ms, max_ms)
                        size_t n;
                        mp_obj_t *items;
                        mp_obj_get_array(e->value, &n, &items);
                        if (n != 2) {
                            mp_raise_ValueError(MP_ERROR_TEXT("bad health_check"));
                        }
                        int ret = halow_wifi_set_health_check(self->halow,
                            mp_obj_get_int(items[0]), mp_obj_get_int(items[1]));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_duty_cycle: {
                        int ret = halow_wifi_set_duty_cycle(self->halow,
                            mp_obj_get_int(e->value));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_pm: {
                        int ret = halow_wifi_pm(self->halow, mp_obj_get_int(e->value));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_ps_timeout: {
                        int ret = halow_wifi_set_ps_timeout(self->halow, mp_obj_get_int(e->value));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_trace: {
                        self->halow->trace_flags = mp_obj_get_int(e->value);
                        break;
                    }
                    case MP_QSTR_txpower:
                        halow_set_radio(self, HALOW_RADIO_TXPOWER,
                            halow_arg_range(e->value, UINT16_MAX));
                        break;
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
            const char *country = get_country_code();
            halow_wifi_set_up(self->halow, self->itf, false, country);
            halow_wifi_set_up(self->halow, self->itf, true, country);
        }

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_halow_config_obj, 1, network_halow_config);

/*******************************************************************************/
// class bindings

// The public SEC_/TWT_/DUTY_CYCLE_ constants are fixed to literals in halow.h so
// that a morselib SDK update renumbering its enums cannot silently change what a
// user's stored value means.  These assert the literals still match the SDK, so
// such a divergence fails the build here instead of shipping.
MP_STATIC_ASSERT(HALOW_SEC_OPEN == MMWLAN_OPEN);
MP_STATIC_ASSERT(HALOW_SEC_OWE == MMWLAN_OWE);
MP_STATIC_ASSERT(HALOW_SEC_SAE == MMWLAN_SAE);
MP_STATIC_ASSERT(HALOW_TWT_REQUEST == MMWLAN_TWT_SETUP_REQUEST);
MP_STATIC_ASSERT(HALOW_TWT_SUGGEST == MMWLAN_TWT_SETUP_SUGGEST);
MP_STATIC_ASSERT(HALOW_TWT_DEMAND == MMWLAN_TWT_SETUP_DEMAND);
MP_STATIC_ASSERT(HALOW_DUTY_CYCLE_SPREAD == MMWLAN_DUTY_CYCLE_MODE_SPREAD);
MP_STATIC_ASSERT(HALOW_DUTY_CYCLE_BURST == MMWLAN_DUTY_CYCLE_MODE_BURST);

static const mp_rom_map_elem_t network_halow_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_ethernet), MP_ROM_PTR(&network_halow_send_ethernet_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&network_halow_ioctl_obj) },

    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&network_halow_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_halow_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&network_halow_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_halow_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_halow_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_halow_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_halow_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&network_halow_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_halow_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&network_halow_config_obj) },

    // Class constants.
    { MP_ROM_QSTR(MP_QSTR_IF_STA), MP_ROM_INT(MOD_NETWORK_STA_IF) },
    #if MICROPY_PY_NETWORK_HALOW_AP
    { MP_ROM_QSTR(MP_QSTR_IF_AP), MP_ROM_INT(MOD_NETWORK_AP_IF) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_SEC_OPEN), MP_ROM_INT(HALOW_SEC_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_SEC_OWE), MP_ROM_INT(HALOW_SEC_OWE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA3), MP_ROM_INT(HALOW_SEC_SAE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_SAE), MP_ROM_INT(HALOW_SEC_SAE) },

    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(HALOW_LINK_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(HALOW_LINK_JOIN) },
    { MP_ROM_QSTR(MP_QSTR_STAT_NOIP), MP_ROM_INT(HALOW_LINK_NOIP) },
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(HALOW_LINK_UP) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(HALOW_LINK_FAIL) },

    { MP_ROM_QSTR(MP_QSTR_PM_NONE), MP_ROM_INT(HALOW_PM_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PM_POWERSAVE), MP_ROM_INT(HALOW_PM_POWERSAVE) },

    { MP_ROM_QSTR(MP_QSTR_TWT_REQUEST), MP_ROM_INT(HALOW_TWT_REQUEST) },
    { MP_ROM_QSTR(MP_QSTR_TWT_SUGGEST), MP_ROM_INT(HALOW_TWT_SUGGEST) },
    { MP_ROM_QSTR(MP_QSTR_TWT_DEMAND), MP_ROM_INT(HALOW_TWT_DEMAND) },

    { MP_ROM_QSTR(MP_QSTR_DUTY_CYCLE_SPREAD), MP_ROM_INT(HALOW_DUTY_CYCLE_SPREAD) },
    { MP_ROM_QSTR(MP_QSTR_DUTY_CYCLE_BURST), MP_ROM_INT(HALOW_DUTY_CYCLE_BURST) },
};
static MP_DEFINE_CONST_DICT(network_halow_locals_dict, network_halow_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_halow_type,
    MP_QSTR_HALOW,
    MP_TYPE_FLAG_NONE,
    make_new, network_halow_make_new,
    print, network_halow_print,
    locals_dict, &network_halow_locals_dict
    );

MP_REGISTER_ROOT_POINTER(struct _machine_spi_obj_t *mp_halow_spi);
MP_REGISTER_ROOT_POINTER(void *halow_heap);

#endif // MICROPY_PY_NETWORK_HALOW
