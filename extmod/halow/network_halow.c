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
#include "extmod/halow/network_halow.h"
#include "extmod/halow/halow_drv.h"
#include "extmod/modnetwork.h"

// PHY back-end selection.  network.HaLow is the single Python-facing class for
// every 802.11ah transceiver family; which driver it drives is chosen by the
// `phy=` argument to the constructor.  The binding reaches the transceiver only
// through the selected family's halow_drv_t, so it names no vendor symbol.  Each
// family is an independent option -- a build selects at least one with
// MICROPY_PY_NETWORK_HALOW_<PHY>, and that is what turns network.HaLow on.  A new
// family is added by giving it that option, a HALOW_PHY_* id, a
// network.HaLow.<PHY> constant and a halow_drv_t adapter, and slotting that
// adapter into halow_drv_table -- with no other change here.  When exactly one
// back-end is compiled in it is the default and `phy=` is optional; when more
// than one is compiled in, `phy=` must be given.
#ifndef MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
#define MICROPY_PY_NETWORK_HALOW_MORSE_MICRO (0)
#endif

enum {
    HALOW_PHY_MORSE_MICRO = 0,
};

#define HALOW_PHY_COUNT (MICROPY_PY_NETWORK_HALOW_MORSE_MICRO)

// The PHY to use when the constructor is not told one (the single compiled-in
// back-end).  It is also the initial value of halow_selected_phy, so the soft-
// reset deinit reaches a valid adapter before any constructor has run.
#if MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
#define HALOW_PHY_DEFAULT HALOW_PHY_MORSE_MICRO
#else
#error "network.HaLow needs a PHY back-end: MICROPY_PY_NETWORK_HALOW_MORSE_MICRO"
#endif

// The adapters for the compiled-in PHY families, indexed by HALOW_PHY_*.
static const halow_drv_t *const halow_drv_table[] = {
    #if MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
    [HALOW_PHY_MORSE_MICRO] = &mm_halow_drv,
    #endif
};

// The PHY the constructor selected (or defaulted to).  A single radio is active
// at a time, so this is process-wide rather than per-interface (the interface
// objects are static and shared).
static uint8_t halow_selected_phy = HALOW_PHY_DEFAULT;

static inline const halow_drv_t *halow_drv(void) {
    return halow_drv_table[halow_selected_phy];
}

typedef struct _network_halow_obj_t {
    mp_obj_base_t base;
    int itf;
} network_halow_obj_t;

static const network_halow_obj_t network_halow_wl_sta = { { &mp_network_halow_type }, HALOW_ITF_STA };

// Tracks the last up or down request made for each interface, to avoid races
// with the callbacks.
static bool if_active[2];

// if_active[] is a static that would otherwise survive into the next run.
void network_halow_deinit_all(void) {
    const halow_drv_t *drv = halow_drv();
    drv->deinit(drv->state);
    if_active[0] = false;
    if_active[1] = false;
}

static void network_halow_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const halow_drv_t *drv = halow_drv();
    struct netif *netif = drv->netif(drv->state, self->itf);
    int status = drv->link_status(drv->state, self->itf);
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
    mp_printf(print, "<HaLow %s %s %u.%u.%u.%u>",
        self->itf == HALOW_ITF_STA ? "STA" : "AP",
        status_str,
        addr->addr & 0xff,
        addr->addr >> 8 & 0xff,
        addr->addr >> 16 & 0xff,
        addr->addr >> 24
        );
}

