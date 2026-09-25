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
 *
 * The Morse Micro adapter for the PHY-neutral halow_drv_t interface: it maps the
 * neutral operations onto the mm-halow driver's mm_halow_* API, and builds the
 * Python objects for the operations that expose structured or vendor data so
 * that morselib's types never reach the network.HALOW binding.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW && MICROPY_PY_NETWORK_HALOW_MORSE_MICRO

#include "py/runtime.h"
#include "extmod/halow/halow_drv.h"

#include "mm_halow.h"
#include "mm_halow_sched.h"

// The neutral halow_drv.h constants are passed straight through to the mm-halow
// driver, so they must carry the same values as the driver's own.  The driver's
// MM_HALOW_* values are in turn asserted against morselib's enums in the binding
// they came from, so a morselib renumbering is still caught at build time.
MP_STATIC_ASSERT(HALOW_ITF_STA == MM_HALOW_ITF_STA);
MP_STATIC_ASSERT(HALOW_ITF_AP == MM_HALOW_ITF_AP);
MP_STATIC_ASSERT(HALOW_LINK_DOWN == MM_HALOW_LINK_DOWN);
MP_STATIC_ASSERT(HALOW_LINK_JOIN == MM_HALOW_LINK_JOIN);
MP_STATIC_ASSERT(HALOW_LINK_NOIP == MM_HALOW_LINK_NOIP);
MP_STATIC_ASSERT(HALOW_LINK_UP == MM_HALOW_LINK_UP);
MP_STATIC_ASSERT(HALOW_LINK_FAIL == MM_HALOW_LINK_FAIL);
MP_STATIC_ASSERT(HALOW_LINK_NONET == MM_HALOW_LINK_NONET);
MP_STATIC_ASSERT(HALOW_LINK_BADAUTH == MM_HALOW_LINK_BADAUTH);
MP_STATIC_ASSERT(HALOW_SEC_OPEN == MM_HALOW_SEC_OPEN);
MP_STATIC_ASSERT(HALOW_SEC_OWE == MM_HALOW_SEC_OWE);
MP_STATIC_ASSERT(HALOW_SEC_SAE == MM_HALOW_SEC_SAE);
MP_STATIC_ASSERT(HALOW_RADIO_AMPDU == MM_HALOW_RADIO_AMPDU);
MP_STATIC_ASSERT(HALOW_RADIO_SGI == MM_HALOW_RADIO_SGI);
MP_STATIC_ASSERT(HALOW_RADIO_SUBBANDS == MM_HALOW_RADIO_SUBBANDS);
MP_STATIC_ASSERT(HALOW_RADIO_RTS == MM_HALOW_RADIO_RTS);
MP_STATIC_ASSERT(HALOW_RADIO_FRAG == MM_HALOW_RADIO_FRAG);
MP_STATIC_ASSERT(HALOW_RADIO_LISTEN == MM_HALOW_RADIO_LISTEN);
MP_STATIC_ASSERT(HALOW_RADIO_WNM_PD == MM_HALOW_RADIO_WNM_PD);
MP_STATIC_ASSERT(HALOW_RADIO_TXPOWER == MM_HALOW_RADIO_TXPOWER);
MP_STATIC_ASSERT(HALOW_TWT_REQUEST == MM_HALOW_TWT_REQUEST);
MP_STATIC_ASSERT(HALOW_TWT_SUGGEST == MM_HALOW_TWT_SUGGEST);
MP_STATIC_ASSERT(HALOW_TWT_DEMAND == MM_HALOW_TWT_DEMAND);
MP_STATIC_ASSERT(HALOW_DUTY_CYCLE_SPREAD == MM_HALOW_DUTY_CYCLE_SPREAD);
MP_STATIC_ASSERT(HALOW_DUTY_CYCLE_BURST == MM_HALOW_DUTY_CYCLE_BURST);
MP_STATIC_ASSERT(HALOW_PM_NONE == MM_HALOW_PM_NONE);
MP_STATIC_ASSERT(HALOW_PM_POWERSAVE == MM_HALOW_PM_POWERSAVE);

