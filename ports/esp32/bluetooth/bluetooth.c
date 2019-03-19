#if MICROPY_PY_BLUETOOTH

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gattc_api.h"
#include "esp_gatt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "py/mperrno.h"
#include "py/runtime.h"
#include "extmod/modupygatt.h"

#define GATTC_TAG "uPygatt"
#define REMOTE_SERVICE_UUID        0x00FF
#define REMOTE_NOTIFY_CHAR_UUID    0xFF01
#define PROFILE_NUM      1
#define PROFILE_A_APP_ID 0
#define INVALID_HANDLE   0

/*
enum {
  BLE_ADDR_TYPE_PUBLIC = 0x00
  BLE_ADDR_TYPE_RANDOM = 0x01
  BLE_ADDR_TYPE_RPA_PUBLIC = 0x02
  BLE_ADDR_TYPE_RPA_RANDOM = 0x03
}
*/

char remote_device_name[] = "RK-G201S";
STATIC bool connect    = false;
STATIC bool get_server = false;
STATIC esp_gattc_char_elem_t *char_elem_result   = NULL;
STATIC esp_gattc_descr_elem_t *descr_elem_result = NULL;

/* Declare static functions */
STATIC void mp_bt_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
STATIC void mp_bt_gattc_callback(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);
STATIC void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param);

// Semaphore to serialze asynchronous calls.
STATIC SemaphoreHandle_t mp_bt_call_complete;
STATIC esp_bt_status_t mp_bt_call_status;
STATIC union {
    // Ugly hack to return values from an event handler back to a caller.
    esp_gatt_if_t gatts_if;
    uint16_t      service_handle;
    uint16_t      attr_handle;
} mp_bt_call_result;

struct gattc_profile_inst {
    esp_gattc_cb_t gattc_cb;
    uint16_t gattc_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_start_handle;
    uint16_t service_end_handle;
    uint16_t char_handle;
    esp_bd_addr_t remote_bda;
};

/* One gatt-based profile one app_id and one gattc_if, this array will store the gattc_if returned by ESP_GATTS_REG_EVT */
static struct gattc_profile_inst gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gattc_cb = gattc_profile_event_handler,
        .gattc_if = ESP_GATT_IF_NONE,       /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

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

STATIC esp_bt_uuid_t remote_filter_service_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_SERVICE_UUID,},
};

STATIC esp_bt_uuid_t remote_filter_char_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = REMOTE_NOTIFY_CHAR_UUID,},
};

STATIC esp_bt_uuid_t notify_descr_uuid = {
    .len = ESP_UUID_LEN_16,
    .uuid = {.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG,},
};

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
    err = esp_ble_gattc_register_callback(mp_bt_gattc_callback);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_ble_gattc_app_register(PROFILE_A_APP_ID);
    if (err != 0) {
        return mp_bt_esp_errno(err);
    }
    err = esp_ble_gatt_set_local_mtu(500);
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

int mp_bt_scan(void) {
  esp_err_t err;
  static esp_ble_scan_params_t ble_scan_params = {
    .scan_type              = BLE_SCAN_TYPE_ACTIVE,
    .own_addr_type          = BLE_ADDR_TYPE_PUBLIC,
    .scan_filter_policy     = BLE_SCAN_FILTER_ALLOW_ALL,
    .scan_interval          = 0x50,
    .scan_window            = 0x30,
    .scan_duplicate         = BLE_SCAN_DUPLICATE_ENABLE
	};

  err = esp_ble_gap_set_scan_params(&ble_scan_params);
	if (err != 0) {
		return mp_bt_esp_errno(err);
	}
  err = esp_ble_gap_start_scanning(10);
	if (err != 0) {
		return mp_bt_esp_errno(err);
	}
  printf("Wait for scans...\r\n");

  return 0;
}

void mp_bt_connect(char* device) {
  connect = true;
  mp_bt_scan();
}

STATIC void gattc_profile_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
    esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;
}

