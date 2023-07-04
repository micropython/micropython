/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien George, Mark Shannon
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

#include <string.h>
#include "py/runtime.h"
#include "microbitimage.h"
#include "microbitconstimage.h"
#include "py/runtime0.h"
#include "microbitfont.h"

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

const monochrome_5by5_t microbit_blank_image = {
    { &microbit_image_type },
    1, 0, 0, 0,
    { 0, 0, 0 }
};

STATIC void microbit_image_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    mp_printf(print, "Image(");
    if (kind == PRINT_STR)
        mp_printf(print, "\n    ");
    mp_printf(print, "'");
    for (int y = 0; y < imageHeight(self); ++y) {
        for (int x = 0; x < imageWidth(self); ++x) {
            mp_printf(print, "%c", "0123456789"[imageGetPixelValue(self, x, y)]);
        }
        mp_printf(print, ":");
        if (kind == PRINT_STR && y < imageHeight(self)-1)
            mp_printf(print, "'\n    '");
    }
    mp_printf(print, "'");
    if (kind == PRINT_STR)
        mp_printf(print, "\n");
    mp_printf(print, ")");
}

uint8_t monochromeGetPixelValue(monochrome_5by5_t * p_mono, mp_int_t x, mp_int_t y) {
    unsigned int index = y*5+x;
    if (index == 24)
        return p_mono->pixel44;
    return (p_mono->bits24[index>>3] >> (index&7))&1;
}

uint8_t greyscaleGetPixelValue(greyscale_t * p_greyscale, mp_int_t x, mp_int_t y) {
    unsigned int index = y*p_greyscale->width+x;
    unsigned int shift = ((index<<2)&4);
    return (p_greyscale->byte_data[index>>1] >> shift)&15;
}

void greyscaleSetPixelValue(greyscale_t * p_greyscale, mp_int_t x, mp_int_t y, mp_int_t val) {
    unsigned int index = y*p_greyscale->width+x;
    unsigned int shift = ((index<<2)&4);
    uint8_t mask = 240 >> shift;
    p_greyscale->byte_data[index>>1] = (p_greyscale->byte_data[index>>1] & mask) | (val << shift);
}

void greyscaleFill(greyscale_t * p_greyscale, mp_int_t val) {
    mp_int_t byte = (val<<4) | val;
    for (int i = 0; i < ((p_greyscale->width*p_greyscale->height+1)>>1); i++) {
        p_greyscale->byte_data[i] = byte;
    }
}

void greyscaleClear(greyscale_t * p_greyscale) {
    memset(&p_greyscale->byte_data, 0, (p_greyscale->width*p_greyscale->height+1)>>1);
}

uint8_t imageGetPixelValue(microbit_image_obj_t * p_image, mp_int_t x, mp_int_t y) {
    if (p_image->base.five)
        return monochromeGetPixelValue(&p_image->monochrome_5by5, x, y)*MAX_BRIGHTNESS;
    else
        return greyscaleGetPixelValue(&p_image->greyscale, x, y);
}

mp_int_t imageWidth(microbit_image_obj_t * p_image) {
    if (p_image->base.five)
        return 5;
    else
        return p_image->greyscale.width;
}

mp_int_t imageHeight(microbit_image_obj_t * p_image) {
    if (p_image->base.five)
        return 5;
    else
        return p_image->greyscale.height;
}

STATIC greyscale_t *greyscale_new(mp_int_t w, mp_int_t h) {
    greyscale_t *result = mp_obj_malloc_var(greyscale_t, uint8_t, (w*h+1)>>1, &microbit_image_type);
    result->five = 0;
    result->width = w;
    result->height = h;
    return result;
}

greyscale_t * imageCopy(microbit_image_obj_t * p_image) {
    mp_int_t w = imageWidth(p_image);
    mp_int_t h = imageHeight(p_image);
    greyscale_t *result = greyscale_new(w, h);
    for (mp_int_t y = 0; y < h; y++) {
        for (mp_int_t x = 0; x < w; ++x) {
            greyscaleSetPixelValue(result, x,y, imageGetPixelValue(p_image, x,y));
        }
    }
    return result;
}

greyscale_t * imageInvert(microbit_image_obj_t * p_image) {
    mp_int_t w = imageWidth(p_image);
    mp_int_t h = imageHeight(p_image);
    greyscale_t *result = greyscale_new(w, h);
    for (mp_int_t y = 0; y < h; y++) {
        for (mp_int_t x = 0; x < w; ++x) {
            greyscaleSetPixelValue(result, x,y, MAX_BRIGHTNESS - imageGetPixelValue(p_image, x,y));
        }
    }
    return result;
}

