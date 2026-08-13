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
 * Driver for the Morse Micro MM6108/MM8108 802.11ah (Wi-Fi HaLow) transceivers.
 *
 * This layer sits between MicroPython's network module and morselib, and mirrors
 * the shape of the cyw43 driver so that the two present the same model to the
 * port: a state object holding the lwIP interfaces, a link status that folds the
 * WLAN and TCP/IP state together, and a poll function driven from PendSV.
 */
#ifndef MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_H
#define MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "shared/netutils/dhcpserver.h"

#include "mmwlan.h"

// No CYW43_THREAD_ENTER equivalent: morselib already serialises its own API
// through the OSAL mutexes, which block by running the scheduler.  A lock that
// suspends the poll across a morselib call stalls those waits instead.

// Optional: the IRQ line is read by level on every poll regardless, and
// mp_hal_pin_interrupt() is not part of the common mphal API.
#ifndef MICROPY_PY_NETWORK_HALOW_PIN_IRQ
#define MICROPY_PY_NETWORK_HALOW_PIN_IRQ (0)
#endif

// Access point mode.  morselib's AP support is an alpha API and
// mmwlan_ap_enable() does not currently succeed on the MM8108, so the mode is
// built out rather than offered and failing.
#ifndef MICROPY_PY_NETWORK_HALOW_AP
#define MICROPY_PY_NETWORK_HALOW_AP (0)
#endif

#ifndef MICROPY_HW_HALOW_CHIP
#define MICROPY_HW_HALOW_CHIP   mmhal_mm8108
#endif

// 25MHz: at 50MHz the SD-over-SPI framing corrupts under sustained traffic and
// the transceiver stops answering.
#ifndef MICROPY_HW_HALOW_SPI_BAUDRATE
#define MICROPY_HW_HALOW_SPI_BAUDRATE   (25000000)
#endif

// IP MTU.  morselib accepts frames up to MMHAL_WLAN_MMPKT_TX_MAX_SIZE, but
// 802.11ah carries ordinary Ethernet traffic so the usual 1500 applies.
#ifndef MICROPY_HW_HALOW_MTU
#define MICROPY_HW_HALOW_MTU    (1500)
#endif

// Address the station comes up on when the build has no DHCP client.  Unused
// otherwise, as the lease supplies all three.
#ifndef MICROPY_HW_HALOW_STA_ADDRESS
#define MICROPY_HW_HALOW_STA_ADDRESS    (0xc0a80102)    // 192.168.1.2
#endif
#ifndef MICROPY_HW_HALOW_STA_NETMASK
#define MICROPY_HW_HALOW_STA_NETMASK    (0xffffff00)    // 255.255.255.0
#endif
#ifndef MICROPY_HW_HALOW_STA_GATEWAY
#define MICROPY_HW_HALOW_STA_GATEWAY    (0xc0a80101)    // 192.168.1.1
#endif

// Address the soft AP hands out, if the board does not override it.
#ifndef MICROPY_HW_HALOW_AP_ADDRESS
#define MICROPY_HW_HALOW_AP_ADDRESS (0xc0a80401)    // 192.168.4.1
#endif
#ifndef MICROPY_HW_HALOW_AP_NETMASK
#define MICROPY_HW_HALOW_AP_NETMASK (0xffffff00)    // 255.255.255.0
#endif

// Networks one scan can report.  A sweep that finds more than this drops the
// rest, so it is the ceiling on what scan() can return.
#ifndef HALOW_SCAN_CACHE_MAX
#define HALOW_SCAN_CACHE_MAX    (32)
#endif

// Stations the AP will admit, matching the morselib default.
#define HALOW_AP_MAX_STAS       (MMWLAN_DEFAULT_AP_MAX_STAS)

// Interfaces, matching the order of MOD_NETWORK_STA_IF and MOD_NETWORK_AP_IF.
#define HALOW_ITF_STA           (0)
#define HALOW_ITF_AP            (1)
#define HALOW_ITF_MAX           (2)

// Link status, with the same meaning as the CYW43_LINK_xxx values so that the
// network.HALOW status values match network.WLAN.
#define HALOW_LINK_DOWN         (0)     // link is down
#define HALOW_LINK_JOIN         (1)     // connecting to an AP
#define HALOW_LINK_NOIP         (2)     // associated, but no IP address
#define HALOW_LINK_UP           (3)     // associated with an IP address
#define HALOW_LINK_FAIL         (-1)    // connection failed
#define HALOW_LINK_NONET        (-2)    // no matching SSID found
#define HALOW_LINK_BADAUTH      (-3)    // authentication failure

