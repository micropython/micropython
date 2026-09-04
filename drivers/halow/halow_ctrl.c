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
 * Control layer for the Morse Micro 802.11ah driver.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

#include "lwip/prot/ethernet.h"

#include "mmwlan.h"
#include "mmregdb.h"

#include "halow.h"
#include "halow_osal.h"
#include "halow_sched.h"

#ifndef HALOW_DEBUG
#define HALOW_DEBUG (0)
#endif

#if HALOW_DEBUG
#define debug_printf(...) mp_printf(&mp_plat_print, __VA_ARGS__)
#else
#define debug_printf(...)
#endif

// Privacy bit in the Capability Information field of a probe response.
#define HALOW_CAP_PRIVACY   (1 << 4)

// Default time the transceiver stays awake after activity, in ms.
#define HALOW_PS_TIMEOUT_DEFAULT_MS (100)

halow_t halow_state = {
    #if MICROPY_PY_NETWORK_HALOW_AP
    .ap_auth = HALOW_SEC_SAE,
    #endif
    .ps_timeout_ms = HALOW_PS_TIMEOUT_DEFAULT_MS,
    // morselib's own defaults, mirrored so that reading them back before the
    // driver starts reports what it will actually use.
    .ampdu = true,
    .sgi = true,
    .subbands = true,
};
void (*halow_poll)(void);

static int halow_scan_start_locked(halow_t *self);
static int halow_apply_pm(halow_t *self);
static void halow_apply_radio(halow_t *self);
static bool halow_scan_idle_cond(void *arg);

// Buffer used to linearise outgoing pbufs that span more than one segment.
// morselib copies the frame into its own packet memory, so this is only ever
// live for the duration of a single call.
#define HALOW_TX_BUF_SIZE (MICROPY_HW_HALOW_MTU + SIZEOF_ETH_HDR)

// How long to wait for the transmit path before giving up on a frame, in ms.
// The wait is not idle -- the driver is serviced throughout -- so this is a
// budget for making progress rather than a stall.
#define HALOW_TX_READY_MS (250)
static uint8_t *halow_tx_buf;

MP_WEAK void halow_schedule_poll(void) {
    // Ports that can raise a software interrupt override this; otherwise the
    // periodic network poll is the only thing that drives the driver.
}

// Translate an mmwlan status into a negative errno, the convention the rest of
// the network module uses.
static int halow_status_to_errno(enum mmwlan_status status) {
    switch (status) {
        case MMWLAN_SUCCESS:
            return 0;
        case MMWLAN_INVALID_ARGUMENT:
            return -MP_EINVAL;
        case MMWLAN_UNAVAILABLE:
            return -MP_EAGAIN;
        case MMWLAN_NO_MEM:
            return -MP_ENOMEM;
        case MMWLAN_TIMED_OUT:
            return -MP_ETIMEDOUT;
        case MMWLAN_CHANNEL_LIST_NOT_SET:
            return -MP_ENODEV;
        case MMWLAN_CHANNEL_INVALID:
            return -MP_ERANGE;
        case MMWLAN_NOT_FOUND:
            return -MP_ENOENT;
        case MMWLAN_NOT_SUPPORTED:
            return -MP_EOPNOTSUPP;
        case MMWLAN_VIF_ERROR:
            return -MP_ENXIO;
        default:
            return -MP_EIO;
    }
}

/*******************************************************************************/
// morselib callbacks

static void halow_sta_status_cb(enum mmwlan_sta_state sta_state) {
    halow_t *self = &halow_state;
    switch (sta_state) {
        case MMWLAN_STA_CONNECTING:
            self->link_status = HALOW_LINK_JOIN;
            break;
        case MMWLAN_STA_CONNECTED:
            self->link_status = HALOW_LINK_NOIP;
            break;
        default:
            self->link_status = HALOW_LINK_DOWN;
            break;
    }
    debug_printf("halow: sta state %d\n", sta_state);
}

static void halow_fatal_error_cb(struct mmwlan_fatal_error_args *args) {
    halow_t *self = args->arg;

    // morselib is left in the state it would be in after mmwlan_shutdown(), and
    // its API must not be called from here.  Report the link as failed so that
    // the interface stops looking healthy, and leave recovery to the caller,
    // which can cycle active() to rebuild everything.
    self->booted = false;
    self->scan_active = false;
    self->link_status = HALOW_LINK_FAIL;
    halow_cb_tcpip_set_link_down(self, HALOW_ITF_STA);
    debug_printf("halow: fatal error at file %u line %u\n",
        (unsigned int)args->fileid, (unsigned int)args->line);
}

static void halow_link_state_cb(enum mmwlan_link_state link_state, void *arg) {
    halow_t *self = arg;
    if (link_state == MMWLAN_LINK_UP) {
        halow_cb_tcpip_set_link_up(self, HALOW_ITF_STA);
    } else {
        halow_cb_tcpip_set_link_down(self, HALOW_ITF_STA);
    }
}

static void halow_rx_cb(uint8_t *header, unsigned header_len,
    uint8_t *payload, unsigned payload_len, void *arg) {
    halow_t *self = arg;
    halow_cb_process_ethernet(self, HALOW_ITF_STA, header, header_len, payload, payload_len);
}

/*******************************************************************************/
// Init

