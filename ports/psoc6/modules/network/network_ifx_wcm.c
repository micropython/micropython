/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK_IFX_WCM

#include "lwip/netif.h"
#include "lwip/dns.h"
#include "whd.h"
#include "whd_network_types.h"
#include "whd_wlioctl.h"
#include "network_ifx_wcm.h"
#include "whd_wifi_api.h"
#include "extmod/modnetwork.h"
#include "cy_wcm.h"
#include "cy_nw_helper.h"
#include "mplogger.h"

#include "FreeRTOS.h"
#include "shared/netutils/netutils.h"

extern uint8_t cy_wcm_is_ap_up();

// Function prototypes
static void network_ifx_wcm_scan_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status);
static void print_scan_result(cy_wcm_scan_result_t *result);

cy_wcm_ip_address_t ip_address;
cy_wcm_ip_address_t net_mask_addr;
cy_wcm_ip_address_t gateway_addr;
cy_wcm_ip_setting_t ap_ip;

cy_wcm_scan_filter_t scan_filter;
uint32_t num_scan_result;
enum scan_filter_mode scan_filter_mode_select = SCAN_FILTER_NONE;
void *ntwk_scan_result;
mp_obj_t scan_list;


#define NETWORK_WLAN_DEFAULT_SSID       "mpy-psoc6-wlan"
#define NETWORK_WLAN_DEFAULT_PASSWORD   "mpy_PSOC6_w3lc0me!"
#define NETWORK_WLAN_DEFAULT_SECURITY   CY_WCM_SECURITY_WPA2_AES_PSK
#define NETWORK_WLAN_DEFAULT_CHANNEL    9

#define NETWORK_WLAN_AP_IP "192.168.0.1"
#define NETWORK_WLAN_AP_GATEWAY_IP "192.168.0.1"
#define NETWORK_WLAN_AP_NETMASK_IP  "255.255.255.0"


typedef struct
{
    cy_wcm_ap_config_t ap_config;
} network_ifx_wcm_ap_obj_t;

typedef struct
{
    uint32_t var;
} network_ifx_wcm_sta_obj_t;

typedef union {
    network_ifx_wcm_ap_obj_t ap_obj;
    network_ifx_wcm_sta_obj_t sta_obj;
} itf_obj_t;

typedef struct _network_ifx_wcm_obj_t {
    mp_obj_base_t base;
    cy_wcm_interface_t itf;
    itf_obj_t itf_obj;
    cy_wcm_ip_setting_t ip_config;
} network_ifx_wcm_obj_t;

#define MAX_WHD_INTERFACE  (2)

extern whd_interface_t whd_ifs[MAX_WHD_INTERFACE];

STATIC network_ifx_wcm_obj_t network_ifx_wcm_wl_sta = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_STA };
STATIC network_ifx_wcm_obj_t network_ifx_wcm_wl_ap = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_AP };

#define wcm_assert_raise(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

// Error handling method
void error_handler(cy_rslt_t result, char *message) {
    if (NULL != message) {
        ERR_INFO(("%lu", result));
        ERR_INFO(("%s", message));
    }
    __disable_irq();
    CY_ASSERT(0);
}

