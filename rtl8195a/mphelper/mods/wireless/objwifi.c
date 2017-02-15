/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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

#include "objwifi.h"

/********************** Local variables ***************************************/
uint8_t  ScanNetworkCounter = 0;
int8_t   ScanNetworkSsidCache[WIFI_MAX_SCAN_NETWORKS][WIFI_MAX_SSID_LEN] = {0};
int8_t   ScanNetworkBssidCache[WIFI_MAX_SCAN_NETWORKS][WIFI_MAC_LEN]     = {0};
int8_t   ScanNetworkBssCache[WIFI_MAX_SCAN_NETWORKS]                     = {0};
int8_t   ScanNetworkSecurityCache[WIFI_MAX_SCAN_NETWORKS]                = {0};
int8_t   ScanNetworkWpsCache[WIFI_MAX_SCAN_NETWORKS]                     = {0};
uint8_t  ScanNetworkChannelCache[WIFI_MAX_SCAN_NETWORKS]                 = {0};
uint8_t  ScanNetworkBandCache[WIFI_MAX_SCAN_NETWORKS]                    = {0};
uint32_t ScanNetworkRssiCache[WIFI_MAX_SCAN_NETWORKS]                    = {0};

const int8_t mpexception_wifi_invalid_argument_in_sta_mode[] = "No SSID";

