#include <string.h>

#include "py/runtime.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_smartconfig.h"

#include "mphalport.h"
#include "modsmartconfig.h"

static EventGroupHandle_t wifi_event_group;

static bool smartconfig_process_done = false;

// smartconfig info variables
static uint8_t ssid[33] = {0};
static uint8_t bssid[6] = {0};
static uint8_t password[65] = {0};
static uint8_t rvd_data[33] = {0};
static int8_t type = SC_TYPE_ESPTOUCH_AIRKISS;

// event_handler used for esp_event_handler_register()
static void event_handler(void *arg, esp_event_base_t event_base,
    int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT &&
        event_id == WIFI_EVENT_STA_START) {
        xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // wrong password or other situations may occurs this event
        xEventGroupSetBits(wifi_event_group, SMARTCONFIG_DONE_BIT);
    } else if (event_base == IP_EVENT &&
               event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    } else if (event_base == SC_EVENT &&
               event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT &&
               event_id == SC_EVENT_GOT_SSID_PSWD) {
        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;
        wifi_config_t wifi_config;

        bzero(&wifi_config, sizeof(wifi_config_t));
        memcpy(wifi_config.sta.ssid,     evt->ssid,     sizeof(wifi_config.sta.ssid));
        memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
        wifi_config.sta.bssid_set = evt->bssid_set;

        if (wifi_config.sta.bssid_set == true) {
            memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
        }

        memcpy(ssid,     evt->ssid,     sizeof(evt->ssid));
        memcpy(bssid,    evt->bssid,    sizeof(evt->bssid));
        memcpy(password, evt->password, sizeof(evt->password));
        type = evt->type;

        if (evt->type == SC_TYPE_ESPTOUCH_V2) {
            check_esp_err(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
        }

        check_esp_err(esp_wifi_disconnect());
        check_esp_err(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

        ESP_LOGI(TAG, "Try to connect to ap");

        esp_wifi_connect();
    } else if (event_base == SC_EVENT &&
               event_id == SC_EVENT_SEND_ACK_DONE) {
        xEventGroupSetBits(wifi_event_group, SMARTCONFIG_DONE_BIT);
        ESP_LOGI(TAG, "Send ack done");
    }
}

// task function used for xTaskCreate()
static void smartconfig_task(void *param) {
    check_esp_err(esp_smartconfig_set_type(type));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    check_esp_err(esp_smartconfig_start(&cfg));

    EventBits_t uxBits;

    while (1) {
        uxBits = xEventGroupWaitBits(
            wifi_event_group,
            CONNECTED_BIT | SMARTCONFIG_DONE_BIT,
            true, false, portMAX_DELAY
            );

        if (uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "Connected to ap");
        }

        if (uxBits & SMARTCONFIG_DONE_BIT) {
            ESP_LOGI(TAG, "Smartconfig done");
            smartconfig_process_done = true;
            smartconfig_stop();
            vTaskDelete(NULL);
        }
    }
}

// initialize info variables, register event handlers and restart wifi
static void smartconfig_init(void) {
    smartconfig_process_done = false;

    memset(ssid,     0, sizeof(ssid));
    memset(bssid,    0, sizeof(bssid));
    memset(password, 0, sizeof(password));
    memset(rvd_data, 0, sizeof(rvd_data));

    wifi_event_group = xEventGroupCreate();

    esp_err_t err = esp_wifi_stop();
    if (err == ESP_ERR_WIFI_NOT_INIT) {
        return;
    }
    check_esp_err(err);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    check_esp_err(esp_wifi_init(&cfg));

    check_esp_err(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,    &event_handler, NULL));
    check_esp_err(esp_event_handler_register(IP_EVENT,   IP_EVENT_STA_GOT_IP, &event_handler, NULL));
    check_esp_err(esp_event_handler_register(SC_EVENT,   ESP_EVENT_ANY_ID,    &event_handler, NULL));

    check_esp_err(esp_wifi_set_mode(WIFI_MODE_STA));
    check_esp_err(esp_wifi_start());
}

