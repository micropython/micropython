/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "freertos/event_groups.h"

#include "py/runtime.h"

static const char *TAG = "sc";

wifi_config_t * wifi_config;
char phoneip[15] = {0};

smartconfig_status_t smartconfig_status;

void STATIC smartconfig_callback(smartconfig_status_t status, void *pdata) {
    switch(status) {
        case SC_STATUS_WAIT:
            ESP_LOGI(TAG, "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
            ESP_LOGI(TAG, "SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
            ESP_LOGI(TAG, "SC_STATUS_LINK");
            wifi_config = (wifi_config_t *)pdata;
            ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config) );
            ESP_ERROR_CHECK( esp_wifi_connect() );
            break;
        case SC_STATUS_LINK_OVER:
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
            if (pdata != NULL) {
                uint8_t phone_ip[4] = {0};
                memcpy(phone_ip, (uint8_t* )pdata, 4);
                ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
                sprintf(phoneip, "%d.%d.%d.%d", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            break;
    }
    smartconfig_status = status;
}

STATIC mp_obj_t espsmartconfig_set_type(size_t n_args, const mp_obj_t *args) {
    smartconfig_type_t tp = SC_TYPE_ESPTOUCH;
    if (n_args > 0) {
        tp = mp_obj_get_int(args[0]);
    }
    esp_smartconfig_set_type(tp);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(espsmartconfig_set_type_obj, 0, 1, espsmartconfig_set_type);

STATIC mp_obj_t espsmartconfig_start(void) {
    smartconfig_status = SC_STATUS_WAIT;
    esp_smartconfig_start(smartconfig_callback);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(espsmartconfig_start_obj, espsmartconfig_start);

STATIC mp_obj_t espsmartconfig_stop(void) {
    esp_smartconfig_stop();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(espsmartconfig_stop_obj, espsmartconfig_stop);

STATIC mp_obj_t espsmartconfig_getssid(void) {
    if (wifi_config != NULL) {
        char ssid[32] = {0};
        sprintf(ssid, "%s", wifi_config->sta.ssid);
        return mp_obj_new_str(ssid, strlen(ssid));
    } else {
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_0(espsmartconfig_getssid_obj, espsmartconfig_getssid);

STATIC mp_obj_t espsmartconfig_getpassword(void) {
    if (wifi_config != NULL) {
        char password[64] = {0};
        sprintf(password, "%s", wifi_config->sta.password);
        return mp_obj_new_str(password, strlen(password));
    } else {
        return mp_const_none;
    }
}

MP_DEFINE_CONST_FUN_OBJ_0(espsmartconfig_getpassword_obj, espsmartconfig_getpassword);

STATIC mp_obj_t espsmartconfig_getphoneip(void) {
    return mp_obj_new_str(phoneip, strlen(phoneip));
}

MP_DEFINE_CONST_FUN_OBJ_0(espsmartconfig_getphoneip_obj, espsmartconfig_getphoneip);

STATIC mp_obj_t espsmartconfig_status(void) {
    return mp_obj_new_int(smartconfig_status);
}

MP_DEFINE_CONST_FUN_OBJ_0(espsmartconfig_status_obj, espsmartconfig_status);


STATIC const mp_map_elem_t mo_module_espsmartconfig_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_smartconfig) },
    { MP_ROM_QSTR(MP_QSTR_set_type), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_set_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_start_obj) },
    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_ssid), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_getssid_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_password), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_getpassword_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_phoneip), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_getphoneip_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR((mp_obj_t *)&espsmartconfig_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_SC_TYPE_ESPTOUCH), MP_ROM_INT(SC_TYPE_ESPTOUCH) },
    { MP_ROM_QSTR(MP_QSTR_SC_TYPE_AIRKISS), MP_ROM_INT(SC_TYPE_AIRKISS) },
    { MP_ROM_QSTR(MP_QSTR_SC_TYPE_ESPTOUCH_AIRKISS), MP_ROM_INT(SC_TYPE_ESPTOUCH_AIRKISS) },
    { MP_ROM_QSTR(MP_QSTR_SC_STATUS_WAIT), MP_ROM_INT((mp_uint_t)SC_STATUS_WAIT) },
    { MP_ROM_QSTR(MP_QSTR_SC_STATUS_FIND_CHANNEL), MP_ROM_INT((mp_uint_t)SC_STATUS_FIND_CHANNEL) },
    { MP_ROM_QSTR(MP_QSTR_SC_STATUS_GETTING_SSID_PSWD), MP_ROM_INT((mp_uint_t)SC_STATUS_GETTING_SSID_PSWD) },
    { MP_ROM_QSTR(MP_QSTR_SC_STATUS_LINK), MP_ROM_INT((mp_uint_t)SC_STATUS_LINK) },
    { MP_ROM_QSTR(MP_QSTR_SC_STATUS_LINK_OVER), MP_ROM_INT((mp_uint_t)SC_STATUS_LINK_OVER) },
};

STATIC MP_DEFINE_CONST_DICT(mo_module_espsmartconfig_globals, mo_module_espsmartconfig_globals_table);

const mp_obj_module_t mp_module_espsmartconfig = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mo_module_espsmartconfig_globals,
};