int halow_init(halow_t *self) {
    if (self->initted) {
        return 0;
    }

    // Only the state of a previous session is cleared here.  This runs on the
    // first activation rather than at boot, so anything configured beforehand
    // has to survive it.
    self->itf_state = 0;
    self->scan_active = false;
    self->scan_cache = NULL;
    self->scan_cache_len = 0;
    self->scan_cache_max = 0;
    #if MICROPY_PY_NETWORK_HALOW_AP
    self->ap_sta_count = 0;
    #endif
    self->link_status = HALOW_LINK_DOWN;

    // Take the driver's memory pool before morselib starts allocating: from here
    // on it runs from PendSV, where the MicroPython heap is off limits.
    if (!halow_osal_init()) {
        return -MP_ENOMEM;
    }
    halow_tx_buf = halow_osal_malloc(HALOW_TX_BUF_SIZE);
    if (halow_tx_buf == NULL) {
        halow_osal_deinit();
        return -MP_ENOMEM;
    }

    mmwlan_init();

    // morselib enables power save by default.  A dozing station cannot receive
    // unsolicited traffic and is slow to associate, so apply the configured
    // mode, which defaults to always-on.
    halow_apply_pm(self);
    halow_apply_radio(self);

    mmwlan_register_fatal_error_handler(halow_fatal_error_cb, self);
    mmwlan_register_link_state_cb(halow_link_state_cb, self);
    mmwlan_register_rx_cb(halow_rx_cb, self);

    mmwlan_get_vif_mac_addr(MMWLAN_VIF_STA, self->mac);

    self->initted = true;
    halow_poll = halow_poll_func;
    return 0;
}

void halow_deinit(halow_t *self) {
    if (!self->initted) {
        return;
    }
    if (halow_sched_in_callback) {
        // Reached from a scheduled callback run during a morselib wait: freeing the
        // pool here would pull it out from under the frames still standing on it.
        return;
    }

    halow_poll = NULL;

    for (int itf = 0; itf < HALOW_ITF_MAX; itf++) {
        if (self->itf_state & (1 << itf)) {
            halow_wifi_set_up(self, itf, false, self->country);
        }
    }

    mmwlan_register_rx_cb(NULL, NULL);
    halow_sched_teardown = true;
    mmwlan_shutdown();
    mmwlan_deinit();
    halow_sched_teardown = false;
    self->booted = false;

    halow_sched_deinit();
    halow_osal_deinit();
    halow_tx_buf = NULL;
    // These point into the pool that has just gone back to the GC heap.
    self->scan_cache = NULL;
    self->scan_cache_len = 0;
    self->scan_cache_max = 0;
    self->scan_active = false;

    self->initted = false;
    self->link_status = HALOW_LINK_DOWN;
}

// Per-channel dwell.  morselib defaults to 30ms, but a beacon interval is
// typically 100TU (~102ms), so a channel visit that short catches a beacon less
// than a third of the time and a sweep misses networks that are plainly there.
// Dwelling for longer than one beacon interval is what makes a single sweep
// reliable, at the cost of a longer scan.
#define HALOW_SCAN_DWELL_MS (110)

// Upper bound on one sweep: 48 channels of the United States plan at the dwell
// above, with room for a larger channel plan.
#define HALOW_SCAN_MS (30000)

void halow_poll_func(void) {
    // A dispatch raised just before deinit still runs after it, by which point
    // there is nothing left to service.
    if (!halow_state.initted) {
        return;
    }
    if (halow_sched_task_current() != NULL) {
        // Reached from inside a driver task, by way of lwIP calling back into
        // the transmit path.  The scheduler is already running one level up;
        // claiming here would leave the claim parked on this task's stack the
        // moment it yields, and nothing would ever release it.
        return;
    }
    // Whoever is already servicing the transceiver finishes the job; cutting in
    // would put a second bus transaction on top of one in flight.
    if (!halow_sched_claim()) {
        return;
    }
    halow_hal_poll_irqs();
    halow_osal_timer_poll();
    halow_sched_run();
    halow_sched_release();
    halow_hal_irq_rearm();
}

/*******************************************************************************/
// Interface control

bool halow_country_supported(const char *country) {
    return mmwlan_lookup_regulatory_domain(get_regulatory_db(), country) != NULL;
}

static int halow_set_country(halow_t *self, const char *country) {
    const struct mmwlan_s1g_channel_list *channels =
        mmwlan_lookup_regulatory_domain(get_regulatory_db(), country);
    if (channels == NULL) {
        return -MP_EINVAL;
    }
    self->country[0] = country[0];
    self->country[1] = country[1];
    self->channels = channels;
    return halow_status_to_errno(mmwlan_set_channel_list(channels));
}

#if MICROPY_PY_NETWORK_HALOW_AP
static void halow_ap_sta_status_cb(const struct mmwlan_ap_sta_status *status, void *arg) {
    halow_t *self = (halow_t *)arg;

    for (unsigned i = 0; i < self->ap_sta_count; i++) {
        if (memcmp(self->ap_stas[i], status->mac_addr, MMWLAN_MAC_ADDR_LEN) == 0) {
            if (status->state != MMWLAN_AP_STA_AUTHORIZED) {
                self->ap_sta_count--;
                memmove(self->ap_stas[i], self->ap_stas[i + 1],
                    (self->ap_sta_count - i) * MMWLAN_MAC_ADDR_LEN);
            }
            return;
        }
    }
    if (status->state == MMWLAN_AP_STA_AUTHORIZED && self->ap_sta_count < HALOW_AP_MAX_STAS) {
        memcpy(self->ap_stas[self->ap_sta_count++], status->mac_addr, MMWLAN_MAC_ADDR_LEN);
    }
}

