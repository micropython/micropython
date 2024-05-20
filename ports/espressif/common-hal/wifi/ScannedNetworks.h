// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

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
    bool channel_scan_in_progress;
} wifi_scannednetworks_obj_t;

void wifi_scannednetworks_scan_next_channel(wifi_scannednetworks_obj_t *self);
void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self);