/********************** Local functions ***************************************/
void validate_wifi_mode(uint8_t mode) {
    if (mode != RTW_MODE_AP &&
        mode != RTW_MODE_STA &&
        mode != RTW_MODE_STA_AP &&
        mode != RTW_MODE_PROMISC &&
        mode != RTW_MODE_P2P) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

void validate_ssid(int8_t **ssid, mp_obj_t obj) {
    mp_uint_t ssid_len = 0;

    if (obj != NULL) {
        *ssid = mp_obj_str_get_data(obj, &ssid_len);
        if (ssid_len > WIFI_MAX_SSID_LEN || ssid_len < WIFI_MIN_SSID_LEN)
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

void validate_key(uint8_t **key, uint8_t *key_len, uint32_t *sec_type, mp_obj_t obj) {
    uint8_t key_temp[32] = {0};

    if (obj != mp_const_none) {
        mp_obj_t *sec;
        mp_obj_get_array_fixed_n(obj, 2, &sec);

        *sec_type = mp_obj_get_int(sec[0]);
        *key = mp_obj_str_get_data(sec[1], key_len);

        memcpy(key_temp, *key, sizeof(key_temp));

        if (*sec_type != RTW_SECURITY_OPEN &&
            *sec_type != RTW_SECURITY_WEP_PSK &&
            *sec_type != RTW_SECURITY_WEP_SHARED &&
            *sec_type != RTW_SECURITY_WPA_TKIP_PSK &&
            *sec_type != RTW_SECURITY_WPA_AES_PSK &&
            *sec_type != RTW_SECURITY_WPA2_TKIP_PSK &&
            *sec_type != RTW_SECURITY_WPA2_AES_PSK &&
            *sec_type != RTW_SECURITY_WPA2_MIXED_PSK &&
            *sec_type != RTW_SECURITY_WPA_WPA2_MIXED &&
            *sec_type != RTW_SECURITY_WPS_OPEN &&
            *sec_type != RTW_SECURITY_WPS_SECURE) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        }

        if (*sec_type == RTW_SECURITY_WEP_SHARED || 
            *sec_type == RTW_SECURITY_WEP_PSK) { 
            for (mp_uint_t i = 0; i < *key_len; i++) {
                if (!unichar_isdigit(key_temp[i])) {
                    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
                }
            }
            // TODO: It's quite mess here 
            uint8_t wep_key[32] = {0};
            uint8_t i = 0, idx = 0;
            for (i = 0, idx = 0; i < strlen(key_temp); i++) {
                wep_key[idx] += unichar_xdigit_value(key_temp[i]);
                if ((i % 2) == 0) {
                    wep_key[idx] *= 16;
                } else {
                    idx++;
                }
            }
            memcpy(*key, wep_key, sizeof(wep_key));
            *key_len /= 2;
        }
    }
}

void validate_channel(uint8_t channel) {
    if (channel < 1 || channel > 11) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
    }
}

/********************** function bodies ***************************************/

void wifi_init0(void) {
    // to WIFI init here
}

rtw_result_t wifi_scan_handler(rtw_scan_handler_result_t *malloced_scan_result) {
    rtw_scan_result_t *record;
    if (malloced_scan_result->scan_complete != RTW_TRUE) {
        record = &malloced_scan_result->ap_details;
        record->SSID.val[record->SSID.len] = 0;
        if (ScanNetworkCounter < WIFI_MAX_SCAN_NETWORKS) {
            memcpy(ScanNetworkSsidCache[ScanNetworkCounter], record->SSID.val, record->SSID.len);
            memcpy(ScanNetworkBssidCache[ScanNetworkCounter], record->BSSID.octet, WIFI_MAC_LEN);
            ScanNetworkRssiCache[ScanNetworkCounter] = record->signal_strength;
            ScanNetworkBssCache[ScanNetworkCounter] = record->bss_type;
            ScanNetworkSecurityCache[ScanNetworkCounter] = record->security;
            ScanNetworkWpsCache[ScanNetworkCounter] = record->wps_type;
            ScanNetworkChannelCache[ScanNetworkCounter] = record->channel;
            ScanNetworkBandCache[ScanNetworkCounter] = record->band;
            ScanNetworkCounter++;
        }
    }
}

STATIC mp_obj_t wifi_scan_network(mp_obj_t self_in) {
    STATIC const qstr wifi_scan_info_fields[] = {
        MP_QSTR_ssid, MP_QSTR_bssid, MP_QSTR_rssi, MP_QSTR_bss, MP_QSTR_security, MP_QSTR_wps, MP_QSTR_channel, MP_QSTR_band
    };
    mp_obj_t nets = mp_obj_new_list(0, NULL);
    uint8_t cnt = 0;
    
    ScanNetworkCounter = 0;

    wifi_scan_networks(wifi_scan_handler, NULL);

    for (cnt=0;cnt<ScanNetworkCounter;cnt++) {
        mp_obj_t tuple[8];
        tuple[0] = mp_obj_new_str((const char *)ScanNetworkSsidCache[cnt], strlen(ScanNetworkSsidCache[cnt]), false);
        tuple[1] = mp_obj_new_bytes((const char *)ScanNetworkBssidCache[cnt], WIFI_MAC_LEN);
        tuple[2] = mp_obj_new_int(ScanNetworkRssiCache[cnt]);
        tuple[3] = mp_obj_new_int(ScanNetworkBssCache[cnt]);
        tuple[4] = mp_obj_new_int(ScanNetworkSecurityCache[cnt]);
        tuple[5] = mp_obj_new_int(ScanNetworkWpsCache[cnt]);
        tuple[6] = mp_obj_new_int(ScanNetworkChannelCache[cnt]);
        tuple[7] = mp_obj_new_int(ScanNetworkBandCache[cnt]);
        mp_obj_list_append(nets, mp_obj_new_attrtuple(wifi_scan_info_fields, 8, tuple));
    }
    return nets;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wifi_scan_obj, wifi_scan_network);

STATIC mp_obj_t wifi_rssi(mp_obj_t self_in) {
    int16_t ret = RTW_ERROR;
    int16_t rssi = 0;

    ret = wifi_get_rssi(&rssi);

    if (ret == RTW_SUCCESS) {
        return mp_obj_new_int(rssi);
    } else {
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wifi_rssi_obj, wifi_rssi);

STATIC mp_obj_t wifi_mac(mp_uint_t n_args, const mp_obj_t *args) {
    wifi_obj_t *self = args[0];
    int8_t mac_str[17] = {0};
    int16_t ret = RTW_ERROR;

    if (n_args == 1) {
        ret = wifi_get_mac_address(mac_str);
        if (ret == RTW_SUCCESS) {
            return mp_obj_new_str(mac_str, sizeof(mac_str), false);
        }
        else {
            // TODO find a proper exception
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, mpexception_value_invalid_arguments));
        }
    }
    else {
        // Parse MAC address here
    }
#if 0
    if (n_args == 1) {
        // get the value
        return MP_OBJ_NEW_SMALL_INT();
    } else {
        // set the pin value
        if (mp_obj_is_true(args[1])) {
            self->value = 1;
            gpio_write(&(self->obj), 1);
        } else {
            self->value = 0;
            gpio_write(&(self->obj), 0);
        }
        return mp_const_none;
    }
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wifi_mac_obj, 1, 2, wifi_mac);

