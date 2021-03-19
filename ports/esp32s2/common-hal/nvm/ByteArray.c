/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "common-hal/nvm/ByteArray.h"

#include "py/runtime.h"
#include "nvs_flash.h"

uint32_t common_hal_nvm_bytearray_get_length(nvm_bytearray_obj_t *self) {
    return self->len;
}

static void get_nvs_handle(nvs_handle_t *nvs_handle) {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // Open NVS handle
    if (nvs_open("CPY", NVS_READWRITE, nvs_handle) != ESP_OK) {
        mp_raise_RuntimeError(translate("NVS Error"));
    }
}

bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {
    char index[9];

    // start nvs
    nvs_handle_t handle;
    get_nvs_handle(&handle);

    // stage flash changes
    for (uint32_t i = 0; i < len; i++) {
        sprintf(index, "%i", start_index + i);
        if (nvs_set_u8(handle, (const char *)index, values[i]) != ESP_OK) {
            return false;
        }
    }

    // commit flash changes
    if (nvs_commit(handle) != ESP_OK) {
        return false;
    }

    // close nvs
    nvs_close(handle);
    return true;
}

void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {
    char index[9];

    // start nvs
    nvs_handle_t handle;
    get_nvs_handle(&handle);

    // get from flash
    for (uint32_t i = 0; i < len; i++) {
        sprintf(index, "%i", start_index + i);
        if (nvs_get_u8(handle, (const char *)index, &values[i]) != ESP_OK) {
            mp_raise_RuntimeError(translate("NVS Error"));
        }
    }

    // close nvs
    nvs_close(handle);
}
