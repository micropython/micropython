// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <stdarg.h>

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

//| adapter: Adapter
//| """BLE Adapter used to manage device discovery and connections.
//| This object is the sole instance of `_bleio.Adapter`."""
//|

//| class BluetoothError(Exception):
//|     """Catchall exception for Bluetooth related errors."""
//|
//|     ...
//|
MP_DEFINE_BLEIO_EXCEPTION(BluetoothError, Exception)
NORETURN void mp_raise_bleio_BluetoothError(mp_rom_error_text_t fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_bleio_BluetoothError, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}

//| class RoleError(BluetoothError):
//|     """Raised when a resource is used as the mismatched role. For example, if a local CCCD is
//|     attempted to be set but they can only be set when remote."""
//|
//|     ...
//|
MP_DEFINE_BLEIO_EXCEPTION(RoleError, bleio_BluetoothError)
NORETURN void mp_raise_bleio_RoleError(mp_rom_error_text_t msg) {
    mp_raise_msg(&mp_type_bleio_RoleError, msg);
}

//| class SecurityError(BluetoothError):
//|     """Raised when a security related error occurs."""
//|
//|     ...
//|
MP_DEFINE_BLEIO_EXCEPTION(SecurityError, bleio_BluetoothError)
NORETURN void mp_raise_bleio_SecurityError(mp_rom_error_text_t fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    mp_obj_t exception = mp_obj_new_exception_msg_vlist(&mp_type_bleio_SecurityError, fmt, argptr);
    va_end(argptr);
    nlr_raise(exception);
}

// Called when _bleio is imported.
static mp_obj_t bleio___init__(void) {
// HCI cannot be enabled on import, because we need to setup the HCI adapter first.
    common_hal_bleio_init();
    #if !CIRCUITPY_BLEIO_HCI
    common_hal_bleio_adapter_set_enabled(&common_hal_bleio_adapter_obj, true);
    #endif
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(bleio___init___obj, bleio___init__);


// Need a forward reference due to mutual references.
#if CIRCUITPY_BLEIO_HCI
static mp_obj_dict_t bleio_module_globals;
#endif

//| def set_adapter(adapter: Optional[_bleio.Adapter]) -> None:
//|     """Set the adapter to use for BLE, such as when using an HCI adapter.
//|     Raises `NotImplementedError` when the adapter is a singleton and cannot be set."""
//|     ...
//|
mp_obj_t bleio_set_adapter(mp_obj_t adapter_obj) {
    #if CIRCUITPY_BLEIO_HCI
    (void)mp_arg_validate_type_or_none(adapter_obj, &bleio_adapter_type, MP_QSTR_adapter);

    // Equivalent of:
    // bleio.adapter = adapter_obj
    mp_map_elem_t *elem = mp_map_lookup(&bleio_module_globals.map, MP_ROM_QSTR(MP_QSTR_adapter), MP_MAP_LOOKUP);
    if (elem) {
        elem->value = adapter_obj;
    }
    #else
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Read-only"));
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bleio_set_adapter_obj, bleio_set_adapter);

#if CIRCUITPY_BLEIO_HCI
// Make the module dictionary be in RAM, so that _bleio.adapter can be set.
// Use a local macro to define how table entries should be converted.
#define OBJ_FROM_PTR MP_OBJ_FROM_PTR
static mp_map_elem_t bleio_module_globals_table[] = {
#else
#define OBJ_FROM_PTR MP_ROM_PTR
static const mp_rom_map_elem_t bleio_module_globals_table[] = {
    #endif
    // Name must be the first entry so that the exception printing below is correct.
    { MP_ROM_QSTR(MP_QSTR___name__),             MP_ROM_QSTR(MP_QSTR__bleio) },
    { MP_ROM_QSTR(MP_QSTR_Adapter),              OBJ_FROM_PTR(&bleio_adapter_type) },
    { MP_ROM_QSTR(MP_QSTR_Address),              OBJ_FROM_PTR(&bleio_address_type) },
    { MP_ROM_QSTR(MP_QSTR_Attribute),            OBJ_FROM_PTR(&bleio_attribute_type) },
    { MP_ROM_QSTR(MP_QSTR_Connection),           OBJ_FROM_PTR(&bleio_connection_type) },
    { MP_ROM_QSTR(MP_QSTR_Characteristic),       OBJ_FROM_PTR(&bleio_characteristic_type) },
    { MP_ROM_QSTR(MP_QSTR_CharacteristicBuffer), OBJ_FROM_PTR(&bleio_characteristic_buffer_type) },
    { MP_ROM_QSTR(MP_QSTR_Descriptor),           OBJ_FROM_PTR(&bleio_descriptor_type) },
    { MP_ROM_QSTR(MP_QSTR_PacketBuffer),         OBJ_FROM_PTR(&bleio_packet_buffer_type) },
    { MP_ROM_QSTR(MP_QSTR_ScanEntry),            OBJ_FROM_PTR(&bleio_scanentry_type) },
    { MP_ROM_QSTR(MP_QSTR_ScanResults),          OBJ_FROM_PTR(&bleio_scanresults_type) },
    { MP_ROM_QSTR(MP_QSTR_Service),              OBJ_FROM_PTR(&bleio_service_type) },
    { MP_ROM_QSTR(MP_QSTR_UUID),                 OBJ_FROM_PTR(&bleio_uuid_type) },

    #if CIRCUITPY_BLEIO_HCI
    // For HCI, _bleio.adapter is settable, and starts as None.
    { MP_ROM_QSTR(MP_QSTR_adapter),              mp_const_none },
    { MP_ROM_QSTR(MP_QSTR_set_adapter),          (mp_obj_t)&bleio_set_adapter_obj },
    #else
    // For non-HCI _bleio.adapter is a fixed singleton, and is not settable.
    // _bleio.set_adapter will raise NotImplementedError.
    { MP_ROM_QSTR(MP_QSTR_adapter),              MP_ROM_PTR(&common_hal_bleio_adapter_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_adapter),          MP_ROM_PTR(&bleio_set_adapter_obj) },
    #endif

    // Errors
    { MP_ROM_QSTR(MP_QSTR_BluetoothError),       OBJ_FROM_PTR(&mp_type_bleio_BluetoothError) },
    { MP_ROM_QSTR(MP_QSTR_RoleError),            OBJ_FROM_PTR(&mp_type_bleio_RoleError) },
    { MP_ROM_QSTR(MP_QSTR_SecurityError),        OBJ_FROM_PTR(&mp_type_bleio_SecurityError) },

    // Initialization
    { MP_ROM_QSTR(MP_QSTR___init__),             OBJ_FROM_PTR(&bleio___init___obj) },
};

#if CIRCUITPY_BLEIO_HCI
// Module dict is mutable to allow setting _bleio.adapter.
static MP_DEFINE_MUTABLE_DICT(bleio_module_globals, bleio_module_globals_table);
#else
static MP_DEFINE_CONST_DICT(bleio_module_globals, bleio_module_globals_table);
#endif

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
    .globals = (mp_obj_dict_t *)&bleio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__bleio, bleio_module);
