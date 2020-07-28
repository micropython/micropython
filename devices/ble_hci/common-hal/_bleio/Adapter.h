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

#ifndef MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ADAPTER_H
#define MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ADAPTER_H

#include "py/obj.h"
#include "py/objtuple.h"

#include "shared-bindings/_bleio/Connection.h"
#include "shared-bindings/_bleio/ScanResults.h"
#include "shared-bindings/busio/UART.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

#ifndef BLEIO_TOTAL_CONNECTION_COUNT
#define BLEIO_TOTAL_CONNECTION_COUNT 5
#endif

extern bleio_connection_internal_t bleio_connections[BLEIO_TOTAL_CONNECTION_COUNT];

typedef struct _bleio_adapter_obj_t {
    mp_obj_base_t base;
    bleio_scanresults_obj_t *scan_results;
    mp_obj_t name;
    mp_obj_tuple_t *connection_objs;
    busio_uart_obj_t* hci_uart;
    digitalio_digitalinout_obj_t *rts_digitalinout;
    digitalio_digitalinout_obj_t *cts_digitalinout;
    bool now_advertising;
    bool extended_advertising;
    bool circuitpython_advertising;
    bool enabled;

    // Used to monitor advertising timeout for legacy avertising.
    uint64_t advertising_start_ticks;
    uint64_t advertising_timeout_msecs;  // If zero, do not check.

    uint16_t max_acl_buffer_len;
    uint16_t max_acl_num_buffers;
    uint16_t max_adv_data_len;
    uint8_t features[8];        // Supported BLE features.

    // All the local attributes for this device. The index into the list
    // corresponds to the handle.
    mp_obj_list_t *attributes;
} bleio_adapter_obj_t;

void bleio_adapter_background(bleio_adapter_obj_t* adapter);
void bleio_adapter_gc_collect(bleio_adapter_obj_t* adapter);
void bleio_adapter_reset(bleio_adapter_obj_t* adapter);

#endif // MICROPY_INCLUDED_BLE_HCI_COMMON_HAL_ADAPTER_H
