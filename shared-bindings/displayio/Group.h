// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t displayio_group_type;

displayio_group_t *native_group(mp_obj_t group_obj);

void common_hal_displayio_group_construct(displayio_group_t *self, uint32_t scale, mp_int_t x, mp_int_t y);
uint32_t common_hal_displayio_group_get_scale(displayio_group_t *self);
void common_hal_displayio_group_set_scale(displayio_group_t *self, uint32_t scale);
bool common_hal_displayio_group_get_hidden(displayio_group_t *self);
void common_hal_displayio_group_set_hidden(displayio_group_t *self, bool hidden);
mp_int_t common_hal_displayio_group_get_x(displayio_group_t *self);
void common_hal_displayio_group_set_x(displayio_group_t *self, mp_int_t x);
mp_int_t common_hal_displayio_group_get_y(displayio_group_t *self);
void common_hal_displayio_group_set_y(displayio_group_t *self, mp_int_t y);
void common_hal_displayio_group_append(displayio_group_t *self, mp_obj_t layer);
void common_hal_displayio_group_insert(displayio_group_t *self, size_t index, mp_obj_t layer);
size_t common_hal_displayio_group_get_len(displayio_group_t *self);
mp_obj_t common_hal_displayio_group_pop(displayio_group_t *self, size_t index);
mp_int_t common_hal_displayio_group_index(displayio_group_t *self, mp_obj_t layer);
mp_obj_t common_hal_displayio_group_get(displayio_group_t *self, size_t index);
void common_hal_displayio_group_set(displayio_group_t *self, size_t index, mp_obj_t layer);
