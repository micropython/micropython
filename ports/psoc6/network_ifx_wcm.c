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

#define NETWORK_WLAN_DEFAULT_SSID       "mpy-psoc6-wlan"
#define NETWORK_WLAN_DEFAULT_PASSWORD   "mpy_PSOC6_w3lc0me!"
#define NETWORK_WLAN_DEFAULT_SECURITY   CY_WCM_SECURITY_WPA2_AES_PSK
#define NETWORK_WLAN_DEFAULT_CHANNEL    9

#define NETWORK_WLAN_AP_IP              "192.168.0.1"
#define NETWORK_WLAN_AP_GATEWAY_IP      "192.168.0.1"
#define NETWORK_WLAN_AP_NETMASK_IP      "255.255.255.0"

#define NETWORK_WLAN_MAX_AP_STATIONS    8

#define NET_IFX_WCM_SEC_OPEN 0
#define NET_IFX_WCM_SEC_WEP  1
#define NET_IFX_WCM_SEC_WPA  2
#define NET_IFX_WCM_SEC_WPA2 3
#define NET_IFX_WCM_SEC_WPA_WPA2 4
#define NET_IFX_WCM_SEC_WPA3 5
#define NET_IFX_WCM_SEC_UNKNOWN 6

typedef struct
{
    cy_wcm_ap_config_t ap_config;
} network_ifx_wcm_ap_obj_t;

typedef struct
{
    cy_wcm_associated_ap_info_t ap_info;
    uint8_t connect_retries;
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

static network_ifx_wcm_obj_t network_ifx_wcm_wl_sta = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_STA };
static network_ifx_wcm_obj_t network_ifx_wcm_wl_ap = { { &mp_network_ifx_wcm_type }, CY_WCM_INTERFACE_TYPE_AP };

#define wcm_get_ap_conf_ptr(net_obj) & (net_obj.itf_obj.ap_obj.ap_config)
#define wcm_get_sta_conf_ptr(net_obj) & (net_obj.itf_obj.sta_obj)
#define wcm_get_sta_ap_info_ptr(net_obj) & (net_obj.itf_obj.sta_obj.ap_info)

#define wcm_assert_raise(msg, ret)   if (ret != CY_RSLT_SUCCESS) { \
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT(msg), ret); \
}

void network_deinit(void) {
    cy_rslt_t ret = cy_wcm_deinit();
    wcm_assert_raise("network deinit error (code: %d)", ret);
}

// Network Access Point initialization with default network parameters
void network_ap_init() {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
    cy_wcm_ip_setting_t *ap_ip_settings = &(ap_conf->ip_settings);

    ap_conf->channel = NETWORK_WLAN_DEFAULT_CHANNEL;
    memcpy(ap_conf->ap_credentials.SSID, NETWORK_WLAN_DEFAULT_SSID, strlen(NETWORK_WLAN_DEFAULT_SSID) + 1);
    memcpy(ap_conf->ap_credentials.password, NETWORK_WLAN_DEFAULT_PASSWORD, strlen(NETWORK_WLAN_DEFAULT_PASSWORD) + 1);
    ap_conf->ap_credentials.security = NETWORK_WLAN_DEFAULT_SECURITY;

    cy_wcm_set_ap_ip_setting(ap_ip_settings, NETWORK_WLAN_AP_IP, NETWORK_WLAN_AP_NETMASK_IP, NETWORK_WLAN_AP_GATEWAY_IP, CY_WCM_IP_VER_V4);
    wcm_assert_raise("network ap ip setting error (code: %d)", ret);
}

static void restart_ap(cy_wcm_ap_config_t *ap_conf) {
    if (cy_wcm_is_ap_up()) {
        uint32_t ret = cy_wcm_stop_ap();
        wcm_assert_raise("network ap deactivate error (with code: %d)", ret);
        ret = cy_wcm_start_ap(ap_conf);
        wcm_assert_raise("network ap active error (with code: %d)", ret);
    }
}

