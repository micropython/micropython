// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar
//
// SPDX-License-Identifier: MIT

// Hardware JPEG decode for jpegio, on chips with SOC_JPEG_DECODE_SUPPORTED.
// The engine decodes a whole baseline JPEG to RGB565 with no scaling, so only
// decode() calls with scale=0 into a 16-bit Bitmap take this path. Anything the
// hardware does not handle returns false and jpegio falls back to TJpgDec.

#include "py/runtime.h"

#include "shared-bindings/bitmaptools/__init__.h"
#include "shared-module/displayio/Bitmap.h"
#include "shared-module/jpegio/JpegDecoder.h"

#include "driver/jpeg_decode.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_memory_utils.h"

// A bad image blocks the engine for at most this long.
#define JPEG_DECODE_TIMEOUT_MS (1000)

void jpegio_jpegdecoder_hw_init(jpegio_jpegdecoder_obj_t *self) {
    jpeg_decode_engine_cfg_t engine_config = {
        .timeout_ms = JPEG_DECODE_TIMEOUT_MS,
    };
    jpeg_decoder_handle_t engine;
    if (jpeg_new_decoder_engine(&engine_config, &engine) != ESP_OK) {
        engine = NULL;
    }
    self->hw_decoder = engine;
}

void jpegio_jpegdecoder_hw_deinit(jpegio_jpegdecoder_obj_t *self) {
    if (self->hw_decoder != NULL) {
        jpeg_del_decoder_engine(self->hw_decoder);
        self->hw_decoder = NULL;
    }
}

// GC blocks start on a cache line, so the engine can write Bitmap pixels directly.
#ifdef CONFIG_CACHE_L2_CACHE_LINE_SIZE
_Static_assert(MICROPY_BYTES_PER_GC_BLOCK % CONFIG_CACHE_L2_CACHE_LINE_SIZE == 0, "GC block is not a cache line");
#endif
#ifdef CONFIG_CACHE_L1_DCACHE_LINE_SIZE
_Static_assert(MICROPY_BYTES_PER_GC_BLOCK % CONFIG_CACHE_L1_DCACHE_LINE_SIZE == 0, "GC block is not a cache line");
#endif

// True if the hardware may write `size` bytes at `buf` directly: the start and
// length must both be whole cache lines.
static bool output_aligned(const void *buf, size_t size) {
    size_t align = esp_cache_get_line_size_by_addr(buf);
    if (align < 4) {
        align = 4;
    }
    return ((uintptr_t)buf & (align - 1)) == 0 && (size & (align - 1)) == 0;
}

// These return false so decode() uses TJpgDec, or raise ValueError if
// software decoding is not allowed.
static bool use_software(jpegio_jpegdecoder_obj_t *self, mp_rom_error_text_t msg) {
    if (!self->software_ok) {
        mp_raise_ValueError(msg);
    }
    return false;
}

static bool use_software_for_arg(jpegio_jpegdecoder_obj_t *self, qstr arg_name) {
    if (!self->software_ok) {
        mp_raise_ValueError_varg(MP_ERROR_TEXT("Invalid %q"), arg_name);
    }
    return false;
}