// get/set smartconfig protocol type
// smartconfig.type([type])
STATIC mp_obj_t smartconfig_type(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0 || args[0] == mp_const_none) {
        return mp_obj_new_int(type);
    } else {
        if (mp_obj_is_integer(args[0])) {
            type = mp_obj_get_int(args[0]);

            if (type > SC_TYPE_ESPTOUCH_V2 || type < SC_TYPE_ESPTOUCH) {
                type = SC_TYPE_ESPTOUCH_AIRKISS;
            }
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(smartconfig_type_obj, 0, 1, smartconfig_type);

STATIC mp_obj_t smartconfig_start(void) {
    smartconfig_stop();
    smartconfig_init();

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_start_obj, smartconfig_start);

STATIC mp_obj_t smartconfig_stop(void) {
    esp_smartconfig_stop();

    check_esp_err(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    check_esp_err(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    check_esp_err(esp_event_handler_unregister(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler));

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_stop_obj, smartconfig_stop);

STATIC mp_obj_t smartconfig_done(void) {
    return mp_obj_new_bool(smartconfig_process_done);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_done_obj, smartconfig_done);

// get smartconfig info
// return: tuple(ssid, password, bssid, type[, rvd_data])
//         rvd_data - EspTouch V2 reserved data
STATIC mp_obj_t smartconfig_info(void) {
    mp_obj_t info[] = {
        mp_obj_new_str((const char *)ssid, strlen((const char *)ssid)),
        mp_obj_new_str((const char *)password, strlen((const char *)password)),
        mp_obj_new_bytes(bssid, sizeof(bssid)),
        mp_obj_new_int(type),
        mp_obj_new_bytes(rvd_data, strlen((const char *)rvd_data))
    };

    if (rvd_data[0] == 0x00) {
        return mp_obj_new_tuple(4, info);
    }

    return mp_obj_new_tuple(5, info);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_info_obj, smartconfig_info);

// get ssid string
STATIC mp_obj_t smartconfig_ssid(void) {
    return mp_obj_new_str((const char *)ssid, strlen((const char *)ssid));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_ssid_obj, smartconfig_ssid);

// get password string
STATIC mp_obj_t smartconfig_password(void) {
    return mp_obj_new_str((const char *)password, strlen((const char *)password));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_password_obj, smartconfig_password);

// get bssid bytes
STATIC mp_obj_t smartconfig_bssid(void) {
    return mp_obj_new_bytes(bssid, sizeof(bssid));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_bssid_obj, smartconfig_bssid);

// get rvd_data bytes
STATIC mp_obj_t smartconfig_rvd_data(void) {
    return mp_obj_new_bytes(rvd_data, strlen((const char *)rvd_data));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_rvd_data_obj, smartconfig_rvd_data);

STATIC const mp_rom_map_elem_t smartconfig_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__),                 MP_ROM_QSTR(MP_QSTR_smartconfig)},
    {MP_ROM_QSTR(MP_QSTR_type),                     MP_ROM_PTR(&smartconfig_type_obj)},
    {MP_ROM_QSTR(MP_QSTR_start),                    MP_ROM_PTR(&smartconfig_start_obj)},
    {MP_ROM_QSTR(MP_QSTR_stop),                     MP_ROM_PTR(&smartconfig_stop_obj)},
    {MP_ROM_QSTR(MP_QSTR_done),                     MP_ROM_PTR(&smartconfig_done_obj)},
    {MP_ROM_QSTR(MP_QSTR_info),                     MP_ROM_PTR(&smartconfig_info_obj)},
    {MP_ROM_QSTR(MP_QSTR_ssid),                     MP_ROM_PTR(&smartconfig_ssid_obj)},
    {MP_ROM_QSTR(MP_QSTR_password),                 MP_ROM_PTR(&smartconfig_password_obj)},
    {MP_ROM_QSTR(MP_QSTR_bssid),                    MP_ROM_PTR(&smartconfig_bssid_obj)},
    {MP_ROM_QSTR(MP_QSTR_rvd_data),                 MP_ROM_PTR(&smartconfig_rvd_data_obj)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_ESPTOUCH),            MP_ROM_INT(SC_TYPE_ESPTOUCH)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_AIRKISS),             MP_ROM_INT(SC_TYPE_AIRKISS)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_ESPTOUCH_AIRKISS),    MP_ROM_INT(SC_TYPE_ESPTOUCH_AIRKISS)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_ESPTOUCH_V2),         MP_ROM_INT(SC_TYPE_ESPTOUCH_V2)},
};
STATIC MP_DEFINE_CONST_DICT(smartconfig_module_globals, smartconfig_module_globals_table);

const mp_obj_module_t smartconfig_user_cmodule = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&smartconfig_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_smartconfig, smartconfig_user_cmodule);