void network_sta_init() {
    network_ifx_wcm_sta_obj_t *sta_conf = wcm_get_sta_conf_ptr(network_ifx_wcm_wl_sta);
    sta_conf->connect_retries = 3; // Default connect retries
}

// Network Initialization function (called from main.c)
void network_init(void) {
    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_config_t wcm_config = { .interface = CY_WCM_INTERFACE_TYPE_AP_STA };

    ret = cy_wcm_init(&wcm_config);
    wcm_assert_raise("network init error (code: %d)", ret);

    network_ap_init();
    network_sta_init();
}

// Print after constructor invoked
static void network_ifx_wcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    const char *status_str;
    if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
        if (cy_wcm_is_connected_to_ap()) {
            status_str = "joined";
        } else {
            status_str = "down";
        }
    } else {
        if (cy_wcm_is_ap_up()) {
            status_str = "up";
        } else {
            status_str = "down";
        }
    }

    cy_wcm_ip_address_t ip_address;
    cy_rslt_t ret = cy_wcm_get_ip_addr(self->itf, &ip_address);
    if (ret != CY_RSLT_SUCCESS) {
        ip_address.ip.v4 = 0;
    }

    mp_printf(print, "<IFX WCM %s %s %u.%u.%u.%u>",
        self->itf == CY_WCM_INTERFACE_TYPE_STA ? "STA" : "AP",
        status_str,
        ip_address.ip.v4 & 0xff,
        ip_address.ip.v4 >> 8 & 0xff,
        ip_address.ip.v4 >> 16 & 0xff,
        ip_address.ip.v4 >> 24
        );
}

static mp_obj_t network_ifx_wcm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    if (n_args == 0 || mp_obj_get_int(args[0]) == MOD_NETWORK_STA_IF) {
        return MP_OBJ_FROM_PTR(&network_ifx_wcm_wl_sta);
    } else {
        return MP_OBJ_FROM_PTR(&network_ifx_wcm_wl_ap);
    }
    return mp_const_none;
}

/*******************************************************************************/
// network API

static mp_obj_t network_ifx_wcm_deinit(mp_obj_t self_in) {
    network_deinit();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_deinit_obj, network_ifx_wcm_deinit);

static mp_obj_t network_ifx_wcm_active(size_t n_args, const mp_obj_t *args) {
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
                cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
                ret = cy_wcm_start_ap(ap_conf);
                wcm_assert_raise("network ap active error (with code: %d)", ret);
            } else {
                ret = cy_wcm_stop_ap();
                wcm_assert_raise("network ap deactivate error (with code: %d)", ret);
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_active_obj, 1, 2, network_ifx_wcm_active);

typedef struct
{
    mp_obj_t *scan_list;
    cy_wcm_scan_status_t status;
}scan_user_data_t;

