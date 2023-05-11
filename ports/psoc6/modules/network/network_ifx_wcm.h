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

/*******************************************************************************
 * Macros
 ******************************************************************************/

/* Provide the value of SSID which should be used to filter the scan results.*/
#define SCAN_FOR_SSID_VALUE                     "SSID"

/* Provide the value of the MAC address which should be used to filter the scan
 * results. For example, MAC Address: 12:34:56:78:9A:BC should be entered as
 * shown below.
 */
#define SCAN_FOR_MAC_ADDRESS                     0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC

/* Provide the value of the ISM band (2.4 GHz/ 5 GHz/ both) which should be used
 * to filter the scan results. The valid values are provided in the enumeration
 * cy_wcm_wifi_band_t in cy_wcm.h.
 * Note: CY8CPROTO-062-4343W is a single band device and can only scan for
 * networks in 2.4 GHz.
 */
#define SCAN_FOR_BAND_VALUE                     CY_WCM_WIFI_BAND_ANY

/* Provide the value of the RSSI range that should be used to filter the scan
 * results. The valid values are provided in the enumeration
 * cy_wcm_scan_rssi_range_t in cy_wcm.h.
 */
#define SCAN_FOR_RSSI_VALUE                     CY_WCM_SCAN_RSSI_EXCELLENT

/* The delay in milliseconds between successive scans.*/
#define SCAN_DELAY_MS                           (3000u)

#define SCAN_TASK_STACK_SIZE                    (4096u)
#define SCAN_TASK_PRIORITY                      (3u)

#define MAX_SECURITY_STRING_LENGTH              (15)

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

#define APP_INFO(x)           do { printf("\nInfo: "); printf x;} while (0);
#define ERR_INFO(x)           do { printf("\nError: "); printf x;} while (0);

/*******************************************************************************
 * Enumerations
 ******************************************************************************/
/* This enumeration contains the different types of scan filters available. They
 * are,
 * SCAN_FILTER_NONE: No scan filter.
 * SCAN_FILTER_SSID: The scan results are filtered by SSID.
 * SCAN_FILTER_MAC: The scan results are filtered by MAC address of the AP.
 * SCAN_FILTER_BAND: The scan results are filtered by the ISM band (2.4 or 5 GHz
 * or both) that the AP is occupying.
 * SCAN_FILTER_RSSI: The scan results are filtered by the RSSI strength.
 * SCAN_FILTER_INVALID: Invalid scan filter.
 */
enum scan_filter_mode
{
    SCAN_FILTER_NONE = 0,
    SCAN_FILTER_SSID,
    SCAN_FILTER_MAC,
    SCAN_FILTER_BAND,
    SCAN_FILTER_RSSI,
    SCAN_FILTER_INVALID
};

extern const mp_obj_type_t mp_network_ifx_wcm_type;

void network_init(void);
void network_deinit(void);
void error_handler(cy_rslt_t result, char *message);

#endif // MICROPY_INCLUDED_EXTMOD_NETWORK_IFX_WCM_H
