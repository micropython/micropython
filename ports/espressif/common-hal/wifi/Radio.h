/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ESP32S2_COMMON_HAL_WIFI_RADIO_H
#define MICROPY_INCLUDED_ESP32S2_COMMON_HAL_WIFI_RADIO_H

#include "py/obj.h"

#include "components/esp_event/include/esp_event.h"

#include "shared-bindings/wifi/ScannedNetworks.h"
#include "shared-bindings/wifi/Network.h"

// Event bits for the Radio event group.
#define WIFI_SCAN_DONE_BIT BIT0
#define WIFI_CONNECTED_BIT BIT1
#define WIFI_DISCONNECTED_BIT BIT2

typedef struct {
    mp_obj_base_t base;
    esp_event_handler_instance_t handler_instance_all_wifi;
    esp_event_handler_instance_t handler_instance_got_ip;
    wifi_scannednetworks_obj_t *current_scan;
    StaticEventGroup_t event_group;
    EventGroupHandle_t event_group_handle;
    wifi_config_t sta_config;
    wifi_network_obj_t ap_info;
    esp_netif_ip_info_t ip_info;
    esp_netif_dns_info_t dns_info;
    esp_netif_t *netif;
    bool started;
    bool ap_mode;
    bool sta_mode;
    uint8_t retries_left;
    uint8_t starting_retries;
    uint8_t last_disconnect_reason;

    wifi_config_t ap_config;
    esp_netif_ip_info_t ap_ip_info;
    esp_netif_t *ap_netif;
} wifi_radio_obj_t;

extern void common_hal_wifi_radio_gc_collect(wifi_radio_obj_t *self);

#endif // MICROPY_INCLUDED_ESP32S2_COMMON_HAL_WIFI_RADIO_H
