/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Rose Hooper
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

#include "py/smallint.h"
#include "py/runtime.h"

#include "shared-bindings/_pixelmap/PixelMap.h"
#include "shared-bindings/adafruit_pixelbuf/PixelBuf.h"
#include "shared-module/_pixelmap/PixelMap.h"


static void pixelmap_set_pixel_rgbw(pixelmap_pixelmap_obj_t *self, size_t i, color_u rgbw) {
    mp_arg_validate_index_range(i, 0, self->len - 1, MP_QSTR_index);

    mp_obj_t item = self->items[i];
    if (mp_obj_is_small_int(item)) {
        common_hal_adafruit_pixelbuf_pixelbuf_set_pixel_color(self->pixelbuf, MP_OBJ_SMALL_INT_VALUE(item), rgbw.r, rgbw.g, rgbw.b, rgbw.w);
    } else {
        size_t len;
        mp_obj_t *items;
        mp_obj_tuple_get(item, &len, &items);

        for (size_t j = 0; j < len; j++) {
            common_hal_adafruit_pixelbuf_pixelbuf_set_pixel_color(self->pixelbuf, MP_OBJ_SMALL_INT_VALUE(items[j]), rgbw.r, rgbw.g, rgbw.b, rgbw.w);
        }
    }
}

static void pixelmap_set_pixel(pixelmap_pixelmap_obj_t *self, size_t i, mp_obj_t color) {
    color_u rgbw;
    common_hal_adafruit_pixelbuf_pixelbuf_parse_color(self->pixelbuf, color, &rgbw.r, &rgbw.g, &rgbw.b, &rgbw.w);
    pixelmap_set_pixel_rgbw(self, i, rgbw);
}

void shared_module_pixelmap_pixelmap_construct(pixelmap_pixelmap_obj_t *self, mp_obj_t pixelbuf, mp_obj_t indices) {
    self->pixelbuf = pixelbuf;
    self->indices = indices;
    mp_obj_tuple_get(indices, &self->len, &self->items);
}

static bool auto_write_get_and_clear(pixelmap_pixelmap_obj_t *self) {
    bool auto_write = self->auto_write && common_hal_adafruit_pixelbuf_pixelbuf_get_auto_write(self->pixelbuf);
    if (auto_write) {
        common_hal_adafruit_pixelbuf_pixelbuf_set_auto_write(self->pixelbuf, false);
    }
    return auto_write;
}

static void auto_write_reapply(pixelmap_pixelmap_obj_t *self, bool auto_write) {
    if (auto_write) {
        common_hal_adafruit_pixelbuf_pixelbuf_set_auto_write(self->pixelbuf, true);
        common_hal_adafruit_pixelbuf_pixelbuf_show(self->pixelbuf);
    }
}

bool shared_module_pixelmap_pixelmap_auto_write_get(pixelmap_pixelmap_obj_t *self) {
    return self->auto_write;
}

void shared_module_pixelmap_pixelmap_auto_write_set(pixelmap_pixelmap_obj_t *self, bool auto_write) {
    self->auto_write = auto_write;
}

void shared_module_pixelmap_pixelmap_fill(pixelmap_pixelmap_obj_t *self, const mp_obj_t color) {
    color_u rgbw;
    common_hal_adafruit_pixelbuf_pixelbuf_parse_color(self->pixelbuf, color, &rgbw.r, &rgbw.g, &rgbw.b, &rgbw.w);
    bool auto_write = auto_write_get_and_clear(self);

    for (size_t i = 0; i < self->len; i++) {
        pixelmap_set_pixel_rgbw(self, i, rgbw);
    }

    auto_write_reapply(self, auto_write);
}

mp_obj_t shared_module_pixelmap_pixelmap_indices(pixelmap_pixelmap_obj_t *self, int index) {
    mp_arg_validate_index_range(index, 0, self->len - 1, MP_QSTR_index);

    mp_obj_t item = self->items[index];
    if (mp_obj_is_small_int(item)) {
        return mp_obj_new_tuple(1, &item);
    } else {
        return item;
    }
}

#if MICROPY_PY_BUILTINS_SLICE

mp_obj_t shared_module_pixelmap_pixelmap_getslice(pixelmap_pixelmap_obj_t *self, mp_bound_slice_t slice, size_t slice_len) {
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(slice_len, NULL));
    for (uint i = 0; i < slice_len; i++) {
        t->items[i] = shared_module_pixelmap_pixelmap_getitem(self, i * slice.step + slice.start);
    }
    return MP_OBJ_FROM_PTR(t);
}

void shared_module_pixelmap_pixelmap_setslice(pixelmap_pixelmap_obj_t *self, const mp_obj_t values, mp_bound_slice_t slice, size_t slice_len) {
    size_t num_items = mp_obj_get_int(mp_obj_len(values));
    if (num_items != slice_len) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Unmatched number of items on RHS (expected %d, got %d)."), slice_len, num_items);
    }

    bool auto_write = auto_write_get_and_clear(self);

    // because we didn't preflight the pixel values, an exception could occur.
    // In that case we need to do the auto-write of any pixels that were set
    // before re-raising the exception
    nlr_buf_t nlr;

    size_t start = slice.start;
    mp_int_t step = slice.step;

    if (nlr_push(&nlr) == 0) {
        mp_obj_iter_buf_t iter_buf;
        mp_obj_t iterable = mp_getiter(values, &iter_buf);
        mp_obj_t item;
        while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
            pixelmap_set_pixel(self, start, item);
            start += step;
        }
        nlr_pop();
        auto_write_reapply(self, auto_write);
    } else {
        auto_write_reapply(self, auto_write);
        // exception converting color value, re-raise
        nlr_raise(MP_OBJ_FROM_PTR(nlr.ret_val));
    }
}
#endif

void shared_module_pixelmap_pixelmap_setitem(pixelmap_pixelmap_obj_t *self, mp_int_t i, mp_obj_t color) {
    color_u rgbw;
    common_hal_adafruit_pixelbuf_pixelbuf_parse_color(self->pixelbuf, color, &rgbw.r, &rgbw.g, &rgbw.b, &rgbw.w);
    bool auto_write = auto_write_get_and_clear(self);
    pixelmap_set_pixel_rgbw(self, i, rgbw);
    auto_write_reapply(self, auto_write);
}

mp_obj_t shared_module_pixelmap_pixelmap_getitem(pixelmap_pixelmap_obj_t *self, mp_int_t i) {
    mp_arg_validate_index_range(i, 0, self->len - 1, MP_QSTR_index);
    mp_obj_t item = self->items[i];
    if (mp_obj_is_small_int(item)) {
        return common_hal_adafruit_pixelbuf_pixelbuf_get_pixel(self->pixelbuf, MP_OBJ_SMALL_INT_VALUE(item));
    } else {
        size_t len;
        mp_obj_t *items;
        mp_obj_tuple_get(item, &len, &items);
        return common_hal_adafruit_pixelbuf_pixelbuf_get_pixel(self->pixelbuf, MP_OBJ_SMALL_INT_VALUE(items[0]));
    }
}
