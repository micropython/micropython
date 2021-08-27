/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "esp_error.h"
#include "py/runtime.h"

#include "bindings/espidf/__init__.h"

void raise_esp_error(esp_err_t err) {
    const compressed_string_t *msg = NULL;
    const mp_obj_type_t *exception_type = &mp_type_espidf_IDFError;
    switch (err) {
        case ESP_FAIL:
            msg = translate("Generic Failure");
            break;
        case ESP_ERR_NO_MEM:
            exception_type = &mp_type_espidf_MemoryError;
            msg = translate("Out of memory");
            break;
        case ESP_ERR_INVALID_ARG:
            msg = translate("Invalid argument");
            break;
        case ESP_ERR_INVALID_STATE:
            msg = translate("Invalid state");
            break;
        case ESP_ERR_INVALID_SIZE:
            msg = translate("Invalid size");
            break;
        case ESP_ERR_NOT_FOUND:
            msg = translate("Requested resource not found");
            break;
        case ESP_ERR_NOT_SUPPORTED:
            msg = translate("Operation or feature not supported");
            break;
        case ESP_ERR_TIMEOUT:
            msg = translate("Operation timed out");
            break;
        case ESP_ERR_INVALID_RESPONSE:
            msg = translate("Received response was invalid");
            break;
        case ESP_ERR_INVALID_CRC:
            msg = translate("CRC or checksum was invalid");
            break;
        case ESP_ERR_INVALID_VERSION:
            msg = translate("Version was invalid");
            break;
        case ESP_ERR_INVALID_MAC:
            msg = translate("MAC address was invalid");
            break;
    }
    if (msg) {
        mp_raise_msg(exception_type, msg);
    }

    const char *group = "ESP-IDF";

    // tests must be in descending order
    MP_STATIC_ASSERT(ESP_ERR_FLASH_BASE > ESP_ERR_MESH_BASE);
    MP_STATIC_ASSERT(ESP_ERR_MESH_BASE > ESP_ERR_WIFI_BASE);
    if (err >= ESP_ERR_FLASH_BASE) {
        group = "Flash";
    } else if (err >= ESP_ERR_MESH_BASE) {
        group = "Mesh";
    } else if (err >= ESP_ERR_WIFI_BASE) {
        group = "WiFi";
    }
    mp_raise_msg_varg(exception_type, translate("%s error 0x%x"), group, err);
}
