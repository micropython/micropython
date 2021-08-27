/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_ADAPTER_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_ADAPTER_H

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanResults.h"

#include "supervisor/background_callback.h"

#ifndef BLEIO_TOTAL_CONNECTION_COUNT
#define BLEIO_TOTAL_CONNECTION_COUNT 5
#endif

extern bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

typedef struct {
    mp_obj_base_t base;
    // Pointer to buffers we maintain so that the data is long lived.
    uint8_t *advertising_data;
    uint8_t *scan_response_data;
    // Pointer to current data.
    const uint8_t *current_advertising_data;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    ble_drv_evt_handler_entry_t connection_handler_entry;
    ble_drv_evt_handler_entry_t advertising_handler_entry;
    background_callback_t background_callback;
    bool user_advertising;
} bleio_adapter_obj_t;

void bleio_adapter_gc_collect(bleio_adapter_obj_t *adapter);
void bleio_adapter_reset(bleio_adapter_obj_t *adapter);

#endif // MICROPY_INCLUDED_NRF_COMMON_HAL_BLEIO_ADAPTER_H
