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
#ifndef MICROPY_INCLUDED_STM32_CYW43_H
#define MICROPY_INCLUDED_STM32_CYW43_H

// For cyw43_trace_flags
#define CYW43_TRACE_ASYNC_EV (0x0001)
#define CYW43_TRACE_ETH_TX (0x0002)
#define CYW43_TRACE_ETH_RX (0x0004)
#define CYW43_TRACE_ETH_FULL (0x0008)
#define CYW43_TRACE_MAC (0x0010)

// For cyw43_ioctl
#define CYW43_IOCTL_GET_CHANNEL (0x3a)
#define CYW43_IOCTL_GET_ANTDIV (0x7e)
#define CYW43_IOCTL_SET_ANTDIV (0x81)
#define CYW43_IOCTL_SET_MONITOR (0xd9)
#define CYW43_IOCTL_GET_VAR (0x20c)
#define CYW43_IOCTL_SET_VAR (0x20f)

// Return value of cyw43_wifi_link_status
#define CYW43_LINK_DOWN (0)
#define CYW43_LINK_JOIN (1)
#define CYW43_LINK_NOIP (2)
#define CYW43_LINK_UP (3)
#define CYW43_LINK_FAIL (-1)
#define CYW43_LINK_NONET (-2)
#define CYW43_LINK_BADAUTH (-3)

// Async events
#define CYW43_EV_SET_SSID (0)
#define CYW43_EV_JOIN (1)
#define CYW43_EV_AUTH (3)
#define CYW43_EV_DEAUTH_IND (6)
#define CYW43_EV_ASSOC (7)
#define CYW43_EV_DISASSOC (11)
#define CYW43_EV_DISASSOC_IND (12)
#define CYW43_EV_LINK (16)
#define CYW43_EV_PSK_SUP (46)
#define CYW43_EV_ESCAN_RESULT (69)
#define CYW43_EV_CSA_COMPLETE_IND (80)
#define CYW43_EV_ASSOC_REQ_IE (87)
#define CYW43_EV_ASSOC_RESP_IE (88)

enum {
    CYW43_ITF_STA,
    CYW43_ITF_AP,
};

typedef struct _cyw43_t cyw43_t;

typedef struct _cyw43_async_event_t {
    uint16_t flags;
    uint32_t event_type;
    uint32_t status;
    uint32_t reason;
    uint8_t interface;
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

typedef struct _cyw43_scan_result_t {
    uint8_t bssid[6];
    uint8_t ssid_len;
    uint8_t ssid[32];
    int16_t rssi;
    uint8_t channel;
    uint8_t auth_mode;
} cyw43_scan_result_t;

extern cyw43_t cyw43_state;
extern void (*cyw43_poll)(void);
extern uint32_t cyw43_sleep;
extern uint32_t cyw43_trace_flags;

void cyw43_init(cyw43_t *self, const uint8_t *mac);
void cyw43_deinit(cyw43_t *self);

int cyw43_ioctl(cyw43_t *self, uint32_t cmd, size_t len, uint8_t *buf, uint32_t iface);
int cyw43_send_ethernet(cyw43_t *self, int itf, size_t len, const void *buf, bool is_pbuf);
void cyw43_allow_sleep(cyw43_t *self);

int cyw43_wifi_on(cyw43_t *self, uint32_t country);
int cyw43_wifi_pm(cyw43_t *self, uint32_t pm);
int cyw43_wifi_link_status(cyw43_t *self, int itf);
void cyw43_wifi_set_up(cyw43_t *self, int itf, bool up);
int cyw43_wifi_get_mac(cyw43_t *self, int itf, uint8_t mac[6]);
int cyw43_wifi_scan(cyw43_t *self, cyw43_wifi_scan_options_t *opts, void *env, int (*result_cb)(void*, cyw43_scan_result_t*));
bool cyw43_wifi_scan_active(cyw43_t *self);
int cyw43_wifi_join(cyw43_t *self, size_t ssid_len, const char *ssid, size_t key_len, const char *key, const uint8_t *bssid, uint32_t channel);
int cyw43_wifi_leave(cyw43_t *self, int itf);

void cyw43_wifi_ap_get_ssid(cyw43_t *self, size_t *len, const uint8_t **buf);
void cyw43_wifi_ap_set_channel(cyw43_t *self, uint32_t channel);
void cyw43_wifi_ap_set_ssid(cyw43_t *self, size_t len, const uint8_t *buf);
void cyw43_wifi_ap_set_password(cyw43_t *self, size_t len, const uint8_t *buf);
void cyw43_wifi_ap_init(cyw43_t *self, size_t ssid_len, const char *ssid, uint32_t auth, size_t key_len, const char *key, uint32_t channel);
void cyw43_wifi_ap_set_up(cyw43_t *self, bool up);
void cyw43_wifi_ap_get_stas(cyw43_t *self, int *num_stas, uint8_t *macs);

void cyw43_async_event_callback(cyw43_t *self, const cyw43_async_event_t *ev);
void cyw43_tcpip_init(cyw43_t *self, int itf);
void cyw43_tcpip_deinit(cyw43_t *self, int itf);
void cyw43_tcpip_set_link_up(cyw43_t *self, int itf);
void cyw43_tcpip_set_link_down(cyw43_t *self, int itf);
void cyw43_tcpip_process_ethernet(cyw43_t *self, int itf, size_t len, const uint8_t *buf);
int cyw43_tcpip_link_status(cyw43_t *self, int itf);

#endif // MICROPY_INCLUDED_STM32_CYW43_H