// Based on the scan result, get micropython defined equivalent security type (possible value 0-4, extended till 7 to include all cases) and security string (mapped to IFX stack)
static char *get_security_string_and_type(cy_wcm_scan_result_t *result, uint8_t *security_type) {
    /* Convert the security type of the scan result to the corresponding
     * security string
     */
    char *security_type_string;
    switch (result->security)
    {
        case CY_WCM_SECURITY_OPEN:
            security_type_string = SECURITY_OPEN;
            *security_type = 0;
            break;
        case CY_WCM_SECURITY_WEP_PSK:
            security_type_string = SECURITY_WEP_PSK;
            *security_type = 1;
            break;
        case CY_WCM_SECURITY_WEP_SHARED:
            security_type_string = SECURITY_WEP_SHARED;
            *security_type = 1;
            break;
        case CY_WCM_SECURITY_WPA_TKIP_PSK:
            security_type_string = SECURITY_WEP_TKIP_PSK;
            *security_type = 1;
            break;
        case CY_WCM_SECURITY_WPA_AES_PSK:
            security_type_string = SECURITY_WPA_AES_PSK;
            *security_type = 4;
            break;
        case CY_WCM_SECURITY_WPA_MIXED_PSK:
            security_type_string = SECURITY_WPA_MIXED_PSK;
            *security_type = 4;
            break;
        case CY_WCM_SECURITY_WPA2_AES_PSK:
            security_type_string = SECURITY_WPA2_AES_PSK;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_TKIP_PSK:
            security_type_string = SECURITY_WPA2_TKIP_PSK;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_MIXED_PSK:
            security_type_string = SECURITY_WPA2_MIXED_PSK;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_FBT_PSK:
            security_type_string = SECURITY_WPA2_FBT_PSK;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA3_SAE:
            security_type_string = SECURITY_WPA3_SAE;
            *security_type = 5; // Not supported value in mpy. New define!
            break;
        case CY_WCM_SECURITY_WPA3_WPA2_PSK:
            security_type_string = SECURITY_WPA3_WPA2_PSK;
            *security_type = 5; // Not supported value in mpy. New define!
            break;
        case CY_WCM_SECURITY_IBSS_OPEN:
            security_type_string = SECURITY_IBSS_OPEN;
            *security_type = 0;
            break;
        case CY_WCM_SECURITY_WPS_SECURE:
            security_type_string = SECURITY_WPS_SECURE;
            *security_type = 6; // Not supported value in mpy. New define!
            break;
        case CY_WCM_SECURITY_UNKNOWN:
            security_type_string = SECURITY_UNKNOWN;
            *security_type = 7; // Not supported value in mpy. New define!
            break;
        case CY_WCM_SECURITY_WPA2_WPA_AES_PSK:
            security_type_string = SECURITY_WPA2_WPA_AES_PSK;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK:
            security_type_string = SECURITY_WPA2_WPA_MIXED_PSK;
            *security_type = 4;
            break;
        case CY_WCM_SECURITY_WPA_TKIP_ENT:
            security_type_string = SECURITY_WPA_TKIP_ENT;
            *security_type = 2;
            break;
        case CY_WCM_SECURITY_WPA_AES_ENT:
            security_type_string = SECURITY_WPA_AES_ENT;
            *security_type = 2;
            break;
        case CY_WCM_SECURITY_WPA_MIXED_ENT:
            security_type_string = SECURITY_WPA_MIXED_ENT;
            *security_type = 2;
            break;
        case CY_WCM_SECURITY_WPA2_TKIP_ENT:
            security_type_string = SECURITY_WPA2_TKIP_ENT;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_AES_ENT:
            security_type_string = SECURITY_WPA2_AES_ENT;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_MIXED_ENT:
            security_type_string = SECURITY_WPA2_MIXED_ENT;
            *security_type = 3;
            break;
        case CY_WCM_SECURITY_WPA2_FBT_ENT:
            security_type_string = SECURITY_WPA2_FBT_ENT;
            *security_type = 3;
            break;
        default:
            security_type_string = SECURITY_UNKNOWN;
            *security_type = 7; // Not supported value in mpy. New define!
            break;
    }
    return security_type_string;
}

// Helper function to print scan results
static void print_scan_result(cy_wcm_scan_result_t *result) {
    uint8_t security_type = 0;

    char *security_type_string = get_security_string_and_type(result, &security_type);

    printf(" %2" PRIu32 "   %-32s     %4d     %2d      %02X:%02X:%02X:%02X:%02X:%02X         %-15s\n",
        num_scan_result, result->SSID,
        result->signal_strength, result->channel, result->BSSID[0], result->BSSID[1],
        result->BSSID[2], result->BSSID[3], result->BSSID[4], result->BSSID[5],
        security_type_string);
}

// Callback function for scan method. After each scan result, the scan callback is executed.
static void network_ifx_wcm_scan_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status) {
    scan_list = MP_OBJ_FROM_PTR(ntwk_scan_result);
    char bssid_buf[24];
    uint8_t hidden_status = 1; // HIDDEN
    uint8_t security_type = 0; // OPEN

    if (status == CY_WCM_SCAN_INCOMPLETE) {
        num_scan_result++;

        // Populate BSSID buffer
        snprintf(bssid_buf, sizeof(bssid_buf), "%u.%u.%u.%u.%u.%u", result_ptr->BSSID[0], result_ptr->BSSID[1], result_ptr->BSSID[2], result_ptr->BSSID[3], result_ptr->BSSID[4], result_ptr->BSSID[5]);

        // Get the network status : hidden(1) or open(0)
        if (strlen((const char *)result_ptr->SSID) != 0) {
            hidden_status = 0;
        }

        // Get security type as mapped in micropython function description
        get_security_string_and_type(result_ptr, &security_type);

        mp_obj_t tuple[6] = {
            mp_obj_new_bytes(result_ptr->SSID, strlen((const char *)result_ptr->SSID)),
            mp_obj_new_bytes((const byte *)bssid_buf, strlen((const char *)bssid_buf)),
            MP_OBJ_NEW_SMALL_INT(result_ptr->channel),
            MP_OBJ_NEW_SMALL_INT(result_ptr->signal_strength),
            MP_OBJ_NEW_SMALL_INT(security_type),
            MP_OBJ_NEW_SMALL_INT(hidden_status)
        };
        mp_obj_list_append(scan_list, mp_obj_new_tuple(6, tuple));
        // print_scan_result(result_ptr);

    }

    if ((CY_WCM_SCAN_COMPLETE == status)) {
        /* Reset the number of scan results to 0 for the next scan.*/
        num_scan_result = 0;
    }
    return;
}

