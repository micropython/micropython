/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdint.h>
#include <stdbool.h>

#include "modubluepy.h"

uint32_t ble_drv_stack_enable(void);

void ble_drv_stack_disable(void);

uint8_t ble_drv_stack_enabled(void);

void ble_drv_address_get(void);

void ble_drv_advertise(void);

bool ble_drv_uuid_add_vs(uint8_t * p_uuid, uint8_t * idx);

bool ble_drv_service_add(ubluepy_service_obj_t * p_service_obj);

bool ble_drv_characteristic_add(ubluepy_characteristic_obj_t * p_char_obj);

bool ble_drv_advertise_data(ubluepy_advertise_data_t * p_adv_params);

void ble_drv_gap_event_handler_set(mp_obj_t obs, ubluepy_gap_evt_callback_t evt_handler);

void ble_drv_gatts_event_handler_set(mp_obj_t obj, ubluepy_gatts_evt_callback_t evt_handler);

void ble_drv_attr_read(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

void ble_drv_attr_write(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

void ble_drv_attr_notify(uint16_t conn_handle, uint16_t handle, uint16_t len, uint8_t * p_data);

#endif // BLUETOOTH_LE_DRIVER_H__
