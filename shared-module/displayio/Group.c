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

void common_hal_displayio_group_construct(displayio_group_t* self, uint32_t max_size, uint32_t scale, mp_int_t x, mp_int_t y) {
    displayio_group_child_t* children = m_new(displayio_group_child_t, max_size);
    displayio_group_construct(self, children, max_size, scale, x, y);
}

uint32_t common_hal_displayio_group_get_scale(displayio_group_t* self) {
    return self->scale;
}

void common_hal_displayio_group_set_scale(displayio_group_t* self, uint32_t scale) {
    self->needs_refresh = self->scale != scale;
    self->scale = scale;
}

mp_int_t common_hal_displayio_group_get_x(displayio_group_t* self) {
    return self->x;
}

void common_hal_displayio_group_set_x(displayio_group_t* self, mp_int_t x) {
    self->needs_refresh = self->x != x;
    self->x = x;
}

mp_int_t common_hal_displayio_group_get_y(displayio_group_t* self) {
    return self->y;
}

void common_hal_displayio_group_set_y(displayio_group_t* self, mp_int_t y) {
    self->needs_refresh = self->y != y;
    self->y = y;
}

void common_hal_displayio_group_insert(displayio_group_t* self, size_t index, mp_obj_t layer) {
    if (self->size == self->max_size) {
        mp_raise_RuntimeError(translate("Group full"));
    }
    mp_obj_t native_layer = mp_instance_cast_to_native_base(layer, &displayio_group_type);
    if (native_layer == MP_OBJ_NULL) {
        native_layer = mp_instance_cast_to_native_base(layer, &displayio_tilegrid_type);
    }
    if (native_layer == MP_OBJ_NULL) {
        mp_raise_ValueError(translate("Layer must be a Group or TileGrid subclass."));
    }
    // Shift everything right.
    for (size_t i = self->size; i > index; i--) {
        self->children[i] = self->children[i - 1];
    }
    self->children[index].native = native_layer;
    self->children[index].original = layer;
    self->size++;
    self->needs_refresh = true;
}

mp_obj_t common_hal_displayio_group_pop(displayio_group_t* self, size_t index) {
    self->size--;
    mp_obj_t item = self->children[index].original;
    // Shift everything left.
    for (size_t i = index; i < self->size; i++) {
        self->children[i] = self->children[i + 1];
    }
    self->children[self->size].native = NULL;
    self->children[self->size].original = NULL;
    self->needs_refresh = true;
    return item;
}

size_t common_hal_displayio_group_get_len(displayio_group_t* self) {
    return self->size;
}

mp_obj_t common_hal_displayio_group_get(displayio_group_t* self, size_t index) {
    return self->children[index].original;
}

void common_hal_displayio_group_set(displayio_group_t* self, size_t index, mp_obj_t layer) {
    mp_obj_t native_layer = mp_instance_cast_to_native_base(layer, &displayio_group_type);
    if (native_layer == MP_OBJ_NULL) {
        native_layer = mp_instance_cast_to_native_base(layer, &displayio_tilegrid_type);
    }
    if (native_layer == MP_OBJ_NULL) {
        mp_raise_ValueError(translate("Layer must be a Group or TileGrid subclass."));
    }
    self->children[index].native = native_layer;
    self->children[index].original = layer;
    self->needs_refresh = true;
}

void displayio_group_construct(displayio_group_t* self, displayio_group_child_t* child_array, uint32_t max_size, uint32_t scale, mp_int_t x, mp_int_t y) {
    self->x = x;
    self->y = y;
    self->children = child_array;
    self->max_size = max_size;
    self->needs_refresh = false;
    self->scale = scale;
}

bool displayio_group_get_pixel(displayio_group_t *self, int16_t x, int16_t y, uint16_t* pixel) {
    x -= self->x;
    y -= self->y;
    x /= self->scale;
    y /= self->scale;
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i].native;
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            if (displayio_tilegrid_get_pixel(layer, x, y, pixel)) {
                return true;
            }
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            if (displayio_group_get_pixel(layer, x, y, pixel)) {
                return true;
            }
        }
    }
    return false;
}

bool displayio_group_needs_refresh(displayio_group_t *self) {
    if (self->needs_refresh) {
        return true;
    }
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i].native;
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            if (displayio_tilegrid_needs_refresh(layer)) {
                return true;
            }
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            if (displayio_group_needs_refresh(layer)) {
                return true;
            }
        }
    }
    return false;
}

void displayio_group_finish_refresh(displayio_group_t *self) {
    self->needs_refresh = false;
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i].native;
        if (MP_OBJ_IS_TYPE(layer, &displayio_tilegrid_type)) {
            displayio_tilegrid_finish_refresh(layer);
        } else if (MP_OBJ_IS_TYPE(layer, &displayio_group_type)) {
            displayio_group_finish_refresh(layer);
        }
    }
}
