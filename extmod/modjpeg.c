/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

// The jpeg module, which decodes JPEG images straight into a framebuf-style buffer.
// The port provides the decoder (see modjpeg.h), which may be a hardware codec.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"

#if MICROPY_PY_JPEG

#include "extmod/modjpeg.h"

// Standard Huffman tables (ITU T.81 Annex K.3) as a DHT segment.  Motion JPEG from
// webcams often leaves these out, expecting the decoder to use them.
static const uint8_t jpeg_std_dht[] = {
    0xff, 0xc4, 0x01, 0xa2,
    0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x01, 0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
    0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01,
    0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61,
    0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1,
    0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88,
    0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
    0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4,
    0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa,
    0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02,
    0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61,
    0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52,
    0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a,
    0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86,
    0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4,
    0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
    0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9,
    0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
    0xf8, 0xf9, 0xfa,
};

typedef struct {
    uint16_t width;
    uint16_t height;
    bool has_dht;
} jpeg_header_t;

// Parse the JPEG markers up to the start of scan.
static void jpeg_parse_header(const uint8_t *data, size_t len, jpeg_header_t *hdr) {
    if (len < 4 || data[0] != 0xff || data[1] != 0xd8) {
        mp_raise_ValueError(MP_ERROR_TEXT("not a JPEG"));
    }
    bool have_sof = false;
    hdr->has_dht = false;
    size_t i = 2;
    while (i + 4 <= len) {
        if (data[i] != 0xff) {
            break;
        }
        uint8_t marker = data[i + 1];
        if (marker == 0xff) {
            // Fill byte.
            ++i;
            continue;
        }
        if (marker == 0xda) {
            // Start of scan.
            if (!have_sof) {
                break;
            }
            return;
        }
        size_t seg_len = data[i + 2] << 8 | data[i + 3];
        if (seg_len < 2 || i + 2 + seg_len > len) {
            break;
        }
        if (marker == 0xc4) {
            hdr->has_dht = true;
        } else if (marker == 0xc0 || marker == 0xc1) {
            // Baseline or extended sequential DCT, which is what the codec supports.
            if (seg_len < 7) {
                break;
            }
            hdr->height = data[i + 5] << 8 | data[i + 6];
            hdr->width = data[i + 7] << 8 | data[i + 8];
            have_sof = true;
        } else if (marker >= 0xc2 && marker <= 0xcf && marker != 0xc8 && marker != 0xcc) {
            mp_raise_ValueError(MP_ERROR_TEXT("unsupported JPEG type"));
        }
        i += 2 + seg_len;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("invalid JPEG"));
}

static inline uint8_t clamp8(int v) {
    return v < 0 ? 0 : v > 255 ? 255 : v;
}

// Convert one row of luma samples to RGB565, given the chroma terms for each column of
// chroma samples.  Output pixel i comes from luma column i * scale.
// This and mp_jpeg_output_mcu() process every pixel, so ports may run them from RAM.
static MP_NOINLINE void jpeg_row_rgb565(uint16_t *dest, int i_start, int i_end, int scale, const uint8_t *y_row,
    const int16_t *r_term, const int16_t *g_term, const int16_t *b_term, int chroma_shift, bool swap) {
    for (int i = i_start; i < i_end; ++i) {
        const int px = i * scale;
        const int y = y_row[(px >> 3) * 64 + (px & 7)];
        const int cx = px >> chroma_shift;
        const uint32_t r = clamp8(y + r_term[cx]);
        const uint32_t g = clamp8(y - g_term[cx]);
        const uint32_t b = clamp8(y + b_term[cx]);
        uint16_t c = (r & 0xf8) << 8 | (g & 0xfc) << 3 | b >> 3;
        dest[i] = swap ? __builtin_bswap16(c) : c;
    }
}