// Based on the scan result, get micropython defined equivalent security type (possible value 0-4, extended till 7 to include all cases) and security string (mapped to IFX stack)
uint8_t get_mpy_security_type(cy_wcm_security_t wcm_sec) {
    uint8_t mpy_sec_type = NET_IFX_WCM_SEC_UNKNOWN;

    switch (wcm_sec)
    {
        case CY_WCM_SECURITY_OPEN:
            mpy_sec_type = NET_IFX_WCM_SEC_OPEN;
            break;
        case CY_WCM_SECURITY_WEP_PSK:
        case CY_WCM_SECURITY_WEP_SHARED:
        case CY_WCM_SECURITY_IBSS_OPEN:
            mpy_sec_type = NET_IFX_WCM_SEC_WEP;
            break;
        case CY_WCM_SECURITY_WPA_AES_PSK:
        case CY_WCM_SECURITY_WPA_MIXED_PSK:
        case CY_WCM_SECURITY_WPA_TKIP_PSK:
        case CY_WCM_SECURITY_WPA_TKIP_ENT:
        case CY_WCM_SECURITY_WPA_AES_ENT:
        case CY_WCM_SECURITY_WPA_MIXED_ENT:
            mpy_sec_type = NET_IFX_WCM_SEC_WPA;
            break;
        case CY_WCM_SECURITY_WPA2_AES_PSK:
        case CY_WCM_SECURITY_WPA2_TKIP_PSK:
        case CY_WCM_SECURITY_WPA2_MIXED_PSK:
        case CY_WCM_SECURITY_WPA2_FBT_PSK:
        case CY_WCM_SECURITY_WPA2_TKIP_ENT:
        case CY_WCM_SECURITY_WPA2_AES_ENT:
        case CY_WCM_SECURITY_WPA2_MIXED_ENT:
        case CY_WCM_SECURITY_WPA2_FBT_ENT:
            mpy_sec_type = NET_IFX_WCM_SEC_WPA2;
            break;
        case CY_WCM_SECURITY_WPA2_WPA_AES_PSK:
        case CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK:
            mpy_sec_type = NET_IFX_WCM_SEC_WPA_WPA2;
            break;
        case CY_WCM_SECURITY_WPA3_SAE:
        case CY_WCM_SECURITY_WPA3_WPA2_PSK:
            mpy_sec_type = NET_IFX_WCM_SEC_WPA3;
            break;
        case CY_WCM_SECURITY_WPS_SECURE:
        case CY_WCM_SECURITY_UNKNOWN:
        default:
            mpy_sec_type = NET_IFX_WCM_SEC_UNKNOWN;
            break;
    }
    return mpy_sec_type;
}

// Callback function for scan method. After each scan result, the scan callback is executed.
static void network_ifx_wcm_scan_cb(cy_wcm_scan_result_t *result_ptr, void *user_data, cy_wcm_scan_status_t status) {
    scan_user_data_t *scan_user_data = (scan_user_data_t *)user_data;
    mp_obj_t scan_list = MP_OBJ_FROM_PTR(scan_user_data->scan_list);
    uint8_t hidden_status = 1; // HIDDEN
    uint8_t security_type = NET_IFX_WCM_SEC_OPEN;

    if (status == CY_WCM_SCAN_INCOMPLETE) {
        // Get the network status : hidden(1) or open(0)
        if (strlen((const char *)result_ptr->SSID) != 0) {
            hidden_status = 0;
        }

        // Get security type as mapped in micropython function description
        security_type = get_mpy_security_type(result_ptr->security);

        mp_obj_t tuple[6] = {
            mp_obj_new_bytes(result_ptr->SSID, strlen((const char *)result_ptr->SSID)),
            mp_obj_new_bytes(result_ptr->BSSID, CY_WCM_MAC_ADDR_LEN),
            MP_OBJ_NEW_SMALL_INT(result_ptr->channel),
            MP_OBJ_NEW_SMALL_INT(result_ptr->signal_strength),
            MP_OBJ_NEW_SMALL_INT(security_type),
            MP_OBJ_NEW_SMALL_INT(hidden_status)
        };
        mp_obj_list_append(scan_list, mp_obj_new_tuple(6, tuple));
    }

    scan_user_data->status = status;
}

