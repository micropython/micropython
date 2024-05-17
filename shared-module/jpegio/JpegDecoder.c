// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2023 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"

#include "shared-bindings/jpegio/JpegDecoder.h"
#include "shared-bindings/bitmaptools/__init__.h"
#include "shared-module/jpegio/JpegDecoder.h"

typedef size_t (*input_func)(JDEC *jd, uint8_t *dest, size_t len);

// Given a pointer `ptr` to the field `field_name` inside a structure of type `type`,
// retrieve a pointer to the containing object.
// This is used to retrieve the jpegio_jpegdecoder_obj_t object given the JDEC.
// Similar macros of this type are frequently employed in low-level code, but this is
// not standardized.
#define CONTAINER_OF(ptr, type, field_name) ((type *)(void *)(((uint8_t *)ptr) - offsetof(type, field_name)))

static void check_jresult(JRESULT j) {
    mp_rom_error_text_t msg = 0;
    switch (j) {
        case JDR_OK:
            return;
        case JDR_INTR:
            msg = MP_ERROR_TEXT("Interrupted by output function");
            break;
        case JDR_INP:
            msg = MP_ERROR_TEXT("Device error or wrong termination of input stream");
            break;
        case JDR_MEM1:
            msg = MP_ERROR_TEXT("Insufficient memory pool for the image");
            break;
        case JDR_MEM2:
            msg = MP_ERROR_TEXT("Insufficient stream input buffer");
            break;
        case JDR_PAR:
            msg = MP_ERROR_TEXT("Parameter error");
            break;
        case JDR_FMT1:
            msg = MP_ERROR_TEXT("Data format error (may be broken data)");
            break;
        case JDR_FMT2:
            msg = MP_ERROR_TEXT("Right format but not supported");
            break;
        case JDR_FMT3:
            msg = MP_ERROR_TEXT("Not supported JPEG standard");
            break;
    }
    mp_raise_RuntimeError(msg);
}

void common_hal_jpegio_jpegdecoder_construct(jpegio_jpegdecoder_obj_t *self) {
    self->data_obj = MP_OBJ_NULL;
}

void common_hal_jpegio_jpegdecoder_close(jpegio_jpegdecoder_obj_t *self) {
    self->data_obj = MP_OBJ_NULL;
    memset(&self->bufinfo, 0, sizeof(self->bufinfo));
}

static mp_obj_t common_hal_jpegio_jpegdecoder_decode_common(jpegio_jpegdecoder_obj_t *self, input_func fun) {
    JRESULT result = jd_prepare(&self->decoder, fun, &self->workspace, sizeof(self->workspace), NULL);
    if (result != JDR_OK) {
        common_hal_jpegio_jpegdecoder_close(self);
    }
    check_jresult(result);
    mp_obj_t elems[] = {
        MP_OBJ_NEW_SMALL_INT(self->decoder.width),
        MP_OBJ_NEW_SMALL_INT(self->decoder.height)
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(elems), elems);
}

static size_t file_input(JDEC *jd, uint8_t *dest, size_t len) {
    jpegio_jpegdecoder_obj_t *self = CONTAINER_OF(jd, jpegio_jpegdecoder_obj_t, decoder);

    if (!dest) {
        // caller passes NULL to skip data; we need to read over the data.
        // Don't assume a seekable stream, because we want to decode jpegs
        // right from a native socket object
        uint8_t buf[512];
        size_t total = 0;
        size_t read;
        do {
            size_t to_discard = MIN(len - total, sizeof(buf));
            read = file_input(jd, buf, to_discard);
            total += read;
        } while (read != 0 && total != len);
        return len;
    }

    int errcode = 0;
    size_t result = mp_stream_rw(self->data_obj, dest, len, &errcode, MP_STREAM_RW_READ);
    if (errcode != 0) { // raise our own error in case of I/O failure, it's better than the decoder's error
        mp_raise_OSError(errcode);
    }
    return result;
}

mp_obj_t common_hal_jpegio_jpegdecoder_set_source_file(jpegio_jpegdecoder_obj_t *self, mp_obj_t file_obj) {
    self->data_obj = file_obj;
    return common_hal_jpegio_jpegdecoder_decode_common(self, file_input);
}