// Like jpeg_row_rgb565(), but to 3 bytes per pixel in R, G, B order (or B, G, R if swap).
static MP_NOINLINE void jpeg_row_rgb888(uint8_t *dest, int i_start, int i_end, int scale, const uint8_t *y_row,
    const int16_t *r_term, const int16_t *g_term, const int16_t *b_term, int chroma_shift, bool swap) {
    const int r_pos = swap ? 2 : 0;
    const int b_pos = 2 - r_pos;
    for (int i = i_start; i < i_end; ++i) {
        const int px = i * scale;
        const int y = y_row[(px >> 3) * 64 + (px & 7)];
        const int cx = px >> chroma_shift;
        uint8_t *p = dest + i * 3;
        p[r_pos] = clamp8(y + r_term[cx]);
        p[1] = clamp8(y - g_term[cx]);
        p[b_pos] = clamp8(y + b_term[cx]);
    }
}

MP_NOINLINE void mp_jpeg_output_mcu(mp_jpeg_decode_t *d, const uint8_t *mcu) {
    const uint8_t *cb_block = mcu + (d->num_blocks - 2) * 64;
    const uint8_t *cr_block = mcu + (d->num_blocks - 1) * 64;
    const int s = d->scale;

    // Destination of the MCU's first pixel.  The MCU size is a multiple of the scale.
    const int dx0 = d->mcu_x / s + d->dest_x;
    const int dy0 = d->mcu_y / s + d->dest_y;

    // Range of output pixels from this MCU, clipped to the image and the destination.
    const int i_start = MAX(0, -dx0);
    const int i_end = MIN((MIN(d->mcu_w, d->width - d->mcu_x) + s - 1) / s, d->dest_w - dx0);
    const int j_start = MAX(0, -dy0);
    const int j_end = MIN((MIN(d->mcu_h, d->height - d->mcu_y) + s - 1) / s, d->dest_h - dy0);

    // Chroma contributions to R, G and B for each column of a row of chroma samples.
    int16_t r_term[8] = {0}, g_term[8] = {0}, b_term[8] = {0};
    int chroma_row = -1;

    for (int j = j_start; j < j_end; ++j) {
        const int py = j * s;
        const uint8_t *y_row = mcu + (py >> 3) * d->y_blocks_h * 64 + (py & 7) * 8;
        const size_t offset = (dy0 + j) * d->dest_w + dx0;

        if (d->format == MP_JPEG_FORMAT_GS8) {
            uint8_t *dest = d->dest + offset;
            for (int i = i_start; i < i_end; ++i) {
                const int px = i * s;
                dest[i] = y_row[(px >> 3) * 64 + (px & 7)];
            }
            continue;
        }

        if (!d->gray && (py >> d->chroma_shift_y) != chroma_row) {
            chroma_row = py >> d->chroma_shift_y;
            const uint8_t *cb_row = cb_block + chroma_row * 8;
            const uint8_t *cr_row = cr_block + chroma_row * 8;
            for (int cx = 0; cx < 8; ++cx) {
                const int cb = cb_row[cx] - 128;
                const int cr = cr_row[cx] - 128;
                r_term[cx] = (91881 * cr) >> 16;
                g_term[cx] = (22554 * cb + 46802 * cr) >> 16;
                b_term[cx] = (116130 * cb) >> 16;
            }
        }

        if (d->format == MP_JPEG_FORMAT_RGB888) {
            jpeg_row_rgb888(d->dest + offset * 3, i_start, i_end, s, y_row,
                r_term, g_term, b_term, d->chroma_shift_x, d->swap);
        } else {
            jpeg_row_rgb565((uint16_t *)d->dest + offset, i_start, i_end, s, y_row,
                r_term, g_term, b_term, d->chroma_shift_x, d->swap);
        }
    }

    // Advance to the next MCU, in raster order.
    d->mcu_x += d->mcu_w;
    if (d->mcu_x >= d->width) {
        d->mcu_x = 0;
        d->mcu_y += d->mcu_h;
    }
}

