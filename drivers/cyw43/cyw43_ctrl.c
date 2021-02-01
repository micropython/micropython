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

#include "py/mphal.h"
#include "drivers/cyw43/cyw43.h"
#include "pendsv.h"
#include "sdio.h"

#define CYW_ENTER MICROPY_PY_LWIP_ENTER
#define CYW_EXIT MICROPY_PY_LWIP_EXIT

#ifdef pyb_pin_WL_HOST_WAKE
#define USE_SDIOIT (0)
#else
#define USE_SDIOIT (1)
#endif

#define CYW43_SLEEP_MAX (50)

#define WIFI_JOIN_STATE_ACTIVE  (0x0001)
#define WIFI_JOIN_STATE_FAIL    (0x0002)
#define WIFI_JOIN_STATE_NONET   (0x0003)
#define WIFI_JOIN_STATE_BADAUTH (0x0004)
#define WIFI_JOIN_STATE_AUTH    (0x0200)
#define WIFI_JOIN_STATE_LINK    (0x0400)
#define WIFI_JOIN_STATE_KEYED   (0x0800)
#define WIFI_JOIN_STATE_ALL     (0x0e01)

cyw43_t cyw43_state;
void (*cyw43_poll)(void);
uint32_t cyw43_sleep;

STATIC void cyw43_poll_func(void);
STATIC void cyw43_wifi_ap_init(cyw43_t *self);
STATIC void cyw43_wifi_ap_set_up(cyw43_t *self, bool up);

static inline uint32_t cyw43_get_be16(const uint8_t *buf) {
    return buf[0] << 8 | buf[1];
}

static inline uint32_t cyw43_get_be32(const uint8_t *buf) {
    return buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3];
}

static inline void cyw43_delay_ms(uint32_t ms) {
    mp_hal_delay_ms(ms);
}

/*******************************************************************************/
// Initialisation and polling

void cyw43_init(cyw43_t *self) {
    #ifdef pyb_pin_WL_HOST_WAKE
    mp_hal_pin_config(pyb_pin_WL_HOST_WAKE, MP_HAL_PIN_MODE_INPUT, MP_HAL_PIN_PULL_NONE, 0);
    #endif
    mp_hal_pin_config(pyb_pin_WL_REG_ON, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0);
    mp_hal_pin_low(pyb_pin_WL_REG_ON);
    #ifdef pyb_pin_WL_RFSW_VDD
    mp_hal_pin_config(pyb_pin_WL_RFSW_VDD, MP_HAL_PIN_MODE_OUTPUT, MP_HAL_PIN_PULL_NONE, 0); // RF-switch power
    mp_hal_pin_low(pyb_pin_WL_RFSW_VDD);
    #endif

    cyw43_ll_init(&self->cyw43_ll, self);

    self->itf_state = 0;
    self->wifi_scan_state = 0;
    self->wifi_join_state = 0;
    self->pend_disassoc = false;
    self->pend_rejoin= false;
    self->pend_rejoin_wpa = false;
    self->ap_channel = 3;
    self->ap_ssid_len = 0;
    self->ap_key_len = 0;

    cyw43_poll = NULL;
}

void cyw43_deinit(cyw43_t *self) {
    CYW_ENTER

    cyw43_ll_bus_sleep(&self->cyw43_ll, true);
    cyw43_delay_ms(2);
    cyw43_tcpip_deinit(self, 0);
    cyw43_tcpip_deinit(self, 1);

    self->itf_state = 0;

    // Disable async polling
    SDMMC1->MASK &= ~SDMMC_MASK_SDIOITIE;
    cyw43_poll = NULL;

    #ifdef pyb_pin_WL_RFSW_VDD
    // Turn the RF-switch off
    mp_hal_pin_low(pyb_pin_WL_RFSW_VDD);
    #endif

    // Power down the WL chip and the SDIO bus
    mp_hal_pin_low(pyb_pin_WL_REG_ON);
    sdio_deinit();

    CYW_EXIT
}