// Find the channel the AP should beacon on: the requested one, or else the
// widest the regulatory domain allows.
static const struct mmwlan_s1g_channel *halow_ap_channel(halow_t *self) {
    const struct mmwlan_s1g_channel_list *list = self->channels;
    const struct mmwlan_s1g_channel *best = NULL;

    if (list == NULL) {
        return NULL;
    }
    for (unsigned i = 0; i < list->num_channels; i++) {
        const struct mmwlan_s1g_channel *ch = &list->channels[i];
        if (self->ap_chan_num != 0) {
            if (ch->s1g_chan_num == self->ap_chan_num) {
                return ch;
            }
        } else if (best == NULL || ch->bw_mhz > best->bw_mhz) {
            best = ch;
        }
    }
    return best;
}

static int halow_ap_enable(halow_t *self) {
    if (self->pm != HALOW_PM_NONE) {
        return -MP_EPERM;
    }
    struct mmwlan_ap_args args = MMWLAN_AP_ARGS_INIT;

    if (self->ap_ssid_len == 0) {
        return -MP_EINVAL;
    }

    if (!(self->itf_state & (1 << HALOW_ITF_STA))) {
        // A zero operating class and channel number mean "use whatever the STA
        // is on", so a standalone AP has to choose for itself.
        const struct mmwlan_s1g_channel *ch = halow_ap_channel(self);
        if (ch == NULL) {
            return -MP_ENODEV;
        }
        args.op_class = ch->s1g_operating_class != MMWLAN_SKIP_OP_CLASS_CHECK
            ? ch->s1g_operating_class : ch->global_operating_class;
        args.s1g_chan_num = ch->s1g_chan_num;
    }

    self->ap_sta_count = 0;
    args.sta_status_cb = halow_ap_sta_status_cb;
    args.sta_status_cb_arg = self;

    memcpy(args.ssid, self->ap_ssid, self->ap_ssid_len);
    args.ssid_len = self->ap_ssid_len;
    args.security_type = self->ap_auth;
    if (self->ap_auth == HALOW_SEC_SAE) {
        if (self->ap_key_len == 0) {
            return -MP_EINVAL;
        }
        memcpy(args.passphrase, self->ap_key, self->ap_key_len);
        args.passphrase_len = self->ap_key_len;
    }
    if (self->ap_auth == HALOW_SEC_OPEN) {
        args.pmf_mode = MMWLAN_PMF_DISABLED;
    }

    // mmwlan_ap_enable() otherwise blocks until the AP has started, which it
    // cannot do: morselib's tasks are run cooperatively from the driver poll,
    // so nothing progresses while the calling thread is blocked.
    args.async_start = true;

    return halow_status_to_errno(mmwlan_ap_enable(&args));
}
#endif // MICROPY_PY_NETWORK_HALOW_AP

int halow_wifi_set_up(halow_t *self, int itf, bool up, const char *country) {
    if (itf < 0 || itf >= HALOW_ITF_MAX) {
        return -MP_EINVAL;
    }

    // Bringing an interface up when it is already up is a no-op: the channel
    // list cannot be changed once the transceiver is running.
    if (up && (self->itf_state & (1 << itf))) {
        return 0;
    }

    if (!up && !self->initted) {
        // Nothing was ever brought up, so there is nothing to take down -- and
        // morselib has not been initialised to be told about it.
        return 0;
    }

    if (up) {
        if (!self->initted) {
            int ret = halow_init(self);
            if (ret != 0) {
                return ret;
            }
        }
        // The channel list has to be set before the transceiver is booted, and
        // it is shared by both interfaces.
        int ret = halow_set_country(self, country);
        if (ret != 0) {
            return ret;
        }

        // Boot the transceiver so that scanning and the MAC address are
        // available before any connection is attempted, and because AP mode
        // cannot be started until it is running.
        if (!self->booted) {
            struct mmwlan_boot_args boot_args = MMWLAN_BOOT_ARGS_INIT;
            ret = halow_status_to_errno(mmwlan_boot(&boot_args));
            if (ret != 0) {
                return ret;
            }
            self->booted = true;
            mmwlan_get_vif_mac_addr(MMWLAN_VIF_STA, self->mac);
        }

        #if MICROPY_PY_NETWORK_HALOW_AP
        if (itf == HALOW_ITF_AP) {
            ret = halow_ap_enable(self);
            if (ret != 0) {
                return ret;
            }
        }
        #endif

        if (!(self->itf_state & (1 << itf))) {
            halow_cb_tcpip_init(self, itf);
            self->itf_state |= 1 << itf;
        }
        #if MICROPY_PY_NETWORK_HALOW_AP
        if (itf == HALOW_ITF_AP) {
            halow_cb_tcpip_set_link_up(self, itf);
        }
        #endif
    } else {
        #if MICROPY_PY_NETWORK_HALOW_AP
        if (itf == HALOW_ITF_AP) {
            mmwlan_ap_disable();
        } else
        #endif
        {
            mmwlan_sta_disable();
            self->link_status = HALOW_LINK_DOWN;
        }
        if (self->itf_state & (1 << itf)) {
            halow_cb_tcpip_set_link_down(self, itf);
            halow_cb_tcpip_deinit(self, itf);
            self->itf_state &= ~(1 << itf);
        }
        if (self->itf_state == 0) {
            halow_sched_teardown = true;
            mmwlan_shutdown();
            halow_sched_teardown = false;
            self->booted = false;
        }
    }

    return 0;
}