// jpeg.info(data) -> (width, height)
static mp_obj_t jpeg_info(mp_obj_t data_in) {
    mp_buffer_info_t data;
    mp_get_buffer_raise(data_in, &data, MP_BUFFER_READ);
    jpeg_header_t hdr;
    jpeg_parse_header(data.buf, data.len, &hdr);
    mp_obj_t items[2] = {
        MP_OBJ_NEW_SMALL_INT(hdr.width),
        MP_OBJ_NEW_SMALL_INT(hdr.height),
    };
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(jpeg_info_obj, jpeg_info);

// jpeg.decode(data, buf, width, height, format=RGB565, *, x=0, y=0, scale=1, swap=False)
// -> (width, height) of the decoded (and scaled) image
static mp_obj_t jpeg_decode_(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_data, ARG_buf, ARG_width, ARG_height, ARG_format, ARG_x, ARG_y, ARG_scale, ARG_swap };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buf, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_width, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_height, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_format, MP_ARG_INT, {.u_int = MP_JPEG_FORMAT_RGB565} },
        { MP_QSTR_x, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_y, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_scale, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_swap, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t data;
    mp_get_buffer_raise(args[ARG_data].u_obj, &data, MP_BUFFER_READ);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(args[ARG_buf].u_obj, &buf, MP_BUFFER_WRITE);

    mp_jpeg_decode_t d = {
        .dest = buf.buf,
        .dest_w = args[ARG_width].u_int,
        .dest_h = args[ARG_height].u_int,
        .dest_x = args[ARG_x].u_int,
        .dest_y = args[ARG_y].u_int,
        .scale = args[ARG_scale].u_int,
        .format = args[ARG_format].u_int,
        .swap = args[ARG_swap].u_bool,
    };

    size_t bpp;
    if (d.format == MP_JPEG_FORMAT_RGB565) {
        bpp = 2;
    } else if (d.format == MP_JPEG_FORMAT_GS8) {
        bpp = 1;
    } else if (d.format == MP_JPEG_FORMAT_RGB888) {
        bpp = 3;
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("format"));
    }
    if (d.dest_w <= 0 || d.dest_h <= 0 || (size_t)d.dest_w * d.dest_h * bpp > buf.len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer too small"));
    }
    if (d.scale != 1 && d.scale != 2 && d.scale != 4 && d.scale != 8) {
        mp_raise_ValueError(MP_ERROR_TEXT("scale"));
    }

    // The decoder needs word aligned data with Huffman tables, so copy the data if
    // needed, inserting the standard tables after the start of image marker.
    jpeg_header_t hdr;
    jpeg_parse_header(data.buf, data.len, &hdr);
    const uint8_t *src = data.buf;
    size_t len = data.len;
    uint8_t *copy = NULL;
    if (!hdr.has_dht || ((uintptr_t)src & 3)) {
        size_t extra = hdr.has_dht ? 0 : sizeof(jpeg_std_dht);
        copy = m_new(uint8_t, len + extra);
        memcpy(copy, src, 2);
        memcpy(copy + 2, jpeg_std_dht, extra);
        memcpy(copy + 2 + extra, src + 2, len - 2);
        src = copy;
        len += extra;
    }

    mp_jpeg_port_decode(&d, src, len);

    if (copy != NULL) {
        m_del(uint8_t, copy, len);
    }

    mp_obj_t items[2] = {
        MP_OBJ_NEW_SMALL_INT((d.width + d.scale - 1) / d.scale),
        MP_OBJ_NEW_SMALL_INT((d.height + d.scale - 1) / d.scale),
    };
    return mp_obj_new_tuple(2, items);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(jpeg_decode_obj, 2, jpeg_decode_);

static const mp_rom_map_elem_t jpeg_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_jpeg) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&jpeg_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_decode), MP_ROM_PTR(&jpeg_decode_obj) },
    { MP_ROM_QSTR(MP_QSTR_RGB565), MP_ROM_INT(MP_JPEG_FORMAT_RGB565) },
    { MP_ROM_QSTR(MP_QSTR_GS8), MP_ROM_INT(MP_JPEG_FORMAT_GS8) },
    { MP_ROM_QSTR(MP_QSTR_RGB888), MP_ROM_INT(MP_JPEG_FORMAT_RGB888) },
};
static MP_DEFINE_CONST_DICT(jpeg_module_globals, jpeg_module_globals_table);

const mp_obj_module_t mp_module_jpeg = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&jpeg_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_jpeg, mp_module_jpeg);

#endif // MICROPY_PY_JPEG