STATIC int cyw43_ensure_up(cyw43_t *self) {
    if (cyw43_poll != NULL) {
        cyw43_ll_bus_sleep(&self->cyw43_ll, false);
        return 0;
    }

    CYW_ENTER

    // Disable the netif if it was previously up
    cyw43_tcpip_deinit(self, CYW43_ITF_STA);
    cyw43_tcpip_deinit(self, CYW43_ITF_AP);
    self->itf_state = 0;

    // Reset and power up the WL chip
    mp_hal_pin_low(pyb_pin_WL_REG_ON);
    cyw43_delay_ms(20);
    mp_hal_pin_high(pyb_pin_WL_REG_ON);
    cyw43_delay_ms(50);

    // Initialise SDIO bus
    // IRQ priority only needs to be higher than CYW_ENTER/EXIT protection (PENDSV)
    sdio_init(NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 14, 0));

    // Initialise the low-level driver
    uint8_t mac[6];
    mp_hal_get_mac(MP_HAL_MAC_WLAN0, mac);
    int ret = cyw43_ll_bus_init(&self->cyw43_ll, mac);

    if (ret != 0) {
        CYW_EXIT
        return ret;
    }

    // Enable async events from low-level driver
    cyw43_sleep = CYW43_SLEEP_MAX;
    cyw43_poll = cyw43_poll_func;
    #if USE_SDIOIT
    SDMMC1->MASK |= SDMMC_MASK_SDIOITIE;
    #else
    extern void extint_set(const pin_obj_t *pin, uint32_t mode);
    extint_set(pyb_pin_WL_HOST_WAKE, GPIO_MODE_IT_FALLING);
    #endif

    CYW_EXIT

    return ret;
}

// This function must always be executed at the level where CYW_ENTER is effectively active
STATIC void cyw43_poll_func(void) {
    if (cyw43_poll == NULL) {
        // Poll scheduled during deinit, just ignore it
        return;
    }

    cyw43_t *self = &cyw43_state;
    cyw43_ll_process_packets(&self->cyw43_ll);

    if (self->pend_disassoc) {
        self->pend_disassoc = false;
        cyw43_ll_ioctl(&self->cyw43_ll, CYW43_IOCTL_SET_DISASSOC, 0, NULL, CYW43_ITF_STA);
    }

    if (self->pend_rejoin_wpa) {
        self->pend_rejoin_wpa = false;
        cyw43_ll_wifi_set_wpa_auth(&self->cyw43_ll);
    }

    if (self->pend_rejoin) {
        self->pend_rejoin = false;
        cyw43_ll_wifi_rejoin(&self->cyw43_ll);
        self->wifi_join_state = WIFI_JOIN_STATE_ACTIVE;
    }

    if (cyw43_sleep == 0) {
        cyw43_ll_bus_sleep(&self->cyw43_ll, true);
        #if !USE_SDIOIT
        sdio_deinit(); // save power while WLAN bus sleeps
        #endif
    }

    #if USE_SDIOIT
    SDMMC1->MASK |= SDMMC_MASK_SDIOITIE;
    #endif
}

/*******************************************************************************/
// Callback interface to low-level driver

int cyw43_cb_read_host_interrupt_pin(void *cb_data) {
    #ifdef pyb_pin_WL_HOST_WAKE
    return mp_hal_pin_read(pyb_pin_WL_HOST_WAKE);
    #else
    return mp_hal_pin_read(pyb_pin_WL_SDIO_1);
    #endif
}

void cyw43_cb_ensure_awake(void *cb_data) {
    cyw43_sleep = CYW43_SLEEP_MAX;
    #if !USE_SDIOIT
    if (__HAL_RCC_SDMMC1_IS_CLK_DISABLED()) {
        __HAL_RCC_SDMMC1_CLK_ENABLE(); // enable SDIO peripheral
        sdio_enable_high_speed_4bit();
    }
    #endif
}