void network_deinit(void) {
    cy_rslt_t result = cy_wcm_deinit();
    if (result != CY_RSLT_SUCCESS) {
        // error_handler(result, "Failed to initialize Wi-Fi Connection Manager.\n");
        // mp_raise_ValueError(MP_ERROR_TEXT("wifi connection manager deinit error"));
    }
}

#define MAX_WIFI_RETRY_COUNT                (3u)
#define WIFI_CONN_RETRY_INTERVAL_MSEC       (100u)

// Network Access Point initialization with default network parameters
void network_ap_init() {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_ap_config_t *ap_conf = &(network_ifx_wcm_wl_ap.itf_obj.ap_obj.ap_config);
    cy_wcm_ip_setting_t *ap_ip_settings = &(network_ifx_wcm_wl_ap.itf_obj.ap_obj.ap_config.ip_settings);

    ap_conf->channel = NETWORK_WLAN_DEFAULT_CHANNEL;
    memcpy(ap_conf->ap_credentials.SSID, NETWORK_WLAN_DEFAULT_SSID, strlen(NETWORK_WLAN_DEFAULT_SSID) + 1);
    memcpy(ap_conf->ap_credentials.password, NETWORK_WLAN_DEFAULT_PASSWORD, strlen(NETWORK_WLAN_DEFAULT_PASSWORD) + 1);
    ap_conf->ap_credentials.security = NETWORK_WLAN_DEFAULT_SECURITY;

    cy_wcm_set_ap_ip_setting(ap_ip_settings, NETWORK_WLAN_AP_IP, NETWORK_WLAN_AP_NETMASK_IP, NETWORK_WLAN_AP_GATEWAY_IP, CY_WCM_IP_VER_V4);
    wcm_assert_raise("network ap ip setting error (code: %d)", ret);
}

// Network Initialization function (called from main.c)
void network_init(void) {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_config_t wcm_config = { .interface = CY_WCM_INTERFACE_TYPE_AP_STA };

    ret = cy_wcm_init(&wcm_config);
    wcm_assert_raise("network init error (code: %d)", ret);

    network_ap_init();
}

// Print after constructor invoked
STATIC void network_ifx_wcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    // network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    //     TODO: Implement print with following parameters
    //       - link status (requires tcpip stack integration)
    //       - get and print ip address
    const char *status_str = "network status unknown. To be implemented in network.status().";

    /*if (status == CYW43_LINK_DOWN) {
        status_str = "down";
    } else if (status == CYW43_LINK_JOIN || status == CYW43_LINK_NOIP) {
        status_str = "join";
    } else if (status == CYW43_LINK_UP) {
        status_str = "up";
    } else if (status == CYW43_LINK_NONET) {
        status_str = "nonet";
    } else if (status == CYW43_LINK_BADAUTH) {
        status_str = "badauth";
    } else {
        status_str = "fail";
    }*/

    mp_printf(print, "<IFX WCM %s %u.%u.%u.%u>",
        status_str,
        0, // netif->ip_addr.addr & 0xff,
        0, // netif->ip_addr.addr >> 8 & 0xff,
        0, // netif->ip_addr.addr >> 16 & 0xff,
        0 // netif->ip_addr.addr >> 24
        );
}

STATIC mp_obj_t network_ifx_wcm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    if (n_args == 0 || mp_obj_get_int(args[0]) == MOD_NETWORK_STA_IF) {
        return MP_OBJ_FROM_PTR(&network_ifx_wcm_wl_sta);
    } else {
        return MP_OBJ_FROM_PTR(&network_ifx_wcm_wl_ap);
    }
    return mp_const_none;
}

