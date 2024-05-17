// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/vectorio/Circle.h"
#include "shared-module/vectorio/__init__.h"
#include "shared-module/displayio/area.h"

#include "py/runtime.h"
#include "stdlib.h"


void common_hal_vectorio_circle_construct(vectorio_circle_t *self, uint16_t radius, uint16_t color_index) {
    self->radius = radius;
    self->on_dirty.obj = NULL;
    self->color_index = color_index + 1;
}

void common_hal_vectorio_circle_set_on_dirty(vectorio_circle_t *self, vectorio_event_t on_dirty) {
    if (self->on_dirty.obj != NULL) {
        mp_raise_TypeError(MP_ERROR_TEXT("can only have one parent"));
    }
    self->on_dirty = on_dirty;
}


uint32_t common_hal_vectorio_circle_get_pixel(void *obj, int16_t x, int16_t y) {
    vectorio_circle_t *self = obj;
    int16_t radius = self->radius;
    x = abs(x);
    y = abs(y);
    if (x + y <= radius) {
        return self->color_index;
    }
    if (x > radius) {
        return 0;
    }
    if (y > radius) {
        return 0;
    }
    const bool pythagorasSmallerThanRadius = (int32_t)x * x + (int32_t)y * y <= (int32_t)radius * radius;
    return pythagorasSmallerThanRadius ? self->color_index : 0;
}


void common_hal_vectorio_circle_get_area(void *circle, displayio_area_t *out_area) {
    vectorio_circle_t *self = circle;
    out_area->x1 = -1 * self->radius - 1;
    out_area->y1 = -1 * self->radius - 1;
    out_area->x2 = self->radius + 1;
    out_area->y2 = self->radius + 1;
}

int16_t common_hal_vectorio_circle_get_radius(void *obj) {
    vectorio_circle_t *self = obj;
    return self->radius;
}

void common_hal_vectorio_circle_set_radius(void *obj, int16_t radius) {
    vectorio_circle_t *self = obj;
    self->radius = abs(radius);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

uint16_t common_hal_vectorio_circle_get_color_index(void *obj) {
    vectorio_circle_t *self = obj;
    return self->color_index - 1;
}

void common_hal_vectorio_circle_set_color_index(void *obj, uint16_t color_index) {
    vectorio_circle_t *self = obj;
    self->color_index = abs(color_index + 1);
    if (self->on_dirty.obj != NULL) {
        self->on_dirty.event(self->on_dirty.obj);
    }
}

mp_obj_t common_hal_vectorio_circle_get_draw_protocol(void *circle) {
    vectorio_circle_t *self = circle;
    return self->draw_protocol_instance;
}
