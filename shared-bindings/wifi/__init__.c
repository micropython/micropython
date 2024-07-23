// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Packet.h"
#include "shared-bindings/wifi/Radio.h"

//| """
//| The `wifi` module provides necessary low-level functionality for managing
//| wifi connections. Use `socketpool` for communicating over the network.
//| """

//| radio: Radio
//| """Wifi radio used to manage both station and AP modes.
//| This object is the sole instance of `wifi.Radio`."""

// Called when wifi is imported.
static mp_obj_t wifi___init__(void) {
    common_hal_wifi_init(true);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(wifi___init___obj, wifi___init__);

static const mp_rom_map_elem_t wifi_module_globals_table[] = {
    // Name
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_wifi) },

    // Initialization
    { MP_ROM_QSTR(MP_QSTR___init__),    MP_ROM_PTR(&wifi___init___obj) },

    // Classes
    { MP_ROM_QSTR(MP_QSTR_AuthMode),    MP_ROM_PTR(&wifi_authmode_type) },
    { MP_ROM_QSTR(MP_QSTR_Monitor),     MP_ROM_PTR(&wifi_monitor_type) },
    { MP_ROM_QSTR(MP_QSTR_Network),     MP_ROM_PTR(&wifi_network_type) },
    { MP_ROM_QSTR(MP_QSTR_Packet),      MP_ROM_PTR(&wifi_packet_type) },
    { MP_ROM_QSTR(MP_QSTR_Radio),       MP_ROM_PTR(&wifi_radio_type) },
    { MP_ROM_QSTR(MP_QSTR_Station),     MP_ROM_PTR(&wifi_radio_station_type) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_radio),       MP_ROM_PTR(&common_hal_wifi_radio_obj) },
};
static MP_DEFINE_CONST_DICT(wifi_module_globals, wifi_module_globals_table);

const mp_obj_module_t wifi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&wifi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_wifi, wifi_module);