STATIC mp_obj_t network_ifx_wcm_send_ethernet(mp_obj_t self_in, mp_obj_t buf_in) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    whd_buffer_t *whd_buff = (whd_buffer_t *)&buf;
    int ret = whd_network_send_ethernet_data(whd_ifs[self->itf], *whd_buff);
    wcm_assert_raise("network send ethernet error (code: %d)", ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(network_ifx_wcm_send_ethernet_obj, network_ifx_wcm_send_ethernet);

/*******************************************************************************/
// network API

STATIC mp_obj_t network_ifx_wcm_deinit(mp_obj_t self_in) {
    uint32_t ret = cy_wcm_deinit();
    wcm_assert_raise("network deinit error (code: %d)", ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_deinit_obj, network_ifx_wcm_deinit);

STATIC mp_obj_t network_ifx_wcm_active(size_t n_args, const mp_obj_t *args) {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
        if (n_args == 1) {
            return mp_obj_new_bool(cy_wcm_is_connected_to_ap());
        }
    } else if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
        if (n_args == 1) {
            return mp_obj_new_bool(cy_wcm_is_ap_up());
        } else {
            if (mp_obj_is_true(args[1])) {
                ret = cy_wcm_start_ap(&(self->itf_obj.ap_obj.ap_config));
                wcm_assert_raise("network ap active error (with code: %d)", ret);
            } else {
                ret = cy_wcm_stop_ap();
                wcm_assert_raise("network ap deactive error (with code: %d)", ret);
            }
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_active_obj, 1, 2, network_ifx_wcm_active);

STATIC mp_obj_t network_ifx_wcm_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_passive, ARG_ssid, ARG_essid, ARG_bssid, ARG_channels };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_passive, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_ssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_essid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_channels, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    const char *band_string[] =
    {
        [CY_WCM_WIFI_BAND_ANY] = "2.4 and 5 GHz",
        [CY_WCM_WIFI_BAND_2_4GHZ] = "2.4 GHz",
        [CY_WCM_WIFI_BAND_5GHZ] = "5 GHz"
    };
    cy_wcm_mac_t scan_for_mac_value = {SCAN_FOR_MAC_ADDRESS};
    cy_rslt_t result = CY_RSLT_SUCCESS;

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    switch (scan_filter_mode_select)
    {
        case SCAN_FILTER_NONE:
            // APP_INFO(("Scanning without any filter\n"));
            break;

        case SCAN_FILTER_SSID:
            APP_INFO(("Scanning for %s.\n", SCAN_FOR_SSID_VALUE));

            /* Configure the scan filter for SSID specified by
             * SCAN_FOR_SSID_VALUE.
             */
            scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_SSID;
            memcpy(scan_filter.param.SSID, SCAN_FOR_SSID_VALUE, sizeof(SCAN_FOR_SSID_VALUE));
            break;

        case SCAN_FILTER_RSSI:
            APP_INFO(("Scanning for RSSI > %d dBm.\n", SCAN_FOR_RSSI_VALUE));

            /* Configure the scan filter for RSSI range specified by
             * SCAN_FOR_RSSI_VALUE.
             */
            scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_RSSI;
            scan_filter.param.rssi_range = SCAN_FOR_RSSI_VALUE;
            break;

        case SCAN_FILTER_MAC:
            APP_INFO(("Scanning for %02X:%02X:%02X:%02X:%02X:%02X.\n", scan_for_mac_value[0], scan_for_mac_value[1], scan_for_mac_value[2], scan_for_mac_value[3], scan_for_mac_value[4], scan_for_mac_value[5]));

            /* Configure the scan filter for MAC specified by scan_for_mac_value
             */
            scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_MAC;
            memcpy(scan_filter.param.BSSID, &scan_for_mac_value, sizeof(scan_for_mac_value));
            break;

        case SCAN_FILTER_BAND:
            APP_INFO(("Scanning in %s band.\n", band_string[SCAN_FOR_BAND_VALUE]));

            /* Configure the scan filter for band specified by
             * SCAN_FOR_BAND_VALUE.
             */
            scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_BAND;
            scan_filter.param.band = SCAN_FOR_BAND_VALUE;
            break;

        default:
            break;
    }
    // PRINT_SCAN_TEMPLATE();
    mp_obj_t res = mp_obj_new_list(0, NULL);
    ntwk_scan_result = MP_OBJ_TO_PTR(res);

    if (SCAN_FILTER_NONE == scan_filter_mode_select) {
        result = cy_wcm_start_scan(network_ifx_wcm_scan_cb, NULL, NULL);
    } else {
        result = cy_wcm_start_scan(network_ifx_wcm_scan_cb, NULL, &scan_filter);
    }

    /* Wait for scan completion if scan was started successfully. The API
     * cy_wcm_start_scan doesn't wait for scan completion. If it is called
     * again when the scan hasn't completed, the API returns
     * CY_WCM_RESULT_SCAN_IN_PROGRESS.

     * Wait for 10s timeout
     */
    if (CY_RSLT_SUCCESS == result) {
        cyhal_system_delay_ms(10000);
    }

    /* Define PRINT_HEAP_USAGE using DEFINES variable in the Makefile. */
    // print_heap_usage("After scan results are printed to UART");
    return res;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_scan_obj, 1, network_ifx_wcm_scan);

