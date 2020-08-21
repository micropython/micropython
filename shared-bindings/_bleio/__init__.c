/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include "shared-bindings/_bleio/__init__.h"
#include "shared-bindings/_bleio/Address.h"
#include "shared-bindings/_bleio/Attribute.h"
#include "shared-bindings/_bleio/Characteristic.h"
#include "shared-bindings/_bleio/CharacteristicBuffer.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/Descriptor.h"
#include "shared-bindings/_bleio/PacketBuffer.h"
#include "shared-bindings/_bleio/ScanEntry.h"
#include "shared-bindings/_bleio/ScanResults.h"
#include "shared-bindings/_bleio/Service.h"
#include "shared-bindings/_bleio/UUID.h"

//| """Bluetooth Low Energy (BLE) communication
//|
//| The `_bleio` module provides necessary low-level functionality for communicating
//| using Bluetooth Low Energy (BLE). The '_' prefix indicates this module is meant
//| for internal use by libraries but not by the end user. Its API may change incompatibly
//| between minor versions of CircuitPython.
//| Please use the
//| `adafruit_ble <https://circuitpython.readthedocs.io/projects/ble/en/latest/>`_
//| CircuitPython library instead, which builds on `_bleio`, and
//| provides higher-level convenience functionality, including predefined beacons, clients,
//| servers."""
//|

//| adapter: Adapter
//| """BLE Adapter used to manage device discovery and connections.
//| This object is the sole instance of `_bleio.Adapter`."""
//|

//| class BluetoothError(Exception):
//|     """Catchall exception for Bluetooth related errors."""
//|     ...
MP_DEFINE_BLEIO_EXCEPTION(BluetoothError, Exception)

NORETURN void mp_raise_bleio_BluetoothError(const compressed_string_t* fmt, ...) {
    va_list argptr;
    va_start(argptr,fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_bleio_BluetoothError, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}
//| class ConnectionError(BluetoothError):
//|     """Raised when a connection is unavailable."""
//|     ...
//|
MP_DEFINE_BLEIO_EXCEPTION(ConnectionError, bleio_BluetoothError)
NORETURN void mp_raise_bleio_ConnectionError(const compressed_string_t* fmt, ...) {
    va_list argptr;
    va_start(argptr,fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_bleio_ConnectionError, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}

//| class RoleError(BluetoothError):
//|     """Raised when a resource is used as the mismatched role. For example, if a local CCCD is
//|     attempted to be set but they can only be set when remote."""
//|     ...
//|
MP_DEFINE_BLEIO_EXCEPTION(RoleError, bleio_BluetoothError)
NORETURN void mp_raise_bleio_RoleError(const compressed_string_t* msg) {
    mp_raise_msg(&mp_type_bleio_RoleError, msg);
}
//| class SecurityError(BluetoothError):
//|     """Raised when a security related error occurs."""
//|     ...
//|
MP_DEFINE_BLEIO_EXCEPTION(SecurityError, bleio_BluetoothError)
NORETURN void mp_raise_bleio_SecurityError(const compressed_string_t* fmt, ...) {
    va_list argptr;
    va_start(argptr,fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_bleio_SecurityError, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}

// Called when _bleio is imported.
STATIC mp_obj_t bleio___init__(void) {
#if !CIRCUITPY_BLEIO_HCI
    // HCI cannot be enabled on import, because we need to setup the HCI adapter first.
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, true);
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(bleio___init___obj, bleio___init__);


STATIC const mp_rom_map_elem_t bleio_module_globals_table[] = {
    // Name must be the first entry so that the exception printing below is correct.
    { MP_ROM_QSTR(MP_QSTR___name__),             MP_ROM_QSTR(MP_QSTR__bleio) },
    { MP_ROM_QSTR(MP_QSTR_Adapter),              MP_ROM_PTR(&bleio_adapter_type) },
    { MP_ROM_QSTR(MP_QSTR_Address),              MP_ROM_PTR(&bleio_address_type) },
    { MP_ROM_QSTR(MP_QSTR_Attribute),            MP_ROM_PTR(&bleio_attribute_type) },
    { MP_ROM_QSTR(MP_QSTR_Connection),           MP_ROM_PTR(&bleio_connection_type) },
    { MP_ROM_QSTR(MP_QSTR_Characteristic),       MP_ROM_PTR(&bleio_characteristic_type) },
    { MP_ROM_QSTR(MP_QSTR_CharacteristicBuffer), MP_ROM_PTR(&bleio_characteristic_buffer_type) },
    { MP_ROM_QSTR(MP_QSTR_Descriptor),           MP_ROM_PTR(&bleio_descriptor_type) },
    { MP_ROM_QSTR(MP_QSTR_PacketBuffer),         MP_ROM_PTR(&bleio_packet_buffer_type) },
    { MP_ROM_QSTR(MP_QSTR_ScanEntry),            MP_ROM_PTR(&bleio_scanentry_type) },
    { MP_ROM_QSTR(MP_QSTR_ScanResults),          MP_ROM_PTR(&bleio_scanresults_type) },
    { MP_ROM_QSTR(MP_QSTR_Service),              MP_ROM_PTR(&bleio_service_type) },
    { MP_ROM_QSTR(MP_QSTR_UUID),                 MP_ROM_PTR(&bleio_uuid_type) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_adapter),              MP_ROM_PTR(&common_hal_bleio_adapter_obj) },

    // Errors
    { MP_ROM_QSTR(MP_QSTR_BluetoothError),      MP_ROM_PTR(&mp_type_bleio_BluetoothError) },
    { MP_ROM_QSTR(MP_QSTR_ConnectionError),     MP_ROM_PTR(&mp_type_bleio_ConnectionError) },
    { MP_ROM_QSTR(MP_QSTR_RoleError),           MP_ROM_PTR(&mp_type_bleio_RoleError) },
    { MP_ROM_QSTR(MP_QSTR_SecurityError),       MP_ROM_PTR(&mp_type_bleio_SecurityError) },

    // Initialization
    { MP_ROM_QSTR(MP_QSTR___init__),            MP_ROM_PTR(&bleio___init___obj) },

};

STATIC MP_DEFINE_CONST_DICT(bleio_module_globals, bleio_module_globals_table);

void bleio_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind) {
    mp_print_kind_t k = kind & ~PRINT_EXC_SUBCLASS;
    bool is_subclass = kind & PRINT_EXC_SUBCLASS;
    if (!is_subclass && (k == PRINT_EXC)) {
        mp_print_str(print, qstr_str(MP_OBJ_QSTR_VALUE(bleio_module_globals_table[0].value)));
        mp_print_str(print, ".");
    }
    mp_obj_exception_print(print, o_in, kind);
}

const mp_obj_module_t bleio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&bleio_module_globals,
};
