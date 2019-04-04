/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#pragma once

#include <stdbool.h>
#include "bluetooth/bluetooth.h"
#include "py/obj.h"

typedef struct {
    mp_obj_base_t          base;
    mp_bt_uuid_t           uuid;
    mp_bt_service_handle_t handle;
    #if ESP_PLATFORM
    uint8_t                esp_gatts_if; /* Only for ESP-IDF */
    #endif
} mp_bt_service_t;

// A characteristic.
// Object fits in 4 words (1 GC object), with 1 byte unused at the end.
typedef struct {
    mp_obj_base_t                 base;
    mp_bt_uuid_t                  uuid;
    mp_bt_service_t               *service;
    mp_bt_characteristic_handle_t value_handle;
    uint8_t                       flags;
} mp_bt_characteristic_t;

// One entry in the linked list of write callbacks.
typedef struct _mp_bt_characteristic_callback_t {
    struct _mp_bt_characteristic_callback_t *next;
    mp_bt_characteristic_t                  *characteristic;
    mp_obj_t                                callback;
} mp_bt_characteristic_callback_t;

// Enables the Bluetooth stack. Returns errno on failure.
int mp_bt_enable(void);

// Disables the Bluetooth stack. Is a no-op when not enabled.
void mp_bt_disable(void);

// Returns true when the Bluetooth stack is enabled.
bool mp_bt_is_enabled(void);

// Start advertisement. Will re-start advertisement when already enabled.
// Returns errno on failure.
int mp_bt_advertise_start(mp_bt_adv_type_t type, uint16_t interval, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len);

// Stop advertisement. No-op when already stopped.
void mp_bt_advertise_stop(void);

// Call this when a central disconnects.
void mp_bt_connected(uint16_t conn_handle);

// Call this when a central connects.
void mp_bt_disconnected(uint16_t conn_handle);

// Add a service with the given list of characteristics.
int mp_bt_add_service(mp_bt_service_t *service, size_t num_characteristics, mp_bt_characteristic_t **characteristics);

// Set the given characteristic to the given value.
int mp_bt_characteristic_value_set(mp_bt_characteristic_t *characteristic, const void *value, size_t value_len);

// Set the given characteristic and notify a central using the given
// connection handle.
int mp_bt_characteristic_value_notify(mp_bt_characteristic_t *characteristic, uint16_t conn_handle, const void *value, size_t value_len);

// Read the characteristic value. The size of the buffer must be given in
// value_len, which will be updated with the actual value.
int mp_bt_characteristic_value_get(mp_bt_characteristic_t *characteristic, void *value, size_t *value_len);

// Call this when a characteristic is written to.
void mp_bt_characteristic_on_write(uint16_t value_handle, const void *value, size_t value_len);

// Parse an UUID object from the caller and stores the result in the uuid
// parameter. Must accept both strings and integers for 128-bit and 16-bit
// UUIDs.
void mp_bt_parse_uuid(mp_obj_t obj, mp_bt_uuid_t *uuid);

// Format an UUID object to be returned from a .uuid() call. May result in
// a small int or a string.
mp_obj_t mp_bt_format_uuid(mp_bt_uuid_t *uuid);

// Parse a string UUID object into the 16-byte buffer. The string must be
// the correct size, otherwise this function will throw an error.
void mp_bt_parse_uuid_str(mp_obj_t obj, uint8_t *uuid);

// Format a 128-bit UUID from the 16-byte buffer as a string.
mp_obj_t mp_bt_format_uuid_str(uint8_t *uuid);

// Data types of advertisement packet.
#define MP_BLE_GAP_AD_TYPE_FLAG                  (0x01)
#define MP_BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME   (0x09)

// Flags element of advertisement packet.
#define MP_BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE         (0x02)  // discoverable for everyone
#define MP_BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED         (0x04)  // BLE only - no classic BT supported
#define MP_BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE   (MP_BLE_GAP_ADV_FLAG_LE_GENERAL_DISC_MODE | MP_BLE_GAP_ADV_FLAG_BR_EDR_NOT_SUPPORTED)

#define MP_BLE_FLAG_READ     (1 << 1)
#define MP_BLE_FLAG_WRITE    (1 << 3)
#define MP_BLE_FLAG_NOTIFY   (1 << 4)