// The public SEC_/TWT_/DUTY_CYCLE_ constants are fixed to literals in mm_halow.h
// so that a morselib SDK update renumbering its enums cannot silently change what
// a user's stored value means.  These assert the literals still match the SDK, so
// such a divergence fails the build here instead of shipping.
MP_STATIC_ASSERT(MM_HALOW_SEC_OPEN == MMWLAN_OPEN);
MP_STATIC_ASSERT(MM_HALOW_SEC_OWE == MMWLAN_OWE);
MP_STATIC_ASSERT(MM_HALOW_SEC_SAE == MMWLAN_SAE);
MP_STATIC_ASSERT(MM_HALOW_TWT_REQUEST == MMWLAN_TWT_SETUP_REQUEST);
MP_STATIC_ASSERT(MM_HALOW_TWT_SUGGEST == MMWLAN_TWT_SETUP_SUGGEST);
MP_STATIC_ASSERT(MM_HALOW_TWT_DEMAND == MMWLAN_TWT_SETUP_DEMAND);
MP_STATIC_ASSERT(MM_HALOW_DUTY_CYCLE_SPREAD == MMWLAN_DUTY_CYCLE_MODE_SPREAD);
MP_STATIC_ASSERT(MM_HALOW_DUTY_CYCLE_BURST == MMWLAN_DUTY_CYCLE_MODE_BURST);

#define SELF(state) ((mm_halow_t *)(state))

static void mm_adapt_deinit(void *s) {
    mm_halow_deinit(SELF(s));
}
static int mm_adapt_set_up(void *s, int itf, bool up, const char *country) {
    return mm_halow_wifi_set_up(SELF(s), itf, up, country);
}
static int mm_adapt_link_status(void *s, int itf) {
    return mm_halow_tcpip_link_status(SELF(s), itf);
}
static struct netif *mm_adapt_netif(void *s, int itf) {
    return &SELF(s)->netif[itf];
}
static int mm_adapt_send_ethernet(void *s, int itf, size_t len, const void *buf, bool is_pbuf) {
    return mm_halow_send_ethernet(SELF(s), itf, len, buf, is_pbuf);
}
static bool mm_adapt_sched_in_wait(void) {
    return mm_halow_sched_in_wait();
}
static bool mm_adapt_country_supported(const char *country) {
    return mm_halow_country_supported(country);
}

static int mm_adapt_join(void *s, size_t ssid_len, const uint8_t *ssid,
    size_t key_len, const uint8_t *key, int security, const uint8_t *bssid) {
    return mm_halow_wifi_join(SELF(s), ssid_len, ssid, key_len, key, security, bssid);
}
static int mm_adapt_leave(void *s, int itf) {
    return mm_halow_wifi_leave(SELF(s), itf);
}

