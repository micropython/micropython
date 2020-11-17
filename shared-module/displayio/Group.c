/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "shared-bindings/displayio/Group.h"

#include "py/runtime.h"
#include "shared-bindings/displayio/TileGrid.h"

#if CIRCUITPY_VECTORIO
#include "shared-bindings/vectorio/VectorShape.h"
#endif


void common_hal_displayio_group_construct(displayio_group_t* self, uint32_t max_size, uint32_t scale, mp_int_t x, mp_int_t y) {
    displayio_group_child_t* children = m_new(displayio_group_child_t, max_size);
    displayio_group_construct(self, children, max_size, scale, x, y);
}

bool common_hal_displayio_group_get_hidden(displayio_group_t* self) {
    return self->hidden;
}

void common_hal_displayio_group_set_hidden(displayio_group_t* self, bool hidden) {
    if (self->hidden == hidden) {
        return;
    }
    self->hidden = hidden;
    if (self->hidden_by_parent) {
        return;
    }
    for (size_t i = 0; i < self->size; i++) {
        mp_obj_t layer = self->children[i].native;
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            displayio_tilegrid_set_hidden_by_parent(layer, hidden);
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            displayio_group_set_hidden_by_parent(layer, hidden);
        }
    }
}

void displayio_group_set_hidden_by_parent(displayio_group_t *self, bool hidden) {
    if (self->hidden_by_parent == hidden) {
        return;
    }
    self->hidden_by_parent = hidden;
    // If we're already hidden, then we're done.
    if (self->hidden) {
        return;
    }
    for (size_t i = 0; i < self->size; i++) {
        mp_obj_t layer = self->children[i].native;
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            displayio_tilegrid_set_hidden_by_parent(layer, hidden);
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            displayio_group_set_hidden_by_parent(layer, hidden);
        }
    }
}

uint32_t common_hal_displayio_group_get_scale(displayio_group_t* self) {
    return self->scale;
}

bool displayio_group_get_previous_area(displayio_group_t *self, displayio_area_t* area) {
    bool first = true;
    for (size_t i = 0; i < self->size; i++) {
        mp_obj_t layer = self->children[i].native;
        displayio_area_t layer_area;
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            if (!displayio_tilegrid_get_previous_area(layer, &layer_area)) {
                continue;
            }
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            if (!displayio_group_get_previous_area(layer, &layer_area)) {
                continue;
            }
        }
        if (first) {
            displayio_area_copy(&layer_area, area);
            first = false;
        } else {
            displayio_area_expand(area, &layer_area);
        }
    }
    if (self->item_removed) {
        if (first) {
            displayio_area_copy(&self->dirty_area, area);
            first = false;
        } else {
            displayio_area_expand(area, &self->dirty_area);
        }
    }
    return !first;
}

static void _update_child_transforms(displayio_group_t* self) {
    if (!self->in_group) {
        return;
    }
    for (size_t i = 0; i < self->size; i++) {
        mp_obj_t layer = self->children[i].native;
#if CIRCUITPY_VECTORIO
        if (MP_OBJ_IS_TYPE(layer, &vectorio_vector_shape_type)) {
            vectorio_vector_shape_update_transform(layer, &self->absolute_transform);
        }
        else
#endif
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            displayio_tilegrid_update_transform(layer, &self->absolute_transform);
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            displayio_group_update_transform(layer, &self->absolute_transform);
        }
    }
}

void displayio_group_update_transform(displayio_group_t *self,
                                      const displayio_buffer_transform_t* parent_transform) {
    self->in_group = parent_transform != NULL;
    if (self->in_group) {
        int16_t x = self->x;
        int16_t y = self->y;
        if (parent_transform->transpose_xy) {
            x = y;
            y = self->x;
        }
        self->absolute_transform.x = parent_transform->x + parent_transform->dx * x;
        self->absolute_transform.y = parent_transform->y + parent_transform->dy * y;
        self->absolute_transform.dx = parent_transform->dx * self->scale;
        self->absolute_transform.dy = parent_transform->dy * self->scale;
        self->absolute_transform.transpose_xy = parent_transform->transpose_xy;
        self->absolute_transform.mirror_x = parent_transform->mirror_x;
        self->absolute_transform.mirror_y = parent_transform->mirror_y;

        self->absolute_transform.scale = parent_transform->scale * self->scale;
    }
    _update_child_transforms(self);
}

