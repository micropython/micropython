/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"

//| class Radio:
//|     """Native wifi radio.
//|
//|        This class manages the station and access point functionality of the native
//|        Wifi radio.
//|
//|     """
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `wifi.Radio`.
//|         Use `wifi.radio` to access the sole instance available."""
//|         ...
//|

//|     enabled: bool
//|     """True when the wifi radio is enabled."""
//|
STATIC mp_obj_t wifi_radio_get_enabled(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_wifi_radio_get_enabled(self));

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_enabled_obj, wifi_radio_get_enabled);

const mp_obj_property_t wifi_radio_enabled_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&wifi_radio_get_enabled_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|     mac_address: bytes
//|     """MAC address of the wifi radio. (read-only)"""
//|
STATIC mp_obj_t wifi_radio_get_mac_address(mp_obj_t self) {
    return MP_OBJ_FROM_PTR(common_hal_wifi_radio_get_mac_address(self));

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_mac_address_obj, wifi_radio_get_mac_address);

const mp_obj_property_t wifi_radio_mac_address_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&wifi_radio_get_mac_address_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};


//|     def start_scanning_networks(self, *, start_channel=1, stop_channel=11) -> Iterable[Network]:
//|         """Scans for available wifi networks over the given channel range. Make sure the channels are allowed in your country."""
//|         ...
//|
STATIC mp_obj_t wifi_radio_start_scanning_networks(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return common_hal_wifi_radio_start_scanning_networks(self);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_start_scanning_networks_obj, wifi_radio_start_scanning_networks);

//|     def stop_scanning_networks(self) -> None:
//|         """Stop scanning for Wifi networks and free any resources used to do it."""
//|         ...
//|
STATIC mp_obj_t wifi_radio_stop_scanning_networks(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_wifi_radio_stop_scanning_networks(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_stop_scanning_networks_obj, wifi_radio_stop_scanning_networks);

//|     def set_hostname(self, hostname: ReadableBuffer) -> None:
//|         """Sets hostname for wifi interface. When the hostname is altered after interface started/connected
//|            the changes would only be reflected once the interface restarts/reconnects."""
//|         ...
//|
STATIC mp_obj_t wifi_radio_set_hostname(mp_obj_t self_in, mp_obj_t hostname_in) {
    mp_buffer_info_t hostname;
    mp_get_buffer_raise(hostname_in, &hostname, MP_BUFFER_READ);

    if (hostname.len < 1 || hostname.len > 63) {
        mp_raise_ValueError(translate("Hostname must be between 1 and 63 characters"));
    }

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_wifi_radio_set_hostname(self, hostname.buf);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_hostname_obj, wifi_radio_set_hostname);

//|     def connect(self, ssid: ReadableBuffer, password: ReadableBuffer = b"", *, channel: Optional[int] = 0, timeout: Optional[float] = None) -> bool:
//|         """Connects to the given ssid and waits for an ip address. Reconnections are handled
//|            automatically once one connection succeeds."""
//|         ...
//|
STATIC mp_obj_t wifi_radio_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_channel, ARG_bssid, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_password,  MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY |  MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t timeout = 0;
    if (args[ARG_timeout].u_obj != mp_const_none) {
        timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    }

    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);

    mp_buffer_info_t password;
    password.len = 0;
    if (args[ARG_password].u_obj != MP_OBJ_NULL) {
        mp_get_buffer_raise(args[ARG_password].u_obj, &password, MP_BUFFER_READ);
        if (password.len > 0 && (password.len < 8 || password.len > 63)) {
            mp_raise_ValueError(translate("WiFi password must be between 8 and 63 characters"));
        }
    }

    #define MAC_ADDRESS_LENGTH 6

    mp_buffer_info_t bssid;
    bssid.len = 0;
    // Should probably make sure bssid is just bytes and not something else too
    if (args[ARG_bssid].u_obj != MP_OBJ_NULL) {
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        if (bssid.len != MAC_ADDRESS_LENGTH) {
            mp_raise_ValueError(translate("Invalid BSSID"));
        }
    }

    wifi_radio_error_t error = common_hal_wifi_radio_connect(self, ssid.buf, ssid.len, password.buf, password.len, args[ARG_channel].u_int, timeout, bssid.buf, bssid.len);
    if (error == WIFI_RADIO_ERROR_AUTH) {
        mp_raise_ConnectionError(translate("Authentication failure"));
    } else if (error == WIFI_RADIO_ERROR_NO_AP_FOUND) {
        mp_raise_ConnectionError(translate("No network with that ssid"));
    } else if (error != WIFI_RADIO_ERROR_NONE) {
        mp_raise_ConnectionError(translate("Unknown failure"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_radio_connect_obj, 1, wifi_radio_connect);

//|     ipv4_address: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the radio when connected to an access point. None otherwise."""
//|
STATIC mp_obj_t wifi_radio_get_ipv4_address(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_address(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_address_obj, wifi_radio_get_ipv4_address);

const mp_obj_property_t wifi_radio_ipv4_address_obj = {
    .base.type = &mp_type_property,
    .proxy = { (mp_obj_t)&wifi_radio_get_ipv4_address_obj,
               (mp_obj_t)&mp_const_none_obj,
               (mp_obj_t)&mp_const_none_obj },
};

//|     def ping(self, ip, *, timeout: float = 0.5) -> float:
//|         """Ping an IP to test connectivity. Returns echo time in seconds.
//|            Returns None when it times out."""
//|         ...
//|
STATIC mp_obj_t wifi_radio_ping(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ip, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ip, MP_ARG_REQUIRED | MP_ARG_OBJ, },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_float_t timeout = 0.5;
    if (args[ARG_timeout].u_obj != mp_const_none) {
        timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    }

    mp_int_t time_ms = common_hal_wifi_radio_ping(self, args[ARG_ip].u_obj, timeout);
    if (time_ms == -1) {
        return mp_const_none;
    }

    return mp_obj_new_float(time_ms / 1000.0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_radio_ping_obj, 1, wifi_radio_ping);

STATIC const mp_rom_map_elem_t wifi_radio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enabled), MP_ROM_PTR(&wifi_radio_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_mac_address), MP_ROM_PTR(&wifi_radio_mac_address_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_scanning_networks),    MP_ROM_PTR(&wifi_radio_start_scanning_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_scanning_networks),    MP_ROM_PTR(&wifi_radio_stop_scanning_networks_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_hostname),    MP_ROM_PTR(&wifi_radio_set_hostname_obj) },

    { MP_ROM_QSTR(MP_QSTR_connect),    MP_ROM_PTR(&wifi_radio_connect_obj) },
    // { MP_ROM_QSTR(MP_QSTR_connect_to_enterprise),    MP_ROM_PTR(&wifi_radio_connect_to_enterprise_obj) },

    { MP_ROM_QSTR(MP_QSTR_ipv4_address),    MP_ROM_PTR(&wifi_radio_ipv4_address_obj) },

    // { MP_ROM_QSTR(MP_QSTR_access_point_active),   MP_ROM_PTR(&wifi_radio_access_point_active_obj) },
    // { MP_ROM_QSTR(MP_QSTR_start_access_point), MP_ROM_PTR(&wifi_radio_start_access_point_obj) },

    { MP_ROM_QSTR(MP_QSTR_ping), MP_ROM_PTR(&wifi_radio_ping_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wifi_radio_locals_dict, wifi_radio_locals_dict_table);

const mp_obj_type_t wifi_radio_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Radio,
    .locals_dict = (mp_obj_t)&wifi_radio_locals_dict,
};
