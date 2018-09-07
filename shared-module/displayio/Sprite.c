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

#include "shared-bindings/displayio/Sprite.h"


#include "shared-bindings/displayio/Bitmap.h"

void common_hal_displayio_sprite_construct(displayio_sprite_t *self, mp_obj_t bitmap,
        mp_obj_t palette, uint16_t width, uint16_t height, uint16_t x, uint16_t y) {
    self->width = width;
    self->height = height;
    self->bitmap = bitmap;
    self->palette = palette;
    self->x = x;
    self->y = y;
}

void common_hal_displayio_sprite_get_position(displayio_sprite_t *self, int16_t* x, int16_t* y) {
    *x = self->x;
    *y = self->y;
}

void common_hal_displayio_sprite_set_position(displayio_sprite_t *self, int16_t x, int16_t y) {
    self->x = x;
    self->y = y;
    self->needs_refresh = true;
}


displayio_palette_t* common_hal_displayio_sprite_get_palette(displayio_sprite_t *self) {
    return self->palette;
}

void common_hal_displayio_sprite_set_palette(displayio_sprite_t *self, displayio_palette_t* palette) {
    self->palette = palette;
    self->needs_refresh = true;
}

bool displayio_sprite_get_pixel(displayio_sprite_t *self, int16_t x, int16_t y, uint16_t* pixel) {
    x -= self->x;
    y -= self->y;
    if (y < 0 || y >= self->height || x >= self->width || x < 0) {
        return false;
    }

    uint32_t value = common_hal_displayio_bitmap_get_pixel(self->bitmap, x, y);
    if (self->palette == mp_const_none) {
        *pixel = value;
        return true;
    } else if (MP_OBJ_IS_TYPE(self->palette, &displayio_palette_type) && displayio_palette_get_color(self->palette, value, pixel)) {
        return true;
    }

    return false;
}

bool displayio_sprite_needs_refresh(displayio_sprite_t *self) {
    return self->needs_refresh || displayio_palette_needs_refresh(self->palette);
}

void displayio_sprite_finish_refresh(displayio_sprite_t *self) {
    self->needs_refresh = false;
    displayio_palette_finish_refresh(self->palette);
    // TODO(tannewt): We could double buffer changes to position and move them over here.
    // That way they won't change during a refresh and tear.
}