static mp_obj_t network_ifx_wcm_scan(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    cy_rslt_t ret = CY_RSLT_SUCCESS;
    cy_wcm_scan_filter_t scan_filter;
    bool is_filter_used = false;

    if (self->itf != CY_WCM_INTERFACE_TYPE_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("network STA required"));
    }

    if (n_args != 1) {
        mp_raise_TypeError(MP_ERROR_TEXT("network scan accepts no query parameters"));
    }

    if (kwargs->used != 0) {
        if (kwargs->alloc != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("network scan only accepts one filter mode"));
        }

        is_filter_used = true;
        mp_map_elem_t *e = &kwargs->table[0];
        switch (mp_obj_str_get_qstr(e->key))
        {
            case MP_QSTR_ssid: {
                scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_SSID;
                size_t len;
                const char *ssid = mp_obj_str_get_data(e->value, &len);
                len = MIN(len, CY_WCM_MAX_SSID_LEN + 1);
                memcpy(scan_filter.param.SSID, ssid, len);
                memset(&scan_filter.param.SSID[len], 0, 1); // null terminated str.
                break;
            }

            case MP_QSTR_bssid: {
                scan_filter.mode = CY_WCM_SCAN_FILTER_TYPE_MAC;
                mp_buffer_info_t bssid;
                mp_get_buffer(e->value, &bssid, MP_BUFFER_READ);
                if (bssid.len != CY_WCM_MAC_ADDR_LEN) {
                    mp_raise_ValueError(MP_ERROR_TEXT("bssid address invalid length"));
                }
                memcpy(scan_filter.param.BSSID, bssid.buf, bssid.len);
                break;
            }

            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    }

    mp_obj_t network_list = mp_obj_new_list(0, NULL);
    scan_user_data_t scan_user_params;
    scan_user_params.scan_list = MP_OBJ_TO_PTR(network_list);
    scan_user_params.status = CY_WCM_SCAN_INCOMPLETE;

    cy_wcm_scan_filter_t *scan_filter_ptr = NULL;
    if (is_filter_used) {
        scan_filter_ptr = &scan_filter;
    }

    ret = cy_wcm_start_scan(network_ifx_wcm_scan_cb, (void *)&scan_user_params, scan_filter_ptr);
    wcm_assert_raise("network scan error (with code: %d)", ret);

    while (scan_user_params.status == CY_WCM_SCAN_INCOMPLETE /*|| TODO: timeout_expired */) {
    }

    return scan_user_params.scan_list;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_scan_obj, 1, network_ifx_wcm_scan);

static mp_obj_t network_ifx_wcm_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    enum { ARG_ssid, ARG_key, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} }
    };

    if (self->itf != CY_WCM_INTERFACE_TYPE_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("network STA required"));
    }

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    cy_wcm_connect_params_t connect_param;
    cy_wcm_ip_address_t ipaddress;
    memset(&connect_param, 0, sizeof(cy_wcm_connect_params_t));

    // Extract the SSID.
    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);
    if (ssid.len > CY_WCM_MAX_SSID_LEN) {
        mp_raise_TypeError(MP_ERROR_TEXT("network connect SSID too long"));
    }
    memcpy(connect_param.ap_credentials.SSID, ssid.buf, ssid.len);

    // Extract the key, if given.
    if (args[ARG_key].u_obj != mp_const_none) {
        mp_buffer_info_t key;
        mp_get_buffer_raise(args[ARG_key].u_obj, &key, MP_BUFFER_READ);
        memcpy(connect_param.ap_credentials.password, key.buf, key.len);
    }

    // Extract the BSSID, if given.
    if (args[ARG_bssid].u_obj != mp_const_none) {
        mp_buffer_info_t bssid;
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        if (bssid.len != CY_WCM_MAC_ADDR_LEN) {
            mp_raise_ValueError(MP_ERROR_TEXT("bssid address invalid length"));
        }
        memcpy(connect_param.BSSID, bssid.buf, bssid.len);
    }

    // Let the wcm driver discover the network security
    connect_param.ap_credentials.security = CY_WCM_SECURITY_UNKNOWN;

    network_ifx_wcm_sta_obj_t *sta_conf = wcm_get_sta_conf_ptr(network_ifx_wcm_wl_sta);
    uint8_t retries = sta_conf->connect_retries;
    cy_rslt_t ret = CY_WCM_EVENT_CONNECT_FAILED;
    do
    {
        ret = cy_wcm_connect_ap(&connect_param, &ipaddress);
    } while (--retries < 0 && ret != CY_RSLT_SUCCESS);
    wcm_assert_raise("network sta connect error (with code: %d)", ret);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_connect_obj, 1, network_ifx_wcm_connect);