STATIC microbit_image_obj_t *image_from_parsed_str(const char *s, mp_int_t len) {
    mp_int_t w = 0;
    mp_int_t h = 0;
    mp_int_t line_len = 0;
    greyscale_t *result;
    /*First pass -- Establish metadata */
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == '\n' || c == ':') {
            w = max(line_len, w);
            line_len = 0;
            ++h;
        } else if (c == ' ') {
            ++line_len;
        } else if ('c' >= '0' && c <= '9') {
            ++line_len;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Unexpected character in Image definition."));
        }
    }
    if (line_len) {
        // Omitted trailing terminator
        ++h;
        w = max(line_len, w);
    }
    result = greyscale_new(w, h);
    mp_int_t x = 0;
    mp_int_t y = 0;
    /* Second pass -- Fill in data */
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == '\n' || c == ':') {
            while (x < w) {
                greyscaleSetPixelValue(result, x, y, 0);
                x++;
            }
            ++y;
            x = 0;
        } else if (c == ' ') {
            /* Treat spaces as 0 */
            greyscaleSetPixelValue(result, x, y, 0);
            ++x;
        } else if ('c' >= '0' && c <= '9') {
            greyscaleSetPixelValue(result, x, y, c - '0');
            ++x;
        }
    }
    if (y < h) {
        while (x < w) {
            greyscaleSetPixelValue(result, x, y, 0);
            x++;
        }
    }
    return (microbit_image_obj_t *)result;
}


STATIC mp_obj_t microbit_image_make_new(const mp_obj_type_t *type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    (void)type_in;
    mp_arg_check_num(n_args, n_kw, 0, 3, false);

    switch (n_args) {
        case 0: {
            greyscale_t *image = greyscale_new(5, 5);
            greyscaleClear(image);
            return image;
        }

        case 1: {
            if (mp_obj_is_str(args[0])) {
                // arg is a string object
                size_t len;
                const char *str = mp_obj_str_get_data(args[0], &len);
                // make image from string
                if (len == 1) {
                    /* For a single character, return the font glyph */
                    return microbit_image_for_char(str[0]);
                } else {
                    /* Otherwise parse the image description string */
                    return image_from_parsed_str(str, len);
                }
            } else {
                mp_raise_TypeError(MP_ERROR_TEXT("Image(s) takes a string."));
            }
        }

        case 2:
        case 3: {
            mp_int_t w = mp_obj_get_int(args[0]);
            mp_int_t h = mp_obj_get_int(args[1]);
            greyscale_t *image = greyscale_new(w, h);
            if (n_args == 2) {
                greyscaleClear(image);
            } else {
                mp_buffer_info_t bufinfo;
                mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);

                if (w < 0 || h < 0 || (size_t)(w * h) != bufinfo.len) {
                    mp_raise_ValueError(MP_ERROR_TEXT("image data is incorrect size"));
                }
                mp_int_t i = 0;
                for (mp_int_t y = 0; y < h; y++) {
                    for (mp_int_t x = 0; x < w; ++x) {
                        uint8_t val = min(((const uint8_t*)bufinfo.buf)[i], MAX_BRIGHTNESS);
                        greyscaleSetPixelValue(image, x, y, val);
                        ++i;
                    }
                }
            }
            return image;
        }

        default: {
            mp_raise_TypeError(MP_ERROR_TEXT("Image() takes 0 to 3 arguments"));
        }
    }
}

static void clear_rect(greyscale_t *img, mp_int_t x0, mp_int_t y0,mp_int_t x1, mp_int_t y1) {
    for (int i = x0; i < x1; ++i) {
        for (int j = y0; j < y1; ++j) {
            greyscaleSetPixelValue(img, i, j, 0);
        }
    }
}

STATIC void image_blit(microbit_image_obj_t *src, greyscale_t *dest, mp_int_t x, mp_int_t y, mp_int_t w, mp_int_t h, mp_int_t xdest, mp_int_t ydest) {
    if (w < 0)
        w = 0;
    if (h < 0)
        h = 0;
    mp_int_t intersect_x0 = max(max(0, x), -xdest);
    mp_int_t intersect_y0 = max(max(0, y), -ydest);
    mp_int_t intersect_x1 = min(min(dest->width+x-xdest, imageWidth(src)), x+w);
    mp_int_t intersect_y1 = min(min(dest->height+y-ydest, imageHeight(src)), y+h);
    mp_int_t xstart, xend, ystart, yend, xdel, ydel;
    mp_int_t clear_x0 = max(0, xdest);
    mp_int_t clear_y0 = max(0, ydest);
    mp_int_t clear_x1 = min(dest->width, xdest+w);
    mp_int_t clear_y1 = min(dest->height, ydest+h);
    if (intersect_x0 >= intersect_x1 || intersect_y0 >= intersect_y1) {
        // Nothing to copy
        clear_rect(dest, clear_x0, clear_y0, clear_x1, clear_y1);
        return;
    }
    if (x > xdest) {
        xstart = intersect_x0; xend = intersect_x1; xdel = 1;
    } else {
        xstart = intersect_x1-1; xend = intersect_x0-1; xdel = -1;
    }
    if (y > ydest) {
        ystart = intersect_y0; yend = intersect_y1; ydel = 1;
    } else {
        ystart = intersect_y1-1; yend = intersect_y0-1; ydel = -1;
    }
    for (int i = xstart; i != xend; i += xdel) {
        for (int j = ystart; j != yend; j += ydel) {
            int val = imageGetPixelValue(src, i, j);
            greyscaleSetPixelValue(dest, i+xdest-x, j+ydest-y, val);
        }
    }
    // Adjust intersection rectangle to dest
    intersect_x0 += xdest-x;
    intersect_y0 += ydest-y;
    intersect_x1 += xdest-x;
    intersect_y1 += ydest-y;
    // Clear four rectangles in the cleared area surrounding the copied area.
    clear_rect(dest, clear_x0, clear_y0, intersect_x0, intersect_y1);
    clear_rect(dest, clear_x0, intersect_y1, intersect_x1, clear_y1);
    clear_rect(dest, intersect_x1, intersect_y0, clear_x1, clear_y1);
    clear_rect(dest, intersect_x0, clear_y0, clear_x1, intersect_y0);
}

