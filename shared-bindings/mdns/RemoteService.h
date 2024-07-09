// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
// SPDX-FileCopyrightText: Copyright (c) 2017 Glenn Ruben Bakke
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "common-hal/mdns/RemoteService.h"

extern const mp_obj_type_t mdns_remoteservice_type;

const char *common_hal_mdns_remoteservice_get_service_type(mdns_remoteservice_obj_t *self);
const char *common_hal_mdns_remoteservice_get_protocol(mdns_remoteservice_obj_t *self);
const char *common_hal_mdns_remoteservice_get_instance_name(mdns_remoteservice_obj_t *self);
const char *common_hal_mdns_remoteservice_get_hostname(mdns_remoteservice_obj_t *self);
mp_int_t common_hal_mdns_remoteservice_get_port(mdns_remoteservice_obj_t *self);
mp_obj_t common_hal_mdns_remoteservice_get_ipv4_address(mdns_remoteservice_obj_t *self);
void common_hal_mdns_remoteservice_deinit(mdns_remoteservice_obj_t *self);

// For internal use.
uint32_t mdns_remoteservice_get_ipv4_address(mdns_remoteservice_obj_t *self);