/*******************************************************************************/
// Scanning


// Insert or refresh one result in the cache, keyed by BSSID. Runs from PendSV.
static void halow_scan_cache_add(halow_t *self, const halow_ev_scan_result_t *res) {
    if (self->scan_cache == NULL) {
        return;
    }
    for (uint8_t i = 0; i < self->scan_cache_len; i++) {
        if (memcmp(self->scan_cache[i].bssid, res->bssid, sizeof(res->bssid)) == 0) {
            self->scan_cache[i] = *res;
            return;
        }
    }
    if (self->scan_cache_len < self->scan_cache_max) {
        self->scan_cache[self->scan_cache_len++] = *res;
    }
}

size_t halow_wifi_scan_cached(halow_t *self, halow_ev_scan_result_t *out, size_t max) {
    // Scanning hops the radio across every channel, so it must not run while a
    // join is in flight or the link is up.
    if (self->link_status != HALOW_LINK_DOWN) {
        return 0;
    }

    if (self->scan_cache == NULL) {
        self->scan_cache = halow_osal_malloc(
            HALOW_SCAN_CACHE_MAX * sizeof(*self->scan_cache));
        if (self->scan_cache == NULL) {
            return 0;
        }
        self->scan_cache_max = HALOW_SCAN_CACHE_MAX;
    }

    if (!self->scan_active) {
        self->scan_cache_len = 0;
        if (halow_scan_start_locked(self) != 0) {
            return 0;
        }
    }
    halow_sched_wait(halow_scan_idle_cond, self, HALOW_SCAN_MS);

    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    size_t n = self->scan_cache_len < max ? self->scan_cache_len : max;
    for (size_t i = 0; i < n; i++) {
        out[i] = self->scan_cache[i];
    }
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    return n;
}

// Scan results carry a frequency, but a channel is only identified by the pair
// of frequency and width, so the regulatory list is what turns one into the
// other.  Zero when the frequency is not one of the domain's channels, which an
// access point operating outside the local plan will be.
static uint8_t halow_freq_to_chan(halow_t *self, uint32_t freq_hz, uint8_t bw_mhz) {
    const struct mmwlan_s1g_channel_list *list = self->channels;

    if (list == NULL) {
        return 0;
    }
    for (unsigned i = 0; i < list->num_channels; i++) {
        const struct mmwlan_s1g_channel *ch = &list->channels[i];
        if (ch->centre_freq_hz == freq_hz && ch->bw_mhz == bw_mhz) {
            return ch->s1g_chan_num;
        }
    }
    return 0;
}

static void halow_scan_rx_cb(const struct mmwlan_scan_result *result, void *arg) {
    halow_t *self = arg;
    halow_ev_scan_result_t res = { 0 };
    res.ssid_len = MIN(result->ssid_len, sizeof(res.ssid));
    if (result->ssid != NULL) {
        memcpy(res.ssid, result->ssid, res.ssid_len);
    }
    if (result->bssid != NULL) {
        memcpy(res.bssid, result->bssid, sizeof(res.bssid));
    }
    res.rssi = result->rssi;
    res.channel_freq_hz = result->channel_freq_hz;
    res.bw_mhz = result->bw_mhz;
    res.op_bw_mhz = result->op_bw_mhz;
    res.chan_num = halow_freq_to_chan(self, result->channel_freq_hz, result->bw_mhz);
    // 802.11ah only defines open, OWE and SAE, and the privacy bit is the only
    // one of those distinctions visible without parsing the RSN element.
    res.security = (result->capability_info & HALOW_CAP_PRIVACY) ? HALOW_SEC_SAE : HALOW_SEC_OPEN;

    halow_scan_cache_add(self, &res);
}

// An aborted sweep stops at the end of the channel it is on, so this only has to
// cover one channel dwell, not a whole sweep.
#define HALOW_SCAN_ABORT_MS (5000)

static bool halow_scan_idle_cond(void *arg) {
    return !((halow_t *)arg)->scan_active;
}

static void halow_scan_complete_cb(enum mmwlan_scan_state scan_state, void *arg) {
    halow_t *self = arg;
    self->scan_active = false;
    debug_printf("halow: sweep took %ums, %u results\n",
        (unsigned int)(mp_hal_ticks_ms() - self->scan_started_ms),
        (unsigned int)self->scan_cache_len);
}

// Callers must already have decided that starting a sweep is allowed.
static int halow_scan_start_locked(halow_t *self) {
    struct mmwlan_scan_req req = MMWLAN_SCAN_REQ_INIT;
    req.scan_rx_cb = halow_scan_rx_cb;
    req.scan_complete_cb = halow_scan_complete_cb;
    req.scan_cb_arg = self;
    req.args.dwell_time_ms = HALOW_SCAN_DWELL_MS;

    self->scan_active = true;
    self->scan_started_ms = mp_hal_ticks_ms();

    enum mmwlan_status status = mmwlan_scan_request(&req);
    if (status != MMWLAN_SUCCESS) {
        self->scan_active = false;
        return halow_status_to_errno(status);
    }
    return 0;
}

