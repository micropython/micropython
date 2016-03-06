/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Chester Tseng
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
#ifndef OBJWIFI_H_
#define OBJWIFI_H_

#include <stdint.h>
#include <stdbool.h>

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "exception.h"

#include "wifi_conf.h"
#include "lwip_netconf.h"

#define WIFI_INTERFACE_NUMBER   2
#define WIFI_MIN_SSID_LEN       3
#define WIFI_MAX_SSID_LEN       32
#define WIFI_MAC_LEN            6
#define WIFI_KEY_LEN            65
#define WIFI_MAX_SCAN_NETWORKS  50

void wifi_init0(void);

extern const mp_obj_type_t wifi_type;

typedef struct {
    mp_obj_base_t    base;
    uint8_t          idx;                       // Reserved for RTl8195A cocurrency mode
    uint8_t          mode;                      // WiFi mode, STA / AP / MIX
    uint8_t          security_type;             // WiFi security mode, WPA2 / WEP ...
    uint8_t          channel;                   // WiFi channel 0 ~ 11
    uint8_t          mac[WIFI_MAC_LEN];         // WiFi mac address
    uint8_t          ssid[WIFI_MAX_SSID_LEN+1]; // WiFi ssid in *AP mode*
    uint8_t          bssid[WIFI_MAC_LEN];       // WiFi bssid in *AP mode*
    uint8_t          key[WIFI_KEY_LEN];         // WiFi key in *AP mode*
} wifi_obj_t;


#endif  // OBJWIFI_H_
