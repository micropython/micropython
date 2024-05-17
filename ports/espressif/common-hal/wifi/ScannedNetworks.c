// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared/runtime/interrupt_char.h"
#include "py/gc.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/wifi/__init__.h"
#include "shared-bindings/wifi/Network.h"
#include "shared-bindings/wifi/Radio.h"
#include "shared-bindings/wifi/ScannedNetworks.h"

#include "components/esp_wifi/include/esp_wifi.h"

static void wifi_scannednetworks_done(wifi_scannednetworks_obj_t *self) {
    self->done = true;
    if (self->results != NULL) {
        // Check to see if the heap is still active. If not, it'll be freed automatically.
        if (gc_alloc_possible()) {
            m_free(self->results);
        }
        self->results = NULL;
    }
}

static bool wifi_scannednetworks_wait_for_scan(wifi_scannednetworks_obj_t *self) {
    EventBits_t bits = xEventGroupWaitBits(self->radio_event_group,
        WIFI_SCAN_DONE_BIT,
        pdTRUE,
        pdTRUE,
        0);
    while ((bits & WIFI_SCAN_DONE_BIT) == 0 && !mp_hal_is_interrupted()) {
        RUN_BACKGROUND_TASKS;
        bits = xEventGroupWaitBits(self->radio_event_group,
            WIFI_SCAN_DONE_BIT,
            pdTRUE,
            pdTRUE,
            0);
    }
    return !mp_hal_is_interrupted();
}

mp_obj_t common_hal_wifi_scannednetworks_next(wifi_scannednetworks_obj_t *self) {
    if (self->done) {
        return mp_const_none;
    }
    // If we are scanning, wait and then load them.
    if (self->channel_scan_in_progress) {
        // We may have to scan more than one channel to get a result.
        while (!self->done) {
            if (!wifi_scannednetworks_wait_for_scan(self)) {
                wifi_scannednetworks_done(self);
                return mp_const_none;
            }

            esp_wifi_scan_get_ap_num(&self->total_results);
            self->channel_scan_in_progress = false;
            if (self->total_results > 0) {
                break;
            }
            // If total_results is zero then we need to start a scan and wait again.
            wifi_scannednetworks_scan_next_channel(self);
        }
        // We not have found any more results so we're done.
        if (self->done) {
            return mp_const_none;
        }
        // If we need more space than we have, realloc.
        if (self->total_results > self->max_results) {
            wifi_ap_record_t *results = m_renew_maybe(wifi_ap_record_t,
                self->results,
                self->max_results,
                self->total_results,
                true /* allow move */);
            if (results != NULL) {
                self->results = results;
                self->max_results = self->total_results;
            } else {
                if (self->max_results == 0) {
                    // No room for any results should error.
                    mp_raise_msg(&mp_type_MemoryError, MP_ERROR_TEXT("Failed to allocate wifi scan memory"));
                }
                // Unable to allocate more results, so load what we can.
                self->total_results = self->max_results;
            }
        }
        esp_wifi_scan_get_ap_records(&self->total_results, self->results);
        self->channel_scan_in_progress = false;
    }

    wifi_network_obj_t *entry = mp_obj_malloc(wifi_network_obj_t, &wifi_network_type);
    memcpy(&entry->record, &self->results[self->current_result], sizeof(wifi_ap_record_t));
    self->current_result++;

    // If we're returning our last network then start the next channel scan or
    // be done.
    if (self->current_result >= self->total_results) {
        wifi_scannednetworks_scan_next_channel(self);
        self->total_results = 0;
        self->current_result = 0;
    }

    return MP_OBJ_FROM_PTR(entry);
}

// We don't do a linear scan so that we look at a variety of spectrum up front.
static uint8_t scan_pattern[] = {6, 1, 11, 3, 9, 13, 2, 4, 8, 12, 5, 7, 10, 14, 0};

void wifi_scannednetworks_scan_next_channel(wifi_scannednetworks_obj_t *self) {
    // There is no channel 0, so use that as a flag to indicate we've run out of channels to scan.
    uint8_t next_channel = 0;
    while (self->current_channel_index < sizeof(scan_pattern)) {
        next_channel = scan_pattern[self->current_channel_index];
        self->current_channel_index++;
        // Scan only channels that are in the specified range.
        if (self->start_channel <= next_channel && next_channel <= self->end_channel) {
            break;
        }
    }
    wifi_scan_config_t config = { 0 };
    config.channel = next_channel;
    if (next_channel == 0) {
        wifi_scannednetworks_done(self);
    } else {
        esp_err_t result = esp_wifi_scan_start(&config, false);
        if (result != ESP_OK) {
            wifi_scannednetworks_done(self);
        } else {
            self->channel_scan_in_progress = true;
        }
    }
}

void wifi_scannednetworks_deinit(wifi_scannednetworks_obj_t *self) {
    // if a scan is active, make sure and clean up the idf's buffer of results.
    if (self->channel_scan_in_progress) {
        esp_wifi_scan_stop();
        if (wifi_scannednetworks_wait_for_scan(self)) {
            // Discard the scanned records, one at a time, to avoid memory leaks.
            uint16_t number;
            do {
                number = 1;
                wifi_ap_record_t record;
                esp_wifi_scan_get_ap_records(&number, &record);
            } while (number > 0);
            // TODO: available in ESP-IDF v5.0; do instead of the above.
            // Discard scan results.
            // esp_wifi_clear_ap_list();
            self->channel_scan_in_progress = false;
        }
    }
    wifi_scannednetworks_done(self);
}
