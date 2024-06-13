// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "supervisor/background_callback.h"

#include "freertos/FreeRTOS.h" // IWYU pragma: keep; for BaseType_t

void bleio_background(void *data);

extern background_callback_t bleio_background_callback;

// typedef struct {
//     ble_gap_enc_key_t own_enc;
//     ble_gap_enc_key_t peer_enc;
//     ble_gap_id_key_t peer_id;
// } bonding_keys_t;

// We assume variable length data.
// 20 bytes max (23 - 3).
#define GATT_MAX_DATA_LENGTH (BLE_GATT_ATT_MTU_DEFAULT - 3)

#define NIMBLE_OK (0)

void check_nimble_error(int rc, const char *file, size_t line);
#define CHECK_NIMBLE_ERROR(rc) check_nimble_error(rc, __FILE__, __LINE__)
void check_ble_error(int error_code, const char *file, size_t line);
#define CHECK_BLE_ERROR(error_code) check_ble_error(error_code, __FILE__, __LINE__)
void check_notify(BaseType_t result);
#define CHECK_NOTIFY(result) check_notify(result)

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define SEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000000) / (RESOLUTION))
#define UNITS_TO_SEC(TIME, RESOLUTION) (((TIME)*(RESOLUTION)) / 1000000)
// 0.625 msecs (625 usecs)
#define ADV_INTERVAL_UNIT_FLOAT_SECS (0.000625)
// Microseconds is the base unit. The macros above know that.
#define UNIT_0_625_MS (625)
#define UNIT_1_MS  (1000)
#define UNIT_1_25_MS  (1250)
#define UNIT_10_MS    (10000)
