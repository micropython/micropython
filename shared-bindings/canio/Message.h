// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/canio/Message.h"

extern const mp_obj_type_t canio_message_type;
extern const mp_obj_type_t canio_remote_transmission_request_type;

void common_hal_canio_message_construct(canio_message_obj_t *self, int id, void *data, size_t size, bool extended);
const void *common_hal_canio_message_get_data(const canio_message_obj_t *self);
void common_hal_canio_message_set_data(canio_message_obj_t *self, const void *data, size_t size);
bool common_hal_canio_message_get_extended(const canio_message_obj_t *self);
void common_hal_canio_message_set_extended(canio_message_obj_t *self, bool extended);
int common_hal_canio_message_get_id(const canio_message_obj_t *self);
void common_hal_canio_message_set_id(canio_message_obj_t *self, int id);
size_t common_hal_canio_message_get_length(const canio_message_obj_t *self);
