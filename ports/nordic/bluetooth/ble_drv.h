// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2016 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "ble.h"

#ifndef BLE_GATT_ATT_MTU_DEFAULT
    #define BLE_GATT_ATT_MTU_DEFAULT GATT_MTU_SIZE_DEFAULT
#endif

#define BLE_CONN_CFG_TAG_CUSTOM 1

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))
#define SEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000000) / (RESOLUTION))
#define UNITS_TO_SEC(TIME, RESOLUTION) (((TIME)*(RESOLUTION)) / 1000000)
// 0.625 msecs (625 usecs)
#define ADV_INTERVAL_UNIT_FLOAT_SECS (0.000625)
// Microseconds is the base unit. The macros above know that.
#define UNIT_0_625_MS (625)
#define UNIT_1_25_MS  (1250)
#define UNIT_10_MS    (10000)

typedef bool (*ble_drv_evt_handler_t)(ble_evt_t *, void *);

typedef enum {
    SD_FLASH_OPERATION_DONE,
    SD_FLASH_OPERATION_IN_PROGRESS,
    SD_FLASH_OPERATION_ERROR,
} sd_flash_operation_status_t;

// Flag indicating progress of internal flash operation.
extern volatile sd_flash_operation_status_t sd_flash_operation_status;

typedef struct ble_drv_evt_handler_entry {
    struct ble_drv_evt_handler_entry *next;
    void *param;
    ble_drv_evt_handler_t func;
} ble_drv_evt_handler_entry_t;

void ble_drv_reset(void);
void ble_drv_remove_heap_handlers(void);
void ble_drv_add_event_handler(ble_drv_evt_handler_t func, void *param);
void ble_drv_remove_event_handler(ble_drv_evt_handler_t func, void *param);
void ble_drv_gc_collect(void);

// Allow for user provided entries to prevent allocations outside the VM.
void ble_drv_add_event_handler_entry(ble_drv_evt_handler_entry_t *entry, ble_drv_evt_handler_t func, void *param);