bool jpegio_jpegdecoder_hw_decode(jpegio_jpegdecoder_obj_t *self, const mp_buffer_info_t *jpeg, int scale) {
    if (self->hw_decoder == NULL) {
        return use_software(self, MP_ERROR_TEXT("Operation or feature not supported"));
    }
    displayio_bitmap_t *bitmap = self->dest;
    if (scale != 0) {
        return use_software_for_arg(self, MP_QSTR_scale);
    }
    if (bitmap->bits_per_value != 16 || bitmap->read_only) {
        return use_software_for_arg(self, MP_QSTR_bitmap);
    }
    // Skip indices are slower on this path than in TJpgDec.
    if (!self->skip_source_index_none) {
        return use_software_for_arg(self, MP_QSTR_skip_source_index);
    }
    if (!self->skip_dest_index_none) {
        return use_software_for_arg(self, MP_QSTR_skip_dest_index);
    }
    // The engine reads the input by DMA, so it must be in DMA-capable memory.
    if (!esp_ptr_dma_capable(jpeg->buf) && !esp_ptr_dma_ext_capable(jpeg->buf)) {
        return use_software_for_arg(self, MP_QSTR_data_source);
    }

    jpeg_decode_picture_info_t info;
    if (jpeg_decoder_get_info(jpeg->buf, jpeg->len, &info) != ESP_OK) {
        return use_software(self, MP_ERROR_TEXT("Format not supported"));
    }

    // The engine pads its output to whole MCUs: 8x8 for 4:4:4, 16x8 for 4:2:2
    // and 16x16 for 4:2:0. Grayscale is left to TJpgDec.
    uint32_t mcu_width, mcu_height;
    switch (info.sample_method) {
        case JPEG_DOWN_SAMPLING_YUV444:
            mcu_width = 8;
            mcu_height = 8;
            break;
        case JPEG_DOWN_SAMPLING_YUV422:
            mcu_width = 16;
            mcu_height = 8;
            break;
        case JPEG_DOWN_SAMPLING_YUV420:
            mcu_width = 16;
            mcu_height = 16;
            break;
        default:
            return use_software(self, MP_ERROR_TEXT("Format not supported"));
    }
    uint32_t width = info.width;
    uint32_t height = info.height;
    // The driver rejects images whose pixel count is not a multiple of 8.
    if ((width * height) % 8 != 0) {
        return use_software(self, MP_ERROR_TEXT("Format not supported"));
    }
    uint32_t output_width = (width + mcu_width - 1) / mcu_width * mcu_width;
    uint32_t output_height = (height + mcu_height - 1) / mcu_height * mcu_height;
    size_t output_size = output_width * output_height * sizeof(uint16_t);

    // Decode straight into the Bitmap when it is exactly the image, unpadded and
    // cache-line aligned, and the whole image lands at 0,0.
    bool direct = width == output_width && height == output_height &&
        bitmap->width == width && bitmap->height == height &&
        self->x == 0 && self->y == 0 &&
        self->lim.x1 <= 0 && self->lim.y1 <= 0 &&
        self->lim.x2 >= (int)width && self->lim.y2 >= (int)height &&
        output_aligned(bitmap->data, output_size);

    // Otherwise decode into a temporary buffer and copy the visible part out of it.
    uint8_t *output;
    size_t output_buffer_size;
    uint8_t *scratch = NULL;
    if (direct) {
        output = (uint8_t *)bitmap->data;
        output_buffer_size = output_size;
    } else {
        jpeg_decode_memory_alloc_cfg_t mem_config = {
            .buffer_direction = JPEG_DEC_ALLOC_OUTPUT_BUFFER,
        };
        scratch = jpeg_alloc_decoder_mem(output_size, &mem_config, &output_buffer_size);
        if (scratch == NULL) {
            return use_software(self, MP_ERROR_TEXT("Operation or feature not supported"));
        }
        output = scratch;
    }

    // RGB order with RGB565 makes the engine emit the high byte first, the same
    // byte-swapped RGB565 that TJpgDec produces here. JFIF files use full range
    // BT601 YCbCr.
    jpeg_decode_cfg_t decode_config = {
        .output_format = JPEG_DECODE_OUT_FORMAT_RGB565,
        .rgb_order = JPEG_DEC_RGB_ELEMENT_ORDER_RGB,
        .conv_std = JPEG_YUV_RGB_CONV_STD_BT601,
        .yuv_range = COLOR_RANGE_FULL,
    };
    uint32_t decoded_size;
    if (jpeg_decoder_process(self->hw_decoder, &decode_config, jpeg->buf, jpeg->len,
        output, output_buffer_size, &decoded_size) != ESP_OK) {
        // A failed direct decode may have written part of the Bitmap. TJpgDec
        // then decodes the whole image over it.
        heap_caps_free(scratch);
        return use_software(self, MP_ERROR_TEXT("Format not supported"));
    }

    if (direct) {
        displayio_area_t area = { 0, 0, width, height, NULL };
        displayio_bitmap_set_dirty_area(bitmap, &area);
        return true;
    }

    // Copy out the visible part with the same clipping TJpgDec's output callback
    // applies, treating the whole image as a single block at 0,0.
    displayio_bitmap_t src = {
        .width = width,
        .height = height,
        .data = (uint32_t *)(void *)output,
        .stride = output_width / 2, // in units of uint32_t
        .bits_per_value = 16,
        .x_shift = 1,
        .x_mask = 1,
        .bitmask = 0xffff,
    };
    int x = self->x;
    int y = self->y;
    int x1 = self->lim.x1;
    int y1 = self->lim.y1;
    int x2 = MIN(self->lim.x2, (int)width);
    int y2 = MIN(self->lim.y2, (int)height);
    if (x1 < 0) {
        x += -x1;
        x1 = 0;
    }
    if (y1 < 0) {
        y += -y1;
        y1 = 0;
    }
    if (x2 > 0 && y2 > 0) {
        common_hal_bitmaptools_blit(bitmap, &src, x, y, x1, y1, x2, y2,
            self->skip_source_index, self->skip_source_index_none,
            self->skip_dest_index, self->skip_dest_index_none);
    }
    heap_caps_free(scratch);
    return true;
}
