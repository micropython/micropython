// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 by kvc0/WarriorOfWire
//
// SPDX-License-Identifier: MIT


#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"
#include "shared-module/displayio/area.h"
#include "shared-module/displayio/Palette.h"

typedef void get_area_function(mp_obj_t shape, displayio_area_t *out_area);
typedef uint32_t get_pixel_function(mp_obj_t shape, int16_t x, int16_t y);

// This struct binds a shape's common Shape support functions (its vector shape interface)
//   to its instance pointer.  We only check at construction time what the type of the
//   associated shape is and link the correct functions up.
// Later when using the shape for drawing logic these functions may be invoked
//   unconditionally.  This simplifies the addition of new types and restricts the
//   respective responsibilities of VectorShape and actual shape implementations.
typedef struct {
    mp_obj_t shape;
    get_area_function *get_area;
    get_pixel_function *get_pixel;
} vectorio_ishape_t;

typedef struct {
    mp_obj_base_t base;
    vectorio_ishape_t ishape;
    mp_obj_t pixel_shader;
    int16_t x;
    int16_t y;
    displayio_buffer_transform_t *absolute_transform;
    // Tracks current shape footprint and expands outward as the shape dirties and changes.
    // This is suboptimal if you move your shape far.  Could add more state to only redraw
    //   exactly what we left behind.
    displayio_area_t ephemeral_dirty_area;
    displayio_area_t current_area;
    bool current_area_dirty;
    bool hidden;
} vectorio_vector_shape_t;

displayio_area_t *vectorio_vector_shape_get_refresh_areas(vectorio_vector_shape_t *self, displayio_area_t *tail);

bool vectorio_vector_shape_get_dirty_area(vectorio_vector_shape_t *self, displayio_area_t *current_dirty_area);

// Area is always in absolute screen coordinates.
bool vectorio_vector_shape_fill_area(vectorio_vector_shape_t *self, const _displayio_colorspace_t *colorspace, const displayio_area_t *area, uint32_t *mask, uint32_t *buffer);

// Fills in out_area with the maximum bounds of all related pixels in the last rendered frame. Returns
// false if the vector shape wasn't rendered in the last frame.
bool vectorio_vector_shape_get_previous_area(vectorio_vector_shape_t *self, displayio_area_t *out_area);
void vectorio_vector_shape_finish_refresh(vectorio_vector_shape_t *self);
