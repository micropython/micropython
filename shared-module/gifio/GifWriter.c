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

#include "py/runtime.h"

#include "shared-module/gifio/GifWriter.h"
#include "shared-bindings/gifio/GifWriter.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/util.h"

#define BLOCK_SIZE (126) // (2^7) - 2 // (DO NOT CHANGE!)

static void handle_error(const char *what, int error) {
    if (error != 0) {
        mp_raise_OSError(error);
    }
}

static void write_data(gifio_gifwriter_t *self, const void *data, size_t size) {
    int error = 0;
    self->file_proto->write(self->file, data, size, &error);
    handle_error("write_data", error);
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

void shared_module_gifio_gifwriter_construct(gifio_gifwriter_t *self, mp_obj_t *file, int width, int height, displayio_colorspace_t colorspace, bool loop, bool own_file) {
    self->file = file;
    self->file_proto = mp_proto_get_or_throw(MP_QSTR_protocol_stream, file);
    if (self->file_proto->is_text) {
        mp_raise_TypeError(translate("file must be a file opened in byte mode"));
    }
    self->width = width;
    self->height = height;
    self->colorspace = colorspace;
    self->own_file = own_file;

    write_data(self, "GIF89a", 6);
    write_word(self, width);
    write_word(self, height);
    write_data(self, (uint8_t []) {0xF6, 0x00, 0x00}, 3);

    if (colorspace == DISPLAYIO_COLORSPACE_RGB888) {
        mp_raise_TypeError(translate("unsupported colorspace for GifWriter"));
    }

    bool color = (colorspace != DISPLAYIO_COLORSPACE_L8);

    if (color) {
        for (int i = 0; i < 128; i++) {
            int red = (int)(((((i & 0x60) >> 5) * 255) + 1.5) / 3);
            int green = (int)(((((i & 0x1C) >> 2) * 255) + 3.5) / 7);
            int blue = (int)((((i & 0x3) * 255) + 1.5) / 3);
            write_data(self, (uint8_t []) {red, green, blue}, 3);
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

    uint8_t block_data[2 + BLOCK_SIZE];
    block_data[1] = 0x80;

    if (self->colorspace == DISPLAYIO_COLORSPACE_L8) {
        mp_get_index(&mp_type_memoryview, bufinfo->len, MP_OBJ_NEW_SMALL_INT(pixel_count - 1), false);

        uint8_t *pixels = bufinfo->buf;
        for (int i = 0; i < blocks; i++) {
            assert(pixel_count >= 0);
            int block_size = MIN(BLOCK_SIZE, pixel_count);
            pixel_count -= block_size;
            block_data[0] = 1 + block_size;
            for (int j = 0; j < block_size; j++) {
                block_data[j + 2] = (*pixels++) >> 1;
            }
            write_data(self, block_data, 2 + block_size);
        }
    } else {
        mp_get_index(&mp_type_memoryview, bufinfo->len, MP_OBJ_NEW_SMALL_INT(2 * pixel_count - 1), false);

        uint16_t *pixels = bufinfo->buf;
        for (int i = 0; i < blocks; i++) {
            int block_size = MIN(BLOCK_SIZE, pixel_count);
            pixel_count -= block_size;

            block_data[0] = 1 + block_size;
            for (int j = 0; j < block_size; j++) {
                int pixel = displayio_colorconverter_convert_pixel(self->colorspace, (*pixels++));
                int red = (pixel >> (16 + 6)) & 0x3;
                int green = (pixel >> (8 + 5)) & 0x7;
                int blue = (pixel >> 6) & 0x3;
                block_data[j + 2] = (red << 5) | (green << 2) | blue;
            }
            write_data(self, block_data, 2 + block_size);
        }
    }

    write_data(self, (uint8_t []) {0x01, 0x81, 0x00}, 3); // end code

    int error = 0;
    self->file_proto->ioctl(self->file, MP_STREAM_FLUSH, 0, &error);
    handle_error("flush", error);
}

void shared_module_gifio_gifwriter_close(gifio_gifwriter_t *self) {
    // we want to ensure the stream is closed even if the first write failed, so we don't use write_data
    int error1 = 0;
    self->file_proto->write(self->file, ";", 1, &error1);

    int error2 = 0;
    if (self->own_file) {
        self->file_proto->ioctl(self->file, MP_STREAM_CLOSE, 0, &error2);
    } else {
        self->file_proto->ioctl(self->file, MP_STREAM_FLUSH, 0, &error2);
    }
    self->file = NULL;

    handle_error("write", error1);
    handle_error("close", error2);
}
