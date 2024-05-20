// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/Network.h"

//| class Network:
//|     """A wifi network provided by a nearby access point."""
//|

//|     def __init__(self) -> None:
//|         """You cannot create an instance of `wifi.Network`. They are returned by `wifi.Radio.start_scanning_networks`."""
//|         ...

//|     ssid: str
//|     """String id of the network"""
static mp_obj_t wifi_network_get_ssid(mp_obj_t self) {
    return common_hal_wifi_network_get_ssid(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_get_ssid_obj, wifi_network_get_ssid);

MP_PROPERTY_GETTER(wifi_network_ssid_obj,
    (mp_obj_t)&wifi_network_get_ssid_obj);


//|     bssid: bytes
//|     """BSSID of the network (usually the AP's MAC address)"""
static mp_obj_t wifi_network_get_bssid(mp_obj_t self) {
    return common_hal_wifi_network_get_bssid(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_get_bssid_obj, wifi_network_get_bssid);

MP_PROPERTY_GETTER(wifi_network_bssid_obj,
    (mp_obj_t)&wifi_network_get_bssid_obj);


//|     rssi: int
//|     """Signal strength of the network"""
static mp_obj_t wifi_network_get_rssi(mp_obj_t self) {
    return common_hal_wifi_network_get_rssi(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_get_rssi_obj, wifi_network_get_rssi);

MP_PROPERTY_GETTER(wifi_network_rssi_obj,
    (mp_obj_t)&wifi_network_get_rssi_obj);


//|     channel: int
//|     """Channel number the network is operating on"""
static mp_obj_t wifi_network_get_channel(mp_obj_t self) {
    return common_hal_wifi_network_get_channel(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_get_channel_obj, wifi_network_get_channel);

MP_PROPERTY_GETTER(wifi_network_channel_obj,
    (mp_obj_t)&wifi_network_get_channel_obj);

//|     country: str
//|     """String id of the country code"""
static mp_obj_t wifi_network_get_country(mp_obj_t self) {
    return common_hal_wifi_network_get_country(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_get_country_obj, wifi_network_get_country);

MP_PROPERTY_GETTER(wifi_network_country_obj,
    (mp_obj_t)&wifi_network_get_country_obj);

//|     authmode: str
//|     """String id of the authmode"""
//|
static mp_obj_t wifi_network_get_authmode(mp_obj_t self) {
    return common_hal_wifi_network_get_authmode(self);

}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_network_get_authmode_obj, wifi_network_get_authmode);

MP_PROPERTY_GETTER(wifi_network_authmode_obj,
    (mp_obj_t)&wifi_network_get_authmode_obj);

static const mp_rom_map_elem_t wifi_network_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ssid), MP_ROM_PTR(&wifi_network_ssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_bssid), MP_ROM_PTR(&wifi_network_bssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_rssi), MP_ROM_PTR(&wifi_network_rssi_obj) },
    { MP_ROM_QSTR(MP_QSTR_channel), MP_ROM_PTR(&wifi_network_channel_obj) },
    { MP_ROM_QSTR(MP_QSTR_country), MP_ROM_PTR(&wifi_network_country_obj) },
    { MP_ROM_QSTR(MP_QSTR_authmode), MP_ROM_PTR(&wifi_network_authmode_obj) },
};

static MP_DEFINE_CONST_DICT(wifi_network_locals_dict, wifi_network_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    wifi_network_type,
    MP_QSTR_Network,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    locals_dict, &wifi_network_locals_dict
    );