STATIC mp_obj_t wifi_connect_network(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    STATIC const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,     MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_auth,                       MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_timeout,  MP_ARG_KW_ONLY  | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    
    wifi_obj_t *self = pos_args[0];

    if (self->mode == RTW_MODE_AP) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, mpexception_os_request_not_possible));
    }

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    int8_t *ssid = NULL;
    validate_ssid(&ssid, args[0].u_obj);

    int8_t   *key = NULL;
    uint32_t  security_type = 0;
    mp_uint_t key_len;
    validate_key((uint8_t *)&key, &key_len, &security_type, args[1].u_obj);

    int32_t timeout = -1;
    if (args[2].u_obj != mp_const_none) {
        timeout = mp_obj_get_int(args[2].u_obj);
    }
    
    int16_t ret = -1;
    ret = wifi_connect(ssid, security_type, key, strlen(ssid), key_len, 0, NULL);

    if (ret == RTW_SUCCESS) {
        return mp_const_none;;
    }
    else {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Can not connect to wifi"));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_connect_obj, 1, wifi_connect_network);

STATIC mp_obj_t wifi_disconnect_network(mp_obj_t self_in) {
    wifi_disconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wifi_disconnect_obj, wifi_disconnect_network);

STATIC void wifi_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    wifi_obj_t *self = self_in;
    mp_printf(print, "Wifi");
}

STATIC mp_obj_t wifi_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *all_args) {
    STATIC const mp_arg_t wifi_init_args[] = {
        { MP_QSTR_mode,                           MP_ARG_INT,  {.u_int = RTW_MODE_STA} },
        { MP_QSTR_ssid,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_auth,         MP_ARG_KW_ONLY  | MP_ARG_OBJ,  {.u_obj = mp_const_none} },
        { MP_QSTR_channel,      MP_ARG_KW_ONLY  | MP_ARG_INT,  {.u_int = 1} },
    };
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(wifi_init_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), wifi_init_args, args);

    wifi_obj_t *self = m_new_obj(wifi_obj_t);

    memset(self, 0x0, sizeof(wifi_obj_t));

    // Verify mode 
    uint8_t mode = 0;
    mode = args[0].u_int;
    validate_wifi_mode(mode);
    self->mode = mode;

    // Verify ssid 
    int8_t *ssid = NULL;
    validate_ssid(&ssid, args[1].u_obj);
    memcpy(self->ssid, ssid, strlen(ssid));
    
    // Verify security type and key
    int8_t   *key = NULL;
    uint32_t  security_type = 0;
    mp_uint_t key_len = 0;
    validate_key((uint8_t *)&key, &key_len, &security_type, args[2].u_obj);
    self->security_type = security_type;
    memcpy(self->key, key, strlen(key));

    // Verify channel
    uint8_t channel = 0;
    channel = args[3].u_int;
    validate_channel(channel);
    self->channel = channel;

    self->base.type = &wifi_type;

    wifi_on(self->mode);

    return (mp_obj_t)self;
}

STATIC const mp_map_elem_t wifi_locals_dict_table[] = {
    // instance methods
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan),                       (mp_obj_t)&wifi_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rssi),                       (mp_obj_t)&wifi_rssi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mac),                       (mp_obj_t)&wifi_mac_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),                    (mp_obj_t)&wifi_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect),                 (mp_obj_t)&wifi_disconnect_obj },

    // class constants
    // WIFI mode 
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_STA),                   MP_OBJ_NEW_SMALL_INT(RTW_MODE_STA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_AP),                    MP_OBJ_NEW_SMALL_INT(RTW_MODE_AP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_STA_AP),                MP_OBJ_NEW_SMALL_INT(RTW_MODE_STA_AP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_PROMISC),               MP_OBJ_NEW_SMALL_INT(RTW_MODE_PROMISC) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_MODE_P2P),                   MP_OBJ_NEW_SMALL_INT(RTW_MODE_P2P) },

    // SECURITY MODE
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_OPEN),              MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_OPEN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WEP_PSK),           MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WEP_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WEP_SHARED),        MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WEP_SHARED) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPA_TKIP_PSK),      MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPA_TKIP_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPA_AES_PSK),       MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPA_AES_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPA2_TKIP_PSK),     MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPA2_TKIP_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPA2_AES_PSK),      MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPA2_AES_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPA2_MIXED_PSK),    MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPA2_MIXED_PSK) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPA_WPA2_MIXED),    MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPA_WPA2_MIXED) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPS_OPEN),          MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPS_OPEN) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SECURITY_WPS_SECURE),        MP_OBJ_NEW_SMALL_INT(RTW_SECURITY_WPS_SECURE) },
};

STATIC MP_DEFINE_CONST_DICT(wifi_locals_dict, wifi_locals_dict_table);

const mp_obj_type_t wifi_type = {
    { &mp_type_type },
    .name        = MP_QSTR_WIFI,
    .print       = wifi_print,
    .make_new    = wifi_make_new,
    .locals_dict = (mp_obj_t)&wifi_locals_dict,
};

