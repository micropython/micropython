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
 * The PHY-neutral driver interface behind network.HALOW.  network.HALOW is the
 * single Python class for every 802.11ah transceiver family; each family's
 * driver provides a halow_drv_t instance, and the binding calls the transceiver
 * only through it -- so a new family is added by writing its driver + a
 * halow_drv_t adapter, with no change to the binding.
 *
 * The interface keeps every vendor-specific type out of the binding: the state
 * handle is opaque, and any operation that would return structured or
 * vendor-shaped data (scan results, rate-control statistics, version strings,
 * ...) instead returns a finished mp_obj_t built by the driver's adapter.  So
 * this header, and the binding, depend on nothing from any particular vendor
 * SDK.
 */
#ifndef MICROPY_INCLUDED_EXTMOD_HALOW_DRV_H
#define MICROPY_INCLUDED_EXTMOD_HALOW_DRV_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "py/obj.h"

struct netif;

// Interfaces (match MOD_NETWORK_STA_IF / MOD_NETWORK_AP_IF).
#define HALOW_ITF_STA          (0)
#define HALOW_ITF_AP           (1)

// Link status, with the same meaning as the CYW43_LINK_xxx values so that the
// network.HALOW status values match network.WLAN.
#define HALOW_LINK_DOWN        (0)
#define HALOW_LINK_JOIN        (1)
#define HALOW_LINK_NOIP        (2)
#define HALOW_LINK_UP          (3)
#define HALOW_LINK_FAIL        (-1)
#define HALOW_LINK_NONET       (-2)
#define HALOW_LINK_BADAUTH     (-3)

// Security types.  802.11ah is open, OWE (opportunistic encryption) or SAE
// (WPA3); there is no WPA2-PSK.
#define HALOW_SEC_OPEN         (0)
#define HALOW_SEC_OWE          (1)
#define HALOW_SEC_SAE          (2)

// Radio settings, as accepted by the set_radio/get_radio ops.
#define HALOW_RADIO_AMPDU      (0)
#define HALOW_RADIO_SGI        (1)
#define HALOW_RADIO_SUBBANDS   (2)
#define HALOW_RADIO_RTS        (3)
#define HALOW_RADIO_FRAG       (4)
#define HALOW_RADIO_LISTEN     (5)
#define HALOW_RADIO_WNM_PD     (6)
#define HALOW_RADIO_TXPOWER    (7)

// Target wake time negotiation, as asked for in the association request.
#define HALOW_TWT_REQUEST      (0)
#define HALOW_TWT_SUGGEST      (1)
#define HALOW_TWT_DEMAND       (2)

// How the regulatory airtime allowance is spent.
#define HALOW_DUTY_CYCLE_SPREAD (0)
#define HALOW_DUTY_CYCLE_BURST  (1)

// Power management modes, as accepted by the pm op.
#define HALOW_PM_NONE          (0)
#define HALOW_PM_POWERSAVE     (1)

// The PHY-neutral driver operations.  The first argument to every op is the
// driver's opaque state handle (halow_drv_t.state).  Ops that read structured
// or vendor-shaped data return a finished mp_obj_t so no vendor type reaches
// the binding; scalar ops return the C value or a negated errno.
typedef struct _halow_drv_t {
    // The driver's opaque per-interface state.
    void *state;

    // Lifecycle and link.
    void (*deinit)(void *state);
    int (*set_up)(void *state, int itf, bool up, const char *country);
    int (*link_status)(void *state, int itf);
    struct netif *(*netif)(void *state, int itf);
    int (*send_ethernet)(void *state, int itf, size_t len, const void *buf, bool is_pbuf);
    bool (*sched_in_wait)(void);
    bool (*country_supported)(const char *country);

    // Station.
    int (*join)(void *state, size_t ssid_len, const uint8_t *ssid,
        size_t key_len, const uint8_t *key, int security, const uint8_t *bssid);
    int (*leave)(void *state, int itf);
    mp_obj_t (*scan)(void *state);                 // list of scan-result tuples
    int (*get_mac)(void *state, int itf, uint8_t mac[6]);
    int (*get_bssid)(void *state, uint8_t bssid[6]);
    int (*get_rssi)(void *state, int32_t *rssi);
    int (*get_channel)(void *state, int itf, uint16_t *chan_num, uint8_t *bw_mhz);
    mp_obj_t (*get_ssid)(void *state, int itf);    // str

    // Power management.
    int (*pm)(void *state, uint32_t pm);
    int (*get_pm)(void *state, uint32_t *pm);
    int (*set_ps_timeout)(void *state, uint32_t ms);
    int (*get_ps_timeout)(void *state, uint32_t *ms);
    int (*wnm_sleep)(void *state, bool enable, bool powerdown);
    int (*twt)(void *state, uint64_t interval_us, uint32_t duration_us, int setup);

    // Radio tuning.
    int (*set_radio)(void *state, int what, uint32_t value);
    uint32_t (*get_radio)(void *state, int what);
    int (*fixed_rate)(void *state, int mcs, int bw_mhz, int gi);
    int (*set_health_check)(void *state, uint32_t min_ms, uint32_t max_ms);
    void (*set_trace)(void *state, uint32_t flags);

    // Duty cycle.
    int (*set_duty_cycle)(void *state, int mode);
    int (*get_duty_cycle_mode)(void *state);       // the configured mode
    mp_obj_t (*duty_cycle_stats)(void *state);     // (duty, mode, remaining, window) tuple

    // Diagnostics.
    mp_obj_t (*rate_stats)(void *state);           // list of (mcs, bw, gi, sent, ok) tuples
    mp_obj_t (*version)(void *state);              // (lib, fw, chip) tuple
    int (*ate_command)(void *state, uint8_t *cmd, size_t cmd_len,
        uint8_t *rsp, size_t *rsp_len);

    // Access-point operations are not part of the interface yet: morselib's AP
    // support is an alpha API that does not currently succeed on the MM8108, so
    // the ap_* ops (set_ssid/set_password/set_auth/set_channel/get_auth/
    // stations) and the binding's AP surface are left out until it works.
} halow_drv_t;

// Each driver family provides its adapter instance.
#if MICROPY_PY_NETWORK_HALOW_MORSE_MICRO
extern const halow_drv_t mm_halow_drv;
#endif

#endif // MICROPY_INCLUDED_EXTMOD_HALOW_DRV_H