STATIC const char *cyw43_async_event_name_table[89] = {
    [0 ... 88] = NULL,
    [CYW43_EV_SET_SSID] = "SET_SSID",
    [CYW43_EV_JOIN] = "JOIN",
    [CYW43_EV_AUTH] = "AUTH",
    [CYW43_EV_DEAUTH_IND] = "DEAUTH_IND",
    [CYW43_EV_ASSOC] = "ASSOC",
    [CYW43_EV_DISASSOC] = "DISASSOC",
    [CYW43_EV_DISASSOC_IND] = "DISASSOC_IND",
    [CYW43_EV_LINK] = "LINK",
    [CYW43_EV_PSK_SUP] = "PSK_SUP",
    [CYW43_EV_ESCAN_RESULT] = "ESCAN_RESULT",
    [CYW43_EV_CSA_COMPLETE_IND] = "CSA_COMPLETE_IND",
    [CYW43_EV_ASSOC_REQ_IE] = "ASSOC_REQ_IE",
    [CYW43_EV_ASSOC_RESP_IE] = "ASSOC_RESP_IE",
};

STATIC void cyw43_dump_async_event(const cyw43_async_event_t *ev) {
    printf("[% 8d] ASYNC(%04x,",
        (int)mp_hal_ticks_ms(),
        (unsigned int)ev->flags
    );
    if (ev->event_type < MP_ARRAY_SIZE(cyw43_async_event_name_table)
        && cyw43_async_event_name_table[ev->event_type] != NULL) {
        printf("%s", cyw43_async_event_name_table[ev->event_type]);
    } else {
        printf("%u", (unsigned int)ev->event_type);
    }
    printf(",%u,%u,%u)\n",
        (unsigned int)ev->status,
        (unsigned int)ev->reason,
        (unsigned int)ev->interface
    );
}

void cyw43_cb_process_async_event(void *cb_data, const cyw43_async_event_t *ev) {
    cyw43_t *self = cb_data;

    if (self->trace_flags & CYW43_TRACE_ASYNC_EV) {
        cyw43_dump_async_event(ev);
    }

    if (ev->event_type == CYW43_EV_ESCAN_RESULT && self->wifi_scan_state == 1) {
        // Escan result event
        if (ev->status == 8) {
            // Partial result
            int ret = self->wifi_scan_cb(self->wifi_scan_env, &ev->u.scan_result);
            if (ret != 0) {
                // TODO need to abort scan, or just ignore any more results
            }
        } else if (ev->status == 0) {
            // Scan complete
            self->wifi_scan_state = 2;
        }

    } else if (ev->event_type == CYW43_EV_DISASSOC) {
        cyw43_tcpip_set_link_down(self, CYW43_ITF_STA);
        self->wifi_join_state = 0x0000;

    /*
    } else if (ev->event_type == CYW43_EV_DISASSOC_IND) {
        if (ev->interface == CYW43_ITF_AP) {
            // Station disassociated with our AP, let DHCP server know so it can free the IP address
            dhcp_server_disassoc(&self->dhcp_server, buf + 24);
        }
    */

    // WiFi join events
    } else if (ev->event_type == CYW43_EV_PRUNE) {
        if (ev->status == 0 && ev->reason == 8) {
            // RSN mismatch, retry join with WPA auth
            self->pend_rejoin = true;
            self->pend_rejoin_wpa = true;
            pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll_func);
        }
    } else if (ev->event_type == CYW43_EV_SET_SSID) {
        if (ev->status == 0) {
            // Success setting SSID
        } else if (ev->status == 3 && ev->reason == 0) {
            self->wifi_join_state = WIFI_JOIN_STATE_NONET;
            // No matching SSID found (could be out of range, or down)
        } else {
            // Other failure setting SSID
            self->wifi_join_state = WIFI_JOIN_STATE_FAIL;
        }
    } else if (ev->event_type == CYW43_EV_AUTH) {
        if (ev->status == 0) {
            self->wifi_join_state |= WIFI_JOIN_STATE_AUTH;
        } else if (ev->status == 6) {
            // Unsolicited auth packet, ignore it
        } else {
            // Cannot authenticate
            self->wifi_join_state = WIFI_JOIN_STATE_BADAUTH;
        }
    } else if (ev->event_type == CYW43_EV_DEAUTH_IND) {
        if (ev->status == 0 && ev->reason == 2) {
            // Deauth, probably because password was wrong; disassociate
            self->pend_disassoc = true;
            pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll_func);
        }
    } else if (ev->event_type == CYW43_EV_LINK) {
        if (ev->status == 0) {
            if (ev->flags & 1) {
                // Link is up
                if (ev->interface == CYW43_ITF_STA) {
                    self->wifi_join_state |= WIFI_JOIN_STATE_LINK;
                } else {
                    cyw43_tcpip_set_link_up(self, ev->interface);
                }
            } else {
                // Link is down
                cyw43_tcpip_set_link_down(self, ev->interface);
            }
        }
    } else if (ev->event_type == CYW43_EV_PSK_SUP) {
        if (ev->status == 6) { // WLC_SUP_KEYED
            self->wifi_join_state |= WIFI_JOIN_STATE_KEYED;
        } else if ((ev->status == 4 || ev->status == 8 || ev->status == 11) && ev->reason == 15) {
            // Timeout waiting for key exchange M1/M3/G1
            // Probably at edge of the cell, retry
            self->pend_rejoin = true;
            pendsv_schedule_dispatch(PENDSV_DISPATCH_CYW43, cyw43_poll_func);
        } else {
            // PSK_SUP failure
            self->wifi_join_state = WIFI_JOIN_STATE_BADAUTH;
        }
    }

    if (self->wifi_join_state == WIFI_JOIN_STATE_ALL) {
        // STA connected
        self->wifi_join_state = WIFI_JOIN_STATE_ACTIVE;
        cyw43_tcpip_set_link_up(self, CYW43_ITF_STA);
    }
}

