/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_ESP32S2_COMMON_HAL_WIFI_SCANNEDNETWORKS_H
#define MICROPY_INCLUDED_ESP32S2_COMMON_HAL_WIFI_SCANNEDNETWORKS_H

#include <stdint.h>

#include "py/obj.h"

#include "FreeRTOS.h"
#include "freertos/event_groups.h"

#include "components/esp_wifi/include/esp_wifi_types.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t current_channel_index;
    EventGroupHandle_t radio_event_group;

    // Results from the last channel scan
    wifi_ap_record_t *results;
    uint16_t current_result;
    uint16_t total_results;
    uint16_t max_results;

    // Limits on what channels to scan.
    uint8_t start_channel;
    uint8_t end_channel; // Inclusive

    bool done;
    bool scanning;
} wifi_scannednetworks_obj_t;

void wifi_scannednetworks_scan_next_channel(wifi_scannednetworks_obj_t *self);
void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_WIFI_SCANNEDNETWORKS_H
