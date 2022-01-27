/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_BLEIO_INIT_H
#define MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_BLEIO_INIT_H

void bleio_background(void);

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

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define SEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000000) / (RESOLUTION))
#define UNITS_TO_SEC(TIME, RESOLUTION) (((TIME)*(RESOLUTION)) / 1000000)
// 0.625 msecs (625 usecs)
#define ADV_INTERVAL_UNIT_FLOAT_SECS (0.000625)
// Microseconds is the base unit. The macros above know that.
#define UNIT_0_625_MS (625)
#define UNIT_1_25_MS  (1250)
#define UNIT_10_MS    (10000)

#endif // MICROPY_INCLUDED_ESPRESSIF_COMMON_HAL_BLEIO_INIT_H
