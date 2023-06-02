/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
 * Copyright (c) 2023 Infineon Technologies AG
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
#ifndef MICROPY_INCLUDED_EXTMOD_NETWORK_IFX_WCM_H
#define MICROPY_INCLUDED_EXTMOD_NETWORK_IFX_WCM_H

#include "FreeRTOS.h"
#include "task.h"
#include "cy_wcm.h"
#include <stdio.h>


#define SECURITY_OPEN                           "OPEN"
#define SECURITY_WEP_PSK                        "WEP-PSK"
#define SECURITY_WEP_SHARED                     "WEP-SHARED"
#define SECURITY_WEP_TKIP_PSK                   "WEP-TKIP-PSK"
#define SECURITY_WPA_TKIP_PSK                   "WPA-TKIP-PSK"
#define SECURITY_WPA_AES_PSK                    "WPA-AES-PSK"
#define SECURITY_WPA_MIXED_PSK                  "WPA-MIXED-PSK"
#define SECURITY_WPA2_AES_PSK                   "WPA2-AES-PSK"
#define SECURITY_WPA2_TKIP_PSK                  "WPA2-TKIP-PSK"
#define SECURITY_WPA2_MIXED_PSK                 "WPA2-MIXED-PSK"
#define SECURITY_WPA2_FBT_PSK                   "WPA2-FBT-PSK"
#define SECURITY_WPA3_SAE                       "WPA3-SAE"
#define SECURITY_WPA2_WPA_AES_PSK               "WPA2-WPA-AES-PSK"
#define SECURITY_WPA2_WPA_MIXED_PSK             "WPA2-WPA-MIXED-PSK"
#define SECURITY_WPA3_WPA2_PSK                  "WPA3-WPA2-PSK"
#define SECURITY_WPA_TKIP_ENT                   "WPA-TKIP-ENT"
#define SECURITY_WPA_AES_ENT                    "WPA-AES-ENT"
#define SECURITY_WPA_MIXED_ENT                  "WPA-MIXED-ENT"
#define SECURITY_WPA2_TKIP_ENT                  "WPA2-TKIP-ENT"
#define SECURITY_WPA2_AES_ENT                   "WPA2-AES-ENT"
#define SECURITY_WPA2_MIXED_ENT                 "WPA2-MIXED-ENT"
#define SECURITY_WPA2_FBT_ENT                   "WPA2-FBT-ENT"
#define SECURITY_IBSS_OPEN                      "IBSS-OPEN"
#define SECURITY_WPS_SECURE                     "WPS-SECURE"
#define SECURITY_UNKNOWN                        "UNKNOWN"

#define PRINT_SCAN_TEMPLATE()                   printf("\n----------------------------------------------------------------------------------------------------\n" \
    "  #                  SSID                  RSSI   Channel       MAC Address              Security\n" \
    "----------------------------------------------------------------------------------------------------\n");

extern const mp_obj_type_t mp_network_ifx_wcm_type;

void network_init(void);
void network_deinit(void);

#endif // MICROPY_INCLUDED_EXTMOD_NETWORK_IFX_WCM_H
