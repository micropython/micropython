/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_INIT_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_INIT_H

#include <stdbool.h>
#include <stdio.h>
#include "shared-bindings/_bleio/UUID.h"

#include "gatt_db.h"
#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_bgapi.h"
#include "sl_bluetooth.h"
#include "sl_bt_rtos_adaptation.h"
#include "sl_cmsis_os2_common.h"
#include <cmsis_os2.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define SEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000000) / (RESOLUTION))
#define UNITS_TO_SEC(TIME, RESOLUTION) (((TIME)*(RESOLUTION)) / 1000000)

// We assume variable length data. 20 bytes max (23 - 3).
#define GATT_MAX_DATA_LENGTH (BT_ATT_DEFAULT_LE_MTU - 3)

#define BLE_GATT_HANDLE_INVALID 0x0000
#define BLE_CONN_HANDLE_INVALID 0xFFFF

// Maximum length for fixed length Attribute Values.
#define BLE_GATTS_FIX_ATTR_LEN_MAX (510)

// Maximum length for variable length Attribute Values.
#define BLE_GATTS_VAR_ATTR_LEN_MAX (512)

// Track if the user code modified the BLE state
// to know if we need to undo it on reload.
extern bool vm_used_ble;

// UUID shared by all CCCD's.
extern bleio_uuid_obj_t cccd_uuid;
extern void bleio_reset();

extern osMutexId_t bluetooth_connection_mutex_id;
extern const osMutexAttr_t bluetooth_connection_mutex_attr;
typedef enum {
    DISCOVER_SERVICES,
    DISCOVER_CHARACTERISTICS,
    RUNNING
} conn_state_t;

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_INIT_H