greyscale_t *image_shift(microbit_image_obj_t *self, mp_int_t x, mp_int_t y) {
    greyscale_t *result = greyscale_new(imageWidth(self), imageWidth(self));
    image_blit(self, result, x, y, imageWidth(self), imageWidth(self), 0, 0);
    return result;
}

STATIC microbit_image_obj_t *image_crop(microbit_image_obj_t *img, mp_int_t x, mp_int_t y, mp_int_t w, mp_int_t h) {
    if (w < 0)
        w = 0;
    if (h < 0)
        h = 0;
    greyscale_t *result = greyscale_new(w, h);
    image_blit(img, result, x, y, w, h, 0, 0);
    return (microbit_image_obj_t *)result;
}

mp_obj_t microbit_image_width(mp_obj_t self_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    return MP_OBJ_NEW_SMALL_INT(imageWidth(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_image_width_obj, microbit_image_width);

mp_obj_t microbit_image_height(mp_obj_t self_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    return MP_OBJ_NEW_SMALL_INT(imageHeight(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_image_height_obj, microbit_image_height);

mp_obj_t microbit_image_get_pixel(mp_obj_t self_in, mp_obj_t x_in, mp_obj_t y_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    mp_int_t x = mp_obj_get_int(x_in);
    mp_int_t y = mp_obj_get_int(y_in);
    if (x < 0 || y < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("index can't be negative"));
    }
    if (x < imageWidth(self) && y < imageHeight(self)) {
        return MP_OBJ_NEW_SMALL_INT(imageGetPixelValue(self, x, y));
    }
    mp_raise_ValueError(MP_ERROR_TEXT("index too large"));
}
MP_DEFINE_CONST_FUN_OBJ_3(microbit_image_get_pixel_obj, microbit_image_get_pixel);

/* Raise an exception if not mutable */
static void check_mutability(microbit_image_obj_t *self) {
    if (self->base.five) {
        mp_raise_TypeError(MP_ERROR_TEXT("image can't be modified (try copying first)"));
    }
}


mp_obj_t microbit_image_set_pixel(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args;
    microbit_image_obj_t *self = (microbit_image_obj_t*)args[0];
    check_mutability(self);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);
    if (x < 0 || y < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("index can't be negative"));
    }
    mp_int_t bright = mp_obj_get_int(args[3]);
    if (bright < 0 || bright > MAX_BRIGHTNESS)
        mp_raise_ValueError(MP_ERROR_TEXT("brightness out of bounds."));
    if (x < imageWidth(self) && y < imageHeight(self)) {
        greyscaleSetPixelValue(&(self->greyscale), x, y, bright);
        return mp_const_none;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("index too large"));
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(microbit_image_set_pixel_obj, 4, 4, microbit_image_set_pixel);

mp_obj_t microbit_image_fill(mp_obj_t self_in, mp_obj_t n_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    check_mutability(self);
    mp_int_t n = mp_obj_get_int(n_in);
    if (n < 0 || n > MAX_BRIGHTNESS) {
        mp_raise_ValueError(MP_ERROR_TEXT("brightness out of bounds."));
    }
    greyscaleFill(&self->greyscale, n);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(microbit_image_fill_obj, microbit_image_fill);

mp_obj_t microbit_image_blit(mp_uint_t n_args, const mp_obj_t *args) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)args[0];
    check_mutability(self);

    mp_obj_t src = args[1];
    if (mp_obj_get_type(src) != &microbit_image_type) {
        mp_raise_TypeError(MP_ERROR_TEXT("expecting an image"));
    }
    if (n_args == 7) {
        mp_raise_TypeError(MP_ERROR_TEXT("must specify both offsets"));
    }
    mp_int_t x = mp_obj_get_int(args[2]);
    mp_int_t y = mp_obj_get_int(args[3]);
    mp_int_t w = mp_obj_get_int(args[4]);
    mp_int_t h = mp_obj_get_int(args[5]);
    if (w < 0 || h < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("size can't be negative"));
    }
    mp_int_t xdest;
    mp_int_t ydest;
    if (n_args == 6) {
        xdest = 0;
        ydest = 0;
    } else {
        xdest = mp_obj_get_int(args[6]);
        ydest = mp_obj_get_int(args[7]);
    }
    image_blit((microbit_image_obj_t *)src, &(self->greyscale), x, y, w, h, xdest, ydest);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(microbit_image_blit_obj, 6, 8, microbit_image_blit);

mp_obj_t microbit_image_crop(mp_uint_t n_args, const mp_obj_t *args) {
    (void)n_args;
    microbit_image_obj_t *self = (microbit_image_obj_t*)args[0];
    mp_int_t x0 = mp_obj_get_int(args[1]);
    mp_int_t y0 = mp_obj_get_int(args[2]);
    mp_int_t x1 = mp_obj_get_int(args[3]);
    mp_int_t y1 = mp_obj_get_int(args[4]);
    return image_crop(self, x0, y0, x1, y1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(microbit_image_crop_obj, 5, 5, microbit_image_crop);

mp_obj_t microbit_image_shift_left(mp_obj_t self_in, mp_obj_t n_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    mp_int_t n = mp_obj_get_int(n_in);
    return image_shift(self, n, 0);
}
MP_DEFINE_CONST_FUN_OBJ_2(microbit_image_shift_left_obj, microbit_image_shift_left);

mp_obj_t microbit_image_shift_right(mp_obj_t self_in, mp_obj_t n_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    mp_int_t n = mp_obj_get_int(n_in);
    return image_shift(self, -n, 0);
}
MP_DEFINE_CONST_FUN_OBJ_2(microbit_image_shift_right_obj, microbit_image_shift_right);

mp_obj_t microbit_image_shift_up(mp_obj_t self_in, mp_obj_t n_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    mp_int_t n = mp_obj_get_int(n_in);
    return image_shift(self, 0, n);
}
MP_DEFINE_CONST_FUN_OBJ_2(microbit_image_shift_up_obj, microbit_image_shift_up);

mp_obj_t microbit_image_shift_down(mp_obj_t self_in, mp_obj_t n_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    mp_int_t n = mp_obj_get_int(n_in);
    return image_shift(self, 0, -n);
}
MP_DEFINE_CONST_FUN_OBJ_2(microbit_image_shift_down_obj, microbit_image_shift_down);

mp_obj_t microbit_image_copy(mp_obj_t self_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    return imageCopy(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_image_copy_obj, microbit_image_copy);

mp_obj_t microbit_image_invert(mp_obj_t self_in) {
    microbit_image_obj_t *self = (microbit_image_obj_t*)self_in;
    return imageInvert(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(microbit_image_invert_obj, microbit_image_invert);


STATIC const mp_rom_map_elem_t microbit_image_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_width), MP_ROM_PTR(&microbit_image_width_obj) },
    { MP_ROM_QSTR(MP_QSTR_height), MP_ROM_PTR(&microbit_image_height_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_pixel), MP_ROM_PTR(&microbit_image_get_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_pixel), MP_ROM_PTR(&microbit_image_set_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_shift_left), MP_ROM_PTR(&microbit_image_shift_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_shift_right), MP_ROM_PTR(&microbit_image_shift_right_obj) },
    { MP_ROM_QSTR(MP_QSTR_shift_up), MP_ROM_PTR(&microbit_image_shift_up_obj) },
    { MP_ROM_QSTR(MP_QSTR_shift_down), MP_ROM_PTR(&microbit_image_shift_down_obj) },
    { MP_ROM_QSTR(MP_QSTR_copy), MP_ROM_PTR(&microbit_image_copy_obj) },
    { MP_ROM_QSTR(MP_QSTR_crop), MP_ROM_PTR(&microbit_image_crop_obj) },
    { MP_ROM_QSTR(MP_QSTR_invert), MP_ROM_PTR(&microbit_image_invert_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&microbit_image_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_blit), MP_ROM_PTR(&microbit_image_blit_obj) },

    { MP_ROM_QSTR(MP_QSTR_HEART), MP_ROM_PTR(&microbit_const_image_heart_obj) },
    { MP_ROM_QSTR(MP_QSTR_HEART_SMALL), MP_ROM_PTR(&microbit_const_image_heart_small_obj) },
    { MP_ROM_QSTR(MP_QSTR_HAPPY), MP_ROM_PTR(&microbit_const_image_happy_obj) },
    { MP_ROM_QSTR(MP_QSTR_SMILE), MP_ROM_PTR(&microbit_const_image_smile_obj) },
    { MP_ROM_QSTR(MP_QSTR_SAD), MP_ROM_PTR(&microbit_const_image_sad_obj) },
    { MP_ROM_QSTR(MP_QSTR_CONFUSED), MP_ROM_PTR(&microbit_const_image_confused_obj) },
    { MP_ROM_QSTR(MP_QSTR_ANGRY), MP_ROM_PTR(&microbit_const_image_angry_obj) },
    { MP_ROM_QSTR(MP_QSTR_ASLEEP), MP_ROM_PTR(&microbit_const_image_asleep_obj) },
    { MP_ROM_QSTR(MP_QSTR_SURPRISED), MP_ROM_PTR(&microbit_const_image_surprised_obj) },
    { MP_ROM_QSTR(MP_QSTR_SILLY), MP_ROM_PTR(&microbit_const_image_silly_obj) },
    { MP_ROM_QSTR(MP_QSTR_FABULOUS), MP_ROM_PTR(&microbit_const_image_fabulous_obj) },
    { MP_ROM_QSTR(MP_QSTR_MEH), MP_ROM_PTR(&microbit_const_image_meh_obj) },
    { MP_ROM_QSTR(MP_QSTR_YES), MP_ROM_PTR(&microbit_const_image_yes_obj) },
    { MP_ROM_QSTR(MP_QSTR_NO), MP_ROM_PTR(&microbit_const_image_no_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK12), MP_ROM_PTR(&microbit_const_image_clock12_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK1), MP_ROM_PTR(&microbit_const_image_clock1_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK2), MP_ROM_PTR(&microbit_const_image_clock2_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK3), MP_ROM_PTR(&microbit_const_image_clock3_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK4), MP_ROM_PTR(&microbit_const_image_clock4_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK5), MP_ROM_PTR(&microbit_const_image_clock5_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK6), MP_ROM_PTR(&microbit_const_image_clock6_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK7), MP_ROM_PTR(&microbit_const_image_clock7_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK8), MP_ROM_PTR(&microbit_const_image_clock8_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK9), MP_ROM_PTR(&microbit_const_image_clock9_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK10), MP_ROM_PTR(&microbit_const_image_clock10_obj) },
    { MP_ROM_QSTR(MP_QSTR_CLOCK11), MP_ROM_PTR(&microbit_const_image_clock11_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_N), MP_ROM_PTR(&microbit_const_image_arrow_n_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_NE), MP_ROM_PTR(&microbit_const_image_arrow_ne_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_E), MP_ROM_PTR(&microbit_const_image_arrow_e_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_SE), MP_ROM_PTR(&microbit_const_image_arrow_se_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_S), MP_ROM_PTR(&microbit_const_image_arrow_s_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_SW), MP_ROM_PTR(&microbit_const_image_arrow_sw_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_W), MP_ROM_PTR(&microbit_const_image_arrow_w_obj) },
    { MP_ROM_QSTR(MP_QSTR_ARROW_NW), MP_ROM_PTR(&microbit_const_image_arrow_nw_obj) },
    { MP_ROM_QSTR(MP_QSTR_TRIANGLE), MP_ROM_PTR(&microbit_const_image_triangle_obj) },
    { MP_ROM_QSTR(MP_QSTR_TRIANGLE_LEFT), MP_ROM_PTR(&microbit_const_image_triangle_left_obj) },
    { MP_ROM_QSTR(MP_QSTR_CHESSBOARD), MP_ROM_PTR(&microbit_const_image_chessboard_obj) },
    { MP_ROM_QSTR(MP_QSTR_DIAMOND), MP_ROM_PTR(&microbit_const_image_diamond_obj) },
    { MP_ROM_QSTR(MP_QSTR_DIAMOND_SMALL), MP_ROM_PTR(&microbit_const_image_diamond_small_obj) },
    { MP_ROM_QSTR(MP_QSTR_SQUARE), MP_ROM_PTR(&microbit_const_image_square_obj) },
    { MP_ROM_QSTR(MP_QSTR_SQUARE_SMALL), MP_ROM_PTR(&microbit_const_image_square_small_obj) },
    { MP_ROM_QSTR(MP_QSTR_RABBIT), MP_ROM_PTR(&microbit_const_image_rabbit) },
    { MP_ROM_QSTR(MP_QSTR_COW), MP_ROM_PTR(&microbit_const_image_cow) },
    { MP_ROM_QSTR(MP_QSTR_MUSIC_CROTCHET), MP_ROM_PTR(&microbit_const_image_music_crotchet_obj) },
    { MP_ROM_QSTR(MP_QSTR_MUSIC_QUAVER), MP_ROM_PTR(&microbit_const_image_music_quaver_obj) },
    { MP_ROM_QSTR(MP_QSTR_MUSIC_QUAVERS), MP_ROM_PTR(&microbit_const_image_music_quavers_obj) },
    { MP_ROM_QSTR(MP_QSTR_PITCHFORK), MP_ROM_PTR(&microbit_const_image_pitchfork_obj) },
    { MP_ROM_QSTR(MP_QSTR_XMAS), MP_ROM_PTR(&microbit_const_image_xmas_obj) },
    { MP_ROM_QSTR(MP_QSTR_PACMAN), MP_ROM_PTR(&microbit_const_image_pacman_obj) },
    { MP_ROM_QSTR(MP_QSTR_TARGET), MP_ROM_PTR(&microbit_const_image_target_obj) },
    { MP_ROM_QSTR(MP_QSTR_ALL_CLOCKS), MP_ROM_PTR(&microbit_const_image_all_clocks_tuple_obj) },
    { MP_ROM_QSTR(MP_QSTR_ALL_ARROWS), MP_ROM_PTR(&microbit_const_image_all_arrows_tuple_obj) },
    { MP_ROM_QSTR(MP_QSTR_TSHIRT), MP_ROM_PTR(&microbit_const_image_tshirt_obj) },
    { MP_ROM_QSTR(MP_QSTR_ROLLERSKATE), MP_ROM_PTR(&microbit_const_image_rollerskate_obj) },
    { MP_ROM_QSTR(MP_QSTR_DUCK), MP_ROM_PTR(&microbit_const_image_duck_obj) },
    { MP_ROM_QSTR(MP_QSTR_HOUSE), MP_ROM_PTR(&microbit_const_image_house_obj) },
    { MP_ROM_QSTR(MP_QSTR_TORTOISE), MP_ROM_PTR(&microbit_const_image_tortoise_obj) },
    { MP_ROM_QSTR(MP_QSTR_BUTTERFLY), MP_ROM_PTR(&microbit_const_image_butterfly_obj) },
    { MP_ROM_QSTR(MP_QSTR_STICKFIGURE), MP_ROM_PTR(&microbit_const_image_stickfigure_obj) },
    { MP_ROM_QSTR(MP_QSTR_GHOST), MP_ROM_PTR(&microbit_const_image_ghost_obj) },
    { MP_ROM_QSTR(MP_QSTR_SWORD), MP_ROM_PTR(&microbit_const_image_sword_obj) },
    { MP_ROM_QSTR(MP_QSTR_GIRAFFE), MP_ROM_PTR(&microbit_const_image_giraffe_obj) },
    { MP_ROM_QSTR(MP_QSTR_SKULL), MP_ROM_PTR(&microbit_const_image_skull_obj) },
    { MP_ROM_QSTR(MP_QSTR_UMBRELLA), MP_ROM_PTR(&microbit_const_image_umbrella_obj) },
    { MP_ROM_QSTR(MP_QSTR_SNAKE), MP_ROM_PTR(&microbit_const_image_snake_obj) },
};

STATIC MP_DEFINE_CONST_DICT(microbit_image_locals_dict, microbit_image_locals_dict_table);

#define THE_FONT font_pendolino3_5x5_pad3msb

#define ASCII_START 32
#define ASCII_END 126

STATIC const unsigned char *get_font_data_from_char(char c) {
    if (c < ASCII_START || c > ASCII_END) {
        c = '?';
    }
    int offset = (c-ASCII_START) * 5;
    return THE_FONT + offset;
}

STATIC mp_int_t get_pixel_from_font_data(const unsigned char *data, int x, int y) {
    /* The following logic belongs in MicroBitFont */
    return ((data[y]>>(4-x))&1);
}

void microbit_image_set_from_char(greyscale_t *img, char c) {
    const unsigned char *data = get_font_data_from_char(c);
    for (int x = 0; x < 5; ++x) {
        for (int y = 0; y < 5; ++y) {
            greyscaleSetPixelValue(img, x, y, get_pixel_from_font_data(data, x, y)*MAX_BRIGHTNESS);
        }
    }
}


microbit_image_obj_t *microbit_image_for_char(char c) {
    greyscale_t *result = greyscale_new(5,5);
    microbit_image_set_from_char(result, c);
    return (microbit_image_obj_t *)result;
}

#if MICROPY_PY_BUILTINS_FLOAT
microbit_image_obj_t *microbit_image_dim(microbit_image_obj_t *lhs, mp_float_t fval) {
#else
microbit_image_obj_t *microbit_image_dim(microbit_image_obj_t *lhs, mp_int_t fval) {
#endif
    if (fval < 0)
        mp_raise_ValueError(MP_ERROR_TEXT("Brightness multiplier must not be negative."));
    greyscale_t *result = greyscale_new(imageWidth(lhs), imageHeight(lhs));
    for (int x = 0; x < imageWidth(lhs); ++x) {
        for (int y = 0; y < imageWidth(lhs); ++y) {
#if MICROPY_PY_BUILTINS_FLOAT
            int val = min((int)imageGetPixelValue(lhs, x,y)*fval+0.5, MAX_BRIGHTNESS);
#else
            int val = min((int)imageGetPixelValue(lhs, x,y)*fval, MAX_BRIGHTNESS);
#endif
            greyscaleSetPixelValue(result, x, y, val);
        }
    }
    return (microbit_image_obj_t *)result;
}

microbit_image_obj_t *microbit_image_sum(microbit_image_obj_t *lhs, microbit_image_obj_t *rhs, bool add) {
    mp_int_t h = imageHeight(lhs);
    mp_int_t w = imageWidth(lhs);
    if (imageHeight(rhs) != h || imageWidth(lhs) != w) {
// TODO: verify that image width in test above should really test (lhs != w)
        mp_raise_ValueError(MP_ERROR_TEXT("Images must be the same size."));
    }
    greyscale_t *result = greyscale_new(w, h);
    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            int val;
            int lval = imageGetPixelValue(lhs, x,y);
            int rval = imageGetPixelValue(rhs, x,y);
            if (add)
                val = min(lval + rval, MAX_BRIGHTNESS);
            else
                val = max(0, lval - rval);
            greyscaleSetPixelValue(result, x, y, val);
        }
    }
    return (microbit_image_obj_t *)result;
}

STATIC mp_obj_t image_binary_op(mp_binary_op_t op, mp_obj_t lhs_in, mp_obj_t rhs_in) {
    if (mp_obj_get_type(lhs_in) != &microbit_image_type) {
        return MP_OBJ_NULL; // op not supported
    }
    microbit_image_obj_t *lhs = (microbit_image_obj_t *)lhs_in;
    switch(op) {
    case MP_BINARY_OP_ADD:
    case MP_BINARY_OP_SUBTRACT:
        break;
    case MP_BINARY_OP_MULTIPLY:
#if MICROPY_PY_BUILTINS_FLOAT
        return microbit_image_dim(lhs, mp_obj_get_float(rhs_in));
#else
        return microbit_image_dim(lhs, mp_obj_get_int(rhs_in) * 10);
#endif
    case MP_BINARY_OP_TRUE_DIVIDE:
#if MICROPY_PY_BUILTINS_FLOAT
        return microbit_image_dim(lhs, 1.0/mp_obj_get_float(rhs_in));
#else
        break;
    case MP_BINARY_OP_FLOOR_DIVIDE:
        return microbit_image_dim(lhs, (100/mp_obj_get_int(rhs_in) + 5) / 10);
#endif
    default:
        return MP_OBJ_NULL; // op not supported
    }
    if (mp_obj_get_type(rhs_in) != &microbit_image_type) {
        return MP_OBJ_NULL; // op not supported
    }
    return microbit_image_sum(lhs, (microbit_image_obj_t *)rhs_in, op == MP_BINARY_OP_ADD);
}


MP_DEFINE_CONST_OBJ_TYPE(
    microbit_image_type,
    MP_QSTR_MicroBitImage,
    MP_TYPE_FLAG_NONE,
    make_new, microbit_image_make_new,
    print, microbit_image_print,
    binary_op, image_binary_op,
    locals_dict, &microbit_image_locals_dict
    );

typedef struct _scrolling_string_t {
    mp_obj_base_t base;
    char const *str;
    mp_uint_t len;
    mp_obj_t ref;
    bool monospace;
    bool repeat;
} scrolling_string_t;

typedef struct _scrolling_string_iterator_t {
    mp_obj_base_t base;
    mp_obj_t ref;
    greyscale_t *img;
    char const *next_char;
    char const *start;
    char const *end;
    uint8_t offset;
    uint8_t offset_limit;
    bool monospace;
    bool repeat;
    char right;
} scrolling_string_iterator_t;

extern const mp_obj_type_t microbit_scrolling_string_type;
extern const mp_obj_type_t microbit_scrolling_string_iterator_type;

mp_obj_t scrolling_string_image_iterable(const char* str, mp_uint_t len, mp_obj_t ref, bool monospace, bool repeat) {
    scrolling_string_t *result = mp_obj_malloc(scrolling_string_t, &microbit_scrolling_string_type);
    result->str = str;
    result->len = len;
    result->ref = ref;
    result->monospace = monospace;
    result->repeat = repeat;
    return result;
}

STATIC int font_column_non_blank(const unsigned char *font_data, unsigned int col) {
    for (int y = 0; y < 5; ++y) {
        if (get_pixel_from_font_data(font_data, col, y)) {
            return 1;
        }
    }
    return 0;
}

/* Not strictly the rightmost non-blank column, but the rightmost in columns 2,3 or 4. */
STATIC unsigned int rightmost_non_blank_column(const unsigned char *font_data) {
    if (font_column_non_blank(font_data, 4)) {
        return 4;
    }
    if (font_column_non_blank(font_data, 3)) {
        return 3;
    }
    return 2;
}

static void restart(scrolling_string_iterator_t *iter) {
    iter->next_char = iter->start;
    iter->offset = 0;
    if (iter->start < iter->end) {
        iter->right = *iter->next_char;
        if (iter->monospace) {
            iter->offset_limit = 5;
        } else {
            iter->offset_limit = rightmost_non_blank_column(get_font_data_from_char(iter->right)) + 1;
        }
    } else {
        iter->right = ' ';
        iter->offset_limit = 5;
    }
}

STATIC mp_obj_t get_microbit_scrolling_string_iter(mp_obj_t o_in, mp_obj_iter_buf_t *iter_buf) {
    (void)iter_buf;
    scrolling_string_t *str = (scrolling_string_t *)o_in;
    scrolling_string_iterator_t *result = mp_obj_malloc(scrolling_string_iterator_t, &microbit_scrolling_string_iterator_type);
    result->img = greyscale_new(5,5);
    result->start = str->str;
    result->ref = str->ref;
    result->monospace = str->monospace;
    result->end = result->start + str->len;
    result->repeat = str->repeat;
    restart(result);
    return result;
}

STATIC mp_obj_t microbit_scrolling_string_iter_next(mp_obj_t o_in) {
    scrolling_string_iterator_t *iter = (scrolling_string_iterator_t *)o_in;
    if (iter->next_char == iter->end && iter->offset == 5) {
        if (iter->repeat) {
            restart(iter);
            greyscaleClear(iter->img);
        } else {
            return MP_OBJ_STOP_ITERATION;
        }
    }
    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 5; y++) {
            greyscaleSetPixelValue(iter->img, x, y, greyscaleGetPixelValue(iter->img, x+1, y));
        }
    }
    for (int y = 0; y < 5; y++) {
        greyscaleSetPixelValue(iter->img, 4, y, 0);
    }
    const unsigned char *font_data;
    if (iter->offset < iter->offset_limit) {
        font_data = get_font_data_from_char(iter->right);
        for (int y = 0; y < 5; ++y) {
            int pix = get_pixel_from_font_data(font_data, iter->offset, y)*MAX_BRIGHTNESS;
            greyscaleSetPixelValue(iter->img, 4, y, pix);
        }
    } else if (iter->offset == iter->offset_limit) {
        ++iter->next_char;
        if (iter->next_char == iter->end) {
            iter->right = ' ';
            iter->offset_limit = 5;
            iter->offset = 0;
        } else {
            iter->right = *iter->next_char;
            font_data = get_font_data_from_char(iter->right);
            if (iter->monospace) {
                iter->offset = -1;
                iter->offset_limit = 5;
            } else {
                iter->offset = -font_column_non_blank(font_data, 0);
                iter->offset_limit = rightmost_non_blank_column(font_data)+1;
            }
        }
    }
    ++iter->offset;
    return iter->img;
}

