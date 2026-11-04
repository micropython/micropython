// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2025 SamantazFox
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/displayio/OnDiskBitmap.h"
#include "shared-bindings/displayio/ColorConverter.h"
#include "shared-bindings/displayio/Palette.h"
#include "shared-module/displayio/ColorConverter.h"
#include "shared-module/displayio/Palette.h"

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"
#if CIRCUITPY_STORAGE_MAP_FILE
#include "shared-bindings/storage/__init__.h"
#endif


#define DISPLAYIO_ODBMP_DEBUG(...) (void)0
// #define DISPLAYIO_ODBMP_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)


#if CIRCUITPY_STORAGE_MAP_FILE
static void add_run(void *self_in, const uint8_t *addr, size_t len) {
    displayio_ondiskbitmap_t *self = self_in;
    if (self->map_count < MP_ARRAY_SIZE(self->map)) {
        self->map[self->map_count].addr = addr;
        self->map[self->map_count].len = len;
        self->map_count++;
    } else {
        self->map_count = MP_ARRAY_SIZE(self->map) + 1;  // too fragmented: read through the cache
    }
}

// Flash address of len bytes at offset if they lie in one run, else NULL.
static const uint8_t *map_at(displayio_ondiskbitmap_t *self, uint32_t offset, uint32_t len) {
    for (uint8_t i = 0; i < self->map_count; i++) {
        if (offset < self->map[i].len) {
            return offset + len <= self->map[i].len ? self->map[i].addr + offset : NULL;
        }
        offset -= self->map[i].len;
    }
    return NULL;
}

const uint8_t *displayio_ondiskbitmap_row(displayio_ondiskbitmap_t *self, int16_t y) {
    return map_at(self, self->data_offset + (self->height - y - 1) * self->stride, self->width);
}
#endif

static uint32_t read_word(uint16_t *bmp_header, uint16_t index) {
    return bmp_header[index] | bmp_header[index + 1] << 16;
}

void common_hal_displayio_ondiskbitmap_construct(displayio_ondiskbitmap_t *self, mp_obj_t file) {
    self->file = file;
    self->cache_len = 0;
    uint16_t bmp_header[69];
    int errcode = 0;

    mp_stream_seek(file, 0, MP_SEEK_SET, &errcode);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }

    // Read the minimum amount of bytes required to parse a BITMAPCOREHEADER.
    // If needed, we will read more bytes down below.
    mp_uint_t bytes_read = mp_stream_rw(file, bmp_header, 26, &errcode, MP_STREAM_RW_ONCE);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    if (bytes_read != 26 || memcmp(bmp_header, "BM", 2) != 0) {
        mp_arg_error_invalid(MP_QSTR_file);
    }

    // Read header size to determine if more header bytes needs to be read.
    uint32_t header_size = read_word(bmp_header, 7);
    DISPLAYIO_ODBMP_DEBUG("header_size: %d\n", header_size);

    if (header_size == 40 || header_size == 108 || header_size == 124) {
        // Read the remaining header bytes
        bytes_read = mp_stream_rw(file, bmp_header + 13, header_size - 12, &errcode, MP_STREAM_RW_ONCE);
        if (errcode != 0) {
            mp_raise_OSError(errcode);
        }
        DISPLAYIO_ODBMP_DEBUG("bytes_read: %d\n", bytes_read);
        if (bytes_read != (header_size - 12)) {
            mp_arg_error_invalid(MP_QSTR_file);
        }
    } else if (header_size != 12) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Only Windows format, uncompressed BMP supported: given header size is %d"), header_size);
    }


    uint32_t compression = read_word(bmp_header, 15);
    DISPLAYIO_ODBMP_DEBUG("compression: %d\n", compression);

    // 0 is uncompressed; 3 is bitfield compressed. 1 and 2 are RLE compression.
    if (compression != 0 && compression != 3) {
        mp_raise_ValueError(MP_ERROR_TEXT("RLE-compressed BMP not supported"));
    }

    // We can't cast because we're not aligned.
    self->data_offset = read_word(bmp_header, 5);

    self->bitfield_compressed = (compression == 3);
    self->bits_per_pixel = bmp_header[14];
    self->width = read_word(bmp_header, 9);
    self->height = read_word(bmp_header, 11);

    DISPLAYIO_ODBMP_DEBUG("data offset: %d\n", self->data_offset);
    DISPLAYIO_ODBMP_DEBUG("width: %d\n", self->width);
    DISPLAYIO_ODBMP_DEBUG("height: %d\n", self->height);
    DISPLAYIO_ODBMP_DEBUG("bpp: %d\n", self->bits_per_pixel);


    displayio_colorconverter_t *colorconverter =
        mp_obj_malloc(displayio_colorconverter_t, &displayio_colorconverter_type);
    common_hal_displayio_colorconverter_construct(colorconverter, false, DISPLAYIO_COLORSPACE_RGB888);
    self->colorconverter = colorconverter;

    if (self->bits_per_pixel == 16) {
        if (((header_size >= 56)) || (self->bitfield_compressed)) {
            self->r_bitmask = read_word(bmp_header, 27);
            self->g_bitmask = read_word(bmp_header, 29);
            self->b_bitmask = read_word(bmp_header, 31);

        } else { // no compression or short header means 5:5:5
            self->r_bitmask = 0x7c00;
            self->g_bitmask = 0x3e0;
            self->b_bitmask = 0x1f;
        }
    } else if (self->bits_per_pixel <= 8) { // indexed
        uint32_t number_of_colors = 0;
        if (header_size >= 40) {
            number_of_colors = read_word(bmp_header, 23);
        }
        if (number_of_colors == 0) {
            number_of_colors = 1 << self->bits_per_pixel;
        }

        displayio_palette_t *palette = mp_obj_malloc(displayio_palette_t, &displayio_palette_type);
        common_hal_displayio_palette_construct(palette, number_of_colors, false);

        if (number_of_colors > 1) {
            uint16_t palette_size = number_of_colors * sizeof(uint32_t);
            uint16_t palette_offset = 0xe + header_size;

            uint32_t *palette_data = m_malloc_without_collect(palette_size);

            mp_stream_seek(file, palette_offset, MP_SEEK_SET, &errcode);
            if (errcode != 0) {
                mp_raise_OSError(errcode);
            }

            mp_uint_t palette_bytes_read = mp_stream_rw(file, palette_data, palette_size, &errcode, MP_STREAM_RW_ONCE);
            if (errcode != 0) {
                mp_raise_OSError(errcode);
            }
            if (palette_bytes_read != palette_size) {
                mp_raise_ValueError(MP_ERROR_TEXT("Unable to read color palette data"));
            }
            for (uint16_t i = 0; i < number_of_colors; i++) {
                common_hal_displayio_palette_set_color(palette, i, palette_data[i]);
            }

            #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
            m_free(palette_data, palette_size);
            #else
            m_free(palette_data);
            #endif
        } else {
            common_hal_displayio_palette_set_color(palette, 0, 0x0);
            common_hal_displayio_palette_set_color(palette, 1, 0xffffff);
        }
        self->palette = palette;
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

    #if CIRCUITPY_STORAGE_MAP_FILE
    self->map_count = 0;
    if (storage_map_runs(file, add_run, self) != 0 || self->map_count > MP_ARRAY_SIZE(self->map)) {
        self->map_count = 0;
    }
    // BMP rows don't start on a 4-byte boundary, so in a split file some row crosses a run.
    self->rows_mapped = self->bits_per_pixel == 8 &&
        map_at(self, self->data_offset, (uint32_t)(self->height - 1) * self->stride + self->width) != NULL;
    #endif
}


