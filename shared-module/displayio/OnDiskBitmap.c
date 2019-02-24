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
    uint16_t bmp_header[69];
    f_rewind(&self->file->fp);
    UINT bytes_read;
    if (f_read(&self->file->fp, bmp_header, 138, &bytes_read) != FR_OK) {
        mp_raise_OSError(MP_EIO);
    }
    if (bytes_read != 138 ||
        memcmp(bmp_header, "BM", 2) != 0) {
        mp_raise_ValueError(translate("Invalid BMP file"));
    }

    // We can't cast because we're not aligned.
    self->data_offset = read_word(bmp_header, 5);

    uint32_t header_size = read_word(bmp_header, 7);
    uint16_t bits_per_pixel = bmp_header[14];
    uint32_t compression = read_word(bmp_header, 15);
    uint32_t number_of_colors = read_word(bmp_header, 23);
    self->bitfield_compressed  = (compression == 3);
    
    self->grayscale = ((bits_per_pixel == 8) && (number_of_colors == 256));
    if (bits_per_pixel == 16){
        if (((header_size == 124) || (header_size == 56)) && (self->bitfield_compressed)) {
            self->r_bitmask = read_word(bmp_header, 27);
            self->g_bitmask = read_word(bmp_header, 29);
            self->b_bitmask = read_word(bmp_header, 31);
            
            if (!((self->r_bitmask == 0xf800) && (self->g_bitmask == 0x07e0) && (self->b_bitmask == 0x001f))){
                mp_raise_ValueError_varg(translate("Only 16bpp RGB 565 supported for bitfield compressed BMPs;  R:%x G:%x B:%x"), 
                self->r_bitmask, self->g_bitmask, self->b_bitmask);
            }
        } else if (header_size == 40){ // no bitmasks means 5:5:5
            self->r_bitmask = 0x7c00;
            self->g_bitmask = 0x3e0;
            self->b_bitmask = 0x1f;
        }

    } else if (!(header_size == 12 || header_size == 40 || header_size == 108 || header_size == 124) ||
        !(compression == 0)) {
        mp_raise_ValueError_varg(translate("Only Windows format, uncompressed BMP supported %d"), header_size);
    }

    if (bits_per_pixel < 16 && !(self->grayscale)) {
        mp_raise_ValueError_varg(translate("Only true color (24 bpp or higher), 16bpp 565 and 555, and 8bpp grayscale BMP supported %d"), bits_per_pixel);
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
        uint32_t tmp = 0;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        if (self->grayscale){
            red = pixel;
            green = pixel;
            blue = pixel;
            tmp = (red << 16 | green << 8 | blue);
            return tmp;
        } else if (self->bytes_per_pixel == 2) {
            if (self->bitfield_compressed){
                red =((pixel & self->r_bitmask) >>11);
                green = ((pixel & self->g_bitmask) >>5);
                blue = ((pixel & self->b_bitmask) >> 0);
            } else {
                red =((pixel & self->r_bitmask) >>10);
                green = ((pixel & self->g_bitmask) >>4);
                blue = ((pixel & self->b_bitmask) >> 0);
            }
            tmp = (red << 19 | green << 10 | blue << 3);
            return tmp;
        }else {
            return pixel;
        }

    }
    return 0;
}

uint16_t common_hal_displayio_ondiskbitmap_get_height(displayio_ondiskbitmap_t *self) {
    return self->height;
}

uint16_t common_hal_displayio_ondiskbitmap_get_width(displayio_ondiskbitmap_t *self) {
    return self->width;
}
