// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/vectorio/__init__.h"
#include "shared-module/vectorio/Circle.h"
#include "shared-module/displayio/area.h"

extern const mp_obj_type_t vectorio_circle_type;

void common_hal_vectorio_circle_construct(vectorio_circle_t *self, uint16_t radius, uint16_t color_index);

void common_hal_vectorio_circle_set_on_dirty(vectorio_circle_t *self, vectorio_event_t notification);

uint32_t common_hal_vectorio_circle_get_pixel(void *circle, int16_t x, int16_t y);

void common_hal_vectorio_circle_get_area(void *circle, displayio_area_t *out_area);


int16_t common_hal_vectorio_circle_get_radius(void *circle);
void common_hal_vectorio_circle_set_radius(void *circle, int16_t radius);

uint16_t common_hal_vectorio_circle_get_color_index(void *obj);
void common_hal_vectorio_circle_set_color_index(void *obj, uint16_t color_index);

mp_obj_t common_hal_vectorio_circle_get_draw_protocol(void *circle);
