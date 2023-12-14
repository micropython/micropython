/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jeff Epler for Adafruit Industries
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
        MP_OBJ_NEW_SMALL_INT(self->decoder.height),
        MP_OBJ_NEW_SMALL_INT(self->decoder.width)
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

static int bitmap_output(JDEC *jd, void *data, JRECT *rect) {
    jpegio_jpegdecoder_obj_t *self = CONTAINER_OF(jd, jpegio_jpegdecoder_obj_t, decoder);
    common_hal_bitmaptools_arrayblit(self->dest, data, 2, rect->left, rect->top, rect->right + 1, rect->bottom + 1, false, 0);
    return 1;
}

void common_hal_jpegio_jpegdecoder_decode_into(jpegio_jpegdecoder_obj_t *self, displayio_bitmap_t *bitmap, int scale) {
    if (self->data_obj == MP_OBJ_NULL) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("%q() without %q()"), MP_QSTR_decode, MP_QSTR_open);
    }
    int dst_height = self->decoder.height >> scale;
    int dst_width = self->decoder.width >> scale;
    if (dst_width > bitmap->width || dst_height > bitmap->height) {
        mp_raise_ValueError(MP_ERROR_TEXT("Destination bitmap too small to contain image"));
    }
    self->dest = bitmap;
    JRESULT result = jd_decomp(&self->decoder, bitmap_output, scale);
    common_hal_jpegio_jpegdecoder_close(self);
    check_jresult(result);
}
