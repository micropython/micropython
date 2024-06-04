// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "common-hal/_bleio/Adapter.h"

#include "py/objstr.h"
#include "shared-module/_bleio/Address.h"

extern const mp_obj_type_t bleio_adapter_type;

#if CIRCUITPY_BLEIO_HCI
void common_hal_bleio_adapter_construct_hci_uart(bleio_adapter_obj_t *self, busio_uart_obj_t *uart, digitalio_digitalinout_obj_t *rts, digitalio_digitalinout_obj_t *cts);
#endif // CIRCUITPY_BLEIO_HCI

extern bool common_hal_bleio_adapter_get_advertising(bleio_adapter_obj_t *self);
extern bool common_hal_bleio_adapter_get_enabled(bleio_adapter_obj_t *self);
extern void common_hal_bleio_adapter_set_enabled(bleio_adapter_obj_t *self, bool enabled);
extern mp_int_t common_hal_bleio_adapter_get_tx_power(bleio_adapter_obj_t *self);
extern void common_hal_bleio_adapter_set_tx_power(bleio_adapter_obj_t *self, mp_int_t tx_power);
extern bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self);
extern bleio_address_obj_t *common_hal_bleio_adapter_get_address(bleio_adapter_obj_t *self);
extern bool common_hal_bleio_adapter_set_address(bleio_adapter_obj_t *self, bleio_address_obj_t *address);

// Copies the adapter name into the given buffer up to len and returns the full length (may be more
// than len if the buffer was too short.)
uint16_t bleio_adapter_get_name(char *buf, uint16_t len);
extern mp_obj_str_t *common_hal_bleio_adapter_get_name(bleio_adapter_obj_t *self);
extern void common_hal_bleio_adapter_set_name(bleio_adapter_obj_t *self, const char *name);

// Returns 0 if ok, otherwise a BLE stack specific error code.
extern uint32_t _common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, float interval,
    const uint8_t *advertising_data, uint16_t advertising_data_len,
    const uint8_t *scan_response_data, uint16_t scan_response_data_len,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to);

extern void common_hal_bleio_adapter_start_advertising(bleio_adapter_obj_t *self,
    bool connectable, bool anonymous, uint32_t timeout, mp_float_t interval,
    mp_buffer_info_t *advertising_data_bufinfo,
    mp_buffer_info_t *scan_response_data_bufinfo,
    mp_int_t tx_power, const bleio_address_obj_t *directed_to);
extern void common_hal_bleio_adapter_stop_advertising(bleio_adapter_obj_t *self);

extern mp_obj_t common_hal_bleio_adapter_start_scan(bleio_adapter_obj_t *self, uint8_t *prefixes, size_t prefix_length, bool extended, mp_int_t buffer_size, mp_float_t timeout, mp_float_t interval, mp_float_t window, mp_int_t minimum_rssi, bool active);
extern void common_hal_bleio_adapter_stop_scan(bleio_adapter_obj_t *self);

extern bool common_hal_bleio_adapter_get_connected(bleio_adapter_obj_t *self);
extern mp_obj_t common_hal_bleio_adapter_get_connections(bleio_adapter_obj_t *self);
extern mp_obj_t common_hal_bleio_adapter_connect(bleio_adapter_obj_t *self, bleio_address_obj_t *address, mp_float_t timeout);

extern void common_hal_bleio_adapter_erase_bonding(bleio_adapter_obj_t *self);
extern bool common_hal_bleio_adapter_is_bonded_to_central(bleio_adapter_obj_t *self);