/* WHD based implementation - Future use
STATIC mp_obj_t network_ifx_wcm_scan(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_passive, ARG_ssid, ARG_essid, ARG_bssid, ARG_channels };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_passive, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_ssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_essid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_channels, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // Deprecated kwarg
    if (args[ARG_essid].u_obj != mp_const_none) {
        args[ARG_ssid].u_obj = args[ARG_essid].u_obj;
    }

    // Extract scan type, if given.
    // Note: WC driver provides also prohibited channel,
    // currently not added to keep the interface as similar
    // to the cy43w network module implementation.
    whd_scan_type_t scan_type = WHD_SCAN_TYPE_PASSIVE;
    if (args[ARG_passive].u_bool) {
        scan_type = WHD_SCAN_TYPE_ACTIVE;
    }

    // Extract ssid, if given
    cy_wcm_ssid_t *ssid_ptr = NULL;
    cy_wcm_ssid_t ssid;


    if (args[ARG_ssid].u_obj != mp_const_none) {
        ssid_ptr = &ssid;
        mp_buffer_info_t ssid_arg;
        mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid_arg, MP_BUFFER_READ);
        ssid.length = MIN(ssid_arg.len, SSID_NAME_SIZE);
        memcpy(ssid.value, ssid_arg.buf, ssid.length);
    }

    // Extract bssid, if given
    cy_wcm_mac_t *bssid_ptr = NULL;
    cy_wcm_mac_t bssid;
    if (args[ARG_bssid].u_obj != mp_const_none) {
        bssid_ptr = &bssid;
        mp_buffer_info_t bssid_arg;
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid_arg, MP_BUFFER_READ);
        memcpy(bssid.octet, bssid_arg.buf, 6);
    }

    // Extract channel list, if provided
    uint16_t channel_list[sizeof(uint8_t)] = {0};
    if (args[ARG_channels].u_obj != mp_const_none) {
        mp_buffer_info_t channels_arg;
        mp_get_buffer_raise(args[ARG_channels].u_obj, &channels_arg, MP_BUFFER_READ);
        memcpy(channel_list, channels_arg.buf, channels_arg.len);
        channel_list[channels_arg.len] = 0; // The channel list array needs to be zero terminated.
    }

    // Note: Optional extended parameters are not provided.
    whd_scan_status = WHD_SCAN_INCOMPLETE;
    mp_obj_t res = mp_obj_new_list(0, NULL);
    uint32_t ret = whd_wifi_scan(whd_ifs[self->itf], scan_type, WHD_BSS_TYPE_ANY, ssid_ptr, bssid_ptr, channel_list, NULL, network_ifx_whd_scan_cb, &whd_scan_result, &res);
    wcm_assert_raise(ret);

    // Wait for scan to finish, with a 10s timeout
    uint32_t start = mp_hal_ticks_ms();
    while (whd_scan_status && (mp_hal_ticks_ms() - start < 10000)) {
        MICROPY_EVENT_POLL_HOOK
    }

    return res;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_whd_scan_obj, 1, network_ifx_whd_scan);
*/

STATIC mp_obj_t network_ifx_wcm_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_auth, ARG_security, ARG_bssid, ARG_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_auth, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };


    /* Maximum Wi-Fi re-connection limit. */
    #define MAX_WIFI_CONN_RETRIES             (120u)

    /* Wi-Fi re-connection time interval in milliseconds. */
    #define WIFI_CONN_RETRY_INTERVAL_MS       (5000)

    #define WIFI_CONNECTED                   (1lu << 1)

    /* Flag to denote initialization status of various operations. */
    // uint32_t status_flag;

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    cy_rslt_t result = CY_RSLT_SUCCESS;
    cy_wcm_connect_params_t connect_param;
    cy_wcm_ip_address_t ipaddress;
    memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));

    // Deprecated kwarg
    if (args[ARG_auth].u_int != -1) {
        args[ARG_security] = args[ARG_auth];
    }

    // Extract the SSID.
    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);
    // TODO: Check length cannot be more than 32 bytes. Limited by whd lib. Validate argument!
    memcpy(connect_param.ap_credentials.SSID, ssid.buf, ssid.len);

    // Extract the key, if given.
    mp_buffer_info_t key;
    key.buf = NULL;
    if (args[ARG_key].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_key].u_obj, &key, MP_BUFFER_READ);
    }
    memcpy(connect_param.ap_credentials.password, key.buf, key.len);

    // // Extract the BSSID, if given.
    // mp_buffer_info_t bssid;
    // bssid.buf = NULL;
    // if (args[ARG_bssid].u_obj != mp_const_none) {
    //     mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
    //     cy_wcm_mac_t wcm_bssid;

    //     // Check if the given bssid matches the interface bssid
    //     uint32_t ret = whd_wifi_get_bssid(itf, &wcm_bssid);
    //     wcm_assert_raise(ret);

    //     if (bssid.len != 6) {
    //         if (strncmp(bssid.buf, (char *)whd_bssid.octet, 6)) {
    //             mp_raise_ValueError(NULL);
    //         }
    //     }
    // }

    // Extract the security type, if given.
    cy_wcm_security_t auth_type;
    if (args[ARG_security].u_int == -1) {
        if (key.buf == NULL || key.len == 0) {
            auth_type = CY_WCM_SECURITY_OPEN;
        } else {
            auth_type = CY_WCM_SECURITY_WPA2_AES_PSK;
        }
    } else {
        auth_type = args[ARG_security].u_int;
    }
    connect_param.ap_credentials.security = auth_type;

    // // Extract channel, if given.
    // if (args[ARG_channel].u_int == -1) {

    //     uint32_t ret = whd_wifi_set_channel(itf, args[ARG_channel].u_int);
    //     wcm_assert_raise(ret);
    // }

    /* Connect to the Wi-Fi AP. */
    for (uint32_t retry_count = 0; retry_count < MAX_WIFI_CONN_RETRIES; retry_count++)
    {
        result = cy_wcm_connect_ap(&connect_param, &ipaddress);

        if (result == CY_RSLT_SUCCESS) {
            // printf("\nSuccessfully connected to Wi-Fi network '%s'.\n", connect_param.ap_credentials.SSID);
            return mp_const_none;
        }
        printf("Connection to Wi-Fi network failed with error code 0x%0X. Retrying in %d ms. Retries left: %d\n",
            (int)result, WIFI_CONN_RETRY_INTERVAL_MS, (int)(MAX_WIFI_CONN_RETRIES - retry_count - 1));

    }
    printf("\nExceeded maximum Wi-Fi connection attempts!\n");
    printf("Wi-Fi connection failed after retrying for %d mins\n\n",
        (int)(WIFI_CONN_RETRY_INTERVAL_MS * MAX_WIFI_CONN_RETRIES) / 60000u);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_connect_obj, 1, network_ifx_wcm_connect);

