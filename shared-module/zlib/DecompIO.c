/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Mark Komus
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

#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/builtin.h"
#include "py/objtuple.h"
#include "py/binary.h"
#include "py/parsenum.h"

#include "shared-bindings/zlib/__init__.h"
#include "shared-bindings/zlib/DecompIO.h"
#include "shared-module/zlib/DecompIO.h"

STATIC int read_src_stream(TINF_DATA *data) {
    mp_obj_decompio_t *self = (mp_obj_decompio_t *)data->self;

    const mp_stream_p_t *stream = mp_get_stream_raise(self->src_stream, MP_STREAM_OP_READ);
    int err;
    byte c;
    mp_uint_t out_sz = stream->read(self->src_stream, &c, 1, &err);

    if (out_sz == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
    if (out_sz == 0) {
        mp_raise_type(&mp_type_EOFError);
    }
    return c;
}

void common_hal_zlib_decompio_construct(mp_obj_decompio_t *self, mp_obj_t src_stream, mp_int_t dict_opt) {
    self->decomp.self = self;
    self->decomp.readSource = read_src_stream;
    self->src_stream = src_stream;
    self->eof = false;

    int dict_sz;

    if (dict_opt >= 16) {
        int st = uzlib_gzip_parse_header(&self->decomp);
        if (st != TINF_OK) {
            goto header_error;
        }
        dict_sz = 1 << (dict_opt - 16);
    } else if (dict_opt >= 0) {
        dict_opt = uzlib_zlib_parse_header(&self->decomp);
        if (dict_opt < 0) {
        header_error:
            mp_raise_ValueError(MP_ERROR_TEXT("compression header"));
        }
        dict_sz = 1 << dict_opt;
    } else {
        dict_sz = 1 << -dict_opt;
    }

    uzlib_uncompress_init(&self->decomp, m_new(byte, dict_sz), dict_sz);
}

mp_uint_t common_hal_zlib_decompio_read(mp_obj_decompio_t *self, void *buf, mp_uint_t size, int *errcode) {
    self->decomp.dest = buf;
    self->decomp.dest_limit = (unsigned char *)buf + size;
    int st = uzlib_uncompress_chksum(&self->decomp);
    if (st == TINF_DONE) {
        self->eof = true;
    }
    if (st < 0) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    return self->decomp.dest - (byte *)buf;
}
