// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "common-hal/mdns/Server.h"

#include "shared-bindings/mdns/RemoteService.h"

extern const mp_obj_type_t mdns_server_type;

void common_hal_mdns_server_construct(mdns_server_obj_t *self, mp_obj_t network_interface);
void common_hal_mdns_server_deinit(mdns_server_obj_t *self);
bool common_hal_mdns_server_deinited(mdns_server_obj_t *self);
const char *common_hal_mdns_server_get_hostname(mdns_server_obj_t *self);
void common_hal_mdns_server_set_hostname(mdns_server_obj_t *self, const char *hostname);
const char *common_hal_mdns_server_get_instance_name(mdns_server_obj_t *self);
void common_hal_mdns_server_set_instance_name(mdns_server_obj_t *self, const char *instance_name);
mp_obj_t common_hal_mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_float_t timeout);

/**
 * @brief Advertises service
 *
 * @param self
 * @param service_type A string indicating the DNS-SD type of service being advertised (e.g., _http)
 * @param protocol A string indicating the DNS-SD protocol of the service (e.g., _tcp or _udp)
 * @param port The TCP or UDP port number of the service
 * @param txt_records An array of strings representing TXT records to publish along with the service
 * @param num_txt_records Number of records expected in txt_records
 */
void common_hal_mdns_server_advertise_service(mdns_server_obj_t *self, const char *service_type, const char *protocol, mp_int_t port, const char *txt_records[], size_t num_txt_records);

// For internal use.
void mdns_server_construct(mdns_server_obj_t *self, bool workflow);
size_t mdns_server_find(mdns_server_obj_t *self, const char *service_type, const char *protocol,
    mp_float_t timeout, mdns_remoteservice_obj_t *out, size_t out_len);
