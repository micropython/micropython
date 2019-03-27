/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include "shared-bindings/bleio/__init__.h"
#include "shared-bindings/bleio/Address.h"
#include "shared-bindings/bleio/AddressType.h"
#include "shared-bindings/bleio/AdvertisementData.h"
#include "shared-bindings/bleio/Broadcaster.h"
#include "shared-bindings/bleio/Characteristic.h"
#include "shared-bindings/bleio/CharacteristicBuffer.h"
#include "shared-bindings/bleio/Descriptor.h"
#include "shared-bindings/bleio/Peripheral.h"
#include "shared-bindings/bleio/ScanEntry.h"
#include "shared-bindings/bleio/Scanner.h"
#include "shared-bindings/bleio/Service.h"
#include "shared-bindings/bleio/UUID.h"

//| :mod:`bleio` --- Bluetooth Low Energy functionality
//| ================================================================
//|
//| .. module:: bleio
//|   :synopsis: Bluetooth Low Energy functionality
//|   :platform: nRF
//|
//| The `bleio` module contains methods for managing the BLE adapter.
//|
//| Libraries
//|
//| .. toctree::
//|     :maxdepth: 3
//|
//|     Address
//|     AddressType
//|     AdvertisementData
//|     Adapter
//|     Broadcaster
//|     Characteristic
//|     CharacteristicBuffer
// Work-in-progress classes are omitted, and marked as :orphan: in their files.
//     Descriptor
//     Device
//|     Peripheral
//    ScanEntry
//    Scanner
//|     Service
//|     UUID
//|
//| .. attribute:: adapter
//|
//|   BLE Adapter information, such as enabled state as well as MAC
//|   address.
//|   This object is the sole instance of `bleio.Adapter`.
//|

STATIC const mp_rom_map_elem_t bleio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),          MP_ROM_QSTR(MP_QSTR_bleio) },
    { MP_ROM_QSTR(MP_QSTR_Address),           MP_ROM_PTR(&bleio_address_type) },
    { MP_ROM_QSTR(MP_QSTR_AdvertisementData), MP_ROM_PTR(&bleio_advertisementdata_type) },
    { MP_ROM_QSTR(MP_QSTR_Broadcaster),       MP_ROM_PTR(&bleio_broadcaster_type) },
    { MP_ROM_QSTR(MP_QSTR_Characteristic),    MP_ROM_PTR(&bleio_characteristic_type) },
    { MP_ROM_QSTR(MP_QSTR_CharacteristicBuffer),    MP_ROM_PTR(&bleio_characteristic_buffer_type) },
//    { MP_ROM_QSTR(MP_QSTR_Descriptor),        MP_ROM_PTR(&bleio_descriptor_type) },
    { MP_ROM_QSTR(MP_QSTR_Peripheral),        MP_ROM_PTR(&bleio_peripheral_type) },
// Hide work-in-progress.
//    { MP_ROM_QSTR(MP_QSTR_ScanEntry),         MP_ROM_PTR(&bleio_scanentry_type) },
//    { MP_ROM_QSTR(MP_QSTR_Scanner),           MP_ROM_PTR(&bleio_scanner_type) },
    { MP_ROM_QSTR(MP_QSTR_Service),           MP_ROM_PTR(&bleio_service_type) },
    { MP_ROM_QSTR(MP_QSTR_UUID),              MP_ROM_PTR(&bleio_uuid_type) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_adapter),           MP_ROM_PTR(&common_hal_bleio_adapter_obj) },

    // Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_AddressType),       MP_ROM_PTR(&bleio_addresstype_type) },
};

STATIC MP_DEFINE_CONST_DICT(bleio_module_globals, bleio_module_globals_table);

const mp_obj_module_t bleio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bleio_module_globals,
};
