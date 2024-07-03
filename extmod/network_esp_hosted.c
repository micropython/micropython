/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 *
 * ESP-Hosted network interface.
 */

#include "py/mphal.h"

#if MICROPY_PY_NETWORK && MICROPY_PY_NETWORK_ESP_HOSTED

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "py/misc.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"
#include "modmachine.h"

#include "esp_hosted_wifi.h"
#include "esp_hosted_hal.h"

typedef struct _esp_hosted_obj_t {
    mp_obj_base_t base;
    uint32_t itf;
} esp_hosted_obj_t;

static esp_hosted_obj_t esp_hosted_sta_if = {{(mp_obj_type_t *)&mod_network_esp_hosted_type}, ESP_HOSTED_STA_IF};
static esp_hosted_obj_t esp_hosted_ap_if = {{(mp_obj_type_t *)&mod_network_esp_hosted_type}, ESP_HOSTED_AP_IF};

static mp_obj_t network_esp_hosted_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_t esp_hosted_obj;
    // TODO fix
    if (n_args == 0 || mp_obj_get_int(args[0]) == ESP_HOSTED_STA_IF) {
        esp_hosted_obj = MP_OBJ_FROM_PTR(&esp_hosted_sta_if);
    } else {
        esp_hosted_obj = MP_OBJ_FROM_PTR(&esp_hosted_ap_if);
    }
    // Register with network module
    mod_network_register_nic(esp_hosted_obj);
    return esp_hosted_obj;
}

static mp_obj_t network_esp_hosted_active(size_t n_args, const mp_obj_t *args) {
    esp_hosted_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 2) {
        bool active = mp_obj_is_true(args[1]);
        if (active) {
            // If the active NIC is changing disable the active one first.
            // Note the host driver, firmware and ESP all support simultaneous AP/STA,
            // however modnetwork.c doesn't support routing traffic to different NICs
            // at the moment.
            if (self->itf == ESP_HOSTED_STA_IF && esp_hosted_wifi_link_status(ESP_HOSTED_AP_IF)) {
                esp_hosted_wifi_disable(ESP_HOSTED_AP_IF);
            } else if (self->itf == ESP_HOSTED_AP_IF && esp_hosted_wifi_link_status(ESP_HOSTED_STA_IF)) {
                esp_hosted_wifi_disable(ESP_HOSTED_STA_IF);
            }
            if (esp_hosted_wifi_init(self->itf) != 0) {
                mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("Failed to initialize ESP32 module"));
            }
        } else {
            esp_hosted_wifi_disable(self->itf);
        }
        return mp_const_none;
    }
    return mp_obj_new_bool(esp_hosted_wifi_link_status(self->itf));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_esp_hosted_active_obj, 1, 2, network_esp_hosted_active);

static int esp_hosted_scan_callback(esp_hosted_scan_result_t *scan_result, void *arg) {
    mp_obj_t scan_list = (mp_obj_t)arg;
    mp_obj_t ap[6] = {
        mp_obj_new_bytes((uint8_t *)scan_result->ssid, strlen(scan_result->ssid)),
        mp_obj_new_bytes(scan_result->bssid, sizeof(scan_result->bssid)),
        mp_obj_new_int(scan_result->channel),
        mp_obj_new_int(scan_result->rssi),
        mp_obj_new_int(scan_result->security),
        MP_OBJ_NEW_SMALL_INT(1),
    };
    mp_obj_list_append(scan_list, mp_obj_new_tuple(MP_ARRAY_SIZE(ap), ap));
    return 0;
}

static mp_obj_t network_esp_hosted_scan(mp_obj_t self_in) {
    mp_obj_t scan_list;
    scan_list = mp_obj_new_list(0, NULL);
    esp_hosted_wifi_scan(esp_hosted_scan_callback, scan_list, 10000);
    return scan_list;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_esp_hosted_scan_obj, network_esp_hosted_scan);