// Security types.  802.11ah has no WPA2-PSK: HaLow networks are either open,
// OWE (opportunistic encryption) or SAE (WPA3).  Fixed to literals rather than
// aliased to the morselib enum: these are a public API, and a value must not
// change if the SDK renumbers its enum.  network_halow.c static-asserts the
// pairing, so a divergence fails the build instead of silently breaking users.
#define HALOW_SEC_OPEN          (0)
#define HALOW_SEC_OWE           (1)
#define HALOW_SEC_SAE           (2)

// Radio settings, as accepted by halow_wifi_set_radio().
#define HALOW_RADIO_AMPDU       (0)
#define HALOW_RADIO_SGI         (1)
#define HALOW_RADIO_SUBBANDS    (2)
#define HALOW_RADIO_RTS         (3)
#define HALOW_RADIO_FRAG        (4)
#define HALOW_RADIO_LISTEN      (5)
#define HALOW_RADIO_WNM_PD      (6)
#define HALOW_RADIO_TXPOWER     (7)

// Target wake time negotiation, as asked for in the association request.
// Fixed to literals; see the security types above.
#define HALOW_TWT_REQUEST       (0)
#define HALOW_TWT_SUGGEST       (1)
#define HALOW_TWT_DEMAND        (2)

// How the regulatory airtime allowance is spent.  Fixed to literals; see above.
#define HALOW_DUTY_CYCLE_SPREAD (0)
#define HALOW_DUTY_CYCLE_BURST  (1)

// Fields packed into the rate word of a rate control statistics entry: four
// bits of bandwidth, four of rate, then a single guard interval bit.
#define HALOW_RC_RATE_SHIFT     (MMWLAN_RC_STATS_RATE_INFO_RATE_OFFSET)
#define HALOW_RC_BW_SHIFT       (MMWLAN_RC_STATS_RATE_INFO_BW_OFFSET)
#define HALOW_RC_GI_SHIFT       (MMWLAN_RC_STATS_RATE_INFO_GUARD_OFFSET)
#define HALOW_RC_FIELD_MASK     (0xf)
#define HALOW_RC_GI_MASK        (0x1)

// Rate table entries reported by status("rates").  Every combination the rate
// word can encode is 16 rates by 4 bandwidths by 2 guard intervals, so this
// cannot truncate a real table; it is a bound on a length the transceiver
// reports rather than one this driver chose.
#define HALOW_RC_STATS_MAX      (128)

// Power management modes, as accepted by halow_wifi_pm().
#define HALOW_PM_NONE           (0)     // always listening
#define HALOW_PM_POWERSAVE      (1)     // transmit only, transceiver dozes

// Trace flags, matching cyw43's.
#define HALOW_TRACE_ASYNC_EV    (0x0001)
#define HALOW_TRACE_ETH_TX      (0x0002)
#define HALOW_TRACE_ETH_RX      (0x0004)
#define HALOW_TRACE_ETH_FULL    (0x0008)
#define HALOW_TRACE_MAC         (0x0010)

// A single scan result, flattened out of struct mmwlan_scan_result.
typedef struct _halow_ev_scan_result_t {
    uint8_t ssid_len;
    uint8_t ssid[MMWLAN_SSID_MAXLEN];
    uint8_t bssid[MMWLAN_MAC_ADDR_LEN];
    int16_t rssi;
    // Centre frequency of the channel the frame was RECEIVED on, in Hz.  A wide
    // AP beacons on its primary channel so that narrowband stations can hear it,
    // so this is not the center of its operating channel: an 8MHz AP centered on
    // 916MHz is seen here at the primary channel's frequency.
    uint32_t channel_freq_hz;
    uint8_t chan_num;                   // S1G channel number, 0 if not a local one
    uint8_t bw_mhz;                     // bandwidth the frame was received on
    uint8_t op_bw_mhz;                  // operating bandwidth of the AP
    uint8_t security;                   // one of HALOW_SEC_xxx
} halow_ev_scan_result_t;

