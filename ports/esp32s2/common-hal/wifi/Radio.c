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
#include "shared-bindings/wifi/ScannedNetworks.h"

#include "esp-idf/components/esp_wifi/include/esp_wifi.h"

bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self) {
	return self->started;
}

void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled) {
	if (self->started && !enabled) {
		esp_wifi_stop();
		self->started = false;
		return;
	}
	if (!self->started && enabled) {
		esp_wifi_start();
		self->started = true;
		return;
	}
}

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
	uint8_t mac[6];
	esp_wifi_get_mac(ESP_IF_WIFI_STA, mac);
	return mp_const_none;
}

mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self) {
	if (self->current_scan != NULL) {
		mp_raise_RuntimeError(translate("Already scanning for wifi networks"));
	}
	// check enabled

	wifi_scannednetworks_obj_t *scan = m_new_obj(wifi_scannednetworks_obj_t);
	self->current_scan = scan;
	scan->base.type = &wifi_scannednetworks_type;
	scan->start_channel = 1;
	scan->end_channel = 11;
	scan->radio_event_group = self->event_group_handle;
	wifi_scannednetworks_scan_next_channel(scan);
	return scan;
}

void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self) {
	// Free the memory used to store the found aps.
	wifi_scannednetworks_deinit(self->current_scan);
	self->current_scan = NULL;
}

bool common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t* ssid, size_t ssid_len, uint8_t* password, size_t password_len, mp_float_t timeout) {
	// check enabled
	wifi_config_t config;
	esp_err_t result = esp_wifi_set_config(ESP_IF_WIFI_STA, &config);
	result = esp_wifi_connect();
	return result == ESP_OK;
}

mp_obj_t common_hal_wifi_radio_get_ip_address(wifi_radio_obj_t *self) {
	return mp_const_none;
}

mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address) {
	return 0;
}