/*******************************************************************************/
// Ioctl and Ethernet interface

int cyw43_ioctl(cyw43_t *self, uint32_t cmd, size_t len, uint8_t *buf, uint32_t iface) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return ret;
    }

    CYW_ENTER
    ret = cyw43_ll_ioctl(&self->cyw43_ll, cmd, len, buf, iface);
    CYW_EXIT

    return ret;
}

int cyw43_send_ethernet(cyw43_t *self, int itf, size_t len, const void *buf, bool is_pbuf) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return ret;
    }

    CYW_ENTER
    ret = cyw43_ll_send_ethernet(&self->cyw43_ll, itf, len, buf, is_pbuf);
    CYW_EXIT

    return ret;
}

/*******************************************************************************/
// WiFi control

STATIC int cyw43_wifi_on(cyw43_t *self, uint32_t country) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return ret;
    }

    #ifdef pyb_pin_WL_RFSW_VDD
    // Turn the RF-switch on
    mp_hal_pin_high(pyb_pin_WL_RFSW_VDD);
    #endif

    CYW_ENTER
    ret = cyw43_ll_wifi_on(&self->cyw43_ll, country);
    CYW_EXIT

    return ret;
}

int cyw43_wifi_pm(cyw43_t *self, uint32_t pm_in) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return ret;
    }

    // pm_in: 0x00adbrrm
    uint32_t pm = pm_in & 0xf;
    uint32_t pm_sleep_ret = (pm_in >> 4) & 0xff;
    uint32_t li_bcn = (pm_in >> 12) & 0xf;
    uint32_t li_dtim = (pm_in >> 16) & 0xf;
    uint32_t li_assoc = (pm_in >> 20) & 0xf;

    CYW_ENTER
    ret = cyw43_ll_wifi_pm(&self->cyw43_ll, pm, pm_sleep_ret, li_bcn, li_dtim, li_assoc);
    CYW_EXIT

    return ret;
}

int cyw43_wifi_get_mac(cyw43_t *self, int itf, uint8_t mac[6]) {
    mp_hal_get_mac(MP_HAL_MAC_WLAN0, &mac[0]);
    return 0;
}

#define MAKE_COUNTRY(a, b, rev) ((a) | (b) << 8 | (rev) << 16)

void cyw43_wifi_set_up(cyw43_t *self, int itf, bool up) {
    if (up) {
        if (self->itf_state == 0) {
            uint32_t country;
            extern char pyb_country_code[2];
            if (pyb_country_code[0] == '\0' || pyb_country_code[1] == '\0') {
                country = MAKE_COUNTRY('X', 'X', 17); // default to world-wide (passive ch 12-14)
            } else {
                country = MAKE_COUNTRY(pyb_country_code[0], pyb_country_code[1], 0);
            }
            if (cyw43_wifi_on(self, country) != 0) {
                return;
            }
            cyw43_wifi_pm(self, 10 << 20 | 1 << 16 | 1 << 12 | 20 << 4 | 2);
        }
        if (itf == CYW43_ITF_AP) {
            cyw43_wifi_ap_init(self);
            cyw43_wifi_ap_set_up(self, true);
        }
        if ((self->itf_state & (1 << itf)) == 0) {
            CYW_ENTER
            cyw43_tcpip_deinit(self, itf);
            cyw43_tcpip_init(self, itf);
            self->itf_state |= 1 << itf;
            CYW_EXIT
        }
    } else {
        if (itf == CYW43_ITF_AP) {
            cyw43_wifi_ap_set_up(self, false);
        }
    }
}