/*******************************************************************************/
// Association

int halow_wifi_join(halow_t *self, size_t ssid_len, const uint8_t *ssid,
    size_t key_len, const uint8_t *key, uint32_t auth_type, const uint8_t *bssid) {
    if (ssid_len == 0 || ssid_len > MMWLAN_SSID_MAXLEN) {
        return -MP_EINVAL;
    }
    if (key_len > MMWLAN_PASSPHRASE_MAXLEN) {
        return -MP_EINVAL;
    }
    if (auth_type == HALOW_SEC_SAE && key_len == 0) {
        return -MP_EINVAL;
    }

    memcpy(self->sta_ssid, ssid, ssid_len);
    self->sta_ssid_len = ssid_len;

    struct mmwlan_sta_args args = MMWLAN_STA_ARGS_INIT;
    memcpy(args.ssid, ssid, ssid_len);
    args.ssid_len = ssid_len;
    args.security_type = auth_type;
    if (key_len != 0) {
        memcpy(args.passphrase, key, key_len);
        args.passphrase_len = key_len;
    }
    if (auth_type == HALOW_SEC_OPEN) {
        args.pmf_mode = MMWLAN_PMF_DISABLED;
    }
    if (bssid != NULL) {
        memcpy(args.bssid, bssid, MMWLAN_MAC_ADDR_LEN);
    }

    self->link_status = HALOW_LINK_JOIN;

    // The radio cannot hop channels while an association completes, so stop any
    // sweep that is in flight rather than joining on a moving radio.  Aborting
    // takes effect at the end of the current channel, so still wait for the
    // completion callback -- but that is one channel, not a whole sweep.
    if (self->scan_active) {
        mmwlan_scan_abort();
        halow_sched_wait(halow_scan_idle_cond, self, HALOW_SCAN_ABORT_MS);
    }

    enum mmwlan_status status = mmwlan_sta_enable(&args, halow_sta_status_cb);
    if (status != MMWLAN_SUCCESS) {
        self->link_status = HALOW_LINK_FAIL;
        return halow_status_to_errno(status);
    }
    return 0;
}

int halow_wifi_leave(halow_t *self, int itf) {
    if (itf == HALOW_ITF_AP) {
        return halow_status_to_errno(mmwlan_ap_disable());
    }
    self->link_status = HALOW_LINK_DOWN;
    return halow_status_to_errno(mmwlan_sta_disable());
}

int halow_wifi_link_status(halow_t *self, int itf) {
    if (itf == HALOW_ITF_AP) {
        return (self->itf_state & (1 << HALOW_ITF_AP)) ? HALOW_LINK_UP : HALOW_LINK_DOWN;
    }
    return self->link_status;
}

int halow_tcpip_link_status(halow_t *self, int itf) {
    int status = halow_wifi_link_status(self, itf);
    if (status != HALOW_LINK_NOIP && status != HALOW_LINK_UP) {
        return status;
    }
    // Associated: report UP only once lwIP has an address on the interface.
    struct netif *netif = &self->netif[itf];
    if ((netif->flags & NETIF_FLAG_UP) && !ip_addr_isany(&netif->ip_addr)) {
        return HALOW_LINK_UP;
    }
    return HALOW_LINK_NOIP;
}

/*******************************************************************************/
// Queries

int halow_wifi_get_mac(halow_t *self, int itf, uint8_t mac[6]) {
    enum mmwlan_vif vif = (itf == HALOW_ITF_AP) ? MMWLAN_VIF_AP : MMWLAN_VIF_STA;
    if (mmwlan_get_vif_mac_addr(vif, mac) != MMWLAN_SUCCESS) {
        memcpy(mac, self->mac, MMWLAN_MAC_ADDR_LEN);
    }
    return 0;
}

int halow_wifi_get_bssid(halow_t *self, uint8_t bssid[6]) {
    (void)self;
    return halow_status_to_errno(mmwlan_get_bssid(bssid));
}

int halow_wifi_get_rssi(halow_t *self, int32_t *rssi) {
    (void)self;
    *rssi = mmwlan_get_rssi();
    return 0;
}

int halow_wifi_get_channel(halow_t *self, int itf, uint16_t *chan_num, uint8_t *bw_mhz) {
    (void)self;
    struct mmwlan_vif_channel_info info;
    enum mmwlan_vif vif = (itf == HALOW_ITF_AP) ? MMWLAN_VIF_AP : MMWLAN_VIF_STA;
    enum mmwlan_status status = mmwlan_get_vif_channel_info(vif, &info);
    if (status != MMWLAN_SUCCESS) {
        return halow_status_to_errno(status);
    }
    *chan_num = info.s1g_chan_num;
    *bw_mhz = info.pri_bw_mhz;
    return 0;
}

// Settings morselib only accepts while it is inactive, applied once it has been
// initialised and before anything associates.
static void halow_apply_radio(halow_t *self) {
    mmwlan_set_ampdu_enabled(self->ampdu);
    mmwlan_set_sgi_enabled(self->sgi);
    mmwlan_set_subbands_enabled(self->subbands);
    if (self->rts_threshold) {
        mmwlan_set_rts_threshold(self->rts_threshold);
    }
    if (self->fragment_threshold) {
        mmwlan_set_fragment_threshold(self->fragment_threshold);
    }
    if (self->listen_interval) {
        mmwlan_set_listen_interval(self->listen_interval);
    }
    if (self->health_max_ms) {
        mmwlan_set_health_check_interval(self->health_min_ms, self->health_max_ms);
    }
}

