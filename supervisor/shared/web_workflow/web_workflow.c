/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/wifi/Radio.h"
#include "supervisor/shared/translate/translate.h"
#include "supervisor/shared/web_workflow/web_workflow.h"

#if CIRCUITPY_WIFI
#include "shared-bindings/wifi/__init__.h"
#endif

#if CIRCUITPY_DOTENV
#include "shared-module/dotenv/__init__.h"
#endif

static wifi_radio_error_t wifi_status = WIFI_RADIO_ERROR_NONE;

void supervisor_web_workflow_status(void) {
    serial_write_compressed(translate("Wi-Fi: "));
    if (common_hal_wifi_radio_get_enabled(&common_hal_wifi_radio_obj)) {
        uint32_t ipv4_address = wifi_radio_get_ipv4_address(&common_hal_wifi_radio_obj);
        if (wifi_status == WIFI_RADIO_ERROR_AUTH_EXPIRE ||
            wifi_status == WIFI_RADIO_ERROR_AUTH_FAIL) {
            serial_write_compressed(translate("Authentication failure"));
        } else if (wifi_status != WIFI_RADIO_ERROR_NONE) {
            mp_printf(&mp_plat_print, "%d", wifi_status);
        } else if (ipv4_address == 0) {
            serial_write_compressed(translate("No IP"));
        } else {
            uint8_t *octets = (uint8_t *)&ipv4_address;
            mp_printf(&mp_plat_print, "%d.%d.%d.%d", octets[0], octets[1], octets[2], octets[3]);
            // TODO: Use these unicode to show signal strength: ▂▄▆█
        }
    } else {
        serial_write_compressed(translate("off"));
    }
}

void supervisor_start_web_workflow(void) {
    #if CIRCUITPY_WEB_WORKFLOW && CIRCUITPY_WIFI
    char ssid[33];
    char password[64];
    mp_int_t ssid_len = 0;
    mp_int_t password_len = 0;

    #if CIRCUITPY_DOTENV
    ssid_len = dotenv_get_key("/.env", "CIRCUITPY_WIFI_SSID", ssid, sizeof(ssid) - 1);
    password_len = dotenv_get_key("/.env", "CIRCUITPY_WIFI_PASSWORD", password, sizeof(password) - 1);
    #endif
    if (ssid_len <= 0 || (size_t)ssid_len >= sizeof(ssid) ||
        password_len <= 0 || (size_t)password_len >= sizeof(password)) {
        return;
    }
    common_hal_wifi_init(false);
    common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, true);

    // TODO: Do our own scan so that we can find the channel we want before calling connect.
    // Otherwise, connect will do a full slow scan to pick the best AP.

    // NUL terminate the strings because dotenv doesn't.
    ssid[ssid_len] = '\0';
    password[password_len] = '\0';
    wifi_status = common_hal_wifi_radio_connect(
        &common_hal_wifi_radio_obj, (uint8_t *)ssid, ssid_len, (uint8_t *)password, password_len,
        0, 0.1, NULL, 0);

    if (wifi_status != WIFI_RADIO_ERROR_NONE) {
        common_hal_wifi_radio_set_enabled(&common_hal_wifi_radio_obj, false);
    }
    #endif
}

void supervisor_stop_web_workflow(void) {
}
