// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/canio/Match.h"

extern const mp_obj_type_t canio_match_type;

void common_hal_canio_match_construct(canio_match_obj_t *self, int id, int mask, bool extended);
int common_hal_canio_match_get_id(const canio_match_obj_t *self);
int common_hal_canio_match_get_mask(const canio_match_obj_t *self);
bool common_hal_canio_match_get_extended(const canio_match_obj_t *self);