static int halow_apply_pm(halow_t *self) {
    enum mmwlan_status status = mmwlan_set_power_save_mode(
        self->pm == HALOW_PM_NONE ? MMWLAN_PS_DISABLED : MMWLAN_PS_ENABLED);
    if (status != MMWLAN_SUCCESS) {
        return halow_status_to_errno(status);
    }
    status = mmwlan_set_dynamic_ps_timeout(self->ps_timeout_ms);
    if (status != MMWLAN_SUCCESS) {
        return halow_status_to_errno(status);
    }
    return 0;
}

int halow_wifi_pm(halow_t *self, uint32_t pm) {
    #if MICROPY_PY_NETWORK_HALOW_AP
    if (pm != HALOW_PM_NONE && (self->itf_state & (1 << HALOW_ITF_AP))) {
        // An access point has to be listening.
        return -MP_EPERM;
    }
    #endif
    uint32_t prev = self->pm;
    self->pm = pm;
    // Only reaches morselib once it has been initialised; halow_init() applies
    // whatever was configured before that.
    if (self->booted) {
        int ret = halow_apply_pm(self);
        if (ret != 0) {
            self->pm = prev;
            return ret;
        }
    }
    return 0;
}

// Regulatory testing.  The command format is defined by the vendor's test tool
// rather than here, so this passes bytes through and hands the response back.
int halow_wifi_ate_command(halow_t *self, uint8_t *cmd, size_t cmd_len,
    uint8_t *rsp, size_t *rsp_len) {
    if (!self->booted) {
        return -MP_ENODEV;
    }
    uint32_t len = *rsp_len;
    enum mmwlan_status status = mmwlan_ate_execute_command(cmd, cmd_len, rsp, &len);
    // A command the transceiver rejects still returns a response worth seeing.
    if (status != MMWLAN_SUCCESS && status != MMWLAN_COMMAND_ERROR) {
        return halow_status_to_errno(status);
    }
    *rsp_len = len;
    return 0;
}

// Pin the transmit rate, so emissions can be measured at a known modulation
// rather than whatever rate control picks.
int halow_wifi_fixed_rate(halow_t *self, int mcs, int bw_mhz, int gi) {
    if (!self->booted) {
        return -MP_ENODEV;
    }
    if (mcs < MMWLAN_MCS_NONE || mcs > MMWLAN_MCS_MAX ||
        gi < MMWLAN_GI_NONE || gi > MMWLAN_GI_MAX) {
        return -MP_EINVAL;
    }
    switch (bw_mhz) {
        case MMWLAN_BW_NONE:
        case MMWLAN_BW_1MHZ:
        case MMWLAN_BW_2MHZ:
        case MMWLAN_BW_4MHZ:
        case MMWLAN_BW_8MHZ:
            break;
        default:
            return -MP_EINVAL;
    }
    return halow_status_to_errno(mmwlan_ate_override_rate_control(mcs, bw_mhz, gi));
}

// Radio settings.  Stored whatever the state, and pushed to morselib when it
// will accept them: some are refused once the interface is up, so those take
// effect the next time it is brought up.
int halow_wifi_set_radio(halow_t *self, int what, uint32_t value) {
    bool live = self->booted && self->link_status == HALOW_LINK_DOWN;
    enum mmwlan_status status = MMWLAN_SUCCESS;

    switch (what) {
        case HALOW_RADIO_AMPDU:
            self->ampdu = value;
            if (live) {
                status = mmwlan_set_ampdu_enabled(value);
            }
            break;
        case HALOW_RADIO_SGI:
            self->sgi = value;
            if (live) {
                status = mmwlan_set_sgi_enabled(value);
            }
            break;
        case HALOW_RADIO_SUBBANDS:
            self->subbands = value;
            if (live) {
                status = mmwlan_set_subbands_enabled(value);
            }
            break;
        case HALOW_RADIO_RTS:
            self->rts_threshold = value;
            if (self->booted) {
                status = mmwlan_set_rts_threshold(value);
            }
            break;
        case HALOW_RADIO_FRAG:
            self->fragment_threshold = value;
            if (self->booted) {
                status = mmwlan_set_fragment_threshold(value);
            }
            break;
        case HALOW_RADIO_LISTEN:
            // Carried in the association request, so it cannot change under an
            // association; it applies to the next one.
            self->listen_interval = value;
            if (live) {
                status = mmwlan_set_listen_interval(value);
            }
            break;
        case HALOW_RADIO_WNM_PD:
            self->wnm_powerdown = value;
            break;
        case HALOW_RADIO_TXPOWER:
            // morselib has no query for this, so it is kept here to be read
            // back.  Zero lifts the override and restores the regulatory limit.
            self->txpower = value;
            if (self->booted) {
                status = mmwlan_override_max_tx_power(value);
            }
            break;
        default:
            return -MP_EINVAL;
    }
    return halow_status_to_errno(status);
}

