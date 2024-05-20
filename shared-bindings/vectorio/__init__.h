// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "py/proto.h"

#include "shared-module/displayio/area.h"
#include "shared-module/displayio/Palette.h"

// Returns the object on which the rest of the draw protocol methods are invoked.
typedef mp_obj_t (*draw_get_protocol_self_fun)(mp_obj_t protocol_container);

typedef bool (*draw_fill_area_fun)(mp_obj_t draw_protocol_self, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer);
typedef bool (*draw_get_dirty_area_fun)(mp_obj_t draw_protocol_self, displayio_area_t *current_dirty_area);
typedef void (*draw_update_transform_fun)(mp_obj_t draw_protocol_self, displayio_buffer_transform_t *group_transform);
typedef void (*draw_finish_refresh_fun)(mp_obj_t draw_protocol_self);
typedef void (*draw_set_dirty_fun)(mp_obj_t draw_protocol_self);
typedef displayio_area_t *(*draw_get_refresh_areas_fun)(mp_obj_t draw_protocol_self, displayio_area_t *tail);

typedef struct _vectorio_draw_protocol_impl_t {
    draw_fill_area_fun draw_fill_area;
    draw_get_dirty_area_fun draw_get_dirty_area;
    draw_update_transform_fun draw_update_transform;
    draw_finish_refresh_fun draw_finish_refresh;
    draw_get_refresh_areas_fun draw_get_refresh_areas;
    draw_set_dirty_fun draw_set_dirty;
} vectorio_draw_protocol_impl_t;

// Draw protocol
typedef struct _vectorio_draw_protocol_t {
    MP_PROTOCOL_HEAD // MP_QSTR_protocol_draw

    // Instance of the draw protocol
    draw_get_protocol_self_fun draw_get_protocol_self;

    // Implementation functions for the draw protocol
    vectorio_draw_protocol_impl_t *draw_protocol_impl;
} vectorio_draw_protocol_t;
