// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/objlist.h"
#include "shared-module/displayio/area.h"
#include "shared-module/displayio/Palette.h"

typedef struct {
    mp_obj_base_t base;
    mp_obj_list_t *members;
    displayio_buffer_transform_t absolute_transform;
    displayio_area_t dirty_area; // Catch all for changed area
    int16_t x;
    int16_t y;
    uint16_t scale;
    bool item_removed : 1;
    bool in_group : 1;
    bool hidden : 1;
    bool hidden_by_parent : 1;
    bool readonly : 1;
    uint8_t padding : 3;
} displayio_group_t;

void displayio_group_construct(displayio_group_t *self, mp_obj_list_t *members, uint32_t scale, mp_int_t x, mp_int_t y);
void displayio_group_set_hidden_by_parent(displayio_group_t *self, bool hidden);
bool displayio_group_get_previous_area(displayio_group_t *group, displayio_area_t *area);
bool displayio_group_fill_area(displayio_group_t *group, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer);
void displayio_group_update_transform(displayio_group_t *group, const displayio_buffer_transform_t *parent_transform);
void displayio_group_finish_refresh(displayio_group_t *self);
displayio_area_t *displayio_group_get_refresh_areas(displayio_group_t *self, displayio_area_t *tail);