typedef struct _halow_t {
    uint8_t itf_state;                  // bitmask of interfaces brought up

    uint32_t trace_flags;

    // State for asynchronous events.
    volatile bool scan_active;
    // Set while the driver is being run from inside lwIP, so that a received
    // frame is dropped rather than pushed back into it.  See halow_send_ethernet().
    volatile bool rx_deferred;
    uint32_t pm;
    uint32_t ps_timeout_ms;
    // Radio settings, kept here because morselib only accepts some of them
    // while it is inactive, so they are applied when the driver initialises.
    bool ampdu;
    bool sgi;
    bool subbands;
    uint16_t listen_interval;
    uint16_t txpower;
    uint8_t duty_cycle_mode;
    unsigned rts_threshold;
    unsigned fragment_threshold;
    // Whether entering WNM sleep should also power the transceiver down.
    bool wnm_powerdown;
    uint32_t health_min_ms;
    uint32_t health_max_ms;
    // Results from the current sweep.  Allocated from the driver heap rather
    // than inline, as halow_t is a static global.
    halow_ev_scan_result_t *scan_cache;
    uint8_t scan_cache_len;
    uint8_t scan_cache_max;
    volatile uint32_t scan_started_ms;
    volatile int8_t link_status;

    // morselib has been mmwlan_init()ed.  NOT the same as the transceiver being
    // usable: taking an interface down calls mmwlan_shutdown() and leaves this
    // set, so anything that actually talks to the chip must test `booted`.
    bool initted;
    // Whether the transceiver has been booted.  Booting it a second time
    // fails, and both interfaces share the one transceiver.
    bool booted;

    // Network last asked for, so that config("ssid") can report it.  The
    // passphrase is not kept: morselib takes its own copy and nothing here
    // needs to read it back.
    uint8_t sta_ssid_len;
    uint8_t sta_ssid[MMWLAN_SSID_MAXLEN];

    #if MICROPY_PY_NETWORK_HALOW_AP
    // AP settings.
    uint32_t ap_auth;
    uint8_t ap_ssid_len;
    uint8_t ap_key_len;
    uint8_t ap_ssid[MMWLAN_SSID_MAXLEN];
    uint8_t ap_key[MMWLAN_PASSPHRASE_MAXLEN];
    // S1G channel number the AP should use, or zero to pick one from the
    // regulatory domain.
    uint8_t ap_chan_num;
    // Stations associated with the AP.  morselib reports each status change but
    // has no enumeration API, so the list is maintained here.
    uint8_t ap_sta_count;
    uint8_t ap_stas[HALOW_AP_MAX_STAS][MMWLAN_MAC_ADDR_LEN];
    #endif

    // Channel list for the configured country, needed to map an AP channel
    // number onto its operating class.
    const struct mmwlan_s1g_channel_list *channels;

    // lwIP data.
    struct netif netif[HALOW_ITF_MAX];
    #if LWIP_IPV4 && LWIP_DHCP
    struct dhcp dhcp_client;
    #endif
    #if MICROPY_PY_NETWORK_HALOW_AP
    dhcp_server_t dhcp_server;
    #endif

    // MAC address, from the transceiver's OTP or derived from the MCU's UID.
    uint8_t mac[MMWLAN_MAC_ADDR_LEN];

    // Country code most recently passed to halow_wifi_set_up().
    char country[2];
} halow_t;

extern halow_t halow_state;

// Set while the driver is up, so that the port knows whether to poll it, as
// cyw43_poll is.  There is no equivalent of cyw43_sleep: morselib does not
// publish a next-deadline query, so the driver cannot say when it next needs
// servicing and is polled on every tick instead.
extern void (*halow_poll)(void);

/*******************************************************************************/
// Control

int halow_init(halow_t *self);
void halow_deinit(halow_t *self);

// Release the driver on soft reset.  Declared for ports, which cannot include
// this header without morselib's, so they declare it themselves.

// Run any work morselib has pending.  Must not be called re-entrantly; the port
// schedules it via PendSV, at the same priority it uses for cyw43_poll().
void halow_poll_func(void);

// Ask the port to run halow_poll_func() soon.  The default implementation does
// nothing and relies on the periodic poll; ports override it to raise PendSV.
void halow_schedule_poll(void);

// True if the regulatory database has an 802.11ah channel list for this country.
// Unlike 2.4GHz there is no worldwide fallback, so the country must be set.
bool halow_country_supported(const char *country);

int halow_wifi_set_up(halow_t *self, int itf, bool up, const char *country);

// Give up on an in-flight scan. morselib has no abort, so the scan keeps running
// in the transceiver; this only detaches our side of it.


