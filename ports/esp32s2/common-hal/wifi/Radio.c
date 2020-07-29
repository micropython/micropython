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

bool common_hal_wifi_radio_get_enabled(wifi_radio_obj_t *self) {
	return true;
}

void common_hal_wifi_radio_set_enabled(wifi_radio_obj_t *self, bool enabled) {

}

mp_obj_t common_hal_wifi_radio_get_mac_address(wifi_radio_obj_t *self) {
	return mp_const_none;
}

mp_obj_t common_hal_wifi_radio_start_scanning_networks(wifi_radio_obj_t *self) {
	return mp_const_none;
}

void common_hal_wifi_radio_stop_scanning_networks(wifi_radio_obj_t *self) {

}

bool common_hal_wifi_radio_connect(wifi_radio_obj_t *self, uint8_t* ssid, size_t ssid_len, uint8_t* password, size_t password_len, mp_float_t timeout) {
	return false;
}

mp_obj_t common_hal_wifi_radio_get_ip_address(wifi_radio_obj_t *self) {
	return mp_const_none;
}

mp_int_t common_hal_wifi_radio_ping(wifi_radio_obj_t *self, mp_obj_t ip_address) {
	return 0;
}