void common_hal_displayio_group_set_scale(displayio_group_t* self, uint32_t scale) {
    if (self->scale == scale) {
        return;
    }
    uint8_t parent_scale = self->absolute_transform.scale / self->scale;
    self->absolute_transform.dx = self->absolute_transform.dx / self->scale * scale;
    self->absolute_transform.dy = self->absolute_transform.dy / self->scale * scale;
    self->absolute_transform.scale = parent_scale * scale;
    self->scale = scale;
    _update_child_transforms(self);
}

mp_int_t common_hal_displayio_group_get_x(displayio_group_t* self) {
    return self->x;
}

void common_hal_displayio_group_set_x(displayio_group_t* self, mp_int_t x) {
    if (self->x == x) {
        return;
    }
    if (self->absolute_transform.transpose_xy) {
        int16_t dy = self->absolute_transform.dy / self->scale;
        self->absolute_transform.y += dy * (x - self->x);
    } else  {
        int16_t dx = self->absolute_transform.dx / self->scale;
        self->absolute_transform.x += dx * (x - self->x);
    }

    self->x = x;
    _update_child_transforms(self);
}

mp_int_t common_hal_displayio_group_get_y(displayio_group_t* self) {
    return self->y;
}

void common_hal_displayio_group_set_y(displayio_group_t* self, mp_int_t y) {
    if (self->y == y) {
        return;
    }
    if (self->absolute_transform.transpose_xy) {
        int8_t dx = self->absolute_transform.dx / self->scale;
        self->absolute_transform.x += dx * (y - self->y);
    } else  {
        int8_t dy = self->absolute_transform.dy / self->scale;
        self->absolute_transform.y += dy * (y - self->y);
    }
    self->y = y;
    _update_child_transforms(self);
}

static mp_obj_t _add_layer(displayio_group_t* self, mp_obj_t layer) {
    mp_obj_t native_layer;
#if CIRCUITPY_VECTORIO
    native_layer = mp_instance_cast_to_native_base(layer, &vectorio_vector_shape_type);
    if (native_layer != MP_OBJ_NULL) {
        vectorio_vector_shape_update_transform(native_layer, &self->absolute_transform);
        return native_layer;
    }
#endif
    native_layer = mp_instance_cast_to_native_base(layer, &displayio_group_type);
    if (native_layer == MP_OBJ_NULL) {
        native_layer = mp_instance_cast_to_native_base(layer, &displayio_tilegrid_type);
        if (native_layer == MP_OBJ_NULL) {
            mp_raise_ValueError(translate("Layer must be a Group or TileGrid subclass."));
        }
        displayio_tilegrid_t* tilegrid = native_layer;
        if (tilegrid->in_group) {
            mp_raise_ValueError(translate("Layer already in a group."));
        } else {
            tilegrid->in_group = true;
        }
        displayio_tilegrid_update_transform(tilegrid, &self->absolute_transform);
    } else {
        displayio_group_t* group = native_layer;
        if (group->in_group) {
            mp_raise_ValueError(translate("Layer already in a group."));
        } else {
            group->in_group = true;
        }
        displayio_group_update_transform(group, &self->absolute_transform);
    }
    return native_layer;
}

static void _remove_layer(displayio_group_t* self, size_t index) {
    mp_obj_t layer = self->children[index].native;
    displayio_area_t layer_area;
    bool rendered_last_frame = false;
#if CIRCUITPY_VECTORIO
    if (MP_OBJ_IS_TYPE(layer, &vectorio_vector_shape_type)) {
        bool has_dirty_area = vectorio_vector_shape_get_dirty_area(layer, &layer_area);
        rendered_last_frame = has_dirty_area;
        vectorio_vector_shape_update_transform(layer, NULL);
    }
    else
#endif
    if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
        displayio_tilegrid_t* tilegrid = layer;
        rendered_last_frame = displayio_tilegrid_get_previous_area(tilegrid, &layer_area);
        displayio_tilegrid_update_transform(tilegrid, NULL);
    } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
        displayio_group_t* group = layer;
        rendered_last_frame = displayio_group_get_previous_area(group, &layer_area);
        displayio_group_update_transform(group, NULL);
    }
    if (!rendered_last_frame) {
        return;
    }
    if (!self->item_removed) {
        displayio_area_copy(&layer_area, &self->dirty_area);
    } else {
        displayio_area_expand(&self->dirty_area, &layer_area);
    }
    self->item_removed = true;
}