// Copy the current cache out. Returns the number of entries written.
// Run one sweep and copy out what it found, up to max results.
size_t halow_wifi_scan_cached(halow_t *self, halow_ev_scan_result_t *out, size_t max);
int halow_wifi_join(halow_t *self, size_t ssid_len, const uint8_t *ssid,
    size_t key_len, const uint8_t *key, uint32_t auth_type, const uint8_t *bssid);
int halow_wifi_leave(halow_t *self, int itf);
int halow_wifi_link_status(halow_t *self, int itf);
int halow_wifi_get_mac(halow_t *self, int itf, uint8_t mac[6]);
int halow_wifi_get_bssid(halow_t *self, uint8_t bssid[6]);
int halow_wifi_get_rssi(halow_t *self, int32_t *rssi);
int halow_wifi_get_channel(halow_t *self, int itf, uint16_t *chan_num, uint8_t *bw_mhz);
int halow_wifi_pm(halow_t *self, uint32_t pm);
int halow_wifi_set_ps_timeout(halow_t *self, uint32_t ms);
int halow_wifi_get_ps_timeout(halow_t *self, uint32_t *ms);
int halow_wifi_get_pm(halow_t *self, uint32_t *pm);
int halow_wifi_wnm_sleep(halow_t *self, bool enable, bool powerdown);
int halow_wifi_get_version(halow_t *self, struct mmwlan_version *version);
int halow_wifi_set_radio(halow_t *self, int what, uint32_t value);
uint32_t halow_wifi_get_radio(halow_t *self, int what);
int halow_wifi_twt(halow_t *self, uint64_t interval_us, uint32_t duration_us, int setup);
int halow_wifi_get_rc_stats(halow_t *self, struct mmwlan_rc_stats **stats);
void halow_wifi_free_rc_stats(struct mmwlan_rc_stats *stats);
int halow_wifi_set_health_check(halow_t *self, uint32_t min_ms, uint32_t max_ms);
int halow_wifi_set_duty_cycle(halow_t *self, int mode);
int halow_wifi_get_duty_cycle(halow_t *self, struct mmwlan_duty_cycle_stats *stats);
int halow_wifi_ate_command(halow_t *self, uint8_t *cmd, size_t cmd_len,
    uint8_t *rsp, size_t *rsp_len);
int halow_wifi_fixed_rate(halow_t *self, int mcs, int bw_mhz, int gi);

void halow_wifi_ap_set_ssid(halow_t *self, size_t len, const uint8_t *buf);
void halow_wifi_ap_set_password(halow_t *self, size_t len, const uint8_t *buf);
void halow_wifi_ap_set_auth(halow_t *self, uint32_t auth);
void halow_wifi_ap_set_channel(halow_t *self, uint8_t chan_num);
void halow_wifi_ap_get_ssid(halow_t *self, size_t *len, const uint8_t **buf);
uint32_t halow_wifi_ap_get_auth(halow_t *self);
int halow_wifi_ap_get_stas(halow_t *self, int *num_stas, uint8_t *macs);

/*******************************************************************************/
// Datapath

int halow_send_ethernet(halow_t *self, int itf, size_t len, const void *buf, bool is_pbuf);

// Overall link status, folding the TCP/IP state into the WLAN link status.
int halow_tcpip_link_status(halow_t *self, int itf);

// lwIP glue, implemented in halow_lwip.c and called from halow_ctrl.c.
void halow_cb_tcpip_init(halow_t *self, int itf);
void halow_cb_tcpip_deinit(halow_t *self, int itf);
void halow_cb_tcpip_set_link_up(halow_t *self, int itf);
void halow_cb_tcpip_set_link_down(halow_t *self, int itf);
// Hand a received frame to lwIP.  morselib reports the 802.3 header and the
// payload separately and they are not contiguous, so both are passed through.
void halow_cb_process_ethernet(void *cb_data, int itf,
    const uint8_t *header, size_t header_len, const uint8_t *payload, size_t payload_len);

/*******************************************************************************/
// HAL hooks

// Poll the transceiver's interrupt lines, implemented in halow_hal.c.
void halow_hal_poll_irqs(void);

// Re-enable the transceiver's pin interrupt after a poll has drained it.
void halow_hal_irq_rearm(void);


#endif // MICROPY_INCLUDED_DRIVERS_HALOW_HALOW_H
