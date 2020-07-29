/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/wifi/Radio.h"

#include "py/runtime.h"

#include "esp-idf/components/esp_wifi/include/esp_wifi.h"

wifi_radio_obj_t common_hal_wifi_radio_obj;

void common_hal_wifi_init(void) {
	common_hal_wifi_radio_obj.base.type = &wifi_radio_type;
	wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();

	esp_err_t result = esp_wifi_init(&config);
	if (result == ESP_ERR_NO_MEM) {
		mp_raise_msg(&mp_type_MemoryError, translate("Failed to allocate Wifi memory"));
	} else if (result != ESP_OK) {
		// handle this
	}
}

void wifi_reset(void) {
	esp_wifi_deinit();
}
