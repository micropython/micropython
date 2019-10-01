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
#ifndef MICROPY_INCLUDED_STM32_CYW43_LL_H
#define MICROPY_INCLUDED_STM32_CYW43_LL_H

// IOCTL commands
#define CYW43_IOCTL_GET_SSID            (0x32)
#define CYW43_IOCTL_GET_CHANNEL         (0x3a)
#define CYW43_IOCTL_SET_DISASSOC        (0x69)
#define CYW43_IOCTL_GET_ANTDIV          (0x7e)
#define CYW43_IOCTL_SET_ANTDIV          (0x81)
#define CYW43_IOCTL_SET_MONITOR         (0xd9)
#define CYW43_IOCTL_GET_VAR             (0x20c)
#define CYW43_IOCTL_SET_VAR             (0x20f)

// Async events, event_type field
#define CYW43_EV_SET_SSID               (0)
#define CYW43_EV_JOIN                   (1)
#define CYW43_EV_AUTH                   (3)
#define CYW43_EV_DEAUTH_IND             (6)
#define CYW43_EV_ASSOC                  (7)
#define CYW43_EV_DISASSOC               (11)
#define CYW43_EV_DISASSOC_IND           (12)
#define CYW43_EV_LINK                   (16)
#define CYW43_EV_PRUNE                  (23)
#define CYW43_EV_PSK_SUP                (46)
#define CYW43_EV_ESCAN_RESULT           (69)
#define CYW43_EV_CSA_COMPLETE_IND       (80)
#define CYW43_EV_ASSOC_REQ_IE           (87)
#define CYW43_EV_ASSOC_RESP_IE          (88)

enum {
    CYW43_ITF_STA,
    CYW43_ITF_AP,
};

typedef struct _cyw43_ev_scan_result_t {
    uint32_t _0[5];
    uint8_t bssid[6];
    uint16_t _1[2];
    uint8_t ssid_len;
    uint8_t ssid[32];
    uint32_t _2[5];
    uint16_t channel;
    uint16_t _3;
    uint8_t auth_mode;
    int16_t rssi;
} cyw43_ev_scan_result_t;

typedef struct _cyw43_async_event_t {
    uint16_t _0;
    uint16_t flags;
    uint32_t event_type;
    uint32_t status;
    uint32_t reason;
    uint8_t _1[30];
    uint8_t interface;
    uint8_t _2;
    union {
        cyw43_ev_scan_result_t scan_result;
    } u;
} cyw43_async_event_t;

typedef struct _cyw43_wifi_scan_options_t {
    uint32_t version;
    uint16_t action;
    uint16_t _;
    uint32_t ssid_len; // 0 to select all
    uint8_t ssid[32];
    uint8_t bssid[6];
    int8_t bss_type; // fill with 0xff to select all
    int8_t scan_type; // 0=active, 1=passive
    int32_t nprobes;
    int32_t active_time;
    int32_t passive_time;
    int32_t home_time;
    int32_t channel_num;
    uint16_t channel_list[1];
} cyw43_wifi_scan_options_t;

typedef struct _cyw43_ll_t {
    uint32_t opaque[528];
} cyw43_ll_t;

void cyw43_ll_init(cyw43_ll_t *self, void *cb_data);
void cyw43_ll_deinit(cyw43_ll_t *self);

int cyw43_ll_bus_init(cyw43_ll_t *self, const uint8_t *mac);
void cyw43_ll_bus_sleep(cyw43_ll_t *self, bool can_sleep);
void cyw43_ll_process_packets(cyw43_ll_t *self);
int cyw43_ll_ioctl(cyw43_ll_t *self, uint32_t cmd, size_t len, uint8_t *buf, uint32_t iface);
int cyw43_ll_send_ethernet(cyw43_ll_t *self, int itf, size_t len, const void *buf, bool is_pbuf);

int cyw43_ll_wifi_on(cyw43_ll_t *self, uint32_t country);
int cyw43_ll_wifi_pm(cyw43_ll_t *self, uint32_t pm, uint32_t pm_sleep_ret, uint32_t li_bcn, uint32_t li_dtim, uint32_t li_assoc);
int cyw43_ll_wifi_scan(cyw43_ll_t *self, cyw43_wifi_scan_options_t *opts);

int cyw43_ll_wifi_join(cyw43_ll_t *self, size_t ssid_len, const uint8_t *ssid, size_t key_len, const uint8_t *key, uint32_t auth_type, const uint8_t *bssid, uint32_t channel);
void cyw43_ll_wifi_set_wpa_auth(cyw43_ll_t *self);
void cyw43_ll_wifi_rejoin(cyw43_ll_t *self);

int cyw43_ll_wifi_ap_init(cyw43_ll_t *self, size_t ssid_len, const uint8_t *ssid, uint32_t auth, size_t key_len, const uint8_t *key, uint32_t channel);
int cyw43_ll_wifi_ap_set_up(cyw43_ll_t *self, bool up);
int cyw43_ll_wifi_ap_get_stas(cyw43_ll_t *self, int *num_stas, uint8_t *macs);

// Callbacks to be provided by mid-level interface
int cyw43_cb_read_host_interrupt_pin(void *cb_data);
void cyw43_cb_ensure_awake(void *cb_data);
void cyw43_cb_process_async_event(void *cb_data, const cyw43_async_event_t *ev);
void cyw43_cb_process_ethernet(void *cb_data, int itf, size_t len, const uint8_t *buf);

#endif // MICROPY_INCLUDED_STM32_CYW43_LL_H