STATIC mp_obj_t network_ifx_wcm_disconnect(mp_obj_t self_in) {
    // network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t ret = cy_wcm_disconnect_ap();
    wcm_assert_raise("msg tbd", ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_disconnect_obj, network_ifx_wcm_disconnect);

STATIC mp_obj_t network_ifx_wcm_isconnected(mp_obj_t self_in) {
    // network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(cy_wcm_is_connected_to_ap());

}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_isconnected_obj, network_ifx_wcm_isconnected);

STATIC mp_obj_t network_ifx_wcm_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        const ip_addr_t *dns = dns_getserver(0);
        cy_wcm_get_ip_addr(self->itf, &ip_address);
        cy_wcm_get_gateway_ip_address(self->itf, &gateway_addr);
        cy_wcm_get_ip_netmask(self->itf, &net_mask_addr);
        mp_obj_t tuple[4] = {
            mp_obj_new_str(ip4addr_ntoa((const ip4_addr_t *)&ip_address.ip.v4), strlen(ip4addr_ntoa((const ip4_addr_t *)&ip_address.ip.v4))),
            mp_obj_new_str(ip4addr_ntoa((const ip4_addr_t *)&net_mask_addr.ip.v4), strlen(ip4addr_ntoa((const ip4_addr_t *)&net_mask_addr.ip.v4))),
            mp_obj_new_str(ip4addr_ntoa((const ip4_addr_t *)&gateway_addr.ip.v4), strlen(ip4addr_ntoa((const ip4_addr_t *)&gateway_addr.ip.v4))),
            netutils_format_ipv4_addr((uint8_t *)dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        const mp_obj_t *argss = args + 1;
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(argss[0], 4, &items);
        cy_rslt_t res = cy_wcm_set_ap_ip_setting(&ap_ip, items[0], items[1], items[2], CY_WCM_IP_VER_V4);
        ip_addr_t dns;
        netutils_parse_ipv4_addr(items[3], (uint8_t *)&dns, NETUTILS_BIG);
        dns_setserver(0, &dns);
        if (res == CY_RSLT_SUCCESS) {
            mp_printf(&mp_plat_print, "successfully configured");
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_ifconfig_obj, 1, 2, network_ifx_wcm_ifconfig);

STATIC mp_obj_t network_ifx_wcm_status(size_t n_args, const mp_obj_t *args) {
    // network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // no arguments: return link status
        return MP_OBJ_NEW_SMALL_INT(cy_wcm_is_connected_to_ap());
    }

    // one argument: return status based on query parameter
    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_stations: {
            // TODO: return list of connected stations to the AP
            // check cyw43 implementation as reference
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_status_obj, 1, 2, network_ifx_wcm_status);

static inline uint32_t nw_get_le32(const uint8_t *buf) {
    return buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
}

static inline void nw_put_le32(uint8_t *buf, uint32_t x) {
    buf[0] = x;
    buf[1] = x >> 8;
    buf[2] = x >> 16;
    buf[3] = x >> 24;
}

STATIC mp_obj_t network_ifx_wcm_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_antenna: {
                uint8_t buf[4];
                uint32_t ret = whd_wifi_get_ioctl_buffer(whd_ifs[self->itf], WLC_GET_ANTDIV, buf, 4);
                wcm_assert_raise("msg tbd", ret);

                return MP_OBJ_NEW_SMALL_INT(nw_get_le32(buf));
            }
            case MP_QSTR_channel: {
                uint32_t channel;
                uint32_t ret = whd_wifi_get_channel(whd_ifs[self->itf], &channel);
                wcm_assert_raise("msg tbd",  ret);

                return MP_OBJ_NEW_SMALL_INT(channel);
            }
            case MP_QSTR_ssid:
            case MP_QSTR_essid: {
                wl_bss_info_t bss_info;
                uint32_t ret = whd_wifi_get_bss_info(whd_ifs[self->itf], &bss_info);
                wcm_assert_raise("msg tbd", ret);

                return mp_obj_new_str((const char *)bss_info.SSID, bss_info.SSID_len);
            }

            case MP_QSTR_security: {
                whd_security_t security;
                uint32_t ret = whd_wifi_get_ap_info(whd_ifs[self->itf], NULL, &security);
                wcm_assert_raise("msg tbd", ret);

                return MP_OBJ_NEW_SMALL_INT(security);
            }

            case MP_QSTR_mac: {
                wl_bss_info_t bss_info;
                uint32_t ret = whd_wifi_get_bss_info(whd_ifs[self->itf], &bss_info);
                wcm_assert_raise("msg tbd", ret);

                return mp_obj_new_bytes(bss_info.BSSID.octet, 6);
            }
            case MP_QSTR_txpower: {
                uint8_t buf[13];
                memcpy(buf, "qtxpower\x00\x00\x00\x00\x00", 13);
                uint32_t ret = whd_wifi_get_ioctl_buffer(whd_ifs[self->itf], WLC_GET_VAR, buf, 13);
                wcm_assert_raise("msg tbd", ret);

                return MP_OBJ_NEW_SMALL_INT(nw_get_le32(buf) / 4);
            }

            case MP_QSTR_hostname: {
                mp_raise_ValueError(MP_ERROR_TEXT("deprecated. use network.hostname() instead."));
                break;
            }

            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_antenna: {
                        uint8_t buf[4];
                        nw_put_le32(buf, mp_obj_get_int(e->value));
                        uint32_t ret = whd_wifi_set_ioctl_buffer(whd_ifs[self->itf], WLC_SET_ANTDIV, buf, 4);
                        wcm_assert_raise("msg tbd", ret);
                        break;
                    }
                    case MP_QSTR_channel: {
                        uint32_t ret = whd_wifi_set_channel(whd_ifs[self->itf], mp_obj_get_int(e->value));
                        wcm_assert_raise("msg tbd", ret);
                        break;
                    }
                    case MP_QSTR_ssid:
                    case MP_QSTR_essid: {
                        // TODO: Implement ssid/essid change. Explore WHD to change name
                        // managing the state. Only available in the API via whd_wifi_init_ap.

                        // size_t len;
                        // const char *ssid_str = mp_obj_str_get_data(e->value, &len);
                        // whd_security_t security;
                        // wl_bss_info_t bss_info;
                        // uint32_t ret = whd_wifi_get_ap_info(itf, &bss_info, &security);
                        // if (ret != WHD_SUCCESS) {
                        //     mp_raise_OSError(-ret);
                        // }
                        // whd_ssid_t whd_ssid;
                        // memcpy(whd_ssid.value, ssid_str, len);
                        // whd_ssid.length = len;
                        // uint32_t ret = whd_wifi_init_ap(itf, &whd_ssid, &security,  );

                        break;
                    }
                    case MP_QSTR_monitor: {
                        mp_int_t value = mp_obj_get_int(e->value);
                        uint8_t buf[9 + 4];
                        memcpy(buf, "allmulti\x00", 9);
                        nw_put_le32(buf + 9, value);
                        uint32_t ret = whd_wifi_set_ioctl_buffer(whd_ifs[self->itf], WLC_SET_VAR, buf, 9 + 4);
                        wcm_assert_raise("msg tbd", ret);
                        nw_put_le32(buf, value);
                        ret = whd_wifi_set_ioctl_buffer(whd_ifs[self->itf], WLC_SET_MONITOR, buf, 4);
                        wcm_assert_raise("msg tbd", ret);
                        // In cyw43 they keep the trace flags in a variable.
                        // TODO. Understand how are these trace flags used, and if
                        // an equivalent tracing feature can/should be enabled
                        // for the WHD.
                        // if (value) {
                        //     self->cyw->trace_flags |= CYW43_TRACE_MAC;
                        // } else {
                        //     self->cyw->trace_flags &= ~CYW43_TRACE_MAC;
                        // }
                        break;
                    }
                    case MP_QSTR_security: {
                        // TODO: Implement AP security change
                        break;
                    }
                    case MP_QSTR_key:
                    case MP_QSTR_password: {
                        // TODO: Implement AP password change
                        break;
                    }
                    case MP_QSTR_pm: {
                        // TODO: Implement pm? What is pm in the cyw43? power management
                        break;
                    }
                    case MP_QSTR_trace: {
                        // TODO: Implement tracing.
                        break;
                    }
                    case MP_QSTR_txpower: {
                        mp_int_t dbm = mp_obj_get_int(e->value);
                        uint8_t buf[9 + 4];
                        memcpy(buf, "qtxpower\x00", 9);
                        nw_put_le32(buf + 9, dbm * 4);
                        uint32_t ret = whd_wifi_set_ioctl_buffer(whd_ifs[self->itf], WLC_SET_VAR, buf, 9 + 4);
                        wcm_assert_raise("msg tbd", ret);
                        break;
                    }
                    case MP_QSTR_hostname: {
                        mp_raise_ValueError(MP_ERROR_TEXT("deprecated. use network.hostname() instead."));
                        break;
                    }

                    default:
                        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
                }
            }
        }

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_config_obj, 1, network_ifx_wcm_config);