STATIC void mp_bt_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
  uint8_t *adv_name = NULL;
  uint8_t adv_name_len = 0;
    switch (event) {
      case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
        //scan start complete event to indicate scan start successfully or failed
        if (param->scan_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "scan start failed, error status = %x", param->scan_start_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "scan start success");
        xSemaphoreGive(mp_bt_call_complete);
        break;
      case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT:
        if (param->scan_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
            ESP_LOGE(GATTC_TAG, "scan stop failed, error status = %x", param->scan_stop_cmpl.status);
            break;
        }
        ESP_LOGI(GATTC_TAG, "stop scan successfully");
        break;
      case ESP_GAP_BLE_SCAN_RESULT_EVT:
        esp_log_buffer_hex(GATTC_TAG, param->scan_rst.bda, 6);
        ESP_LOGI(GATTC_TAG, "searched Adv Data Len %d, Scan Response Len %d", param->scan_rst.adv_data_len, param->scan_rst.scan_rsp_len);
        adv_name = esp_ble_resolve_adv_data(param->scan_rst.ble_adv, ESP_BLE_AD_TYPE_NAME_CMPL, &adv_name_len);
        ESP_LOGI(GATTC_TAG, "searched Device Name Len %d", adv_name_len);
        esp_log_buffer_char(GATTC_TAG, adv_name, adv_name_len);
        #if CONFIG_EXAMPLE_DUMP_ADV_DATA_AND_SCAN_RESP
          if (param->scan_rst.adv_data_len > 0) {
            ESP_LOGI(GATTC_TAG, "adv data:");
            esp_log_buffer_hex(GATTC_TAG, &param->scan_rst.ble_adv[0], param->scan_rst.adv_data_len);
          }
          if (param->scan_rst.scan_rsp_len > 0) {
            ESP_LOGI(GATTC_TAG, "scan resp:");
            esp_log_buffer_hex(GATTC_TAG, &param->scan_rst.ble_adv[param->scan_rst.adv_data_len], param->scan_rst.scan_rsp_len);
          }
        #endif
        ESP_LOGI(GATTC_TAG, "\n");

        ESP_LOGI(GATTC_TAG, "searched device %s\n", remote_device_name);
        if (adv_name != NULL && connect) {
            if (strlen(remote_device_name) == adv_name_len && strncmp((char *)adv_name, remote_device_name, adv_name_len) == 0) {
                ESP_LOGI(GATTC_TAG, "searched device %s\n", remote_device_name);
                ESP_LOGI(GATTC_TAG, "connect to the remote device.");
                esp_ble_gap_stop_scanning();
                ESP_LOGI(GATTC_TAG, "gattc_if: %u, bda: %x, ble_addr_type: %d\r\n", gl_profile_tab[PROFILE_A_APP_ID].gattc_if, param->scan_rst.bda, param->scan_rst.ble_addr_type);
                esp_ble_gattc_open(gl_profile_tab[PROFILE_A_APP_ID].gattc_if, param->scan_rst.bda, param->scan_rst.ble_addr_type, true);
            }
        }

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
        ESP_LOGI(GATTC_TAG, "update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
            param->update_conn_params.status,
            param->update_conn_params.min_int,
            param->update_conn_params.max_int,
            param->update_conn_params.conn_int,
            param->update_conn_params.latency,
            param->update_conn_params.timeout);
        xSemaphoreGive(mp_bt_call_complete);
        break;
      default:
          ESP_LOGI(GATTC_TAG, "GAP: unknown event: %d", event);
          break;
    }
}

STATIC void mp_bt_gattc_callback(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gattc_cb_param_t *param) {
  esp_ble_gattc_cb_param_t *p_data = (esp_ble_gattc_cb_param_t *)param;

  /* If event is register event, store the gattc_if for each profile */
  if (event == ESP_GATTC_REG_EVT) {
      if (param->reg.status == ESP_GATT_OK) {
          gl_profile_tab[param->reg.app_id].gattc_if = gattc_if;
      } else {
          ESP_LOGI(GATTC_TAG, "reg app failed, app_id %04x, status %d",
                  param->reg.app_id,
                  param->reg.status);
          return;
      }
  }

  /* If the gattc_if equal to profile A, call profile A cb handler,
   * so here call each profile's callback */
  do {
      int idx;
      for (idx = 0; idx < PROFILE_NUM; idx++) {
          if (gattc_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                  gattc_if == gl_profile_tab[idx].gattc_if) {
              if (gl_profile_tab[idx].gattc_cb) {
                  gl_profile_tab[idx].gattc_cb(event, gattc_if, param);
              }
          }
      }
  } while (0);
}

#endif //MICROPY_PY_BLUETOOTH