static mp_obj_t mm_adapt_scan(void *s) {
    // Off the heap rather than the stack: the sweep can report a good number of
    // networks and each result is not small.
    mm_halow_ev_scan_result_t *results =
        m_new(mm_halow_ev_scan_result_t, MM_HALOW_SCAN_CACHE_MAX);
    size_t n = mm_halow_wifi_scan_cached(SELF(s), results, MM_HALOW_SCAN_CACHE_MAX);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (size_t i = 0; i < n; i++) {
        const mm_halow_ev_scan_result_t *r = &results[i];
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
    m_del(mm_halow_ev_scan_result_t, results, MM_HALOW_SCAN_CACHE_MAX);
    return list;
}

static int mm_adapt_get_mac(void *s, int itf, uint8_t mac[6]) {
    return mm_halow_wifi_get_mac(SELF(s), itf, mac);
}
static int mm_adapt_get_bssid(void *s, uint8_t bssid[6]) {
    return mm_halow_wifi_get_bssid(SELF(s), bssid);
}
static int mm_adapt_get_rssi(void *s, int32_t *rssi) {
    return mm_halow_wifi_get_rssi(SELF(s), rssi);
}
static int mm_adapt_get_channel(void *s, int itf, uint16_t *chan_num, uint8_t *bw_mhz) {
    return mm_halow_wifi_get_channel(SELF(s), itf, chan_num, bw_mhz);
}
static mp_obj_t mm_adapt_get_ssid(void *s, int itf) {
    (void)itf;
    return mp_obj_new_str((const char *)SELF(s)->sta_ssid, SELF(s)->sta_ssid_len);
}

static int mm_adapt_pm(void *s, uint32_t pm) {
    return mm_halow_wifi_pm(SELF(s), pm);
}
static int mm_adapt_get_pm(void *s, uint32_t *pm) {
    return mm_halow_wifi_get_pm(SELF(s), pm);
}
static int mm_adapt_set_ps_timeout(void *s, uint32_t ms) {
    return mm_halow_wifi_set_ps_timeout(SELF(s), ms);
}
static int mm_adapt_get_ps_timeout(void *s, uint32_t *ms) {
    return mm_halow_wifi_get_ps_timeout(SELF(s), ms);
}
static int mm_adapt_wnm_sleep(void *s, bool enable, bool powerdown) {
    return mm_halow_wifi_wnm_sleep(SELF(s), enable, powerdown);
}
static int mm_adapt_twt(void *s, uint64_t interval_us, uint32_t duration_us, int setup) {
    return mm_halow_wifi_twt(SELF(s), interval_us, duration_us, setup);
}

static int mm_adapt_set_radio(void *s, int what, uint32_t value) {
    return mm_halow_wifi_set_radio(SELF(s), what, value);
}
static uint32_t mm_adapt_get_radio(void *s, int what) {
    return mm_halow_wifi_get_radio(SELF(s), what);
}
static int mm_adapt_fixed_rate(void *s, int mcs, int bw_mhz, int gi) {
    return mm_halow_wifi_fixed_rate(SELF(s), mcs, bw_mhz, gi);
}
static int mm_adapt_set_health_check(void *s, uint32_t min_ms, uint32_t max_ms) {
    return mm_halow_wifi_set_health_check(SELF(s), min_ms, max_ms);
}
static void mm_adapt_set_trace(void *s, uint32_t flags) {
    SELF(s)->trace_flags = flags;
}

static int mm_adapt_set_duty_cycle(void *s, int mode) {
    return mm_halow_wifi_set_duty_cycle(SELF(s), mode);
}
static int mm_adapt_get_duty_cycle_mode(void *s) {
    return SELF(s)->duty_cycle_mode;
}
static mp_obj_t mm_adapt_duty_cycle_stats(void *s) {
    struct mmwlan_duty_cycle_stats st;
    int ret = mm_halow_wifi_get_duty_cycle(SELF(s), &st);
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

static mp_obj_t mm_adapt_rate_stats(void *s) {
    // (mcs, bandwidth_mhz, gi, sent, success) per rate table entry that has
    // been used, so the list stays about the link rather than the whole table.
    // Room for the copy is taken first: the vendor's block is not garbage
    // collected, so an allocation that raises after asking for it would strand
    // it for good.
    uint32_t *info = m_new(uint32_t, MM_HALOW_RC_STATS_MAX * 3);

    struct mmwlan_rc_stats *st;
    int ret = mm_halow_wifi_get_rc_stats(SELF(s), &st);
    if (ret) {
        m_del(uint32_t, info, MM_HALOW_RC_STATS_MAX * 3);
        mp_raise_OSError(-ret);
    }
    if (st == NULL) {
        m_del(uint32_t, info, MM_HALOW_RC_STATS_MAX * 3);
        return mp_obj_new_list(0, NULL);
    }
    uint32_t n = MIN(st->n_entries, (uint32_t)MM_HALOW_RC_STATS_MAX);
    for (uint32_t i = 0; i < n; i++) {
        info[i * 3] = st->rate_info[i];
        info[i * 3 + 1] = st->total_sent[i];
        info[i * 3 + 2] = st->total_success[i];
    }
    mm_halow_wifi_free_rc_stats(st);

    mp_obj_t list = mp_obj_new_list(0, NULL);
    for (uint32_t i = 0; i < n; i++) {
        if (info[i * 3 + 1] == 0) {
            continue;
        }
        uint32_t rate = info[i * 3];
        // The bandwidth field is an index, not a width: 0 is 1MHz.
        uint32_t bw = (rate >> MM_HALOW_RC_BW_SHIFT) & MM_HALOW_RC_FIELD_MASK;
        mp_obj_t entry[5] = {
            MP_OBJ_NEW_SMALL_INT((rate >> MM_HALOW_RC_RATE_SHIFT) & MM_HALOW_RC_FIELD_MASK),
            MP_OBJ_NEW_SMALL_INT(1 << bw),
            MP_OBJ_NEW_SMALL_INT((rate >> MM_HALOW_RC_GI_SHIFT) & MM_HALOW_RC_GI_MASK),
            mp_obj_new_int_from_uint(info[i * 3 + 1]),
            mp_obj_new_int_from_uint(info[i * 3 + 2]),
        };
        mp_obj_list_append(list, mp_obj_new_tuple(5, entry));
    }
    m_del(uint32_t, info, MM_HALOW_RC_STATS_MAX * 3);
    return list;
}

static mp_obj_t mm_adapt_version(void *s) {
    struct mmwlan_version v;
    int ret = mm_halow_wifi_get_version(SELF(s), &v);
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

static int mm_adapt_ate_command(void *s, uint8_t *cmd, size_t cmd_len,
    uint8_t *rsp, size_t *rsp_len) {
    return mm_halow_wifi_ate_command(SELF(s), cmd, cmd_len, rsp, rsp_len);
}

// Access-point operations are intentionally not wired up here.  They would
// reach morselib's mm_halow_wifi_ap_* functions (ap_set_ssid/set_password/
// set_auth/set_channel/get_auth/get_stas), but mmwlan_ap_enable() is an alpha
// API that returns MMWLAN_ERROR on the MM8108, so AP mode does not work yet.
// When it does, restore the mm_adapt_ap_* ops and the ap_* vtable slots below,
// along with the AP surface in network_halow.c.

const halow_drv_t mm_halow_drv = {
    .state = &mm_halow_state,
    .deinit = mm_adapt_deinit,
    .set_up = mm_adapt_set_up,
    .link_status = mm_adapt_link_status,
    .netif = mm_adapt_netif,
    .send_ethernet = mm_adapt_send_ethernet,
    .sched_in_wait = mm_adapt_sched_in_wait,
    .country_supported = mm_adapt_country_supported,
    .join = mm_adapt_join,
    .leave = mm_adapt_leave,
    .scan = mm_adapt_scan,
    .get_mac = mm_adapt_get_mac,
    .get_bssid = mm_adapt_get_bssid,
    .get_rssi = mm_adapt_get_rssi,
    .get_channel = mm_adapt_get_channel,
    .get_ssid = mm_adapt_get_ssid,
    .pm = mm_adapt_pm,
    .get_pm = mm_adapt_get_pm,
    .set_ps_timeout = mm_adapt_set_ps_timeout,
    .get_ps_timeout = mm_adapt_get_ps_timeout,
    .wnm_sleep = mm_adapt_wnm_sleep,
    .twt = mm_adapt_twt,
    .set_radio = mm_adapt_set_radio,
    .get_radio = mm_adapt_get_radio,
    .fixed_rate = mm_adapt_fixed_rate,
    .set_health_check = mm_adapt_set_health_check,
    .set_trace = mm_adapt_set_trace,
    .set_duty_cycle = mm_adapt_set_duty_cycle,
    .get_duty_cycle_mode = mm_adapt_get_duty_cycle_mode,
    .duty_cycle_stats = mm_adapt_duty_cycle_stats,
    .rate_stats = mm_adapt_rate_stats,
    .version = mm_adapt_version,
    .ate_command = mm_adapt_ate_command,
};

#endif // MICROPY_PY_NETWORK_HALOW && MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