MP_DEFINE_CONST_OBJ_TYPE(
    microbit_scrolling_string_type,
    MP_QSTR_ScrollingString,
    MP_TYPE_FLAG_ITER_IS_GETITER,
    iter, get_microbit_scrolling_string_iter
    );

MP_DEFINE_CONST_OBJ_TYPE(
    microbit_scrolling_string_iterator_type,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, microbit_scrolling_string_iter_next
    );

/** Facade types to present a string as a sequence of images.
 * These are necessary to avoid allocation during iteration,
 * which may happen in interrupt handlers.
 */

typedef struct _string_image_facade_t {
    mp_obj_base_t base;
    mp_obj_t string;
    greyscale_t *image;
} string_image_facade_t;

static mp_obj_t string_image_facade_subscr(mp_obj_t self_in, mp_obj_t index_in, mp_obj_t value) {
    if (value == MP_OBJ_SENTINEL) {
        // Fill in image
        string_image_facade_t *self = (string_image_facade_t *)self_in;
        size_t len;
        const char *text = mp_obj_str_get_data(self->string, &len);
        mp_uint_t index = mp_get_index(self->base.type, len, index_in, false);
        microbit_image_set_from_char(self->image, text[index]);
        return self->image;
    } else {
        return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t facade_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    string_image_facade_t *self = (string_image_facade_t *)self_in;
    switch (op) {
        case MP_UNARY_OP_LEN:
            return mp_obj_len(self->string);
        default: return MP_OBJ_NULL; // op not supported
    }
}

static mp_obj_t microbit_facade_iterator(mp_obj_t iterable_in, mp_obj_iter_buf_t *iter_buf);

MP_DEFINE_CONST_OBJ_TYPE(
    string_image_facade_type,
    MP_QSTR_Facade,
    MP_TYPE_FLAG_ITER_IS_GETITER,
    unary_op, facade_unary_op,
    subscr, string_image_facade_subscr,
    iter, microbit_facade_iterator
    );


typedef struct _facade_iterator_t {
    mp_obj_base_t base;
    mp_obj_t string;
    mp_uint_t index;
    greyscale_t *image;
} facade_iterator_t;

mp_obj_t microbit_string_facade(mp_obj_t string) {
    string_image_facade_t *result = mp_obj_malloc(string_image_facade_t, &string_image_facade_type);
    result->string = string;
    result->image = greyscale_new(5,5);
    return result;
}

static mp_obj_t microbit_facade_iter_next(mp_obj_t iter_in) {
    facade_iterator_t *iter = (facade_iterator_t *)iter_in;
    size_t len;
    const char *text = mp_obj_str_get_data(iter->string, &len);
    if (iter->index >= len) {
        return MP_OBJ_STOP_ITERATION;
    }
    microbit_image_set_from_char(iter->image, text[iter->index]);
    iter->index++;
    return iter->image;
}

MP_DEFINE_CONST_OBJ_TYPE(
    microbit_facade_iterator_type,
    MP_QSTR_iterator,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    iter, microbit_facade_iter_next
    );

mp_obj_t microbit_facade_iterator(mp_obj_t iterable_in, mp_obj_iter_buf_t *iter_buf) {
    (void)iter_buf;
    facade_iterator_t *result = m_new_obj(facade_iterator_t);
    string_image_facade_t *iterable = (string_image_facade_t *)iterable_in;
    result->base.type = &microbit_facade_iterator_type;
    result->string = iterable->string;
    result->image = iterable->image;
    result->index = 0;
    return result;
}
