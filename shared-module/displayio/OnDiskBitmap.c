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

#include "shared-bindings/displayio/OnDiskBitmap.h"

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

static uint32_t read_word(uint16_t* bmp_header, uint16_t index) {
    return bmp_header[index] | bmp_header[index + 1] << 16;
}

void common_hal_displayio_ondiskbitmap_construct(displayio_ondiskbitmap_t *self, pyb_file_obj_t* file) {
    // Load the wave
    self->file = file;
    uint16_t bmp_header[24];
    f_rewind(&self->file->fp);
    UINT bytes_read;
    if (f_read(&self->file->fp, bmp_header, 48, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 48 ||
        memcmp(bmp_header, "BM", 2) != 0) {
        mp_raise_ValueError(translate("Invalid BMP file"));
    }

    // We can't cast because we're not aligned.
    self->data_offset = read_word(bmp_header, 5);

    uint32_t header_size = read_word(bmp_header, 7);
    uint32_t compression = read_word(bmp_header, 15);
    if (!(header_size == 12 || header_size == 40 || header_size == 108 || header_size == 124) ||
        !(compression == 0)) {
        mp_raise_ValueError_varg(translate("Only Windows format, uncompressed BMP supported %d"), header_size);
    }
    // TODO(tannewt): Support bitfield compressed colors since RGB565 can be produced by the GIMP.
    uint16_t bits_per_pixel = bmp_header[14];
    if (bits_per_pixel < 24) {
        mp_raise_ValueError_varg(translate("Only true color (24 bpp or higher) BMP supported %x"), bits_per_pixel);
    }
    self->bytes_per_pixel = bits_per_pixel / 8;
    self->width = read_word(bmp_header, 9);
    self->height = read_word(bmp_header, 11);
    uint32_t byte_width = self->width * self->bytes_per_pixel;
    self->stride = byte_width;
    // Rows are word aligned.
    if (self->stride % 4 != 0) {
        self->stride += 4 - self->stride % 4;
    }
}


uint32_t common_hal_displayio_ondiskbitmap_get_pixel(displayio_ondiskbitmap_t *self,
        int16_t x, int16_t y) {
    if (x < 0 || x >= self->width || y < 0 || y >= self->height) {
        return 0;
    }
    uint32_t location = self->data_offset + (self->height - y) * self->stride + x * self->bytes_per_pixel;
    // We don't cache here because the underlying FS caches sectors.
    f_lseek(&self->file->fp, location);
    UINT bytes_read;
    uint32_t pixel = 0;
    uint32_t result = f_read(&self->file->fp, &pixel, self->bytes_per_pixel, &bytes_read);
    if (result == FR_OK) {
        return pixel;
    }
    return 0;
}
