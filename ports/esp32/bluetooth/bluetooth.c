/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ayke van Laethem
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

#if MICROPY_PY_BLUETOOTH

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatts_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "py/mperrno.h"
#include "py/runtime.h"
#include "extmod/modbluetooth.h"

// Semaphore to serialze asynchronous calls.
STATIC SemaphoreHandle_t mp_bt_call_complete;
STATIC esp_bt_status_t mp_bt_call_status;
STATIC union {
    // Ugly hack to return values from an event handler back to a caller.
    esp_gatt_if_t gatts_if;
    uint16_t      service_handle;
} mp_bt_call_result;

STATIC mp_bt_adv_type_t bluetooth_adv_type;
STATIC uint16_t bluetooth_adv_interval;
STATIC uint16_t bluetooth_app_id = 0; // provide unique number for each application profile

STATIC void mp_bt_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
STATIC void mp_bt_gatts_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// Convert an esp_err_t into an errno number.
STATIC int mp_bt_esp_errno(esp_err_t err) {
    if (err != 0) {
        return MP_EPERM;
    }
    return 0;
}

// Convert the result of an asynchronous call to an errno value.
STATIC int mp_bt_status_errno() {
    if (mp_bt_call_status != ESP_BT_STATUS_SUCCESS) {
        return MP_EPERM;
    }
    return 0;
}

// Initialize at early boot.
void mp_bt_init(void) {
    esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    mp_bt_call_complete = xSemaphoreCreateBinary();
}

int mp_bt_enable(void) {
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_err_t err = esp_bt_controller_init(&bt_cfg);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_bluedroid_init();
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_bluedroid_enable();
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_ble_gap_register_callback(mp_bt_gap_callback);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_ble_gatts_register_callback(mp_bt_gatts_callback);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    return 0;
}

void mp_bt_disable(void) {
    esp_bluedroid_disable();
    esp_bluedroid_deinit();
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
}

bool mp_bt_is_enabled(void) {
    return esp_bluedroid_get_status() == ESP_BLUEDROID_STATUS_ENABLED;
}

STATIC esp_err_t mp_bt_advertise_start_internal(void) {
    esp_ble_adv_params_t ble_adv_params = {0,
        .adv_int_min       = bluetooth_adv_interval,
        .adv_int_max       = bluetooth_adv_interval,
        .adv_type          = bluetooth_adv_type,
        .own_addr_type     = BLE_ADDR_TYPE_PUBLIC,
        .channel_map       = ADV_CHNL_ALL,
        .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
    };
    return esp_ble_gap_start_advertising(&ble_adv_params);
}

int mp_bt_advertise_start(mp_bt_adv_type_t type, uint16_t interval, const uint8_t *adv_data, size_t adv_data_len, const uint8_t *sr_data, size_t sr_data_len) {
    if (adv_data != NULL) {
        esp_err_t err = esp_ble_gap_config_adv_data_raw((uint8_t*)adv_data, adv_data_len);
        if (err != 0) {
            return mp_bt_esp_errno(err);
        }
        xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
    }

    if (sr_data != NULL) {
        esp_err_t err = esp_ble_gap_config_scan_rsp_data_raw((uint8_t*)sr_data, sr_data_len);
        if (err != 0) {
            return mp_bt_esp_errno(err);
        }
        xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
    }

    bluetooth_adv_type = type;
    bluetooth_adv_interval = interval;
    esp_err_t err = mp_bt_advertise_start_internal();
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
    return mp_bt_status_errno();
}

void mp_bt_advertise_stop(void) {
    esp_err_t err = esp_ble_gap_stop_advertising();
    if (err != 0) {
        return;
    }
    xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
}

