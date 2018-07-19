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

#include "shared-module/bleio/AdvertisementData.h"
#include "shared-module/bleio/Characteristic.h"
#include "shared-module/bleio/Device.h"
#include "shared-module/bleio/ScanEntry.h"
#include "shared-module/bleio/Scanner.h"
#include "shared-module/bleio/Service.h"

typedef void (*ble_drv_gap_evt_callback_t)(bleio_device_obj_t *device, uint16_t event_id, uint16_t conn_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gatts_evt_callback_t)(bleio_device_obj_t *device, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_gattc_evt_callback_t)(bleio_device_obj_t *device, uint16_t event_id, uint16_t attr_handle, uint16_t length, uint8_t * data);
typedef void (*ble_drv_adv_evt_callback_t)(bleio_scanner_obj_t *scanner, bleio_scanentry_obj_t *entry);
typedef void (*ble_drv_gattc_char_data_callback_t)(bleio_characteristic_obj_t *self, uint16_t length, uint8_t * p_data);

uint32_t ble_drv_stack_enable(void);

void ble_drv_stack_disable(void);

uint8_t ble_drv_stack_enabled(void);

void ble_drv_address_get(bleio_address_obj_t *address);

bool ble_drv_uuid_add_vs(uint8_t * p_uuid, uint8_t * idx);

void ble_drv_service_add(bleio_service_obj_t *service);

bool ble_drv_characteristic_add(bleio_characteristic_obj_t *characteristic);

bool ble_drv_advertise_data(bleio_advertisement_data_t *p_adv_params);

void ble_drv_advertise_stop(void);

void ble_drv_gap_event_handler_set(bleio_device_obj_t *device, ble_drv_gap_evt_callback_t evt_handler);

void ble_drv_gatts_event_handler_set(bleio_device_obj_t *device, ble_drv_gatts_evt_callback_t evt_handler);

void ble_drv_gattc_event_handler_set(bleio_device_obj_t *device, ble_drv_gattc_evt_callback_t evt_handler);

void ble_drv_attr_s_read(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

void ble_drv_attr_c_read(bleio_characteristic_obj_t *characteristic, ble_drv_gattc_char_data_callback_t cb);

void ble_drv_attr_s_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo);

void ble_drv_attr_s_notify(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo);

void ble_drv_attr_c_write(bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *bufinfo);

void ble_drv_scan_start(uint16_t interval, uint16_t window);

void ble_drv_scan_continue(void);

void ble_drv_scan_stop(void);

void ble_drv_adv_report_handler_set(bleio_scanner_obj_t *self, ble_drv_adv_evt_callback_t evt_handler);

void ble_drv_connect(bleio_device_obj_t *device);

void ble_drv_disconnect(bleio_device_obj_t *device);

bool ble_drv_discover_services(bleio_device_obj_t *device, uint16_t start_handle);

bool ble_drv_discover_characteristic(bleio_device_obj_t *device, bleio_service_obj_t *service, uint16_t start_handle);

void ble_drv_discover_descriptors(void);

#endif // BLUETOOTH_SD

#endif // BLUETOOTH_LE_DRIVER_H__
