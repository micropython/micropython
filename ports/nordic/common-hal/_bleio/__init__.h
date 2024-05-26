// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

void bleio_background(void);

typedef struct {
    ble_gap_enc_key_t own_enc;
    ble_gap_enc_key_t peer_enc;
    ble_gap_id_key_t peer_id;
} bonding_keys_t;

// We assume variable length data.
// 20 bytes max (23 - 3).
#define GATT_MAX_DATA_LENGTH (BLE_GATT_ATT_MTU_DEFAULT - 3)

// These helpers raise the appropriate exceptions if the code doesn't equal success.
void check_nrf_error(uint32_t err_code);
void check_gatt_status(uint16_t gatt_status);
void check_sec_status(uint8_t sec_status);
