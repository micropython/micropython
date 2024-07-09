// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lwip/api.h"
#include "components/esp_wifi/include/esp_wifi.h"

void wifi_reset(void);

void ipaddress_ipaddress_to_esp_idf(mp_obj_t ip_address, ip_addr_t *esp_ip_address);
void ipaddress_ipaddress_to_esp_idf_ip4(mp_obj_t ip_address, esp_ip4_addr_t *esp_ip_address);
