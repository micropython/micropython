/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#ifndef BLUETOOTH_LE_DRIVER_H__
#define BLUETOOTH_LE_DRIVER_H__

#if BLUETOOTH_SD

#include <stdint.h>
#include <stdbool.h>

#include "modubluepy.h"

typedef struct {
    uint8_t   addr[6];
    uint8_t   addr_type;
} ble_drv_addr_t;

typedef struct {
    uint8_t * p_peer_addr;
    uint8_t   addr_type;
    bool      is_scan_resp;
    int8_t    rssi;
    uint8_t   data_len;
    uint8_t * p_data;
    uint8_t   adv_type;
} ble_drv_adv_data_t;

typedef struct {
    uint16_t uuid;
    uint8_t  uuid_type;
    uint16_t start_handle;
    uint16_t end_handle;
} ble_drv_service_data_t;

typedef struct {
    uint16_t uuid;
    uint8_t  uuid_type;
    uint8_t  props;
    uint16_t decl_handle;
    uint16_t value_handle;
} ble_drv_char_data_t;

typedef void (*ble_drv_gap_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gatts_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gattc_evt_callback_t)(mp_obj_t self, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_adv_evt_callback_t)(mp_obj_t self, uint16_t event_id, ble_drv_adv_data_t * data);
typedef void (*ble_drv_disc_add_service_callback_t)(mp_obj_t self, ble_drv_service_data_t * p_service_data);
typedef void (*ble_drv_disc_add_char_callback_t)(mp_obj_t self, ble_drv_char_data_t * p_desc_data);
typedef void (*ble_drv_gattc_char_data_callback_t)(mp_obj_t self, uint16_t length, uint8_t * p_data);

uint32_t ble_drv_stack_enable(void);

void ble_drv_stack_disable(void);

uint8_t ble_drv_stack_enabled(void);

void ble_drv_address_get(ble_drv_addr_t * p_addr);

bool ble_drv_uuid_add_vs(uint8_t * p_uuid, uint8_t * idx);

bool ble_drv_service_add(ubluepy_service_obj_t * p_service_obj);

bool ble_drv_characteristic_add(ubluepy_characteristic_obj_t * p_char_obj);

bool ble_drv_advertise_data(ubluepy_advertise_data_t * p_adv_params);

void ble_drv_advertise_stop(void);

void ble_drv_gap_event_handler_set(mp_obj_t obs, ble_drv_gap_evt_callback_t evt_handler);

void ble_drv_gatts_event_handler_set(mp_obj_t obj, ble_drv_gatts_evt_callback_t evt_handler);

void ble_drv_gattc_event_handler_set(mp_obj_t obj, ble_drv_gattc_evt_callback_t evt_handler);

void ble_drv_attr_s_read(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

void ble_drv_attr_c_read(uint16_t conn_handle, uint16_t handle, mp_obj_t obj, ble_drv_gattc_char_data_callback_t cb);

void ble_drv_attr_s_write(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

void ble_drv_attr_s_notify(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

void ble_drv_attr_c_write(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data, bool w_response);

void ble_drv_scan_start(void);

void ble_drv_scan_stop(void);

void ble_drv_adv_report_handler_set(mp_obj_t obj, ble_drv_adv_evt_callback_t evt_handler);

void ble_drv_connect(uint8_t * p_addr, uint8_t addr_type);

bool ble_drv_discover_services(mp_obj_t obj, uint16_t conn_handle, uint16_t start_handle, ble_drv_disc_add_service_callback_t cb);

bool ble_drv_discover_characteristic(mp_obj_t obj,
                                     uint16_t conn_handle,
                                     uint16_t start_handle,
                                     uint16_t end_handle,
                                     ble_drv_disc_add_char_callback_t cb);

void ble_drv_discover_descriptors(void);

#endif // BLUETOOTH_SD

#endif // BLUETOOTH_LE_DRIVER_H__