void common_hal_displayio_ondiskbitmap_deinit(displayio_ondiskbitmap_t *self) {
    if (common_hal_displayio_ondiskbitmap_deinited(self)) {
        return;
    }
    mp_stream_close(self->file);
    self->file = NULL;
    #if CIRCUITPY_STORAGE_MAP_FILE
    // A TileGrid may still draw this bitmap, so it must stop reading rows from mapped flash.
    self->rows_mapped = false;
    #endif
}

bool common_hal_displayio_ondiskbitmap_deinited(displayio_ondiskbitmap_t *self) {
    return self->file == NULL;
}

uint32_t common_hal_displayio_ondiskbitmap_get_pixel(displayio_ondiskbitmap_t *self,
    int16_t x, int16_t y) {
    if (common_hal_displayio_ondiskbitmap_deinited(self) ||
        x < 0 || x >= self->width || y < 0 || y >= self->height) {
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
    uint32_t pixel_data = 0;
    #if CIRCUITPY_STORAGE_MAP_FILE
    if (self->map_count > 0) {
        // Byte by byte, so a pixel split across two runs reads right too. The cache can't
        // help there: it shares memory with the run table.
        for (uint8_t i = 0; i < bytes_per_pixel; i++) {
            const uint8_t *p = map_at(self, location + i, 1);
            if (p == NULL) {
                return 0;
            }
            ((uint8_t *)&pixel_data)[i] = *p;
        }
    } else
    #endif
    {
        int errcode = 0;
        if (location < self->cache_start || location + bytes_per_pixel > self->cache_start + self->cache_len) {
            mp_stream_seek(self->file, location, MP_SEEK_SET, &errcode);
            if (errcode != 0) {
                return 0;
            }
            self->cache_start = location;
            self->cache_len = mp_stream_rw(self->file, self->cache, sizeof(self->cache), &errcode, MP_STREAM_RW_ONCE);
            if (errcode != 0 || self->cache_len < bytes_per_pixel) {
                self->cache_len = 0;
                return 0;
            }
        }
        memcpy(&pixel_data, self->cache + (location - self->cache_start), bytes_per_pixel);
    }
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

uint16_t common_hal_displayio_ondiskbitmap_get_height(displayio_ondiskbitmap_t *self) {
    return self->height;
}

uint16_t common_hal_displayio_ondiskbitmap_get_width(displayio_ondiskbitmap_t *self) {
    return self->width;
}

mp_obj_t common_hal_displayio_ondiskbitmap_get_pixel_shader(displayio_ondiskbitmap_t *self) {
    return MP_OBJ_FROM_PTR(self->pixel_shader_base);
}
