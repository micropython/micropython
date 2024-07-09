// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/vectorio/Rectangle.h"
#include "shared-module/displayio/area.h"
#include "shared-module/vectorio/__init__.h"

extern const mp_obj_type_t vectorio_rectangle_type;

void common_hal_vectorio_rectangle_construct(vectorio_rectangle_t *self, uint32_t width, uint32_t height, uint16_t color_index);
void common_hal_vectorio_rectangle_set_on_dirty(vectorio_rectangle_t *self, vectorio_event_t on_dirty);

uint32_t common_hal_vectorio_rectangle_get_pixel(void *rectangle, int16_t x, int16_t y);

void common_hal_vectorio_rectangle_get_area(void *rectangle, displayio_area_t *out_area);

mp_obj_t common_hal_vectorio_rectangle_get_draw_protocol(void *rectangle);

int16_t common_hal_vectorio_rectangle_get_width(void *obj);
void common_hal_vectorio_rectangle_set_width(void *obj, int16_t width);

uint16_t common_hal_vectorio_rectangle_get_color_index(void *obj);
void common_hal_vectorio_rectangle_set_color_index(void *obj, uint16_t color_index);

int16_t common_hal_vectorio_rectangle_get_height(void *obj);
void common_hal_vectorio_rectangle_set_height(void *obj, int16_t height);
