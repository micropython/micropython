// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-bindings/canio/CAN.h"
#include "shared-bindings/canio/Match.h"

extern const mp_obj_type_t canio_listener_type;

typedef struct canio_listener_obj canio_listener_obj_t;

void common_hal_canio_listener_construct(canio_listener_obj_t *self, canio_can_obj_t *can, size_t nmatch, canio_match_obj_t **matches, float timeout);
void common_hal_canio_listener_check_for_deinit(canio_listener_obj_t *self);
void common_hal_canio_listener_deinit(canio_listener_obj_t *self);
mp_obj_t common_hal_canio_listener_receive(canio_listener_obj_t *self);
int common_hal_canio_listener_in_waiting(canio_listener_obj_t *self);
float common_hal_canio_listener_get_timeout(canio_listener_obj_t *self);
void common_hal_canio_listener_set_timeout(canio_listener_obj_t *self, float timeout);