static mp_obj_t network_ifx_wcm_disconnect(mp_obj_t self_in) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->itf != CY_WCM_INTERFACE_TYPE_STA) {
        mp_raise_ValueError(MP_ERROR_TEXT("network STA required"));
    }
    uint32_t ret = cy_wcm_disconnect_ap();
    wcm_assert_raise("network sta disconnect error (with code: %d)", ret);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_disconnect_obj, network_ifx_wcm_disconnect);

static mp_obj_t network_ifx_wcm_isconnected(mp_obj_t self_in) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
        return mp_obj_new_bool(cy_wcm_is_connected_to_ap());
    } else if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
        /* True if at least one client is connected */
        bool is_a_sta_connected = false;
        cy_wcm_mac_t sta[1] = {0};
        cy_wcm_mac_t not_conn_sta = {0, 0, 0, 0, 0, 0};
        uint32_t ret = cy_wcm_get_associated_client_list(sta, 1);
        wcm_assert_raise("network ap isconnected error (with code: %d)", ret);
        if (memcmp(&sta[0], &not_conn_sta, CY_WCM_MAC_ADDR_LEN) != 0) {
            is_a_sta_connected = true;
        }

        return mp_obj_new_bool(is_a_sta_connected);
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(network_ifx_wcm_isconnected_obj, network_ifx_wcm_isconnected);

static mp_obj_t network_ifx_wcm_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        const ip_addr_t *dns = dns_getserver(0);
        cy_wcm_ip_address_t ip_address;
        cy_wcm_ip_address_t net_mask_addr;
        cy_wcm_ip_address_t gateway_addr;
        cy_rslt_t ret = cy_wcm_get_ip_addr(self->itf, &ip_address);
        wcm_assert_raise("network ifconfig error (with code: %d)", ret);
        ret = cy_wcm_get_gateway_ip_address(self->itf, &gateway_addr);
        wcm_assert_raise("network ifconfig error (with code: %d)", ret);
        ret = cy_wcm_get_ip_netmask(self->itf, &net_mask_addr);
        wcm_assert_raise("network ifconfig error (with code: %d)", ret);
        mp_obj_t tuple[4] = {
            mp_obj_new_str(ip4addr_ntoa((const ip4_addr_t *)&ip_address.ip.v4), strlen(ip4addr_ntoa((const ip4_addr_t *)&ip_address.ip.v4))),
            mp_obj_new_str(ip4addr_ntoa((const ip4_addr_t *)&net_mask_addr.ip.v4), strlen(ip4addr_ntoa((const ip4_addr_t *)&net_mask_addr.ip.v4))),
            mp_obj_new_str(ip4addr_ntoa((const ip4_addr_t *)&gateway_addr.ip.v4), strlen(ip4addr_ntoa((const ip4_addr_t *)&gateway_addr.ip.v4))),
            netutils_format_ipv4_addr((uint8_t *)dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
            const mp_obj_t *argss = args + 1;
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(argss[0], 4, &items);
            const char *ip_address = mp_obj_str_get_str(items[0]);
            const char *net_mask_addr = mp_obj_str_get_str(items[1]);
            const char *gateway_addr = mp_obj_str_get_str(items[2]);
            cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
            cy_rslt_t ret = cy_wcm_set_ap_ip_setting(&(ap_conf->ip_settings), ip_address, net_mask_addr, gateway_addr, CY_WCM_IP_VER_V4);
            wcm_assert_raise("network ifconfig error (with code: %d)", ret);

            ip_addr_t dns;
            netutils_parse_ipv4_addr(items[3], (uint8_t *)&dns, NETUTILS_BIG);
            dns_setserver(0, &dns);

            restart_ap(ap_conf);
        } else if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
            mp_raise_ValueError(MP_ERROR_TEXT("network access point required"));
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_ifconfig_obj, 1, 2, network_ifx_wcm_ifconfig);

