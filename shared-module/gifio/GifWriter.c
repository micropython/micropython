/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
 * Copyright (c) 2013-2021 Ibrahim Abdelkader <iabdalkader@openmv.io>
 * Copyright (c) 2013-2021 Kwabena W. Agyeman <kwagyeman@openmv.io>
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

#include "py/gc.h"
#include "py/runtime.h"

#include "shared-module/gifio/GifWriter.h"
#include "shared-bindings/gifio/GifWriter.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/util.h"

#define BLOCK_SIZE (126) // (2^7) - 2 // (DO NOT CHANGE!)

static void handle_error(gifio_gifwriter_t *self) {
    if (self->error != 0) {
        mp_raise_OSError(self->error);
    }
}

static void flush_data(gifio_gifwriter_t *self) {
    if (self->cur == 0) {
        return;
    }
    int error = 0;
    self->file_proto->write(self->file, self->data, self->cur, &error);
    self->cur = 0;
    if (error != 0) {
        self->error = error;
    }
}

// These "write" calls _MUST_ have enough buffer space available!  This is
// ensured by allocating the proper buffer size in construct.
static void write_data(gifio_gifwriter_t *self, const void *data, size_t size) {
    assert(self->cur + size <= self->size);
    memcpy(self->data + self->cur, data, size);
    self->cur += size;
}

static void write_byte(gifio_gifwriter_t *self, uint8_t value) {
    write_data(self, &value, sizeof(value));
}

static void write_long(gifio_gifwriter_t *self, uint32_t value) {
    write_data(self, &value, sizeof(value));
}

static void write_word(gifio_gifwriter_t *self, uint16_t value) {
    write_data(self, &value, sizeof(value));
}

