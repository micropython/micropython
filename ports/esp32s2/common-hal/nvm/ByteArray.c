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

#include <string.h>

#include "common-hal/nvm/ByteArray.h"
#include "bindings/espidf/__init__.h"

#include "py/runtime.h"
#include "py/gc.h"
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

// Get a copy of the nvm data, or an array initialized to all 0 bytes if it is not present
static esp_err_t get_bytes(nvs_handle_t handle, uint8_t **buf_out) {
    size_t size;
    void *buf;
    esp_err_t result = nvs_get_blob(handle, "data", NULL, &size);
    if (result == ESP_ERR_NVS_NOT_FOUND) {
        size = CIRCUITPY_INTERNAL_NVM_SIZE;
    } else if (result != ESP_OK) {
        *buf_out = NULL;
        return result;
    }
    buf = gc_alloc(size, 0, false); // this SHOULD be the same as
    if (result == ESP_OK) {
        result = nvs_get_blob(handle, "data", buf, &size);
    } else {
        result = ESP_OK;
    }
    *buf_out = buf;
    return result;
}

bool common_hal_nvm_bytearray_set_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint8_t *values, uint32_t len) {

    // start nvs
    nvs_handle_t handle;
    get_nvs_handle(&handle);

    // get from flash
    uint8_t *buf = NULL;
    esp_err_t result = get_bytes(handle, &buf);
    if (result != ESP_OK) {
        gc_free(buf);
        nvs_close(handle);
        raise_esp_error(result);
    }

    // erase old data, including 6.3.x incompatible data
    result = nvs_erase_all(handle);
    if (result != ESP_OK) {
        gc_free(buf);
        nvs_close(handle);
        raise_esp_error(result);
    }

    // make our modification
    memcpy(buf + start_index, values, len);

    result = nvs_set_blob(handle, "data", buf, CIRCUITPY_INTERNAL_NVM_SIZE);
    if (result != ESP_OK) {
        gc_free(buf);
        nvs_close(handle);
        raise_esp_error(result);
    }

    result = nvs_commit(handle);
    if (result != ESP_OK) {
        gc_free(buf);
        nvs_close(handle);
        raise_esp_error(result);
    }

    // close nvs
    gc_free(buf);
    nvs_close(handle);
    return true;
}

void common_hal_nvm_bytearray_get_bytes(nvm_bytearray_obj_t *self,
    uint32_t start_index, uint32_t len, uint8_t *values) {

    // start nvs
    nvs_handle_t handle;
    get_nvs_handle(&handle);

    // get from flash
    uint8_t *buf;
    esp_err_t result = get_bytes(handle, &buf);
    if (result != ESP_OK) {
        gc_free(buf);
        nvs_close(handle);
        raise_esp_error(result);
    }

    // copy the subset of data requested
    memcpy(values, buf + start_index, len);

    // close nvs
    gc_free(buf);
    nvs_close(handle);
}
