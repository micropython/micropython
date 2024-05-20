// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/_bleio/Characteristic.h"
#include "common-hal/_bleio/Connection.h"
#include "common-hal/_bleio/Service.h"

#include "py/objtuple.h"

extern const mp_obj_type_t bleio_service_type;

// Private version that doesn't allocate on the heap
extern uint32_t _common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary, mp_obj_list_t *characteristic_list);
extern void common_hal_bleio_service_construct(bleio_service_obj_t *self, bleio_uuid_obj_t *uuid, bool is_secondary);
extern void common_hal_bleio_service_deinit(bleio_service_obj_t *self);
extern void common_hal_bleio_service_from_remote_service(bleio_service_obj_t *self, bleio_connection_obj_t *connection, bleio_uuid_obj_t *uuid, bool is_secondary);
extern bleio_uuid_obj_t *common_hal_bleio_service_get_uuid(bleio_service_obj_t *self);
extern mp_obj_tuple_t *common_hal_bleio_service_get_characteristics(bleio_service_obj_t *self);
extern bool common_hal_bleio_service_get_is_remote(bleio_service_obj_t *self);
extern bool common_hal_bleio_service_get_is_secondary(bleio_service_obj_t *self);
extern void common_hal_bleio_service_add_characteristic(bleio_service_obj_t *self, bleio_characteristic_obj_t *characteristic, mp_buffer_info_t *initial_value_bufinfo, const char *user_description);
