// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/canio/RemoteTransmissionRequest.h"

extern const mp_obj_type_t canio_remote_transmission_request_type;

void common_hal_canio_remote_transmission_request_construct(canio_remote_transmission_request_obj_t *self, int id, size_t size, bool extended);
const void *common_hal_canio_remote_transmission_request_get_data(const canio_remote_transmission_request_obj_t *self);
void common_hal_canio_remote_transmission_request_set_data(canio_remote_transmission_request_obj_t *self, const void *data, size_t size);
bool common_hal_canio_remote_transmission_request_get_extended(const canio_remote_transmission_request_obj_t *self);
void common_hal_canio_remote_transmission_request_set_extended(canio_remote_transmission_request_obj_t *self, bool extended);
int common_hal_canio_remote_transmission_request_get_id(const canio_remote_transmission_request_obj_t *self);
void common_hal_canio_remote_transmission_request_set_id(canio_remote_transmission_request_obj_t *self, int id);
size_t common_hal_canio_remote_transmission_request_get_length(const canio_remote_transmission_request_obj_t *self);
void common_hal_canio_remote_transmission_request_set_length(canio_remote_transmission_request_obj_t *self, size_t length);
