#if MICROPY_PY_BLUETOOTH

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gatts_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "extmod/modupygatt.h"

// Semaphore to serialze asynchronous calls.
STATIC SemaphoreHandle_t mp_bt_call_complete;
STATIC esp_bt_status_t mp_bt_call_status;
STATIC union {
    // Ugly hack to return values from an event handler back to a caller.
    esp_gatt_if_t gatts_if;
    uint16_t      service_handle;
    uint16_t      attr_handle;
} mp_bt_call_result;

// Convert an esp_err_t into an errno number.
STATIC int mp_bt_esp_errno(esp_err_t err) {
    if (err != 0) {
        return MP_EPERM;
    }
    return 0;
}

// Convert the result of an asynchronous call to an errno value.
STATIC int mp_bt_status_errno(void) {
    if (mp_bt_call_status != ESP_BT_STATUS_SUCCESS) {
        return MP_EPERM;
    }
    return 0;
}

STATIC void mp_bt_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
//STATIC void mp_bt_gatts_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// Initialize at early boot.
void mp_bt_init(void) {
    printf("mp_bt_init\r\n");
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
    // err = esp_ble_gatts_register_callback(mp_bt_gatts_callback);
    // if (err != 0) {
    //     return mp_bt_esp_errno(err);
    // }
    // err = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    // if (err != 0) {
    //     return mp_bt_esp_errno(err);
    // }
    // err = esp_ble_gatt_set_local_mtu(500);
    // if (err != 0) {
    //     return mp_bt_esp_errno(err);
    // }
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

int mp_bt_scan(void) {
  esp_err_t err;
  static esp_ble_scan_params_t ble_scan_params = {
		.scan_type              = BLE_SCAN_TYPE_ACTIVE,
		.own_addr_type          = ESP_PUBLIC_ADDR,
		.scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
		.scan_interval          = 0x50,
		.scan_window            = 0x30
	};

  err = esp_ble_gap_set_scan_params(&ble_scan_params);
	if (err != 0) {
		return mp_bt_esp_errno(err);
	}
  err = esp_ble_gap_start_scanning(60);
	if (err != 0) {
		return mp_bt_esp_errno(err);
	}
  printf("Wait for scans...\r\n");

  return 0;
}

STATIC void mp_bt_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
    switch (event) {
        case ESP_GAP_SEARCH_INQ_RES_EVT:
            esp_ble_gap_cb_param_t *p = (esp_ble_gap_cb_param_t *)param;
            p->scan_rst.ble_adv[2]  = 0x00;
        		for (int i=0; i < sizeof(ibeacon_prefix); i++) {
        			if (p->scan_rst.ble_adv[i] != ibeacon_prefix[i]) {
        				return;
        			}
        		}
        		printf("BDA: %02x:%02x:%02x:%02x:%02x:%02x, RSSI %d",
        			p->scan_rst.bda[0],
        			p->scan_rst.bda[1],
        			p->scan_rst.bda[2],
        			p->scan_rst.bda[3],
        			p->scan_rst.bda[4],
        			p->scan_rst.bda[5],
        			p->scan_rst.rssi);

            xSemaphoreGive(mp_bt_call_complete);
            break;
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

#endif //MICROPY_PY_BLUETOOTH