int cyw43_wifi_scan(cyw43_t *self, cyw43_wifi_scan_options_t *opts, void *env, int (*result_cb)(void*, const cyw43_ev_scan_result_t*)) {
    if (self->itf_state == 0) {
        return -1;
    }

    cyw43_ensure_up(self);

    CYW_ENTER

    // Set state and callback data
    self->wifi_scan_state = 1;
    self->wifi_scan_env = env;
    self->wifi_scan_cb = result_cb;

    // Start the scan
    int ret = cyw43_ll_wifi_scan(&self->cyw43_ll, opts);

    CYW_EXIT

    return ret;
}

int cyw43_wifi_link_status(cyw43_t *self, int itf) {
    if (itf == CYW43_ITF_STA) {
        int s = self->wifi_join_state & 0xf;
        if (s == WIFI_JOIN_STATE_ACTIVE) {
            return CYW43_LINK_JOIN;
        } else if (s == WIFI_JOIN_STATE_FAIL) {
            return CYW43_LINK_FAIL;
        } else if (s == WIFI_JOIN_STATE_NONET) {
            return CYW43_LINK_NONET;
        } else if (s == WIFI_JOIN_STATE_BADAUTH) {
            return CYW43_LINK_BADAUTH;
        } else {
            return CYW43_LINK_DOWN;
        }
    } else {
        return CYW43_LINK_DOWN;
    }
}

/*******************************************************************************/
// WiFi STA

int cyw43_wifi_join(cyw43_t *self, size_t ssid_len, const uint8_t *ssid, size_t key_len, const uint8_t *key, uint32_t auth_type, const uint8_t *bssid, uint32_t channel) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return ret;
    }

    CYW_ENTER

    ret = cyw43_ll_wifi_join(&self->cyw43_ll, ssid_len, ssid, key_len, key, auth_type, bssid, channel);

    if (ret == 0) {
        // Wait for responses: EV_AUTH, EV_LINK, EV_SET_SSID, EV_PSK_SUP
        // Will get EV_DEAUTH_IND if password is invalid
        self->wifi_join_state = WIFI_JOIN_STATE_ACTIVE;

        if (auth_type == 0) {
            // For open security we don't need EV_PSK_SUP, so set that flag indicator now
            self->wifi_join_state |= WIFI_JOIN_STATE_KEYED;
        }
    }

    CYW_EXIT

    return ret;
}

int cyw43_wifi_leave(cyw43_t *self, int itf) {
    // Disassociate with SSID
    return cyw43_ioctl(self, CYW43_IOCTL_SET_DISASSOC, 0, NULL, itf);
}

/*******************************************************************************/
// WiFi AP

STATIC void cyw43_wifi_ap_init(cyw43_t *self) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return;
    }

    CYW_ENTER
    cyw43_ll_wifi_ap_init(&self->cyw43_ll, self->ap_ssid_len, self->ap_ssid, self->ap_auth, self->ap_key_len, self->ap_key, self->ap_channel);
    CYW_EXIT
}

STATIC void cyw43_wifi_ap_set_up(cyw43_t *self, bool up) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return;
    }

    CYW_ENTER
    cyw43_ll_wifi_ap_set_up(&self->cyw43_ll, up);
    CYW_EXIT
}

void cyw43_wifi_ap_get_stas(cyw43_t *self, int *num_stas, uint8_t *macs) {
    int ret = cyw43_ensure_up(self);
    if (ret) {
        return;
    }

    CYW_ENTER
    cyw43_ll_wifi_ap_get_stas(&self->cyw43_ll, num_stas, macs);
    CYW_EXIT
}
