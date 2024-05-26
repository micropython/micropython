// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objtuple.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/Service.h"

extern const mp_obj_type_t bleio_connection_type;

void common_hal_bleio_connection_pair(bleio_connection_internal_t *self, bool bond);
void common_hal_bleio_connection_disconnect(bleio_connection_internal_t *self);
bool common_hal_bleio_connection_get_connected(bleio_connection_obj_t *self);
mp_int_t common_hal_bleio_connection_get_max_packet_length(bleio_connection_internal_t *self);
bool common_hal_bleio_connection_get_paired(bleio_connection_obj_t *self);
mp_obj_tuple_t *common_hal_bleio_connection_discover_remote_services(bleio_connection_obj_t *self, mp_obj_t service_uuids_whitelist);

mp_float_t common_hal_bleio_connection_get_connection_interval(bleio_connection_internal_t *self);
void common_hal_bleio_connection_set_connection_interval(bleio_connection_internal_t *self, mp_float_t new_interval);

void bleio_connection_ensure_connected(bleio_connection_obj_t *self);
