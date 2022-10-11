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

#include "bindings/espidf/__init__.h"
#include "common-hal/wifi/__init__.h"
#include "shared-bindings/wifi/__init__.h"

#include "shared-bindings/ipaddress/IPv4Address.h"
#include "shared-bindings/wifi/Monitor.h"
#include "shared-bindings/wifi/Radio.h"

#include "py/mpstate.h"
#include "py/runtime.h"

#include "components/esp_wifi/include/esp_wifi.h"

#include "components/heap/include/esp_heap_caps.h"

wifi_radio_obj_t common_hal_wifi_radio_obj;

#include "components/log/include/esp_log.h"

#include "supervisor/port.h"
#include "supervisor/workflow.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#include "esp_ipc.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "nvs_flash.h"
#endif

static const char *TAG = "CP wifi";

STATIC void schedule_background_on_cp_core(void *arg) {
    #if CIRCUITPY_STATUS_BAR
    supervisor_status_bar_request_update(false);
    #endif

    // CircuitPython's VM is run in a separate FreeRTOS task from wifi callbacks. So, we have to
    // notify the main task every time in case it's waiting for us.
    port_wake_main_task();
}

static void event_handler(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data) {
    // This runs on the PRO CORE! It cannot share CP interrupt enable/disable
    // directly.
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
    // Use IPC to ensure we run schedule background on the same core as CircuitPython.
    #if defined(CONFIG_FREERTOS_UNICORE) && CONFIG_FREERTOS_UNICORE
    schedule_background_on_cp_core(NULL);
    #else
    // This only blocks until the start of the function. That's ok since the PRO
    // core shouldn't care what we do.
    esp_ipc_call(CONFIG_ESP_MAIN_TASK_AFFINITY, schedule_background_on_cp_core, NULL);
    #endif
}

static bool wifi_inited;
static bool wifi_ever_inited;
static bool wifi_user_initiated;

void common_hal_wifi_init(bool user_initiated) {
    wifi_radio_obj_t *self = &common_hal_wifi_radio_obj;

    if (wifi_inited) {
        if (user_initiated && !wifi_user_initiated) {
            common_hal_wifi_radio_set_enabled(self, true);
        }
        return;
    }
    wifi_inited = true;
    wifi_user_initiated = user_initiated;
    common_hal_wifi_radio_obj.base.type = &wifi_radio_type;

    if (!wifi_ever_inited) {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
    }
    wifi_ever_inited = true;

    self->netif = esp_netif_create_default_wifi_sta();
    self->ap_netif = esp_netif_create_default_wifi_ap();
    self->started = false;

    // Even though we just called esp_netif_create_default_wifi_sta,
    //   station mode isn't actually ready for use until esp_wifi_set_mode()
    //   is called and the configuration is loaded via esp_wifi_set_config().
    // Set both convenience flags to false so it's not forgotten.
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
    #ifdef CONFIG_IDF_TARGET_ESP32
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
    #endif
    esp_err_t result = esp_wifi_init(&config);
    if (result == ESP_ERR_NO_MEM) {
        mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate Wifi memory"));
    } else if (result != ESP_OK) {
        raise_esp_error(result);
    }
    // set station mode to avoid the default SoftAP
    common_hal_wifi_radio_start_station(self);
    // start wifi
    common_hal_wifi_radio_set_enabled(self, true);
}

void wifi_user_reset(void) {
    if (wifi_user_initiated) {
        wifi_reset();
        wifi_user_initiated = false;
    }
}

void wifi_reset(void) {
    if (!wifi_inited) {
        return;
    }
    common_hal_wifi_monitor_deinit(MP_STATE_VM(wifi_monitor_singleton));
    wifi_radio_obj_t *radio = &common_hal_wifi_radio_obj;
    common_hal_wifi_radio_set_enabled(radio, false);
    #ifndef CONFIG_IDF_TARGET_ESP32
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
    wifi_inited = false;
    #endif
    supervisor_workflow_request_background();
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

void ipaddress_ipaddress_to_esp_idf_ip4(mp_obj_t ip_address, esp_ip4_addr_t *esp_ip_address) {
    if (!mp_obj_is_type(ip_address, &ipaddress_ipv4address_type)) {
        mp_raise_ValueError(translate("Only IPv4 addresses supported"));
    }
    mp_obj_t packed = common_hal_ipaddress_ipv4address_get_packed(ip_address);
    size_t len;
    const char *bytes = mp_obj_str_get_data(packed, &len);
    esp_netif_set_ip4_addr(esp_ip_address, bytes[0], bytes[1], bytes[2], bytes[3]);
}

void common_hal_wifi_gc_collect(void) {
    common_hal_wifi_radio_gc_collect(&common_hal_wifi_radio_obj);
}