static mp_obj_t network_ifx_wcm_status(size_t n_args, const mp_obj_t *args) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    // one argument: return status based on query parameter
    switch (mp_obj_str_get_qstr(args[1])) {

        case MP_QSTR_rssi: {
            if (self->itf != CY_WCM_INTERFACE_TYPE_STA) {
                mp_raise_ValueError(MP_ERROR_TEXT("network station required"));
            }
            int32_t rssi;
            uint32_t ret = whd_wifi_get_rssi(whd_ifs[self->itf], &rssi);
            wcm_assert_raise("network status error (with code: %d)", ret);
            return mp_obj_new_int(rssi);
        }

        case MP_QSTR_stations: {
            if (self->itf != CY_WCM_INTERFACE_TYPE_AP) {
                mp_raise_ValueError(MP_ERROR_TEXT("network access point required"));
            }

            cy_wcm_mac_t sta_list[NETWORK_WLAN_MAX_AP_STATIONS];
            cy_wcm_mac_t not_conn_sta = {0, 0, 0, 0, 0, 0};

            uint32_t ret = cy_wcm_get_associated_client_list(&sta_list[0], NETWORK_WLAN_MAX_AP_STATIONS);
            wcm_assert_raise("network status error (with code: %d)", ret);

            mp_obj_t list = mp_obj_new_list(0, NULL);
            for (int i = 0; i < NETWORK_WLAN_MAX_AP_STATIONS; ++i) {
                if (memcmp(&sta_list[i], &not_conn_sta, CY_WCM_MAC_ADDR_LEN) != 0) {
                    mp_obj_list_append(list, mp_obj_new_bytes(sta_list[i], CY_WCM_MAC_ADDR_LEN));
                }
            }
            return list;
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("network status unknown param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ifx_wcm_status_obj, 1, 2, network_ifx_wcm_status);


static mp_obj_t network_ap_get_config_param(cy_wcm_ap_config_t *ap_conf, qstr query_opt) {
    switch (query_opt) {
        case MP_QSTR_channel: {
            return MP_OBJ_NEW_SMALL_INT(ap_conf->channel);
        }

        case MP_QSTR_ssid:
        case MP_QSTR_essid: {
            return mp_obj_new_str((const char *)ap_conf->ap_credentials.SSID, strlen((const char *)ap_conf->ap_credentials.SSID));
        }

        case MP_QSTR_security: {
            return MP_OBJ_NEW_SMALL_INT(get_mpy_security_type(ap_conf->ap_credentials.security));
        }

        /* Only default password is exposed */
        case MP_QSTR_key:
        case MP_QSTR_password: {
            if (strcmp((const char *)ap_conf->ap_credentials.password, NETWORK_WLAN_DEFAULT_PASSWORD) == 0) {
                return mp_obj_new_str((const char *)NETWORK_WLAN_DEFAULT_PASSWORD, strlen((const char *)NETWORK_WLAN_DEFAULT_PASSWORD));
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("network conf password only queryable for default password"));
            }
            break;
        }

        case MP_QSTR_mac: {
            cy_wcm_mac_t mac;
            uint32_t ret = cy_wcm_get_mac_addr(CY_WCM_INTERFACE_TYPE_AP, &mac);
            wcm_assert_raise("network config mac (code: %d)", ret);

            return mp_obj_new_bytes(mac, 6);
        }

        case MP_QSTR_hostname: {
            mp_raise_ValueError(MP_ERROR_TEXT("deprecated. use network.hostname() instead"));
            break;
        }

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }
}

static mp_obj_t network_sta_get_config_param(network_ifx_wcm_sta_obj_t *sta_conf, qstr query_opt) {
    switch (query_opt) {
        case MP_QSTR_channel: {
            cy_wcm_associated_ap_info_t ap_info;
            uint32_t ret = cy_wcm_get_associated_ap_info(&ap_info);
            wcm_assert_raise("network config error (with code: %d)", ret);
            return MP_OBJ_NEW_SMALL_INT(ap_info.channel);
        }

        case MP_QSTR_mac: {
            cy_wcm_mac_t mac;
            uint32_t ret = cy_wcm_get_mac_addr(CY_WCM_INTERFACE_TYPE_STA, &mac);
            wcm_assert_raise("network config mac (code: %d)", ret);
            return mp_obj_new_bytes(mac, 6);
        }

        case MP_QSTR_ssid:
        case MP_QSTR_essid: {
            cy_wcm_associated_ap_info_t ap_info;
            uint32_t ret = cy_wcm_get_associated_ap_info(&ap_info);
            wcm_assert_raise("network config error (with code: %d)", ret);
            return mp_obj_new_str((const char *)ap_info.SSID, strlen((const char *)ap_info.SSID));
        }

        case MP_QSTR_security: {
            cy_wcm_associated_ap_info_t ap_info;
            uint32_t ret = cy_wcm_get_associated_ap_info(&ap_info);
            wcm_assert_raise("network config error (with code: %d)", ret);
            return MP_OBJ_NEW_SMALL_INT(get_mpy_security_type(ap_info.security));
        }

        case MP_QSTR_password:
        case MP_QSTR_key: {
            mp_raise_ValueError(MP_ERROR_TEXT("network access point required"));
            break;
        }

        case MP_QSTR_hostname: {
            mp_raise_ValueError(MP_ERROR_TEXT("deprecated. use network.hostname() instead"));
            break;
        }

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }
}

cy_wcm_security_t get_wm_security_type(mp_obj_t mpy_sec) {
    switch (mp_obj_get_int(mpy_sec))
    {
        case NET_IFX_WCM_SEC_OPEN:
            return CY_WCM_SECURITY_OPEN;

        case NET_IFX_WCM_SEC_WPA:
            return CY_WCM_SECURITY_WPA_MIXED_PSK;

        case NET_IFX_WCM_SEC_WPA2:
            return CY_WCM_SECURITY_WPA2_MIXED_PSK;

        case NET_IFX_WCM_SEC_WPA3:
            return CY_WCM_SECURITY_WPA3_SAE;

        case NET_IFX_WCM_SEC_WPA_WPA2:
            return CY_WCM_SECURITY_WPA2_WPA_MIXED_PSK;

        default:
            return CY_WCM_SECURITY_UNKNOWN;
    }
}

static void network_ap_set_config_param(cy_wcm_ap_config_t *ap_conf, qstr opt, mp_obj_t opt_value, bool hold) {

    static bool required_ap_restart = false;

    switch (opt) {
        case MP_QSTR_channel: {
            ap_conf->channel = mp_obj_get_int(opt_value);
            required_ap_restart = true;
            break;
        }

        case MP_QSTR_ssid:
        case MP_QSTR_essid: {
            size_t len;
            const char *ssid_str = mp_obj_str_get_data(opt_value, &len);
            memset(ap_conf->ap_credentials.SSID, 0, CY_WCM_MAX_SSID_LEN + 1);
            memcpy(ap_conf->ap_credentials.SSID, ssid_str, len);
            required_ap_restart = true;
            break;
        }

        case MP_QSTR_security: {
            cy_wcm_security_t wcm_sec = get_wm_security_type(opt_value);
            ap_conf->ap_credentials.security = wcm_sec;
            required_ap_restart = true;
            break;
        }

        case MP_QSTR_key:
        case MP_QSTR_password: {
            size_t len;
            const char *pass_str = mp_obj_str_get_data(opt_value, &len);
            memset(ap_conf->ap_credentials.password, 0, CY_WCM_MAX_PASSPHRASE_LEN + 1);
            memcpy(ap_conf->ap_credentials.password, pass_str, len);
            break;
        }

        case MP_QSTR_hostname: {
            mp_raise_ValueError(MP_ERROR_TEXT("deprecated. use network.hostname() instead"));
            break;
        }

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }

    if (required_ap_restart && !hold) {
        restart_ap(ap_conf);
        required_ap_restart = false;
    }
}

static void network_sta_set_config_param(network_ifx_wcm_sta_obj_t *sta_conf, qstr opt, mp_obj_t opt_value) {
    switch (opt) {
        case MP_QSTR_channel:
        case MP_QSTR_key:
        case MP_QSTR_password:
        case MP_QSTR_ssid:
        case MP_QSTR_essid: {
            mp_raise_ValueError(MP_ERROR_TEXT("network access point required"));
            break;
        }

        case MP_QSTR_hostname: {
            mp_raise_ValueError(MP_ERROR_TEXT("deprecated. use network.hostname() instead"));
            break;
        }

        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }
}

static mp_obj_t network_ifx_wcm_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_ifx_wcm_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
            cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
            return network_ap_get_config_param(ap_conf, mp_obj_str_get_qstr(args[1]));
        } else if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
            network_ifx_wcm_sta_obj_t *sta_conf = wcm_get_sta_conf_ptr(network_ifx_wcm_wl_sta);
            return network_sta_get_config_param(sta_conf, mp_obj_str_get_qstr(args[1]));
        }
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }
        size_t kwargs_num = kwargs->alloc;
        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];

                if (self->itf == CY_WCM_INTERFACE_TYPE_AP) {
                    bool hold_config = true;
                    if (i == kwargs_num - 1) {
                        hold_config = false;
                    }
                    cy_wcm_ap_config_t *ap_conf = wcm_get_ap_conf_ptr(network_ifx_wcm_wl_ap);
                    network_ap_set_config_param(ap_conf, mp_obj_str_get_qstr(e->key), e->value, hold_config);
                } else if (self->itf == CY_WCM_INTERFACE_TYPE_STA) {
                    network_ifx_wcm_sta_obj_t *sta_conf = wcm_get_sta_conf_ptr(network_ifx_wcm_wl_sta);
                    network_sta_set_config_param(sta_conf, mp_obj_str_get_qstr(e->key), e->value);
                }
            }
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ifx_wcm_config_obj, 1, network_ifx_wcm_config);