/*******************************************************************************/
// class bindings

STATIC const mp_rom_map_elem_t network_ifx_wcm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send_ethernet), MP_ROM_PTR(&network_ifx_wcm_send_ethernet_obj) },

    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&network_ifx_wcm_deinit_obj) }, // shall this be part of the module ??
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_ifx_wcm_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&network_ifx_wcm_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_ifx_wcm_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_ifx_wcm_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_ifx_wcm_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_ifx_wcm_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_ifx_wcm_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&network_ifx_wcm_config_obj) },

    // Network WCM constants
    // Security modes
    { MP_ROM_QSTR(MP_QSTR_OPEN),                      MP_ROM_INT(CY_WCM_SECURITY_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_WEP_PSK),                   MP_ROM_INT(CY_WCM_SECURITY_WEP_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WEP_SHARED),                MP_ROM_INT(CY_WCM_SECURITY_WEP_SHARED) },
    { MP_ROM_QSTR(MP_QSTR_WPA_TKIP_PSK),              MP_ROM_INT(CY_WCM_SECURITY_WPA_TKIP_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA_AES_PSK),               MP_ROM_INT(CY_WCM_SECURITY_WPA_AES_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA_MIXED_PSK),             MP_ROM_INT(CY_WCM_SECURITY_WPA_MIXED_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_AES_PSK),              MP_ROM_INT(CY_WCM_SECURITY_WPA2_AES_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_TKIP_PSK),             MP_ROM_INT(CY_WCM_SECURITY_WPA2_TKIP_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_MIXED_PSK),            MP_ROM_INT(CY_WCM_SECURITY_WPA2_MIXED_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_FBT_PSK),              MP_ROM_INT(CY_WCM_SECURITY_WPA2_FBT_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA3_SAE),                  MP_ROM_INT(CY_WCM_SECURITY_WPA3_SAE) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_WPA_AES_PSK),          MP_ROM_INT(CY_WCM_SECURITY_WPA2_WPA_AES_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_WPA_MIXED_PSK),        MP_ROM_INT(CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA3_WPA2_PSK),             MP_ROM_INT(CY_WCM_SECURITY_WPA3_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_WPA_TKIP_ENT),              MP_ROM_INT(CY_WCM_SECURITY_WPA_TKIP_ENT) },
    { MP_ROM_QSTR(MP_QSTR_WPA_MIXED_ENT),             MP_ROM_INT(CY_WCM_SECURITY_WPA_MIXED_ENT) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_TKIP_ENT),             MP_ROM_INT(CY_WCM_SECURITY_WPA2_TKIP_ENT) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_MIXED_ENT),            MP_ROM_INT(CY_WCM_SECURITY_WPA2_MIXED_ENT) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_FBT_ENT),              MP_ROM_INT(CY_WCM_SECURITY_WPA2_FBT_ENT) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_IBSS_OPEN),            MP_ROM_INT(CY_WCM_SECURITY_IBSS_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_WPS_SECURE),           MP_ROM_INT(CY_WCM_SECURITY_WPS_SECURE) },
};
STATIC MP_DEFINE_CONST_DICT(network_ifx_wcm_locals_dict, network_ifx_wcm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_ifx_wcm_type,
    MP_QSTR_IFX_WCM,
    MP_TYPE_FLAG_NONE,
    make_new, network_ifx_wcm_make_new,
    print, network_ifx_wcm_print,
    locals_dict, &network_ifx_wcm_locals_dict
    );

#endif // MICROPY_PY_NETWORK_IFX_WCM
