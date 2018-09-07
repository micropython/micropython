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
#include "shared-bindings/displayio/Sprite.h"

void common_hal_displayio_group_construct(displayio_group_t* self, uint32_t max_size) {
    mp_obj_t* children = m_new(mp_obj_t, max_size);
    displayio_group_construct(self, children, max_size);
}

void common_hal_displayio_group_append(displayio_group_t* self, mp_obj_t layer) {
    if (self->size == self->max_size) {
        mp_raise_RuntimeError(translate("Group full"));
    }
    self->children[self->size] = layer;
    self->size++;
}

void displayio_group_construct(displayio_group_t* self, mp_obj_t* child_array, uint32_t max_size) {
    self->x = 0;
    self->y = 1;
    self->children = child_array;
    self->max_size = max_size;
}

bool displayio_group_get_pixel(displayio_group_t *self, int16_t x, int16_t y, uint16_t* pixel) {
    x -= self->x;
    y -= self->y;
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i];
        if (MP_OBJ_IS_TYPE(layer, &displayio_sprite_type)) {
            if (displayio_sprite_get_pixel(layer, x, y, pixel)) {
                return true;
            }
        }
        // TODO: Tiled layer
    }
    return false;
}

bool displayio_group_needs_refresh(displayio_group_t *self) {
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i];
        if (MP_OBJ_IS_TYPE(layer, &displayio_sprite_type)) {
            if (displayio_sprite_needs_refresh(layer)) {
                return true;
            }
        }
        // TODO: Tiled layer
    }
    return false;
}

void displayio_group_finish_refresh(displayio_group_t *self) {
    for (int32_t i = self->size - 1; i >= 0 ; i--) {
        mp_obj_t layer = self->children[i];
        if (MP_OBJ_IS_TYPE(layer, &displayio_sprite_type)) {
            displayio_sprite_finish_refresh(layer);
        }
        // TODO: Tiled layer
    }
}