static mp_obj_t network_halow_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_interface, ARG_phy };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_interface, MP_ARG_INT, { .u_int = MOD_NETWORK_STA_IF } },
        { MP_QSTR_phy, MP_ARG_INT, { .u_int = -1 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Select the PHY back-end.  With one compiled in it is the default; with
    // more than one, phy= is required.  A requested PHY must be compiled in.
    int phy = args[ARG_phy].u_int;
    if (phy == -1) {
        if (HALOW_PHY_COUNT > 1) {
            mp_raise_ValueError(MP_ERROR_TEXT("phy required"));
        }
        phy = HALOW_PHY_DEFAULT;
    }
    bool phy_ok = (MICROPY_PY_NETWORK_HALOW_MORSE_MICRO && phy == HALOW_PHY_MORSE_MICRO);
    if (!phy_ok) {
        mp_raise_ValueError(MP_ERROR_TEXT("unsupported phy"));
    }
    halow_selected_phy = phy;

    if (args[ARG_interface].u_int == MOD_NETWORK_STA_IF) {
        return MP_OBJ_FROM_PTR(&network_halow_wl_sta);
    }
    // AP mode is not offered yet (see halow_drv.h), so only STA exists.
    mp_raise_ValueError(MP_ERROR_TEXT("AP not supported"));
}

static mp_obj_t network_halow_send_ethernet(mp_obj_t self_in, mp_obj_t buf_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const halow_drv_t *drv = halow_drv();
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    int ret = drv->send_ethernet(drv->state, self->itf, buf.len, buf.buf, false);
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
    const halow_drv_t *drv = halow_drv();
    mp_buffer_info_t buf;
    mp_get_buffer_raise(args[n_args - 1], &buf, MP_BUFFER_READ | MP_BUFFER_WRITE);

    // The command is copied out first: morselib may modify the buffer it is
    // given, and the response is written back over the same one.
    vstr_t vstr;
    vstr_init_len(&vstr, buf.len);
    memcpy(vstr.buf, buf.buf, buf.len);

    size_t rsp_len = buf.len;
    int ret = drv->ate_command(drv->state, (uint8_t *)vstr.buf, buf.len,
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
    const halow_drv_t *drv = halow_drv();
    if (drv->sched_in_wait()) {
        // Reached from a scheduled callback that ran while the driver was
        // waiting inside the transceiver.  Giving the memory pool back here
        // would free it out from under the frames that are still standing on it.
        mp_raise_OSError(MP_EBUSY);
    }
    drv->deinit(drv->state);
    if_active[HALOW_ITF_STA] = false;
    if_active[HALOW_ITF_AP] = false;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_deinit_obj, network_halow_deinit);

static mp_obj_t network_halow_active(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const halow_drv_t *drv = halow_drv();
    if (n_args == 1) {
        return mp_obj_new_bool(if_active[self->itf]);
    } else {
        bool value = mp_obj_is_true(args[1]);
        if (value && !drv->country_supported(get_country_code())) {
            // 802.11ah channel plans are country specific and there is no
            // worldwide default, so refuse to come up until one is chosen.
            mp_raise_ValueError(MP_ERROR_TEXT("country not set"));
        }
        if (!value && self->itf == HALOW_ITF_STA) {
            drv->leave(drv->state, self->itf);
        }
        int ret = drv->set_up(drv->state, self->itf, value, get_country_code());
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
    const halow_drv_t *drv = halow_drv();

    if (self->itf != HALOW_ITF_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
    }

    uint32_t pm;
    drv->get_pm(drv->state, &pm);
    if (pm != HALOW_PM_NONE) {
        // Power saving is a transmit-only mode; the transceiver is not
        // listening, so it cannot see probe responses.
        mp_raise_OSError(MP_EPERM);
    }

    return drv->scan(drv->state);
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
    const halow_drv_t *drv = halow_drv();
    (void)self;
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

    int ret = drv->join(drv->state, ssid.len, ssid.buf, key.len, key.buf,
        auth_type, bssid.buf);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_halow_connect_obj, 1, network_halow_connect);

static mp_obj_t network_halow_disconnect(mp_obj_t self_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const halow_drv_t *drv = halow_drv();
    drv->leave(drv->state, self->itf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_disconnect_obj, network_halow_disconnect);

static mp_obj_t network_halow_isconnected(mp_obj_t self_in) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const halow_drv_t *drv = halow_drv();
    bool result = (drv->link_status(drv->state, self->itf) == HALOW_LINK_UP);
    return mp_obj_new_bool(result);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_halow_isconnected_obj, network_halow_isconnected);

static mp_obj_t network_halow_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const halow_drv_t *drv = halow_drv();
    return mod_network_nic_ifconfig(drv->netif(drv->state, self->itf), n_args - 1, args + 1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_halow_ifconfig_obj, 1, 2, network_halow_ifconfig);

static mp_obj_t network_halow_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const halow_drv_t *drv = halow_drv();
    return mod_network_nic_ipconfig(drv->netif(drv->state, self->itf), n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_halow_ipconfig_obj, 1, network_halow_ipconfig);

static mp_obj_t network_halow_status(size_t n_args, const mp_obj_t *args) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const halow_drv_t *drv = halow_drv();

    if (n_args == 1) {
        // no arguments: return link status
        return MP_OBJ_NEW_SMALL_INT(drv->link_status(drv->state, self->itf));
    }

    // one argument: return status based on query parameter
    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_rssi: {
            if (self->itf != HALOW_ITF_STA) {
                mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
            }
            int32_t rssi;
            drv->get_rssi(drv->state, &rssi);
            return mp_obj_new_int(rssi);
        }
        case MP_QSTR_rates: {
            // (mcs, bandwidth_mhz, gi, sent, success) per rate table entry that
            // has been used, so the list stays about the link rather than the
            // whole table.
            return drv->rate_stats(drv->state);
        }
        case MP_QSTR_duty_cycle: {
            return drv->duty_cycle_stats(drv->state);
        }
        case MP_QSTR_bssid: {
            if (self->itf != HALOW_ITF_STA) {
                mp_raise_ValueError(MP_ERROR_TEXT("STA required"));
            }
            uint8_t bssid[6];
            int ret = drv->get_bssid(drv->state, bssid);
            if (ret) {
                mp_raise_OSError(-ret);
            }
            return mp_obj_new_bytes(bssid, sizeof(bssid));
        }
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

static void halow_set_radio(const halow_drv_t *drv, int what, uint32_t value) {
    int ret = drv->set_radio(drv->state, what, value);
    if (ret != 0) {
        mp_raise_OSError(-ret);
    }
}

static mp_obj_t network_halow_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_halow_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    const halow_drv_t *drv = halow_drv();

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
                int ret = drv->get_channel(drv->state, self->itf, &chan_num, &bw_mhz);
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
                int ret = drv->get_channel(drv->state, self->itf, &chan_num, &bw_mhz);
                if (ret) {
                    mp_raise_OSError(-ret);
                }
                return MP_OBJ_NEW_SMALL_INT(bw_mhz);
            }
            case MP_QSTR_ssid:
            case MP_QSTR_essid: {
                return drv->get_ssid(drv->state, self->itf);
            }
            case MP_QSTR_version: {
                return drv->version(drv->state);
            }
            case MP_QSTR_ampdu:
                return mp_obj_new_bool(drv->get_radio(drv->state, HALOW_RADIO_AMPDU));
            case MP_QSTR_sgi:
                return mp_obj_new_bool(drv->get_radio(drv->state, HALOW_RADIO_SGI));
            case MP_QSTR_subbands:
                return mp_obj_new_bool(drv->get_radio(drv->state, HALOW_RADIO_SUBBANDS));
            case MP_QSTR_rts_threshold:
                return mp_obj_new_int_from_uint(drv->get_radio(drv->state, HALOW_RADIO_RTS));
            case MP_QSTR_fragment_threshold:
                return mp_obj_new_int_from_uint(drv->get_radio(drv->state, HALOW_RADIO_FRAG));
            case MP_QSTR_wnm_powerdown:
                return mp_obj_new_bool(drv->get_radio(drv->state, HALOW_RADIO_WNM_PD));
            case MP_QSTR_listen_interval:
                return MP_OBJ_NEW_SMALL_INT(drv->get_radio(drv->state, HALOW_RADIO_LISTEN));
            case MP_QSTR_txpower:
                return MP_OBJ_NEW_SMALL_INT(drv->get_radio(drv->state, HALOW_RADIO_TXPOWER));
            case MP_QSTR_duty_cycle:
                return MP_OBJ_NEW_SMALL_INT(drv->get_duty_cycle_mode(drv->state));
            case MP_QSTR_mac: {
                uint8_t buf[6];
                drv->get_mac(drv->state, self->itf, buf);
                return mp_obj_new_bytes(buf, 6);
            }
            case MP_QSTR_pm: {
                uint32_t pm;
                drv->get_pm(drv->state, &pm);
                return MP_OBJ_NEW_SMALL_INT(pm);
            }
            case MP_QSTR_ps_timeout: {
                uint32_t ms;
                drv->get_ps_timeout(drv->state, &ms);
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
                    case MP_QSTR_ampdu: {
                        halow_set_radio(drv, HALOW_RADIO_AMPDU, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_sgi: {
                        halow_set_radio(drv, HALOW_RADIO_SGI, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_subbands: {
                        halow_set_radio(drv, HALOW_RADIO_SUBBANDS, mp_obj_is_true(e->value));
                        break;
                    }
                    case MP_QSTR_rts_threshold: {
                        halow_set_radio(drv, HALOW_RADIO_RTS, halow_arg_range(e->value, UINT16_MAX));
                        break;
                    }
                    case MP_QSTR_fragment_threshold: {
                        halow_set_radio(drv, HALOW_RADIO_FRAG, halow_arg_range(e->value, UINT16_MAX));
                        break;
                    }
                    case MP_QSTR_listen_interval: {
                        halow_set_radio(drv, HALOW_RADIO_LISTEN,
                            halow_arg_range(e->value, UINT16_MAX));
                        break;
                    }
                    case MP_QSTR_wnm_sleep: {
                        int ret = drv->wnm_sleep(drv->state,
                            mp_obj_is_true(e->value),
                            drv->get_radio(drv->state, HALOW_RADIO_WNM_PD));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_wnm_powerdown: {
                        halow_set_radio(drv, HALOW_RADIO_WNM_PD, mp_obj_is_true(e->value));
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
                        int ret = drv->twt(drv->state,
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
                        int ret = drv->fixed_rate(drv->state,
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
                        int ret = drv->set_health_check(drv->state,
                            mp_obj_get_int(items[0]), mp_obj_get_int(items[1]));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_duty_cycle: {
                        int ret = drv->set_duty_cycle(drv->state,
                            mp_obj_get_int(e->value));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_pm: {
                        int ret = drv->pm(drv->state, mp_obj_get_int(e->value));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_ps_timeout: {
                        int ret = drv->set_ps_timeout(drv->state, mp_obj_get_int(e->value));
                        if (ret != 0) {
                            mp_raise_OSError(-ret);
                        }
                        break;
                    }
                    case MP_QSTR_trace: {
                        drv->set_trace(drv->state, mp_obj_get_int(e->value));
                        break;
                    }
                    case MP_QSTR_txpower:
                        halow_set_radio(drv, HALOW_RADIO_TXPOWER,
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
            drv->set_up(drv->state, self->itf, false, country);
            drv->set_up(drv->state, self->itf, true, country);
        }

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_halow_config_obj, 1, network_halow_config);

/*******************************************************************************/
// class bindings

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
    // PHY back-ends selectable via HALOW(phy=...); see the make_new comment.
    #if MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
    { MP_ROM_QSTR(MP_QSTR_MORSE_MICRO), MP_ROM_INT(HALOW_PHY_MORSE_MICRO) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_IF_STA), MP_ROM_INT(MOD_NETWORK_STA_IF) },
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
    MP_QSTR_HaLow,
    MP_TYPE_FLAG_NONE,
    make_new, network_halow_make_new,
    print, network_halow_print,
    locals_dict, &network_halow_locals_dict
    );


#endif // MICROPY_PY_NETWORK_HALOW