int mp_bt_add_service(mp_bt_service_t *service) {
    // In ESP-IDF, a service is more than just a service, it's an
    // "application profile". One application profile contains exactly one
    // service. For details, see:
    // https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/gatt_server/tutorial/Gatt_Server_Example_Walkthrough.md

    // Register an application profile.
    esp_err_t err = esp_ble_gatts_app_register(bluetooth_app_id);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    bluetooth_app_id++;
    // Wait for ESP_GATTS_REG_EVT
    xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
    if (mp_bt_call_status != 0) {
        return mp_bt_status_errno();
    }
    esp_gatt_if_t gatts_if = mp_bt_call_result.gatts_if;

    // Create the service.
    esp_gatt_srvc_id_t bluetooth_service_id;
    bluetooth_service_id.is_primary = true;
    bluetooth_service_id.id.inst_id = 0;
    bluetooth_service_id.id.uuid = service->uuid;
    err = esp_ble_gatts_create_service(gatts_if, &bluetooth_service_id, 1);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    // Wait for ESP_GATTS_CREATE_EVT
    xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
    if (mp_bt_call_status != 0) {
        return mp_bt_status_errno();
    }
    service->handle = mp_bt_call_result.service_handle;

    // Start the service.
    err = esp_ble_gatts_start_service(service->handle);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    // Wait for ESP_GATTS_START_EVT
    xSemaphoreTake(mp_bt_call_complete, portMAX_DELAY);
    return mp_bt_status_errno();
}

// Parse a UUID object from the caller.
void bluetooth_parse_uuid(mp_obj_t obj, mp_bt_uuid_t *uuid) {
    if (MP_OBJ_IS_SMALL_INT(obj) && MP_OBJ_SMALL_INT_VALUE(obj) == (uint32_t)(uint16_t)MP_OBJ_SMALL_INT_VALUE(obj)) {
        // Integer fits inside 16 bits, assume it's a standard UUID.
        uuid->len = ESP_UUID_LEN_16;
        uuid->uuid.uuid16 = MP_OBJ_SMALL_INT_VALUE(obj);
    } else if (mp_obj_is_str(obj)) {
        // Guessing this is a 128-bit (proprietary) UUID.
        uuid->len = ESP_UUID_LEN_128;
        bluetooth_parse_uuid_str(obj, &uuid->uuid.uuid128[0]);
    } else {
        mp_raise_ValueError("cannot parse UUID");
    }
}

// Event callbacks. Most API calls generate an event here to report the
// result.
STATIC void mp_bt_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
            xSemaphoreGive(mp_bt_call_complete);
            break;
        case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
            xSemaphoreGive(mp_bt_call_complete);
            break;
        case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
            mp_bt_call_status = param->adv_start_cmpl.status;
            // May return an error (queue full) when called from
            // mp_bt_gatts_callback, but that's OK.
            xSemaphoreGive(mp_bt_call_complete);
            break;
        case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
            xSemaphoreGive(mp_bt_call_complete);
            break;
        case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
            break;
        default:
            ESP_LOGI("bluetooth", "GAP: unknown event: %d", event);
            break;
    }
}

STATIC void mp_bt_gatts_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
        case ESP_GATTS_CONNECT_EVT:
            break;
        case ESP_GATTS_DISCONNECT_EVT:
            // restart advertisement
            mp_bt_advertise_start_internal();
            break;
        case ESP_GATTS_REG_EVT:
            // Application profile created.
            mp_bt_call_status = param->reg.status;
            mp_bt_call_result.gatts_if = gatts_if;
            xSemaphoreGive(mp_bt_call_complete);
            break;
        case ESP_GATTS_CREATE_EVT:
            // Service created.
            mp_bt_call_status = param->create.status;
            mp_bt_call_result.service_handle = param->create.service_handle;
            xSemaphoreGive(mp_bt_call_complete);
            break;
        case ESP_GATTS_START_EVT:
            // Service started.
            mp_bt_call_status = param->start.status;
            xSemaphoreGive(mp_bt_call_complete);
            break;
        default:
            ESP_LOGI("bluetooth", "GATTS: unknown event: %d", event);
            break;
    }
}

#endif // MICROPY_PY_BLUETOOTH