void shared_module_gifio_gifwriter_construct(gifio_gifwriter_t *self, mp_obj_t *file, int width, int height, displayio_colorspace_t colorspace, bool loop, bool dither, bool own_file) {
    self->file = file;
    self->file_proto = mp_get_stream_raise(file, MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    if (self->file_proto->is_text) {
        mp_raise_TypeError(MP_ERROR_TEXT("file must be a file opened in byte mode"));
    }
    self->width = width;
    self->height = height;
    self->colorspace = colorspace;
    self->dither = dither;
    self->own_file = own_file;

    size_t nblocks = (width * height + 125) / 126;
    self->size = nblocks * 128 + 4;
    self->data = m_malloc(self->size);
    self->cur = 0;
    self->error = 0;

    write_data(self, "GIF89a", 6);
    write_word(self, width);
    write_word(self, height);
    write_data(self, (uint8_t []) {0xF6, 0x00, 0x00}, 3);

    switch (colorspace) {
        case DISPLAYIO_COLORSPACE_RGB565:
        case DISPLAYIO_COLORSPACE_RGB565_SWAPPED:
        case DISPLAYIO_COLORSPACE_BGR565:
        case DISPLAYIO_COLORSPACE_BGR565_SWAPPED:
        case DISPLAYIO_COLORSPACE_L8:
            break;

        default:
            mp_raise_TypeError(MP_ERROR_TEXT("unsupported colorspace for GifWriter"));
    }

    bool color = (colorspace != DISPLAYIO_COLORSPACE_L8);

    bool bgr = (colorspace == DISPLAYIO_COLORSPACE_BGR565 || colorspace == DISPLAYIO_COLORSPACE_BGR565_SWAPPED);
    self->byteswap = (colorspace == DISPLAYIO_COLORSPACE_RGB565_SWAPPED || colorspace == DISPLAYIO_COLORSPACE_BGR565_SWAPPED);

    if (color) {
        for (int i = 0; i < 128; i++) {
            int red = (int)(((((i & 0x60) >> 5) * 255) + 1.5) / 3);
            int green = (int)(((((i & 0x1C) >> 2) * 255) + 3.5) / 7);
            int blue = (int)((((i & 0x3) * 255) + 1.5) / 3);
            if (bgr) {
                write_data(self, (uint8_t []) {blue, red, green}, 3);
            } else {
                write_data(self, (uint8_t []) {red, green, blue}, 3);
            }
        }
    } else {
        for (int i = 0; i < 128; i++) {
            int gray = (int)(((i * 255) + 63.5) / 127);
            write_data(self, (uint8_t []) {gray, gray, gray}, 3);
        }
    }

    if (loop) {
        write_data(self, (uint8_t []) {'!', 0xFF, 0x0B}, 3);
        write_data(self, "NETSCAPE2.0", 11);
        write_data(self, (uint8_t []) {0x03, 0x01, 0x00, 0x00, 0x00}, 5);
    }

    flush_data(self);
    handle_error(self);
}

bool shared_module_gifio_gifwriter_deinited(gifio_gifwriter_t *self) {
    return !self->file;
}

void shared_module_gifio_gifwriter_check_for_deinit(gifio_gifwriter_t *self) {
    if (shared_module_gifio_gifwriter_deinited(self)) {
        raise_deinited_error();
    }
}

void shared_module_gifio_gifwriter_deinit(gifio_gifwriter_t *self) {
    if (!shared_module_gifio_gifwriter_deinited(self)) {
        shared_module_gifio_gifwriter_close(self);
    }
}

static const uint8_t rb_bayer[4][4] = {
    { 0, 33,  8, 42},
    {50, 16, 58, 25},
    {12, 46,  4, 37},
    {63, 29, 54, 21}
};

static const uint8_t g_bayer[4][4] = {
    { 0, 16,  4, 20},
    {24,  8, 28, 12},
    { 6, 22,  2, 18},
    {31, 14, 26, 10}
};

void shared_module_gifio_gifwriter_add_frame(gifio_gifwriter_t *self, const mp_buffer_info_t *bufinfo, int16_t delay) {
    if (delay) {
        write_data(self, (uint8_t []) {'!', 0xF9, 0x04, 0x04}, 4);
        write_word(self, delay);
        write_word(self, 0); // end
    }

    write_byte(self, 0x2C);
    write_long(self, 0);
    write_word(self, self->width);
    write_word(self, self->height);
    write_data(self, (uint8_t []) {0x00, 0x07}, 2); // 7-bits

    int pixel_count = self->width * self->height;
    int blocks = (pixel_count + BLOCK_SIZE - 1) / BLOCK_SIZE;

    uint8_t *data = self->data + self->cur;

    if (self->colorspace == DISPLAYIO_COLORSPACE_L8) {
        mp_get_index(&mp_type_memoryview, bufinfo->len, MP_OBJ_NEW_SMALL_INT(pixel_count - 1), false);

        uint8_t *pixels = bufinfo->buf;
        for (int i = 0; i < blocks; i++) {
            assert(pixel_count >= 0);
            int block_size = MIN(BLOCK_SIZE, pixel_count);
            pixel_count -= block_size;
            *data++ = 1 + block_size;
            *data++ = 0x80;
            for (int j = 0; j < block_size; j++) {
                *data++ = (*pixels++) >> 1;
            }
        }
    } else if (!self->dither) {
        mp_get_index(&mp_type_memoryview, bufinfo->len, MP_OBJ_NEW_SMALL_INT(2 * pixel_count - 1), false);

        uint16_t *pixels = bufinfo->buf;
        for (int i = 0; i < blocks; i++) {
            int block_size = MIN(BLOCK_SIZE, pixel_count);
            pixel_count -= block_size;

            *data++ = 1 + block_size;
            *data++ = 0x80;
            for (int j = 0; j < block_size; j++) {
                int pixel = *pixels++;
                if (self->byteswap) {
                    pixel = __builtin_bswap16(pixel);
                }
                int red = (pixel >> (11 + (5 - 2))) & 0x3;
                int green = (pixel >> (5 + (6 - 3))) & 0x7;
                int blue = (pixel >> (0 + (5 - 2))) & 0x3;
                *data++ = (red << 5) | (green << 2) | blue;
            }
        }
    } else {
        mp_get_index(&mp_type_memoryview, bufinfo->len, MP_OBJ_NEW_SMALL_INT(2 * pixel_count - 1), false);

        uint16_t *pixels = bufinfo->buf;
        int x = 0, y = 0;
        for (int i = 0; i < blocks; i++) {
            int block_size = MIN(BLOCK_SIZE, pixel_count);
            pixel_count -= block_size;

            *data++ = 1 + block_size;
            *data++ = 0x80;
            for (int j = 0; j < block_size; j++) {
                int pixel = *pixels++;
                if (self->byteswap) {
                    pixel = __builtin_bswap16(pixel);
                }
                int red = (pixel >> 8) & 0xf8;
                int green = (pixel >> 3) & 0xfc;
                int blue = (pixel << 3) & 0xf8;

                red = MAX(0, red - rb_bayer[x % 4][y % 4]);
                green = MAX(0, green - g_bayer[x % 4][(y + 2) % 4]);
                blue = MAX(0, blue - rb_bayer[(x + 2) % 4][y % 4]);
                x++;
                if (x == self->width) {
                    x = 0;
                    y++;
                }

                *data++ = ((red >> 1) & 0x60) | ((green >> 3) & 0x1c) | (blue >> 6);
            }
        }
    }

    self->cur = data - self->data;

    write_data(self, (uint8_t []) {0x01, 0x81, 0x00}, 3); // end code
    flush_data(self);
    handle_error(self);
}

void shared_module_gifio_gifwriter_close(gifio_gifwriter_t *self) {
    write_byte(self, ';');
    flush_data(self);

    int error = 0;
    self->file_proto->ioctl(self->file, self->own_file ? MP_STREAM_CLOSE : MP_STREAM_FLUSH, 0, &error);
    self->file = NULL;

    if (error != 0) {
        self->error = error;
    }
    handle_error(self);
}
