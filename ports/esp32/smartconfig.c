#include "py/obj.h"
#include "py/runtime.h"

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_smartconfig.h"

static EventGroupHandle_t s_wifi_event_group;

static const int ESPTOUCH_DONE_BIT = BIT1;
static const char *TAG = "smartconfig";

static int found_ssid_and_password = false;
static uint8_t ssid[33] = {0};
static uint8_t password[65] = {0};
static uint8_t type = -1;
static uint8_t token = 0;

static void smartconfig_task(void * parm);


static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == SC_EVENT && event_id == SC_EVENT_SCAN_DONE) {
        ESP_LOGI(TAG, "Scan done");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_FOUND_CHANNEL) {
        ESP_LOGI(TAG, "Found channel");
    } else if (event_base == SC_EVENT && event_id == SC_EVENT_GOT_SSID_PSWD) {
        ESP_LOGI(TAG, "Got SSID and password");

        smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;

        memcpy(ssid, evt->ssid, sizeof(evt->ssid));
        memcpy(password, evt->password, sizeof(evt->password));
        type = evt->type;
        token = evt->token;

        ESP_LOGI(TAG, "SSID:%s", ssid);
        ESP_LOGI(TAG, "PASSWORD:%s", password);
        ESP_LOGI(TAG, "TYPE:%d", type);
        ESP_LOGI(TAG, "TOKEN:%d", token);

        found_ssid_and_password = true;

        xEventGroupSetBits(s_wifi_event_group, ESPTOUCH_DONE_BIT);
    }
}

static void initialise_wifi(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));

    xTaskCreate(smartconfig_task, "smartconfig_task", 4096, NULL, 3, NULL);
}

static void smartconfig_task(void * parm)
{
    EventBits_t uxBits;
    ESP_ERROR_CHECK(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS));
    smartconfig_start_config_t cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_smartconfig_start(&cfg));

    while (1) {
        uxBits = xEventGroupWaitBits(s_wifi_event_group, ESPTOUCH_DONE_BIT, true, false, portMAX_DELAY); 
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            ESP_ERROR_CHECK(esp_smartconfig_stop());
            vTaskDelete(NULL);
        }
    }
}

STATIC mp_obj_t smartconfig_start(void)
{
    initialise_wifi();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_start_obj, smartconfig_start);

STATIC mp_obj_t smartconfig_success(void)
{
    return mp_obj_new_bool(found_ssid_and_password);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_success_obj, smartconfig_success);

STATIC mp_obj_t smartconfig_info(void)
{
    mp_obj_t info[] = {
        mp_obj_new_str((const char *)ssid, strlen((const char *)ssid)),
        mp_obj_new_str((const char *)password, strlen((const char *)password)),
        mp_obj_new_int(type),
        mp_obj_new_int(token)
    };

    return mp_obj_new_tuple(4, info);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(smartconfig_info_obj, smartconfig_info);


STATIC const mp_rom_map_elem_t smartconfig_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_smartconfig)},
    {MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&smartconfig_start_obj)},
    {MP_ROM_QSTR(MP_QSTR_success), MP_ROM_PTR(&smartconfig_success_obj)},
    {MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&smartconfig_info_obj)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_UNKNOWN), MP_ROM_INT(-1)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_ESPTOUCH), MP_ROM_INT(SC_TYPE_ESPTOUCH)},
    {MP_ROM_QSTR(MP_QSTR_TYPE_AIRKISS), MP_ROM_INT(SC_TYPE_AIRKISS)},
};

STATIC MP_DEFINE_CONST_DICT(smartconfig_module_globals, smartconfig_module_globals_table);

const mp_obj_module_t smartconfig_user_cmodule = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&smartconfig_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_smartconfig, smartconfig_user_cmodule, 1);