static mp_obj_t network_esp_hosted_connect(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_key, ARG_security, ARG_bssid, ARG_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_key,      MP_ARG_OBJ, {.u_obj = MP_ROM_NONE} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = ESP_HOSTED_SEC_WPA_WPA2_PSK} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_channel,  MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };

    // Extract args.
    esp_hosted_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get ssid
    const char *ssid = NULL;
    if (args[ARG_ssid].u_obj != mp_const_none) {
        ssid = mp_obj_str_get_str(args[ARG_ssid].u_obj);
    } else {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("SSID can't be empty!"));
    }

    // get bssid (if any)
    const char *bssid = NULL;
    if (args[ARG_bssid].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[ARG_bssid].u_obj);
    }

    // get key and security
    const char *key = NULL;
    mp_uint_t security = ESP_HOSTED_SEC_OPEN;
    if (args[ARG_key].u_obj != mp_const_none) {
        key = mp_obj_str_get_str(args[ARG_key].u_obj);
        security = args[ARG_security].u_int;
        if (security != ESP_HOSTED_SEC_OPEN && strlen(key) == 0) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Key can't be empty!"));
        }
    }

    // get channel
    mp_uint_t channel = args[ARG_channel].u_int;

    // If connected or AP active disconnect/stop AP first.
    if (esp_hosted_wifi_is_connected(self->itf)) {
        esp_hosted_wifi_disconnect(self->itf);
    }

    if (self->itf == ESP_HOSTED_STA_IF) {
        // Initialize WiFi in Station mode.
        if (esp_hosted_wifi_connect(ssid, bssid, security, key, channel) != 0) {
            mp_raise_msg_varg(&mp_type_OSError,
                MP_ERROR_TEXT("could not connect to ssid=%s, sec=%d, key=%s\n"), ssid, security, key);
        }
    } else {
        // Initialize WiFi in AP mode.
        if (esp_hosted_wifi_start_ap(ssid, security, key, channel) != 0) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("failed to start in AP mode"));
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_esp_hosted_connect_obj, 1, network_esp_hosted_connect);

