/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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
#include "py/runtime.h"

#include "esp_now.h"
#include "esp_log.h"

#define TAG "espidf"

#ifdef CONFIG_SPIRAM
#include "esp_psram.h"
#include "esp_heap_caps.h"
#include "esp_heap_caps_init.h"
#include "soc/soc.h"
#ifdef CONFIG_IDF_TARGET_ESP32
#include "esp32/himem.h"
#else
#define esp_himem_reserved_area_size() (0)
#endif
#endif

static size_t psram_size_usable(void) {
    #ifdef CONFIG_SPIRAM
    /* PSRAM chip may be larger than the size we can map into address space */
    size_t s = MIN(esp_psram_get_size(), SOC_EXTRAM_DATA_SIZE);
    return s - esp_himem_reserved_area_size();
    #else
    return 0;
    #endif
}

size_t common_hal_espidf_get_total_psram(void) {
    return psram_size_usable();
}

intptr_t common_hal_espidf_get_psram_start(void) {
    #ifdef CONFIG_SPIRAM
    return (intptr_t)esp_psram_get_address();
    #endif
    return 0;
}

intptr_t common_hal_espidf_get_psram_end(void) {
    #ifdef CONFIG_SPIRAM
    if (esp_psram_is_initialized()) {
        return common_hal_espidf_get_psram_start() + psram_size_usable();
    }
    #endif
    return 0;
}

void raise_esp_error(esp_err_t err) {
    mp_rom_error_text_t msg = NULL;
    const mp_obj_type_t *exception_type = &mp_type_espidf_IDFError;
    switch (err) {
        case ESP_FAIL:
            msg = MP_ERROR_TEXT("Generic Failure");
            break;
        case ESP_ERR_NO_MEM:
            exception_type = &mp_type_espidf_MemoryError;
            msg = MP_ERROR_TEXT("Out of memory");
            break;
        case ESP_ERR_INVALID_ARG:
            msg = MP_ERROR_TEXT("Invalid argument");
            break;
        case ESP_ERR_INVALID_STATE:
            msg = MP_ERROR_TEXT("Invalid state");
            break;
        case ESP_ERR_INVALID_SIZE:
            msg = MP_ERROR_TEXT("Invalid size");
            break;
        case ESP_ERR_NOT_FOUND:
            msg = MP_ERROR_TEXT("Requested resource not found");
            break;
        case ESP_ERR_NOT_SUPPORTED:
            msg = MP_ERROR_TEXT("Operation or feature not supported");
            break;
        case ESP_ERR_TIMEOUT:
            msg = MP_ERROR_TEXT("Operation timed out");
            break;
        case ESP_ERR_INVALID_RESPONSE:
            msg = MP_ERROR_TEXT("Received response was invalid");
            break;
        case ESP_ERR_INVALID_CRC:
            msg = MP_ERROR_TEXT("CRC or checksum was invalid");
            break;
        case ESP_ERR_INVALID_VERSION:
            msg = MP_ERROR_TEXT("Version was invalid");
            break;
        case ESP_ERR_INVALID_MAC:
            msg = MP_ERROR_TEXT("MAC address was invalid");
            break;
    }
    if (msg) {
        mp_raise_msg(exception_type, msg);
    }

    const char *group = "ESP-IDF";

    // tests must be in descending order
    MP_STATIC_ASSERT(ESP_ERR_FLASH_BASE > ESP_ERR_MESH_BASE);
    MP_STATIC_ASSERT(ESP_ERR_MESH_BASE > ESP_ERR_ESPNOW_BASE);
    MP_STATIC_ASSERT(ESP_ERR_ESPNOW_BASE > ESP_ERR_WIFI_BASE);

    if (err >= ESP_ERR_FLASH_BASE) {
        group = "Flash";
    } else if (err >= ESP_ERR_MESH_BASE) {
        group = "Mesh";
    } else if (err >= ESP_ERR_ESPNOW_BASE) {
        group = "ESP-NOW";
    } else if (err >= ESP_ERR_WIFI_BASE) {
        group = "WiFi";
    }

    mp_raise_msg_varg(exception_type, MP_ERROR_TEXT("%s error 0x%x"), group, err);
}

void cp_check_esp_error(esp_err_t err) {
    if (err == ESP_OK) {
        return;
    }
    raise_esp_error(err);
}

MP_REGISTER_MODULE(MP_QSTR_espidf, espidf_module);
