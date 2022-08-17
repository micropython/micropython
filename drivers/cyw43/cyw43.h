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

#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "shared/netutils/dhcpserver.h"
#include "drivers/cyw43/cyw43_ll.h"

// For trace_flags
#define CYW43_TRACE_ASYNC_EV    (0x0001)
#define CYW43_TRACE_ETH_TX      (0x0002)
#define CYW43_TRACE_ETH_RX      (0x0004)
#define CYW43_TRACE_ETH_FULL    (0x0008)
#define CYW43_TRACE_MAC         (0x0010)

// Return value of cyw43_wifi_link_status
#define CYW43_LINK_DOWN         (0)
#define CYW43_LINK_JOIN         (1)
#define CYW43_LINK_NOIP         (2)
#define CYW43_LINK_UP           (3)
#define CYW43_LINK_FAIL         (-1)
#define CYW43_LINK_NONET        (-2)
#define CYW43_LINK_BADAUTH      (-3)

#ifndef MICROPY_BOARD_HOSTNAME
#define MICROPY_BOARD_HOSTNAME  "PYBD"
#endif

#ifndef MICROPY_BOARD_HOSTNAME_LENGTH
#define MICROPY_BOARD_HOSTNAME_LENGTH 16
#endif

typedef struct _cyw43_t {
    cyw43_ll_t cyw43_ll;

    uint8_t itf_state;
    uint32_t trace_flags;

    // State for async events
    volatile uint32_t wifi_scan_state;
    uint32_t wifi_join_state;
    void *wifi_scan_env;
    int (*wifi_scan_cb)(void*, const cyw43_ev_scan_result_t*);

    // Pending things to do
    bool pend_disassoc;
    bool pend_rejoin;
    bool pend_rejoin_wpa;

    // AP settings
    uint8_t ap_channel;
    uint8_t ap_auth;
    uint8_t ap_ssid_len;
    uint8_t ap_key_len;
    uint8_t ap_ssid[32];
    uint8_t ap_key[64];

    // lwIP data
    struct netif netif[2];
    struct dhcp dhcp_client;
    dhcp_server_t dhcp_server;
    char hostname[MICROPY_BOARD_HOSTNAME_LENGTH];
} cyw43_t;

extern cyw43_t cyw43_state;
extern void (*cyw43_poll)(void);
extern uint32_t cyw43_sleep;

void cyw43_init(cyw43_t *self);
void cyw43_deinit(cyw43_t *self);

int cyw43_ioctl(cyw43_t *self, uint32_t cmd, size_t len, uint8_t *buf, uint32_t iface);
int cyw43_send_ethernet(cyw43_t *self, int itf, size_t len, const void *buf, bool is_pbuf);

int cyw43_wifi_pm(cyw43_t *self, uint32_t pm);
int cyw43_wifi_link_status(cyw43_t *self, int itf);
void cyw43_wifi_set_up(cyw43_t *self, int itf, bool up);
int cyw43_wifi_get_mac(cyw43_t *self, int itf, uint8_t mac[6]);
int cyw43_wifi_scan(cyw43_t *self, cyw43_wifi_scan_options_t *opts, void *env, int (*result_cb)(void*, const cyw43_ev_scan_result_t*));

static inline bool cyw43_wifi_scan_active(cyw43_t *self) {
    return self->wifi_scan_state == 1;
}

int cyw43_wifi_join(cyw43_t *self, size_t ssid_len, const uint8_t *ssid, size_t key_len, const uint8_t *key, uint32_t auth_type, const uint8_t *bssid, uint32_t channel);
int cyw43_wifi_leave(cyw43_t *self, int itf);

static inline void cyw43_wifi_ap_get_ssid(cyw43_t *self, size_t *len, const uint8_t **buf) {
    *len = self->ap_ssid_len;
    *buf = self->ap_ssid;
}

static inline void cyw43_wifi_ap_set_channel(cyw43_t *self, uint32_t channel) {
    self->ap_channel = channel;
}

static inline void cyw43_wifi_ap_set_ssid(cyw43_t *self, size_t len, const uint8_t *buf) {
    self->ap_ssid_len = MIN(len, sizeof(self->ap_ssid));
    memcpy(self->ap_ssid, buf, self->ap_ssid_len);
}

static inline void cyw43_wifi_ap_set_password(cyw43_t *self, size_t len, const uint8_t *buf) {
    self->ap_key_len = MIN(len, sizeof(self->ap_key));
    memcpy(self->ap_key, buf, self->ap_key_len);
}

static inline void cyw43_wifi_ap_set_auth(cyw43_t *self, uint32_t auth) {
    self->ap_auth = auth;
}

void cyw43_wifi_ap_get_stas(cyw43_t *self, int *num_stas, uint8_t *macs);

void cyw43_tcpip_init(cyw43_t *self, int itf);
void cyw43_tcpip_deinit(cyw43_t *self, int itf);
void cyw43_tcpip_set_link_up(cyw43_t *self, int itf);
void cyw43_tcpip_set_link_down(cyw43_t *self, int itf);
int cyw43_tcpip_link_status(cyw43_t *self, int itf);

#endif // MICROPY_INCLUDED_STM32_CYW43_H