/*******************************************************************************/
// class bindings

static const mp_rom_map_elem_t network_ifx_wcm_locals_dict_table[] = {
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
    { MP_ROM_QSTR(MP_QSTR_OPEN), MP_ROM_INT(NET_IFX_WCM_SEC_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_WEP), MP_ROM_INT(NET_IFX_WCM_SEC_WEP) },
    { MP_ROM_QSTR(MP_QSTR_WPA), MP_ROM_INT(NET_IFX_WCM_SEC_WPA) },
    { MP_ROM_QSTR(MP_QSTR_WPA2), MP_ROM_INT(NET_IFX_WCM_SEC_WPA2) },
    { MP_ROM_QSTR(MP_QSTR_WPA3), MP_ROM_INT(NET_IFX_WCM_SEC_WPA3) },
    { MP_ROM_QSTR(MP_QSTR_WPA2_WPA_PSK), MP_ROM_INT(NET_IFX_WCM_SEC_WPA_WPA2) },
    { MP_ROM_QSTR(MP_QSTR_SEC_UNKNOWN), MP_ROM_INT(NET_IFX_WCM_SEC_UNKNOWN) },
};
static MP_DEFINE_CONST_DICT(network_ifx_wcm_locals_dict, network_ifx_wcm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_ifx_wcm_type,
    MP_QSTR_IFX_WCM,
    MP_TYPE_FLAG_NONE,
    make_new, network_ifx_wcm_make_new,
    print, network_ifx_wcm_print,
    locals_dict, &network_ifx_wcm_locals_dict
    );

#endif // MICROPY_PY_NETWORK_IFX_WCM
