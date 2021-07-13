/*
 * This file is part of the CircuitPython project, https://github.com/adafruit/circuitpython
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Rose Hooper
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


#include "py/obj.h"
#include "py/objstr.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "shared-bindings/_pixelbuf/PixelBuf.h"
#include <string.h>
#include <math.h>

// Helper to ensure we have the native super class instead of a subclass.
static pixelbuf_pixelbuf_obj_t *native_pixelbuf(mp_obj_t pixelbuf_obj) {
    mp_obj_t native_pixelbuf = mp_obj_cast_to_native_base(pixelbuf_obj, &pixelbuf_pixelbuf_type);
    mp_obj_assert_native_inited(native_pixelbuf);
    return MP_OBJ_TO_PTR(native_pixelbuf);
}

void common_hal__pixelbuf_pixelbuf_construct(pixelbuf_pixelbuf_obj_t *self, size_t n,
    pixelbuf_byteorder_details_t *byteorder, mp_float_t brightness, bool auto_write,
    uint8_t *header, size_t header_len, uint8_t *trailer, size_t trailer_len) {

    self->pixel_count = n;
    self->byteorder = *byteorder;  // Copied because we modify for dotstar
    self->bytes_per_pixel = byteorder->is_dotstar ? 4 : byteorder->bpp;
    self->auto_write = false;

    size_t pixel_len = self->pixel_count * self->bytes_per_pixel;
    self->transmit_buffer_obj = mp_obj_new_bytes_of_zeros(header_len + pixel_len + trailer_len);
    mp_obj_str_t *o = MP_OBJ_TO_PTR(self->transmit_buffer_obj);

    // Abuse the bytes object a bit by mutating it's data by dropping the const. If the user's
    // Python code holds onto it, they'll find out that it changes. At least this way it isn't
    // mutable by the code itself.
    uint8_t *transmit_buffer = (uint8_t *)o->data;
    memcpy(transmit_buffer, header, header_len);
    memcpy(transmit_buffer + header_len + pixel_len, trailer, trailer_len);
    self->post_brightness_buffer = transmit_buffer + header_len;

    if (self->byteorder.is_dotstar) {
        // Initialize the buffer with the dotstar start bytes.
        // Note: Header and end must be setup by caller
        for (uint i = 0; i < self->pixel_count * 4; i += 4) {
            self->post_brightness_buffer[i] = DOTSTAR_LED_START_FULL_BRIGHT;
        }
    }
    // Call set_brightness so that it can allocate a second buffer if needed.
    self->brightness = 1.0;
    self->scaled_brightness = 0x100;
    common_hal__pixelbuf_pixelbuf_set_brightness(MP_OBJ_FROM_PTR(self), brightness);

    // Turn on auto_write. We don't want to do it with the above brightness call.
    self->auto_write = auto_write;
}

size_t common_hal__pixelbuf_pixelbuf_get_len(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return self->pixel_count;
}

uint8_t common_hal__pixelbuf_pixelbuf_get_bpp(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return self->byteorder.bpp;
}

mp_obj_t common_hal__pixelbuf_pixelbuf_get_byteorder_string(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return self->byteorder.order_string;
}

bool common_hal__pixelbuf_pixelbuf_get_auto_write(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return self->auto_write;
}

void common_hal__pixelbuf_pixelbuf_set_auto_write(mp_obj_t self_in, bool auto_write) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    self->auto_write = auto_write;
}

mp_float_t common_hal__pixelbuf_pixelbuf_get_brightness(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    return self->brightness;
}

void common_hal__pixelbuf_pixelbuf_set_brightness(mp_obj_t self_in, mp_float_t brightness) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    // Skip out if the brightness is already set. The default of self->brightness is 1.0. So, this
    // also prevents the pre_brightness_buffer allocation when brightness is set to 1.0 again.
    self->brightness = brightness;
    // Use 256 steps of brightness so that we can do integer math below.
    uint16_t new_scaled_brightness = (uint16_t)(brightness * 256);
    if (new_scaled_brightness == self->scaled_brightness) {
        return;
    }
    self->scaled_brightness = new_scaled_brightness;
    size_t pixel_len = self->pixel_count * self->bytes_per_pixel;
    if (self->scaled_brightness == 0x100 && !self->pre_brightness_buffer) {
        return;
    } else {
        if (self->pre_brightness_buffer == NULL) {
            self->pre_brightness_buffer = m_malloc(pixel_len, false);
            memcpy(self->pre_brightness_buffer, self->post_brightness_buffer, pixel_len);
        }
        for (size_t i = 0; i < pixel_len; i++) {
            // Don't adjust per-pixel luminance bytes in dotstar mode
            if (self->byteorder.is_dotstar && i % 4 == 0) {
                continue;
            }
            self->post_brightness_buffer[i] = (self->pre_brightness_buffer[i] * self->scaled_brightness) / 256;
        }

        if (self->auto_write) {
            common_hal__pixelbuf_pixelbuf_show(self_in);
        }
    }
}

uint8_t _pixelbuf_get_as_uint8(mp_obj_t obj) {
    if (mp_obj_is_small_int(obj)) {
        return MP_OBJ_SMALL_INT_VALUE(obj);
    } else if (mp_obj_is_int(obj)) {
        return mp_obj_get_int_truncated(obj);
    } else if (mp_obj_is_float(obj)) {
        return (uint8_t)mp_obj_get_float(obj);
    }
    mp_raise_TypeError_varg(
        translate("can't convert %q to %q"), mp_obj_get_type_qstr(obj), MP_QSTR_int);
}

void _pixelbuf_parse_color(pixelbuf_pixelbuf_obj_t *self, mp_obj_t color, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *w) {
    pixelbuf_byteorder_details_t *byteorder = &self->byteorder;
    // w is shared between white in NeoPixels and brightness in dotstars (so that DotStars can have
    // per-pixel brightness). Set the defaults here in case it isn't set below.
    if (byteorder->is_dotstar) {
        *w = 255;
    } else {
        *w = 0;
    }

    if (mp_obj_is_int(color) || mp_obj_is_float(color)) {
        mp_int_t value = mp_obj_is_int(color) ? mp_obj_get_int_truncated(color) : mp_obj_get_float(color);
        *r = value >> 16 & 0xff;
        *g = (value >> 8) & 0xff;
        *b = value & 0xff;
    } else {
        mp_obj_t *items;
        size_t len;
        mp_obj_get_array(color, &len, &items);
        if (len < 3 || len > 4) {
            mp_raise_ValueError_varg(translate("Expected tuple of length %d, got %d"), byteorder->bpp, len);
        }

        *r = _pixelbuf_get_as_uint8(items[PIXEL_R]);
        *g = _pixelbuf_get_as_uint8(items[PIXEL_G]);
        *b = _pixelbuf_get_as_uint8(items[PIXEL_B]);
        if (len > 3) {
            if (mp_obj_is_float(items[PIXEL_W])) {
                *w = 255 * mp_obj_get_float(items[PIXEL_W]);
            } else {
                *w = mp_obj_get_int_truncated(items[PIXEL_W]);
            }
            return;
        }
    }
    // Int colors can't set white directly so convert to white when all components are equal.
    // Also handles RGBW values assigned an RGB tuple.
    if (!byteorder->is_dotstar && byteorder->bpp == 4 && byteorder->has_white && *r == *g && *r == *b) {
        *w = *r;
        *r = 0;
        *g = 0;
        *b = 0;
    }
}

void _pixelbuf_set_pixel_color(pixelbuf_pixelbuf_obj_t *self, size_t index, uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    // DotStars don't have white, instead they have 5 bit brightness so pack it into w. Shift right
    // by three to leave the top five bits.
    if (self->bytes_per_pixel == 4 && self->byteorder.is_dotstar) {
        w = DOTSTAR_LED_START | w >> 3;
    }
    pixelbuf_rgbw_t *rgbw_order = &self->byteorder.byteorder;
    size_t offset = index * self->bytes_per_pixel;
    uint8_t *scaled_buffer, *unscaled_buffer;
    if (self->pre_brightness_buffer) {
        scaled_buffer = self->post_brightness_buffer + offset;
        unscaled_buffer = self->pre_brightness_buffer + offset;
    } else {
        scaled_buffer = NULL;
        unscaled_buffer = self->post_brightness_buffer + offset;
    }

    if (self->bytes_per_pixel == 4) {
        unscaled_buffer[rgbw_order->w] = w;
    }

    unscaled_buffer[rgbw_order->r] = r;
    unscaled_buffer[rgbw_order->g] = g;
    unscaled_buffer[rgbw_order->b] = b;

    if (scaled_buffer) {
        if (self->bytes_per_pixel == 4) {
            if (!self->byteorder.is_dotstar) {
                w = (w * self->scaled_brightness) / 256;
            }
            scaled_buffer[rgbw_order->w] = w;
        }
        scaled_buffer[rgbw_order->r] = (r * self->scaled_brightness) / 256;
        scaled_buffer[rgbw_order->g] = (g * self->scaled_brightness) / 256;
        scaled_buffer[rgbw_order->b] = (b * self->scaled_brightness) / 256;
    }
}

void _pixelbuf_set_pixel(pixelbuf_pixelbuf_obj_t *self, size_t index, mp_obj_t value) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
    _pixelbuf_parse_color(self, value, &r, &g, &b, &w);
    _pixelbuf_set_pixel_color(self, index, r, g, b, w);
}

void common_hal__pixelbuf_pixelbuf_set_pixels(mp_obj_t self_in, size_t start, mp_int_t step, size_t slice_len, mp_obj_t *values,
    mp_obj_tuple_t *flatten_to) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(values, &iter_buf);
    mp_obj_t item;
    size_t i = 0;
    bool flattened = flatten_to != mp_const_none;
    if (flattened) {
        flatten_to->len = self->bytes_per_pixel;
    }
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        if (flattened) {
            flatten_to->items[i % self->bytes_per_pixel] = item;
            if (++i % self->bytes_per_pixel == 0) {
                _pixelbuf_set_pixel(self, start, flatten_to);
                start += step;
            }
        } else {
            _pixelbuf_set_pixel(self, start, item);
            start += step;
        }
    }
    if (self->auto_write) {
        common_hal__pixelbuf_pixelbuf_show(self_in);
    }
}



void common_hal__pixelbuf_pixelbuf_set_pixel(mp_obj_t self_in, size_t index, mp_obj_t value) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    _pixelbuf_set_pixel(self, index, value);
    if (self->auto_write) {
        common_hal__pixelbuf_pixelbuf_show(self_in);
    }
}

mp_obj_t common_hal__pixelbuf_pixelbuf_get_pixel(mp_obj_t self_in, size_t index) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    mp_obj_t elems[self->byteorder.bpp];
    uint8_t *pixel_buffer = self->post_brightness_buffer;
    if (self->pre_brightness_buffer != NULL) {
        pixel_buffer = self->pre_brightness_buffer;
    }
    pixel_buffer += self->byteorder.bpp * index;

    pixelbuf_rgbw_t *rgbw_order = &self->byteorder.byteorder;
    elems[0] = MP_OBJ_NEW_SMALL_INT(pixel_buffer[rgbw_order->r]);
    elems[1] = MP_OBJ_NEW_SMALL_INT(pixel_buffer[rgbw_order->g]);
    elems[2] = MP_OBJ_NEW_SMALL_INT(pixel_buffer[rgbw_order->b]);
    if (self->byteorder.bpp > 3) {
        uint8_t w = pixel_buffer[rgbw_order->w];
        if (self->byteorder.is_dotstar) {
            elems[3] = mp_obj_new_float((w & 0b00011111) / 31.0);
        } else {
            elems[3] = MP_OBJ_NEW_SMALL_INT(w);
        }
    }

    return mp_obj_new_tuple(self->byteorder.bpp, elems);
}

void common_hal__pixelbuf_pixelbuf_show(mp_obj_t self_in) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);
    mp_obj_t dest[2 + 1];
    mp_load_method(self_in, MP_QSTR__transmit, dest);

    dest[2] = self->transmit_buffer_obj;

    mp_call_method_n_kw(1, 0, dest);
}

void common_hal__pixelbuf_pixelbuf_fill(mp_obj_t self_in, mp_obj_t fill_color) {
    pixelbuf_pixelbuf_obj_t *self = native_pixelbuf(self_in);

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
    _pixelbuf_parse_color(self, fill_color, &r, &g, &b, &w);

    for (size_t i = 0; i < self->pixel_count; i++) {
        _pixelbuf_set_pixel_color(self, i, r, g, b, w);
    }
    if (self->auto_write) {
        common_hal__pixelbuf_pixelbuf_show(self_in);
    }
}
