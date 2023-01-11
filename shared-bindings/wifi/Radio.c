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

#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/AuthMode.h"

#include <string.h>

#include "py/runtime.h"
#include "py/objproperty.h"

#define MAC_ADDRESS_LENGTH 6

STATIC bool hostname_valid(const char *ptr, size_t len) {
    #if 0 // validated by mp_arg_validate_length_range
    if (len == 0 || len > 253) {
        // at most 253 characters long
        return false;
    }
    #endif
    int partlen = 0;
    while (len) {
        char c = *ptr++;
        len--;
        if (c == '.') {
            if (partlen == 0 || partlen > 63) {
                return false;
            }
            partlen = 0;
            continue;
        }
        partlen++;
        if (c == '-') {
            if (partlen == 1) {
                return false; // part cannot begin with a dash
            }
            continue;
        } else if (
            (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')) {
            continue;
        }
        return false;
    }
    // check length of last part
    return !(partlen > 63);
}


//| class Radio:
//|     """Native wifi radio.
//|
//|     This class manages the station and access point functionality of the native
//|     Wifi radio.
//|
//|     """
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `wifi.Radio`.
//|         Use `wifi.radio` to access the sole instance available."""
//|         ...

//|     enabled: bool
//|     """``True`` when the wifi radio is enabled.
//|     If you set the value to ``False``, any open sockets will be closed.
//|     """
STATIC mp_obj_t wifi_radio_get_enabled(mp_obj_t self) {
    return mp_obj_new_bool(common_hal_wifi_radio_get_enabled(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_enabled_obj, wifi_radio_get_enabled);

static mp_obj_t wifi_radio_set_enabled(mp_obj_t self, mp_obj_t value) {
    const bool enabled = mp_obj_is_true(value);

    common_hal_wifi_radio_set_enabled(self, enabled);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_enabled_obj, wifi_radio_set_enabled);

MP_PROPERTY_GETSET(wifi_radio_enabled_obj,
    (mp_obj_t)&wifi_radio_get_enabled_obj,
    (mp_obj_t)&wifi_radio_set_enabled_obj);

//|     hostname: Union[str | ReadableBuffer]
//|     """Hostname for wifi interface. When the hostname is altered after interface started/connected
//|        the changes would only be reflected once the interface restarts/reconnects."""
STATIC mp_obj_t wifi_radio_get_hostname(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return common_hal_wifi_radio_get_hostname(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_hostname_obj, wifi_radio_get_hostname);

STATIC mp_obj_t wifi_radio_set_hostname(mp_obj_t self_in, mp_obj_t hostname_in) {
    mp_buffer_info_t hostname;
    mp_get_buffer_raise(hostname_in, &hostname, MP_BUFFER_READ);

    mp_arg_validate_length_range(hostname.len, 1, 253, MP_QSTR_hostname);

    if (!hostname_valid(hostname.buf, hostname.len)) {
        mp_raise_ValueError(translate("invalid hostname"));
    }

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_wifi_radio_set_hostname(self, hostname.buf);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_hostname_obj, wifi_radio_set_hostname);

MP_PROPERTY_GETSET(wifi_radio_hostname_obj,
    (mp_obj_t)&wifi_radio_get_hostname_obj,
    (mp_obj_t)&wifi_radio_set_hostname_obj);

//|     mac_address: ReadableBuffer
//|     """MAC address for the station. When the address is altered after interface is connected
//|        the changes would only be reflected once the interface reconnects.
//|
//|     **Limitations:** Not settable on RP2040 CYW43 boards, such as Pi Pico W.
//|     """


STATIC mp_obj_t _wifi_radio_get_mac_address(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(common_hal_wifi_radio_get_mac_address(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_mac_address_obj, _wifi_radio_get_mac_address);

#if CIRCUITPY_WIFI_RADIO_SETTABLE_MAC_ADDRESS
STATIC mp_obj_t wifi_radio_set_mac_address(mp_obj_t self_in, mp_obj_t mac_address_in) {
    mp_buffer_info_t mac_address;
    mp_get_buffer_raise(mac_address_in, &mac_address, MP_BUFFER_READ);

    if (mac_address.len != MAC_ADDRESS_LENGTH) {
        mp_raise_ValueError(translate("Invalid MAC address"));
    }

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_wifi_radio_set_mac_address(self, mac_address.buf);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_mac_address_obj, wifi_radio_set_mac_address);
#endif

#if CIRCUITPY_WIFI_RADIO_SETTABLE_MAC_ADDRESS
MP_PROPERTY_GETSET(wifi_radio_mac_address_obj,
    (mp_obj_t)&wifi_radio_get_mac_address_obj,
    (mp_obj_t)&wifi_radio_set_mac_address_obj);
#else
MP_PROPERTY_GETTER(wifi_radio_mac_address_obj,
    (mp_obj_t)&wifi_radio_get_mac_address_obj);
#endif

//|     tx_power: float
//|     """Wifi transmission power, in dBm."""
STATIC mp_obj_t wifi_radio_get_tx_power(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_float(common_hal_wifi_radio_get_tx_power(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_tx_power_obj, wifi_radio_get_tx_power);

STATIC mp_obj_t wifi_radio_set_tx_power(mp_obj_t self_in, mp_obj_t tx_power_in) {
    mp_float_t tx_power = mp_obj_get_float(tx_power_in);
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_wifi_radio_set_tx_power(self, tx_power);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_tx_power_obj, wifi_radio_set_tx_power);

MP_PROPERTY_GETSET(wifi_radio_tx_power_obj,
    (mp_obj_t)&wifi_radio_get_tx_power_obj,
    (mp_obj_t)&wifi_radio_set_tx_power_obj);

//|     mac_address_ap: ReadableBuffer
//|     """MAC address for the AP. When the address is altered after interface is started
//|        the changes would only be reflected once the interface restarts.
//|
//|     **Limitations:** Not settable on RP2040 CYW43 boards, such as Pi Pico W.
//|     """
STATIC mp_obj_t wifi_radio_get_mac_address_ap(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_FROM_PTR(common_hal_wifi_radio_get_mac_address_ap(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_mac_address_ap_obj, wifi_radio_get_mac_address_ap);

#if CIRCUITPY_WIFI_RADIO_SETTABLE_MAC_ADDRESS
STATIC mp_obj_t wifi_radio_set_mac_address_ap(mp_obj_t self_in, mp_obj_t mac_address_in) {
    mp_buffer_info_t mac_address;
    mp_get_buffer_raise(mac_address_in, &mac_address, MP_BUFFER_READ);

    if (mac_address.len != MAC_ADDRESS_LENGTH) {
        mp_raise_ValueError(translate("Invalid MAC address"));
    }

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_wifi_radio_set_mac_address_ap(self, mac_address.buf);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_mac_address_ap_obj, wifi_radio_set_mac_address_ap);
#endif

#if CIRCUITPY_WIFI_RADIO_SETTABLE_MAC_ADDRESS
MP_PROPERTY_GETSET(wifi_radio_mac_address_ap_obj,
    (mp_obj_t)&wifi_radio_get_mac_address_ap_obj,
    (mp_obj_t)&wifi_radio_set_mac_address_ap_obj);
#else
MP_PROPERTY_GETTER(wifi_radio_mac_address_ap_obj,
    (mp_obj_t)&wifi_radio_get_mac_address_ap_obj);
#endif

//|     def start_scanning_networks(
//|         self, *, start_channel: int = 1, stop_channel: int = 11
//|     ) -> Iterable[Network]:
//|         """Scans for available wifi networks over the given channel range. Make sure the channels are allowed in your country.
//|
//|         .. note::
//|
//|             In the raspberrypi port (RP2040 CYW43), ``start_channel`` and ``stop_channel`` are ignored.
//|         """
//|         ...
STATIC mp_obj_t wifi_radio_start_scanning_networks(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_start_channel, ARG_stop_channel };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_stop_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 11} },
    };

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t start_channel =
        (uint8_t)mp_arg_validate_int_range(args[ARG_start_channel].u_int, 1, 14, MP_QSTR_start_channel);
    uint8_t stop_channel =
        (uint8_t)mp_arg_validate_int_range(args[ARG_stop_channel].u_int, 1, 14, MP_QSTR_stop_channel);
    // Swap if in reverse order, without complaining.
    if (start_channel > stop_channel) {
        uint8_t temp = stop_channel;
        stop_channel = start_channel;
        start_channel = temp;
    }

    return common_hal_wifi_radio_start_scanning_networks(self, start_channel, stop_channel);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_radio_start_scanning_networks_obj, 1, wifi_radio_start_scanning_networks);

//|     def stop_scanning_networks(self) -> None:
//|         """Stop scanning for Wifi networks and free any resources used to do it."""
//|         ...
STATIC mp_obj_t wifi_radio_stop_scanning_networks(mp_obj_t self_in) {
    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_wifi_radio_stop_scanning_networks(self);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_stop_scanning_networks_obj, wifi_radio_stop_scanning_networks);

//|     def start_station(self) -> None:
//|         """Starts a Station."""
//|         ...
STATIC mp_obj_t wifi_radio_start_station(mp_obj_t self) {
    common_hal_wifi_radio_start_station(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_start_station_obj, wifi_radio_start_station);

//|     def stop_station(self) -> None:
//|         """Stops the Station."""
//|         ...
STATIC mp_obj_t wifi_radio_stop_station(mp_obj_t self) {
    common_hal_wifi_radio_stop_station(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_stop_station_obj, wifi_radio_stop_station);

//|     def start_ap(
//|         self,
//|         ssid: Union[str | ReadableBuffer],
//|         password: Union[str | ReadableBuffer] = b"",
//|         *,
//|         channel: int = 1,
//|         authmode: Optional[AuthMode] = None,
//|         max_connections: Optional[int] = 4
//|     ) -> None:
//|         """Starts an Access Point with the specified ssid and password.
//|
//|         If ``channel`` is given, the access point will use that channel unless
//|         a station is already operating on a different channel.
//|
//|         If ``authmode`` is not None, the access point will use that Authentication
//|         mode. If a non-empty password is given, ``authmode`` must not be ``OPEN``.
//|         If ``authmode`` is not given or is None,
//|         ``OPEN`` will be used when the password is the empty string,
//|         otherwise ``authmode`` will be ``WPA_WPA2_PSK``.
//|
//|         If ``max_connections`` is given, the access point will allow up to
//|         that number of stations to connect."""
//|         ...
STATIC mp_obj_t wifi_radio_start_ap(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_channel, ARG_authmode, ARG_max_connections };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_password,  MP_ARG_OBJ, {.u_obj = mp_const_empty_bytes} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_authmode, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_max_connections, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 4} },
    };

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // 0 indicates mode wasn't given.
    uint32_t authmodes = 0;
    if (args[ARG_authmode].u_obj != mp_const_none) {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t item, iterable = mp_getiter(args[ARG_authmode].u_obj, &iter_buf);
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            authmodes |= cp_enum_value(&wifi_authmode_type, item, MP_QSTR_authmode);
        }
    }

    mp_buffer_info_t ssid;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);
    mp_arg_validate_length_range(ssid.len, 1, 32, MP_QSTR_ssid);

    mp_buffer_info_t password;
    mp_get_buffer_raise(args[ARG_password].u_obj, &password, MP_BUFFER_READ);
    if (authmodes == 0) {
        if (password.len == 0) {
            authmodes = AUTHMODE_OPEN;
        } else {
            authmodes = AUTHMODE_WPA | AUTHMODE_WPA2 | AUTHMODE_PSK;
        }
    }

    if (authmodes == AUTHMODE_OPEN && password.len > 0) {
        mp_raise_ValueError(translate("AuthMode.OPEN is not used with password"));
    }

    if (authmodes != AUTHMODE_OPEN) {
        mp_arg_validate_length_range(password.len, 8, 63, MP_QSTR_password);
    }

    common_hal_wifi_radio_start_ap(self, ssid.buf, ssid.len, password.buf, password.len, args[ARG_channel].u_int, authmodes, args[ARG_max_connections].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_radio_start_ap_obj, 1, wifi_radio_start_ap);

//|     def stop_ap(self) -> None:
//|         """Stops the Access Point."""
//|         ...
STATIC mp_obj_t wifi_radio_stop_ap(mp_obj_t self) {
    common_hal_wifi_radio_stop_ap(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_stop_ap_obj, wifi_radio_stop_ap);

//|     def connect(
//|         self,
//|         ssid: Union[str | ReadableBuffer],
//|         password: Union[str | ReadableBuffer] = b"",
//|         *,
//|         channel: int = 0,
//|         bssid: Optional[Union[str | ReadableBuffer]] = None,
//|         timeout: Optional[float] = None
//|     ) -> None:
//|         """Connects to the given ssid and waits for an ip address. Reconnections are handled
//|         automatically once one connection succeeds.
//|
//|         By default, this will scan all channels and connect to the access point (AP) with the
//|         given ``ssid`` and greatest signal strength (rssi).
//|
//|         If ``channel`` is non-zero, the scan will begin with the given channel and connect to
//|         the first AP with the given ``ssid``. This can speed up the connection time
//|         significantly because a full scan doesn't occur.
//|
//|         If ``bssid`` is given and not None, the scan will start at the first channel or the one given and
//|         connect to the AP with the given ``bssid`` and ``ssid``."""
//|         ...
STATIC mp_obj_t wifi_radio_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_channel, ARG_bssid, ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_password,  MP_ARG_OBJ, {.u_obj = mp_const_empty_bytes} },
        { MP_QSTR_channel, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
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
    ssid.len = 0;
    mp_get_buffer_raise(args[ARG_ssid].u_obj, &ssid, MP_BUFFER_READ);
    mp_arg_validate_length_range(ssid.len, 1, 32, MP_QSTR_ssid);

    mp_buffer_info_t password;
    password.len = 0;
    if (args[ARG_password].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_password].u_obj, &password, MP_BUFFER_READ);
        if (password.len != 0) {
            mp_arg_validate_length_range(password.len, 8, 63, MP_QSTR_password);
        }
    }

    #define MAC_ADDRESS_LENGTH 6

    mp_buffer_info_t bssid;
    bssid.len = 0;
    // Should probably make sure bssid is just bytes and not something else too
    if (args[ARG_bssid].u_obj != mp_const_none) {
        mp_get_buffer_raise(args[ARG_bssid].u_obj, &bssid, MP_BUFFER_READ);
        if (bssid.len != MAC_ADDRESS_LENGTH) {
            mp_raise_ValueError(translate("Invalid BSSID"));
        }
    }

    wifi_radio_error_t error = common_hal_wifi_radio_connect(self, ssid.buf, ssid.len, password.buf, password.len, args[ARG_channel].u_int, timeout, bssid.buf, bssid.len);
    if (error == WIFI_RADIO_ERROR_AUTH_FAIL) {
        mp_raise_ConnectionError(translate("Authentication failure"));
    } else if (error == WIFI_RADIO_ERROR_NO_AP_FOUND) {
        mp_raise_ConnectionError(translate("No network with that ssid"));
    } else if (error != WIFI_RADIO_ERROR_NONE) {
        mp_raise_msg_varg(&mp_type_ConnectionError, translate("Unknown failure %d"), error);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_radio_connect_obj, 1, wifi_radio_connect);

//|     ipv4_gateway: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the station gateway when connected to an access point. None otherwise."""
STATIC mp_obj_t wifi_radio_get_ipv4_gateway(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_gateway(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_gateway_obj, wifi_radio_get_ipv4_gateway);

MP_PROPERTY_GETTER(wifi_radio_ipv4_gateway_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_gateway_obj);

//|     ipv4_gateway_ap: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the access point gateway, when enabled. None otherwise."""
STATIC mp_obj_t wifi_radio_get_ipv4_gateway_ap(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_gateway_ap(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_gateway_ap_obj, wifi_radio_get_ipv4_gateway_ap);

MP_PROPERTY_GETTER(wifi_radio_ipv4_gateway_ap_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_gateway_ap_obj);

//|     ipv4_subnet: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the station subnet when connected to an access point. None otherwise."""
STATIC mp_obj_t wifi_radio_get_ipv4_subnet(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_subnet(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_subnet_obj, wifi_radio_get_ipv4_subnet);

MP_PROPERTY_GETTER(wifi_radio_ipv4_subnet_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_subnet_obj);

//|     ipv4_subnet_ap: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the access point subnet, when enabled. None otherwise."""
STATIC mp_obj_t wifi_radio_get_ipv4_subnet_ap(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_subnet_ap(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_subnet_ap_obj, wifi_radio_get_ipv4_subnet_ap);

MP_PROPERTY_GETTER(wifi_radio_ipv4_subnet_ap_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_subnet_ap_obj);

//|     def set_ipv4_address(
//|         self,
//|         *,
//|         ipv4: ipaddress.IPv4Address,
//|         netmask: ipaddress.IPv4Address,
//|         gateway: ipaddress.IPv4Address,
//|         ipv4_dns: Optional[ipaddress.IPv4Address]
//|     ) -> None:
//|         """Sets the IP v4 address of the station. Must include the netmask and gateway. DNS address is optional.
//|         Setting the address manually will stop the DHCP client."""
//|         ...
STATIC mp_obj_t wifi_radio_set_ipv4_address(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ipv4, ARG_netmask, ARG_gateway, ARG_ipv4_dns };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ipv4, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, },
        { MP_QSTR_netmask, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, },
        { MP_QSTR_gateway, MP_ARG_REQUIRED | MP_ARG_KW_ONLY | MP_ARG_OBJ, },
        { MP_QSTR_ipv4_dns, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = MP_OBJ_NULL} },
    };

    wifi_radio_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    common_hal_wifi_radio_set_ipv4_address(self, args[ARG_ipv4].u_obj, args[ARG_netmask].u_obj, args[ARG_gateway].u_obj, args[ARG_ipv4_dns].u_obj);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wifi_radio_set_ipv4_address_obj, 1, wifi_radio_set_ipv4_address);

//|     ipv4_address: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the station when connected to an access point. None otherwise."""
STATIC mp_obj_t _wifi_radio_get_ipv4_address(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_address(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_address_obj, _wifi_radio_get_ipv4_address);

MP_PROPERTY_GETTER(wifi_radio_ipv4_address_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_address_obj);

//|     ipv4_address_ap: Optional[ipaddress.IPv4Address]
//|     """IP v4 Address of the access point, when enabled. None otherwise."""
STATIC mp_obj_t wifi_radio_get_ipv4_address_ap(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_address_ap(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_address_ap_obj, wifi_radio_get_ipv4_address_ap);

MP_PROPERTY_GETTER(wifi_radio_ipv4_address_ap_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_address_ap_obj);

//|     ipv4_dns: ipaddress.IPv4Address
//|     """IP v4 Address of the DNS server to be used."""
STATIC mp_obj_t wifi_radio_get_ipv4_dns(mp_obj_t self) {
    return common_hal_wifi_radio_get_ipv4_dns(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ipv4_dns_obj, wifi_radio_get_ipv4_dns);

STATIC mp_obj_t wifi_radio_set_ipv4_dns(mp_obj_t self, mp_obj_t ipv4_dns_addr) {
    common_hal_wifi_radio_set_ipv4_dns(self, ipv4_dns_addr);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(wifi_radio_set_ipv4_dns_obj, wifi_radio_set_ipv4_dns);

MP_PROPERTY_GETSET(wifi_radio_ipv4_dns_obj,
    (mp_obj_t)&wifi_radio_get_ipv4_dns_obj,
    (mp_obj_t)&wifi_radio_set_ipv4_dns_obj);

//|     ap_info: Optional[Network]
//|     """Network object containing BSSID, SSID, authmode, channel, country and RSSI when connected to an access point. None otherwise."""
STATIC mp_obj_t wifi_radio_get_ap_info(mp_obj_t self) {
    return common_hal_wifi_radio_get_ap_info(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_get_ap_info_obj, wifi_radio_get_ap_info);

//|     def start_dhcp(self) -> None:
//|         """Starts the DHCP client."""
//|         ...
STATIC mp_obj_t wifi_radio_start_dhcp_client(mp_obj_t self) {
    common_hal_wifi_radio_start_dhcp_client(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_start_dhcp_client_obj, wifi_radio_start_dhcp_client);

//|     def stop_dhcp(self) -> None:
//|         """Stops the DHCP client. Needed to assign a static IP address."""
//|         ...
STATIC mp_obj_t wifi_radio_stop_dhcp_client(mp_obj_t self) {
    common_hal_wifi_radio_stop_dhcp_client(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_radio_stop_dhcp_client_obj, wifi_radio_stop_dhcp_client);

MP_PROPERTY_GETTER(wifi_radio_ap_info_obj,
    (mp_obj_t)&wifi_radio_get_ap_info_obj);

//|     def ping(
//|         self, ip: ipaddress.IPv4Address, *, timeout: Optional[float] = 0.5
//|     ) -> Optional[float]:
//|         """Ping an IP to test connectivity. Returns echo time in seconds.
//|         Returns None when it times out."""
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

    { MP_ROM_QSTR(MP_QSTR_hostname),    MP_ROM_PTR(&wifi_radio_hostname_obj) },

    { MP_ROM_QSTR(MP_QSTR_mac_address), MP_ROM_PTR(&wifi_radio_mac_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_mac_address_ap), MP_ROM_PTR(&wifi_radio_mac_address_ap_obj) },

    { MP_ROM_QSTR(MP_QSTR_tx_power), MP_ROM_PTR(&wifi_radio_tx_power_obj) },
    { MP_ROM_QSTR(MP_QSTR_start_scanning_networks),    MP_ROM_PTR(&wifi_radio_start_scanning_networks_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_scanning_networks),    MP_ROM_PTR(&wifi_radio_stop_scanning_networks_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_station),    MP_ROM_PTR(&wifi_radio_start_station_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_station),    MP_ROM_PTR(&wifi_radio_stop_station_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_ap),    MP_ROM_PTR(&wifi_radio_start_ap_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_ap),    MP_ROM_PTR(&wifi_radio_stop_ap_obj) },

    { MP_ROM_QSTR(MP_QSTR_start_dhcp),    MP_ROM_PTR(&wifi_radio_start_dhcp_client_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop_dhcp),    MP_ROM_PTR(&wifi_radio_stop_dhcp_client_obj) },

    { MP_ROM_QSTR(MP_QSTR_connect),    MP_ROM_PTR(&wifi_radio_connect_obj) },
    // { MP_ROM_QSTR(MP_QSTR_connect_to_enterprise),    MP_ROM_PTR(&wifi_radio_connect_to_enterprise_obj) },

    { MP_ROM_QSTR(MP_QSTR_ap_info),    MP_ROM_PTR(&wifi_radio_ap_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_dns),    MP_ROM_PTR(&wifi_radio_ipv4_dns_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_gateway),    MP_ROM_PTR(&wifi_radio_ipv4_gateway_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_gateway_ap),    MP_ROM_PTR(&wifi_radio_ipv4_gateway_ap_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_subnet),    MP_ROM_PTR(&wifi_radio_ipv4_subnet_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_subnet_ap),    MP_ROM_PTR(&wifi_radio_ipv4_subnet_ap_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_address),    MP_ROM_PTR(&wifi_radio_ipv4_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipv4_address_ap),    MP_ROM_PTR(&wifi_radio_ipv4_address_ap_obj) },

    { MP_ROM_QSTR(MP_QSTR_set_ipv4_address),    MP_ROM_PTR(&wifi_radio_set_ipv4_address_obj) },

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
