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

#ifndef MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_ADAPTER_H
#define MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_ADAPTER_H

#include "py/obj.h"
#include "py/objtuple.h"
#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanResults.h"
#include "supervisor/background_callback.h"

#ifndef BLEIO_TOTAL_CONNECTION_COUNT
#define BLEIO_TOTAL_CONNECTION_COUNT 5
#endif

#define BLEIO_HANDLE_INVALID 0xffff
#define BLE_GAP_ADDR_LEN 6

extern bleio_connection_internal_t
    bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

extern EventGroupHandle_t xscan_event;

typedef struct {
    mp_obj_base_t base;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    background_callback_t background_callback;
    bool user_advertising;
    bool is_enable;
    uint8_t advertising_handle;
} bleio_adapter_obj_t;

typedef struct {
    uint8_t addr_id_peer : 1;
    uint8_t addr_type : 7;
    uint8_t addr[BLE_GAP_ADDR_LEN];
} ble_gap_addr_t;

typedef struct {
    // Pointer to the data buffer provided to/from the application.
    uint8_t *p_data;
    // Length of the data buffer, in bytes.
    uint16_t len;
} ble_data_t;

typedef struct {
    uint8_t address_type;
    uint8_t conn_handle;
    uint8_t data_len;
    int8_t rssi;
    bd_addr address;
    uint8_t data[255];
} device_scan_info_t;

void set_scan_device_info_on_ble_evt(bd_addr address, uint8_t address_type,
    int8_t rssi, uint8array *data);
void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter);
void bleio_adapter_reset(bleio_adapter_obj_t *adapter);
void common_hal_bleio_adapter_remove_connection(uint8_t conn_handle);

#endif  // MICROPY_INCLUDED_EFR_COMMON_HAL_BLEIO_ADAPTER_H
