// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>

#include "common-hal/wifi/Network.h"

#include "py/objstr.h"

extern const mp_obj_type_t wifi_network_type;

extern mp_obj_t common_hal_wifi_network_get_ssid(wifi_network_obj_t *self);
extern mp_obj_t common_hal_wifi_network_get_bssid(wifi_network_obj_t *self);
extern mp_obj_t common_hal_wifi_network_get_rssi(wifi_network_obj_t *self);
extern mp_obj_t common_hal_wifi_network_get_channel(wifi_network_obj_t *self);
extern mp_obj_t common_hal_wifi_network_get_country(wifi_network_obj_t *self);
extern mp_obj_t common_hal_wifi_network_get_authmode(wifi_network_obj_t *self);
