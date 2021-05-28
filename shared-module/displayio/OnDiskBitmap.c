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
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-module/displayio/ColorConverter.h"
#include "shared-module/displayio/Palette.h"

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"

static uint32_t read_word(uint16_t *bmp_header, uint16_t index) {
    return bmp_header[index] | bmp_header[index + 1] << 16;
}

void common_hal_displayio_ondiskbitmap_construct(displayio_ondiskbitmap_t *self, pyb_file_obj_t *file) {
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

    bool indexed = bits_per_pixel <= 8;
    self->bitfield_compressed = (compression == 3);
    self->bits_per_pixel = bits_per_pixel;
    self->width = read_word(bmp_header, 9);
    self->height = read_word(bmp_header, 11);

    displayio_colorconverter_t *colorconverter = m_new_obj(displayio_colorconverter_t);
    colorconverter->base.type = &displayio_colorconverter_type;
    common_hal_displayio_colorconverter_construct(colorconverter, false, DISPLAYIO_COLORSPACE_RGB888);
    self->colorconverter = colorconverter;

    if (bits_per_pixel == 16) {
        if (((header_size >= 56)) || (self->bitfield_compressed)) {
            self->r_bitmask = read_word(bmp_header, 27);
            self->g_bitmask = read_word(bmp_header, 29);
            self->b_bitmask = read_word(bmp_header, 31);

        } else { // no compression or short header means 5:5:5
            self->r_bitmask = 0x7c00;
            self->g_bitmask = 0x3e0;
            self->b_bitmask = 0x1f;
        }
    } else if (indexed) {
        if (number_of_colors == 0) {
            number_of_colors = 1 << bits_per_pixel;
        }

        displayio_palette_t *palette = m_new_obj(displayio_palette_t);
        palette->base.type = &displayio_palette_type;
        common_hal_displayio_palette_construct(palette, number_of_colors);

        if (number_of_colors > 1) {
            uint16_t palette_size = number_of_colors * sizeof(uint32_t);
            uint16_t palette_offset = 0xe + header_size;

            uint32_t *palette_data = m_malloc(palette_size, false);

            f_rewind(&self->file->fp);
            f_lseek(&self->file->fp, palette_offset);

            UINT palette_bytes_read;
            if (f_read(&self->file->fp, palette_data, palette_size, &palette_bytes_read) != FR_OK) {
                mp_raise_OSError(MP_EIO);
            }
            if (palette_bytes_read != palette_size) {
                mp_raise_ValueError(translate("Unable to read color palette data"));
            }
            for (uint16_t i = 0; i < palette_size; i++) {
                common_hal_displayio_palette_set_color(palette, i, palette_data[i]);
            }
            m_free(palette_data);
        } else {
            common_hal_displayio_palette_set_color(palette, 0, 0x0);
            common_hal_displayio_palette_set_color(palette, 1, 0xffffff);
        }
        self->palette = palette;

    } else if (!(header_size == 12 || header_size == 40 || header_size == 108 || header_size == 124)) {
        mp_raise_ValueError_varg(translate("Only Windows format, uncompressed BMP supported: given header size is %d"), header_size);
    }

    if (bits_per_pixel == 8 && number_of_colors == 0) {
        mp_raise_ValueError_varg(translate("Only monochrome, indexed 4bpp or 8bpp, and 16bpp or greater BMPs supported: %d bpp given"), bits_per_pixel);
    }

    uint8_t bytes_per_pixel = (self->bits_per_pixel / 8)  ? (self->bits_per_pixel / 8) : 1;
    uint8_t pixels_per_byte = 8 / self->bits_per_pixel;
    if (pixels_per_byte == 0) {
        self->stride = (self->width * bytes_per_pixel);
        // Rows are word aligned.
        if (self->stride % 4 != 0) {
            self->stride += 4 - self->stride % 4;
        }
    } else {
        uint32_t bit_stride = self->width * self->bits_per_pixel;
        if (bit_stride % 32 != 0) {
            bit_stride += 32 - bit_stride % 32;
        }
        self->stride = (bit_stride / 8);
    }

}


uint32_t common_hal_displayio_ondiskbitmap_get_pixel(displayio_ondiskbitmap_t *self,
    int16_t x, int16_t y) {
    if (x < 0 || x >= self->width || y < 0 || y >= self->height) {
        return 0;
    }

    uint32_t location;
    uint8_t bytes_per_pixel = (self->bits_per_pixel / 8)  ? (self->bits_per_pixel / 8) : 1;
    uint8_t pixels_per_byte = 8 / self->bits_per_pixel;
    if (pixels_per_byte == 0) {
        location = self->data_offset + (self->height - y - 1) * self->stride + x * bytes_per_pixel;
    } else {
        location = self->data_offset + (self->height - y - 1) * self->stride + x / pixels_per_byte;
    }
    // We don't cache here because the underlying FS caches sectors.
    f_lseek(&self->file->fp, location);
    UINT bytes_read;
    uint32_t pixel_data = 0;
    uint32_t result = f_read(&self->file->fp, &pixel_data, bytes_per_pixel, &bytes_read);
    if (result == FR_OK) {
        uint32_t tmp = 0;
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        if (bytes_per_pixel == 1) {
            uint8_t offset = (x % pixels_per_byte) * self->bits_per_pixel;
            uint8_t mask = (1 << self->bits_per_pixel) - 1;

            return (pixel_data >> ((8 - self->bits_per_pixel) - offset)) & mask;
        } else if (bytes_per_pixel == 2) {
            if (self->g_bitmask == 0x07e0) { // 565
                red = ((pixel_data & self->r_bitmask) >> 11);
                green = ((pixel_data & self->g_bitmask) >> 5);
                blue = ((pixel_data & self->b_bitmask) >> 0);
            } else { // 555
                red = ((pixel_data & self->r_bitmask) >> 10);
                green = ((pixel_data & self->g_bitmask) >> 4);
                blue = ((pixel_data & self->b_bitmask) >> 0);
            }
            tmp = (red << 19 | green << 10 | blue << 3);
            return tmp;
        } else if ((bytes_per_pixel == 4) && (self->bitfield_compressed)) {
            return pixel_data & 0x00FFFFFF;
        } else {
            return pixel_data;
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

mp_obj_t common_hal_displayio_ondiskbitmap_get_pixel_shader(displayio_ondiskbitmap_t *self) {
    return MP_OBJ_FROM_PTR(self->pixel_shader_base);
}