void common_hal_displayio_group_insert(displayio_group_t* self, size_t index, mp_obj_t layer) {
    if (self->size == self->max_size) {
        mp_raise_RuntimeError(translate("Group full"));
    }
    mp_obj_t native_layer = _add_layer(self, layer);
    // Shift everything right.
    for (size_t i = self->size; i > index; i--) {
        self->children[i] = self->children[i - 1];
    }
    self->children[index].native = native_layer;
    self->children[index].original = layer;
    self->size++;
}

mp_obj_t common_hal_displayio_group_pop(displayio_group_t* self, size_t index) {
    self->size--;
    mp_obj_t item = self->children[index].original;
    _remove_layer(self, index);

    // Shift everything left.
    for (size_t i = index; i < self->size; i++) {
        self->children[i] = self->children[i + 1];
    }
    self->children[self->size].native = NULL;
    self->children[self->size].original = NULL;
    return item;
}

mp_int_t common_hal_displayio_group_index(displayio_group_t* self, mp_obj_t layer) {
    for (size_t i = 0; i < self->size; i++) {
        if (self->children[i].original == layer) {
            return i;
        }
    }
    return -1;
}

size_t common_hal_displayio_group_get_len(displayio_group_t* self) {
    return self->size;
}

mp_obj_t common_hal_displayio_group_get(displayio_group_t* self, size_t index) {
    return self->children[index].original;
}

void common_hal_displayio_group_set(displayio_group_t* self, size_t index, mp_obj_t layer) {
    mp_obj_t native_layer = _add_layer(self, layer);
    _remove_layer(self, index);
    self->children[index].native = native_layer;
    self->children[index].original = layer;
}

void displayio_group_construct(displayio_group_t* self, displayio_group_child_t* child_array, uint32_t max_size, uint32_t scale, mp_int_t x, mp_int_t y) {
    self->x = x;
    self->y = y;
    self->children = child_array;
    self->max_size = max_size;
    self->item_removed = false;
    self->scale = scale;
    self->in_group = false;
}

bool displayio_group_fill_area(displayio_group_t *self, const _displayio_colorspace_t* colorspace, const displayio_area_t* area, uint32_t* mask, uint32_t* buffer) {
    // Track if any of the layers finishes filling in the given area. We can ignore any remaining
    // layers at that point.
    bool full_coverage = false;
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i].native;
#if CIRCUITPY_VECTORIO
        if (MP_OBJ_IS_TYPE(layer, &vectorio_vector_shape_type)) {
            if (vectorio_vector_shape_fill_area(layer, colorspace, area, mask, buffer)) {
                full_coverage = true;
                break;
            }
        }
        else
#endif
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            if (displayio_tilegrid_fill_area(layer, colorspace, area, mask, buffer)) {
                full_coverage = true;
                break;
            }
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            if (displayio_group_fill_area(layer, colorspace, area, mask, buffer)) {
                full_coverage = true;
                break;
            }
        }
    }
    return full_coverage;
}

void displayio_group_finish_refresh(displayio_group_t *self) {
    self->item_removed = false;
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i].native;
#if CIRCUITPY_VECTORIO
        if (MP_OBJ_IS_TYPE(layer, &vectorio_vector_shape_type)) {
            vectorio_vector_shape_finish_refresh(layer);
        }
        else
#endif
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            displayio_tilegrid_finish_refresh(layer);
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            displayio_group_finish_refresh(layer);
        }
    }
}

displayio_area_t* displayio_group_get_refresh_areas(displayio_group_t *self, displayio_area_t* tail) {
    if (self->item_removed) {
        self->dirty_area.next = tail;
        tail = &self->dirty_area;
    }

    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i].native;
#if CIRCUITPY_VECTORIO
        if (MP_OBJ_IS_TYPE(layer, &vectorio_vector_shape_type)) {
            tail = vectorio_vector_shape_get_refresh_areas(layer, tail);
        }
        else
#endif
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            tail = displayio_tilegrid_get_refresh_areas(layer, tail);
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            tail = displayio_group_get_refresh_areas(layer, tail);
        }
    }

    return tail;
}