static size_t buffer_input(JDEC *jd, uint8_t *dest, size_t len) {
    jpegio_jpegdecoder_obj_t *self = CONTAINER_OF(jd, jpegio_jpegdecoder_obj_t, decoder);
    mp_buffer_info_t *src = &self->bufinfo;
    size_t to_copy = MIN(len, src->len);
    if (dest) { // passes NULL to skip data
        memcpy(dest, src->buf, to_copy);
    }
    src->buf = (uint8_t *)src->buf + to_copy;
    src->len -= to_copy;
    return to_copy;
}

mp_obj_t common_hal_jpegio_jpegdecoder_set_source_buffer(jpegio_jpegdecoder_obj_t *self, mp_obj_t buffer_obj) {
    self->data_obj = buffer_obj;
    mp_get_buffer_raise(buffer_obj, &self->bufinfo, MP_BUFFER_READ);
    return common_hal_jpegio_jpegdecoder_decode_common(self, buffer_input);
}

#define DECODER_CONTINUE (1)
#define DECODER_INTERRUPT (0)
static int bitmap_output(JDEC *jd, void *data, JRECT *rect) {
    jpegio_jpegdecoder_obj_t *self = CONTAINER_OF(jd, jpegio_jpegdecoder_obj_t, decoder);
    int src_width = rect->right - rect->left + 1, src_pixel_stride = src_width /* in units of pixels! */, src_height = rect->bottom - rect->top + 1;

    displayio_bitmap_t src = {
        .width = src_width,
        .height = src_height,
        .data = data,
        .stride = src_pixel_stride / 2, /* in units of uint32_t */
        .bits_per_value = 16,
        .x_shift = 1,
        .x_mask = 1,
        .bitmask = 0xffff,
    };

    int x = self->x;
    int y = self->y;
    int x1 = self->lim.x1 - rect->left;
    int x2 = self->lim.x2 - rect->left;
    int y1 = self->lim.y1 - rect->top;
    int y2 = self->lim.y2 - rect->top;

    if (y2 < y1) {
        // The last row in the source image to copy FROM is above of this, so
        // no more pixels on any rows
        return DECODER_INTERRUPT;
    }

    y2 = MIN(y2, src_height);
    if (x2 < x1) {
        // The last column in the source image to copy FROM is left of this, so
        // no more pixels on this row but could be on subsequent rows
        return DECODER_CONTINUE;
    }
    x2 = MIN(x2, src_width);

    // The first column in the source image to copy FROM is left of this, so copy starting with
    // *local source* x1 equal to 0, and *target* x adjusted right by the same amount
    if (x1 < 0) {
        x += -x1;
        x1 = 0;
    }

    // the same, but for Y coordinates
    if (y1 < 0) {
        y += -y1;
        y1 = 0;
    }

    // blit takes care of x, y out of range
    assert(x1 >= 0);
    assert(y1 >= 0);
    // blit takes care of x1 >= x2 and y1 >= y2 cases
    assert(x2 <= src_width);
    assert(y2 <= src_height);

    common_hal_bitmaptools_blit(self->dest, &src, x, y, x1, y1, x2, y2, self->skip_source_index, self->skip_source_index_none, self->skip_dest_index, self->skip_dest_index_none);
    return 1;
}

void common_hal_jpegio_jpegdecoder_decode_into(
    jpegio_jpegdecoder_obj_t *self,
    displayio_bitmap_t *bitmap, int scale, int16_t x, int16_t y,
    bitmaptools_rect_t *lim,
    uint32_t skip_source_index, bool skip_source_index_none,
    uint32_t skip_dest_index, bool skip_dest_index_none) {
    if (self->data_obj == MP_OBJ_NULL) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q() without %q()"), MP_QSTR_decode, MP_QSTR_open);
    }

    self->x = x;
    self->y = y;
    self->lim = *lim;
    self->skip_source_index = skip_source_index;
    self->skip_source_index_none = skip_source_index_none;
    self->skip_dest_index = skip_dest_index;
    self->skip_dest_index_none = skip_dest_index_none;

    self->dest = bitmap;
    JRESULT result = jd_decomp(&self->decoder, bitmap_output, scale);
    common_hal_jpegio_jpegdecoder_close(self);
    if (result != JDR_INTR) {
        check_jresult(result);
    }
}