uint32_t halow_wifi_get_radio(halow_t *self, int what) {
    switch (what) {
        case HALOW_RADIO_AMPDU:
            return self->ampdu;
        case HALOW_RADIO_SGI:
            return self->sgi;
        case HALOW_RADIO_SUBBANDS:
            return self->subbands;
        case HALOW_RADIO_RTS:
            return self->rts_threshold;
        case HALOW_RADIO_FRAG:
            return self->fragment_threshold;
        case HALOW_RADIO_WNM_PD:
            return self->wnm_powerdown;
        case HALOW_RADIO_TXPOWER:
            return self->txpower;
        default:
            return self->listen_interval;
    }
}

// Regulatory duty cycle: how the permitted air time is spread, and how much of
// it is left.  Which regions enforce one is part of the channel list.
int halow_wifi_set_duty_cycle(halow_t *self, int mode) {
    self->duty_cycle_mode = mode;
    if (!self->booted) {
        return -MP_ENODEV;
    }
    if (mode != MMWLAN_DUTY_CYCLE_MODE_SPREAD && mode != MMWLAN_DUTY_CYCLE_MODE_BURST) {
        return -MP_EINVAL;
    }
    return halow_status_to_errno(mmwlan_set_duty_cycle_mode(mode));
}

int halow_wifi_get_duty_cycle(halow_t *self, struct mmwlan_duty_cycle_stats *stats) {
    if (!self->booted) {
        return -MP_ENODEV;
    }
    return halow_status_to_errno(mmwlan_get_duty_cycle_stats(stats));
}

// Target Wake Time: negotiate with the access point to be awake only for an
// agreed window every interval, rather than at every DTIM.
int halow_wifi_twt(halow_t *self, uint64_t interval_us, uint32_t duration_us, int setup) {
    if (!self->booted) {
        return -MP_ENODEV;
    }
    if (self->link_status != HALOW_LINK_DOWN) {
        // The agreement is carried in the association request, so it has to be
        // in place before the interface joins anything.
        return -MP_EPERM;
    }
    if (setup < MMWLAN_TWT_SETUP_REQUEST || setup > MMWLAN_TWT_SETUP_DEMAND) {
        return -MP_EINVAL;
    }
    if (interval_us == 0 || duration_us == 0 || duration_us > interval_us) {
        // A wake window has to fit inside the interval it repeats in.
        return -MP_EINVAL;
    }

    struct mmwlan_twt_config_args args = MMWLAN_TWT_CONFIG_ARGS_INIT;
    args.twt_mode = MMWLAN_TWT_REQUESTER;
    args.twt_wake_interval_us = interval_us;
    args.twt_min_wake_duration_us = duration_us;
    args.twt_setup_command = setup;
    return halow_status_to_errno(mmwlan_twt_add_configuration(&args));
}

// Rate control statistics: what the transmitter actually settled on, which is
// otherwise invisible when throughput varies.
// Reports no statistics rather than an error when the rate table is empty,
// which is the state before anything has been transmitted.
int halow_wifi_get_rc_stats(halow_t *self, struct mmwlan_rc_stats **stats) {
    if (!self->booted) {
        return -MP_ENODEV;
    }
    *stats = mmwlan_get_rc_stats();
    return 0;
}

void halow_wifi_free_rc_stats(struct mmwlan_rc_stats *stats) {
    mmwlan_free_rc_stats(stats);
}

// How often the driver checks the transceiver is still healthy.  Each check
// wakes it, so the interval is worth raising when power matters.
int halow_wifi_set_health_check(halow_t *self, uint32_t min_ms, uint32_t max_ms) {
    if (max_ms != 0 && min_ms > max_ms) {
        return -MP_EINVAL;
    }
    self->health_min_ms = min_ms;
    self->health_max_ms = max_ms;
    if (self->booted) {
        return halow_status_to_errno(mmwlan_set_health_check_interval(min_ms, max_ms));
    }
    return 0;
}

// Versions of the parts that make up a link: the library, the transceiver
// firmware and the chip itself.  Needed whenever a problem has to be reported.
int halow_wifi_get_version(halow_t *self, struct mmwlan_version *version) {
    if (!self->booted) {
        return -MP_ENODEV;
    }
    return halow_status_to_errno(mmwlan_get_version(version));
}

// Sleep across DTIM periods, so the transceiver only wakes on its own schedule
// and the access point buffers traffic for it meanwhile.
int halow_wifi_wnm_sleep(halow_t *self, bool enable, bool powerdown) {
    if (enable && self->pm == HALOW_PM_NONE) {
        // morselib only sleeps if 802.11 power save is on, and would otherwise
        // report success while staying awake.
        return -MP_EPERM;
    }
    if (enable && self->link_status != HALOW_LINK_UP && self->link_status != HALOW_LINK_NOIP) {
        // Entering requires the AP to accept the request, so there has to be one.
        return -MP_ENOTCONN;
    }

    struct mmwlan_set_wnm_sleep_enabled_args args = MMWLAN_SET_WNM_SLEEP_ENABLED_ARGS_INIT;
    args.wnm_sleep_enabled = enable;
    args.chip_powerdown_enabled = powerdown;
    return halow_status_to_errno(mmwlan_set_wnm_sleep_enabled_ext(&args));
}

// How long the transceiver stays awake after activity before dozing again.
// Only has an effect while power saving is enabled.
int halow_wifi_set_ps_timeout(halow_t *self, uint32_t ms) {
    if (ms == 0) {
        return -MP_EINVAL;
    }
    if (self->booted) {
        enum mmwlan_status status = mmwlan_set_dynamic_ps_timeout(ms);
        if (status != MMWLAN_SUCCESS) {
            return halow_status_to_errno(status);
        }
    }
    self->ps_timeout_ms = ms;
    return 0;
}

