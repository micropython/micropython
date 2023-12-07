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
    // Nothing(!)
}

static size_t buffer_input(JDEC *jd, uint8_t *dest, size_t len) {
    jpegio_jpegdecoder_obj_t *self = CONTAINER_OF(jd, jpegio_jpegdecoder_obj_t, decoder);
    mp_buffer_info_t *src = &self->src;
    size_t to_copy = MIN(len, src->len);
    if (dest) { // passes NULL to skip data
        memcpy(dest, src->buf, to_copy);
    }
    src->buf = (uint8_t *)src->buf + to_copy;
    src->len -= to_copy;
    return to_copy;
}

static int bitmap_output(JDEC *jd, void *data, JRECT *rect) {
    jpegio_jpegdecoder_obj_t *self = CONTAINER_OF(jd, jpegio_jpegdecoder_obj_t, decoder);
    common_hal_bitmaptools_arrayblit(self->dest, data, 2, rect->left, rect->top, rect->right + 1, rect->bottom + 1, false, 0);
    return 1;
}

mp_obj_t common_hal_jpegio_jpegdecoder_decode(jpegio_jpegdecoder_obj_t *self, displayio_bitmap_t *bitmap, mp_buffer_info_t *jpeg_data, int scale) {
    self->src = *jpeg_data;
    check_jresult(jd_prepare(&self->decoder, buffer_input, &self->workspace, sizeof(self->workspace), NULL));
    int dst_height = self->decoder.height >> scale;
    int dst_width = self->decoder.width >> scale;
    if (bitmap) {
        if (dst_width > bitmap->width || dst_height > bitmap->height) {
            mp_raise_ValueError(MP_ERROR_TEXT("Destination bitmap too small to contain image"));
        }
        self->dest = bitmap;
        check_jresult(jd_decomp(&self->decoder, bitmap_output, scale));
    }
    mp_obj_t elems[] = { MP_OBJ_NEW_SMALL_INT(dst_width), MP_OBJ_NEW_SMALL_INT(dst_height) };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(elems), elems);
}
