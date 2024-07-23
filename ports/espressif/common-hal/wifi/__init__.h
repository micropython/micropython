// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lwip/api.h"
#include "components/esp_wifi/include/esp_wifi.h"

struct sockaddr_storage;

void wifi_reset(void);

void ipaddress_ipaddress_to_esp_idf(mp_obj_t ip_address, ip_addr_t *esp_ip_address);
void ipaddress_ipaddress_to_esp_idf_ip4(mp_obj_t ip_address, esp_ip4_addr_t *esp_ip_address);

mp_obj_t sockaddr_to_str(const struct sockaddr_storage *addr);
mp_obj_t sockaddr_to_tuple(const struct sockaddr_storage *addr);
mp_obj_t espaddr_to_str(const esp_ip_addr_t *espaddr);
mp_obj_t espaddr4_to_str(const esp_ip4_addr_t *espaddr);
mp_obj_t espaddr6_to_str(const esp_ip6_addr_t *espaddr);
void sockaddr_to_espaddr(const struct sockaddr_storage *sockaddr, esp_ip_addr_t *espaddr);
void espaddr_to_sockaddr(const esp_ip_addr_t *espaddr, struct sockaddr_storage *sockaddr, int port);
