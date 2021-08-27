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

#include "py/objexcept.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/AuthMode.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Radio.h"

//| """
//| The `wifi` module provides necessary low-level functionality for managing
//| wifi connections. Use `socketpool` for communicating over the network."""
//|
//| radio: Radio
//| """Wifi radio used to manage both station and AP modes.
//| This object is the sole instance of `wifi.Radio`."""
//|


// Called when wifi is imported.
STATIC mp_obj_t wifi___init__(void) {
    common_hal_wifi_init();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(wifi___init___obj, wifi___init__);


STATIC const mp_rom_map_elem_t wifi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),    MP_ROM_QSTR(MP_QSTR_wifi) },
    { MP_ROM_QSTR(MP_QSTR_Radio),       MP_ROM_PTR(&wifi_radio_type) },
    { MP_ROM_QSTR(MP_QSTR_Network),     MP_ROM_PTR(&wifi_network_type) },
    { MP_ROM_QSTR(MP_QSTR_AuthMode),    MP_ROM_PTR(&wifi_authmode_type) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_radio),       MP_ROM_PTR(&common_hal_wifi_radio_obj) },

    // Initialization
    { MP_ROM_QSTR(MP_QSTR___init__),    MP_ROM_PTR(&wifi___init___obj) },

};

STATIC MP_DEFINE_CONST_DICT(wifi_module_globals, wifi_module_globals_table);


const mp_obj_module_t wifi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&wifi_module_globals,
};