static mp_obj_t network_esp_hosted_disconnect(mp_obj_t self_in) {
    esp_hosted_obj_t *self = self_in;
    esp_hosted_wifi_disconnect(self->itf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_esp_hosted_disconnect_obj, network_esp_hosted_disconnect);

static mp_obj_t network_esp_hosted_isconnected(mp_obj_t self_in) {
    esp_hosted_obj_t *self = self_in;
    return mp_obj_new_bool(esp_hosted_wifi_is_connected(self->itf));
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_esp_hosted_isconnected_obj, network_esp_hosted_isconnected);

static mp_obj_t network_esp_hosted_ifconfig(size_t n_args, const mp_obj_t *args) {
    esp_hosted_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    void *netif = esp_hosted_wifi_get_netif(self->itf);
    return mod_network_nic_ifconfig(netif, n_args - 1, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_esp_hosted_ifconfig_obj, 1, 2, network_esp_hosted_ifconfig);

static mp_obj_t network_esp_hosted_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    esp_hosted_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    void *netif = esp_hosted_wifi_get_netif(self->itf);
    return mod_network_nic_ipconfig(netif, n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_esp_hosted_ipconfig_obj, 1, network_esp_hosted_ipconfig);

static mp_obj_t network_esp_hosted_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    esp_hosted_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_mac: {
                uint8_t buf[6];
                esp_hosted_wifi_get_mac(self->itf, buf);
                return mp_obj_new_bytes(buf, 6);
            }
            case MP_QSTR_ssid:
            case MP_QSTR_essid: {
                esp_hosted_netinfo_t netinfo;
                esp_hosted_wifi_netinfo(&netinfo);
                return mp_obj_new_str(netinfo.ssid, strlen(netinfo.ssid));
            }
            case MP_QSTR_security: {
                esp_hosted_netinfo_t netinfo;
                esp_hosted_wifi_netinfo(&netinfo);
                return mp_obj_new_int(netinfo.security);
            }
            case MP_QSTR_bssid: {
                esp_hosted_netinfo_t netinfo;
                esp_hosted_wifi_netinfo(&netinfo);
                return mp_obj_new_bytes(netinfo.bssid, 6);
            }
            case MP_QSTR_channel: {
                esp_hosted_netinfo_t netinfo;
                esp_hosted_wifi_netinfo(&netinfo);
                return mp_obj_new_int(netinfo.channel);
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        if (self->itf != MOD_NETWORK_AP_IF) {
            mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
        }
        // Call connect to set WiFi access point.
        return network_esp_hosted_connect(n_args, args, kwargs);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_esp_hosted_config_obj, 1, network_esp_hosted_config);

static mp_obj_t network_esp_hosted_status(size_t n_args, const mp_obj_t *args) {
    esp_hosted_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // no arguments: return link status
        return mp_obj_new_bool(esp_hosted_wifi_link_status(self->itf));
    }

    // Query parameter.
    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_rssi: {
            esp_hosted_netinfo_t netinfo;
            esp_hosted_wifi_netinfo(&netinfo);
            return mp_obj_new_int(netinfo.rssi);
        }
        case MP_QSTR_stations: {
            if (self->itf != MOD_NETWORK_AP_IF) {
                mp_raise_ValueError(MP_ERROR_TEXT("AP required"));
            }

            size_t sta_count = 0;
            uint8_t sta_macs[ESP_HOSTED_MAX_AP_CLIENTS * 6];

            mp_obj_t sta_list = mp_obj_new_list(0, NULL);
            if (esp_hosted_wifi_get_stations(sta_macs, &sta_count) == 0) {
                for (int i = 0; i < sta_count; i++) {
                    mp_obj_list_append(sta_list, mp_obj_new_bytes(&sta_macs[i * 6], 6));
                }
            }
            return sta_list;
        }
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_esp_hosted_status_obj, 1, 2, network_esp_hosted_status);

static const mp_rom_map_elem_t network_esp_hosted_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),              MP_ROM_PTR(&network_esp_hosted_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan),                MP_ROM_PTR(&network_esp_hosted_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect),             MP_ROM_PTR(&network_esp_hosted_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect),          MP_ROM_PTR(&network_esp_hosted_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected),         MP_ROM_PTR(&network_esp_hosted_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig),            MP_ROM_PTR(&network_esp_hosted_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig),            MP_ROM_PTR(&network_esp_hosted_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_config),              MP_ROM_PTR(&network_esp_hosted_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_status),              MP_ROM_PTR(&network_esp_hosted_status_obj) },

    // Class constants.
    { MP_ROM_QSTR(MP_QSTR_IF_STA),              MP_ROM_INT(MOD_NETWORK_STA_IF) },
    { MP_ROM_QSTR(MP_QSTR_IF_AP),               MP_ROM_INT(MOD_NETWORK_AP_IF) },
    { MP_ROM_QSTR(MP_QSTR_SEC_OPEN),            MP_ROM_INT(ESP_HOSTED_SEC_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WEP),             MP_ROM_INT(ESP_HOSTED_SEC_WEP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_WPA_WPA2),        MP_ROM_INT(ESP_HOSTED_SEC_WPA_WPA2_PSK) },

    // For backwards compatibility.
    { MP_ROM_QSTR(MP_QSTR_OPEN),                MP_ROM_INT(ESP_HOSTED_SEC_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_WEP),                 MP_ROM_INT(ESP_HOSTED_SEC_WEP) },
    { MP_ROM_QSTR(MP_QSTR_WPA_PSK),             MP_ROM_INT(ESP_HOSTED_SEC_WPA_WPA2_PSK) },
};
static MP_DEFINE_CONST_DICT(network_esp_hosted_locals_dict, network_esp_hosted_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_esp_hosted_type,
    MP_QSTR_ESPHOSTED,
    MP_TYPE_FLAG_NONE,
    make_new, network_esp_hosted_make_new,
    locals_dict, &network_esp_hosted_locals_dict
    );

MP_REGISTER_ROOT_POINTER(struct _machine_spi_obj_t *mp_wifi_spi);

#endif // #if MICROPY_PY_BLUETOOTH && MICROPY_PY_NETWORK_ESP_HOSTED