int halow_wifi_get_ps_timeout(halow_t *self, uint32_t *ms) {
    *ms = self->ps_timeout_ms;
    return 0;
}

int halow_wifi_get_pm(halow_t *self, uint32_t *pm) {
    *pm = self->pm;
    return 0;
}

/*******************************************************************************/
// AP configuration

#if MICROPY_PY_NETWORK_HALOW_AP

void halow_wifi_ap_set_ssid(halow_t *self, size_t len, const uint8_t *buf) {
    self->ap_ssid_len = MIN(len, sizeof(self->ap_ssid));
    memcpy(self->ap_ssid, buf, self->ap_ssid_len);
}

void halow_wifi_ap_set_password(halow_t *self, size_t len, const uint8_t *buf) {
    self->ap_key_len = MIN(len, sizeof(self->ap_key));
    memcpy(self->ap_key, buf, self->ap_key_len);
}

void halow_wifi_ap_set_auth(halow_t *self, uint32_t auth) {
    self->ap_auth = auth;
}

void halow_wifi_ap_get_ssid(halow_t *self, size_t *len, const uint8_t **buf) {
    *len = self->ap_ssid_len;
    *buf = self->ap_ssid;
}

uint32_t halow_wifi_ap_get_auth(halow_t *self) {
    return self->ap_auth;
}

int halow_wifi_ap_get_stas(halow_t *self, int *num_stas, uint8_t *macs) {
    // The list is compacted in place by halow_ap_sta_status_cb() as stations
    // come and go, so it has to be copied out whole rather than read across a
    // station leaving.
    mp_uint_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    int n = MIN(*num_stas, (int)self->ap_sta_count);
    memcpy(macs, self->ap_stas, n * MMWLAN_MAC_ADDR_LEN);
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    *num_stas = n;
    return 0;
}

void halow_wifi_ap_set_channel(halow_t *self, uint8_t chan_num) {
    self->ap_chan_num = chan_num;
}

#endif // MICROPY_PY_NETWORK_HALOW_AP

/*******************************************************************************/
// Datapath

int halow_send_ethernet(halow_t *self, int itf, size_t len, const void *buf, bool is_pbuf) {
    (void)itf;


    if (len > HALOW_TX_BUF_SIZE || halow_tx_buf == NULL) {
        return -MP_EINVAL;
    }

    // mmwlan_tx() would block, and this runs from lwIP's linkoutput with PendSV
    // raised, so nothing would service the transmit path: drive it here instead.
    // That also runs the task delivering received frames, which would re-enter
    // lwIP mid-walk, so receive is held off for the wait.
    if (halow_sched_task_current() == NULL) {
        uint32_t start = mp_hal_ticks_ms();
        self->rx_deferred = true;
        while (mmwlan_tx_wait_until_ready(0) != MMWLAN_SUCCESS) {
            if ((uint32_t)(mp_hal_ticks_ms() - start) >= HALOW_TX_READY_MS) {
                self->rx_deferred = false;
                return -MP_EAGAIN;
            }
            halow_poll_func();
        }
        self->rx_deferred = false;
    } else if (mmwlan_tx_wait_until_ready(0) != MMWLAN_SUCCESS) {
        // A driver task got here, by way of a received frame that lwIP answered
        // straight back -- an ARP reply or an ACK.  Waiting would be a pure
        // spin: the scheduler is already running one level up, so nothing this
        // call does can drain the queue.  Report congestion and let lwIP retry.
        return -MP_EAGAIN;
    }

    // Linearised only once there is somewhere for it to go.  The buffer is
    // shared, and a frame that lwIP generates while this one is still waiting
    // would otherwise copy over it and be transmitted in its place.
    const uint8_t *data;
    if (is_pbuf) {
        struct pbuf *p = (struct pbuf *)buf;
        if (p->next == NULL) {
            // Single segment, send it straight from the pbuf.
            data = p->payload;
        } else {
            pbuf_copy_partial(p, halow_tx_buf, len, 0);
            data = halow_tx_buf;
        }
    } else {
        data = buf;
    }

    struct mmpkt *pkt = mmwlan_alloc_mmpkt_for_tx(len, MMWLAN_TX_DEFAULT_QOS_TID);
    if (pkt == NULL) {
        return -MP_EAGAIN;
    }
    struct mmpktview *pktview = mmpkt_open(pkt);
    mmpkt_append_data(pktview, data, len);
    mmpkt_close(&pktview);

    struct mmwlan_tx_metadata metadata = MMWLAN_TX_METADATA_INIT;
    metadata.tid = MMWLAN_TX_DEFAULT_QOS_TID;
    enum mmwlan_status status = mmwlan_tx_pkt(pkt, &metadata);
    if (status != MMWLAN_SUCCESS) {
        debug_printf("halow: tx failed %d\n", status);
        return halow_status_to_errno(status);
    }

    // Only queued so far.  Ask for a poll rather than running the tasks here:
    // this is inside lwIP, and a task delivering a frame would re-enter it.
    halow_schedule_poll();
    return 0;
}

#endif // MICROPY_PY_NETWORK_HALOW
