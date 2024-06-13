// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objlist.h"

#include "shared-bindings/_bleio/Adapter.h"

#include "common-hal/_bleio/__init__.h"
#include "common-hal/_bleio/Adapter.h"

extern bleio_adapter_obj_t common_hal_bleio_adapter_obj;

void bleio_exception_print(const mp_print_t *print, mp_obj_t o_in, mp_print_kind_t kind);

#define MP_DEFINE_BLEIO_EXCEPTION(exc_name, base_name) \
    MP_DEFINE_CONST_OBJ_TYPE(mp_type_bleio_##exc_name, MP_QSTR_##exc_name, MP_TYPE_FLAG_NONE, \
    make_new, mp_obj_exception_make_new, \
    print, bleio_exception_print, \
    attr, mp_obj_exception_attr, \
    parent, &mp_type_##base_name \
    );

extern const mp_obj_type_t mp_type_bleio_BluetoothError;
extern const mp_obj_type_t mp_type_bleio_RoleError;
extern const mp_obj_type_t mp_type_bleio_SecurityError;

// Resets all user created BLE state in preparation for the heap disappearing.
// It will maintain BLE workflow and connections.
void bleio_user_reset(void);

// Completely resets the BLE stack including BLE connections.
void bleio_reset(void);

// Init any state needed before calling any bleio functions including those
// having to do with bonding. This doesn't enable the BLE adapter though.
void common_hal_bleio_init(void);

extern mp_obj_t bleio_set_adapter(mp_obj_t adapter_obj);

NORETURN void mp_raise_bleio_BluetoothError(mp_rom_error_text_t msg, ...);
NORETURN void mp_raise_bleio_RoleError(mp_rom_error_text_t msg);
NORETURN void mp_raise_bleio_SecurityError(mp_rom_error_text_t msg, ...);

bleio_adapter_obj_t *common_hal_bleio_allocate_adapter_or_raise(void);
void common_hal_bleio_check_connected(uint16_t conn_handle);

uint16_t common_hal_bleio_device_get_conn_handle(mp_obj_t device);
void common_hal_bleio_device_discover_remote_services(mp_obj_t device, mp_obj_t service_uuids_whitelist);

size_t common_hal_bleio_gatts_read(uint16_t handle, uint16_t conn_handle, uint8_t *buf, size_t len);
void common_hal_bleio_gatts_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo);
size_t common_hal_bleio_gattc_read(uint16_t handle, uint16_t conn_handle, uint8_t *buf, size_t len);
void common_hal_bleio_gattc_write(uint16_t handle, uint16_t conn_handle, mp_buffer_info_t *bufinfo, bool write_no_response);

void common_hal_bleio_gc_collect(void);
