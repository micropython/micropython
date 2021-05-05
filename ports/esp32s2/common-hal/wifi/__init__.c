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

#include "common-hal/wifi/__init__.h"

#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/Radio.h"

#include "py/runtime.h"

#include "components/esp_wifi/include/esp_wifi.h"

#include "components/heap/include/esp_heap_caps.h"

wifi_radio_obj_t common_hal_wifi_radio_obj;

#include "components/log/include/esp_log.h"

static const char *TAG = "wifi";

static void event_handler(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data) {
    wifi_radio_obj_t *radio = arg;
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_SCAN_DONE:
                ESP_LOGW(TAG, "scan");
                xEventGroupSetBits(radio->event_group_handle, WIFI_SCAN_DONE_BIT);
                break;
            case WIFI_EVENT_AP_START:
                ESP_LOGW(TAG, "ap start");
                break;
            case WIFI_EVENT_AP_STOP:
                ESP_LOGW(TAG, "ap stop");
                break;
            case WIFI_EVENT_AP_STACONNECTED:
                break;
            case WIFI_EVENT_AP_STADISCONNECTED:
                break;
            case WIFI_EVENT_STA_START:
                ESP_LOGW(TAG, "sta start");
                break;
            case WIFI_EVENT_STA_STOP:
                ESP_LOGW(TAG, "sta stop");
                break;
            case WIFI_EVENT_STA_CONNECTED:
                ESP_LOGW(TAG, "connected");
                break;
            case WIFI_EVENT_STA_DISCONNECTED: {
                ESP_LOGW(TAG, "disconnected");
                wifi_event_sta_disconnected_t *d = (wifi_event_sta_disconnected_t *)event_data;
                uint8_t reason = d->reason;
                ESP_LOGW(TAG, "reason %d 0x%02x", reason, reason);
                if (radio->retries_left > 0 &&
                    reason != WIFI_REASON_AUTH_FAIL &&
                    reason != WIFI_REASON_NO_AP_FOUND &&
                    reason != WIFI_REASON_ASSOC_LEAVE) {
                    radio->retries_left--;
                    ESP_LOGI(TAG, "Retrying connect. %d retries remaining", radio->retries_left);
                    esp_wifi_connect();
                    return;
                }

                radio->last_disconnect_reason = reason;
                xEventGroupSetBits(radio->event_group_handle, WIFI_DISCONNECTED_BIT);
                break;
            }

            // Cases to handle later.
            // case WIFI_EVENT_STA_AUTHMODE_CHANGE:
            default: {
                ESP_LOGW(TAG, "event %d 0x%02x", event_id, event_id);
                break;
            }
        }
    }

    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGW(TAG, "got ip");
        radio->retries_left = radio->starting_retries;
        xEventGroupSetBits(radio->event_group_handle, WIFI_CONNECTED_BIT);
    }
}

static bool wifi_inited, wifi_ever_inited;

void common_hal_wifi_init(void) {
    wifi_inited = true;
    common_hal_wifi_radio_obj.base.type = &wifi_radio_type;

    if (!wifi_ever_inited) {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
    }
    wifi_ever_inited = true;

    wifi_radio_obj_t *self = &common_hal_wifi_radio_obj;
    self->netif = esp_netif_create_default_wifi_sta();
    self->ap_netif = esp_netif_create_default_wifi_ap();
    self->started = false;

    // Even though we just called esp_netif_create_default_wifi_sta,
    //   station mode isn't actually ready for use until esp_wifi_set_mode()
    //   is called and the configuration is loaded via esp_wifi_set_config().
    // Set both convienence flags to false so it's not forgotten.
    self->sta_mode = 0;
    self->ap_mode = 0;

    self->event_group_handle = xEventGroupCreateStatic(&self->event_group);
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        self,
        &self->handler_instance_all_wifi));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        self,
        &self->handler_instance_got_ip));

    wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t result = esp_wifi_init(&config);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate Wifi memory"));
    } else if (result != ESP_OK) {
        mp_raise_RuntimeError(translate("Failed to init wifi"));
    }
    // set station mode to avoid the default SoftAP
    esp_wifi_set_mode(WIFI_MODE_STA);
    // start wifi
    common_hal_wifi_radio_set_enabled(self, true);
}

void wifi_reset(void) {
    if (!wifi_inited) {
        return;
    }
    wifi_radio_obj_t *radio = &common_hal_wifi_radio_obj;
    common_hal_wifi_radio_set_enabled(radio, false);
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        radio->handler_instance_all_wifi));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        radio->handler_instance_got_ip));
    ESP_ERROR_CHECK(esp_wifi_deinit());
    esp_netif_destroy(radio->netif);
    radio->netif = NULL;
    esp_netif_destroy(radio->ap_netif);
    radio->ap_netif = NULL;
}

void ipaddress_ipaddress_to_esp_idf(mp_obj_t ip_address, ip_addr_t *esp_ip_address) {
    if (!mp_obj_is_type(ip_address, &ipaddress_ipv4address_type)) {
        mp_raise_ValueError(translate("Only IPv4 addresses supported"));
    }
    mp_obj_t packed = common_hal_ipaddress_ipv4address_get_packed(ip_address);
    size_t len;
    const char *bytes = mp_obj_str_get_data(packed, &len);

    IP_ADDR4(esp_ip_address, bytes[0], bytes[1], bytes[2], bytes[3]);
}

void common_hal_wifi_gc_collect(void) {
    common_hal_wifi_radio_gc_collect(&common_hal_wifi_radio_obj);
}
