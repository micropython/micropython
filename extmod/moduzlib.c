/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2016 Paul Sokolovsky
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"

#if MICROPY_PY_UZLIB

#include "lib/uzlib/tinf.h"

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

typedef struct _mp_obj_decompio_t {
    mp_obj_base_t base;
    mp_obj_t src_stream;
    TINF_DATA decomp;
    bool eof;
} mp_obj_decompio_t;

STATIC int read_src_stream(TINF_DATA *data) {
    byte *p = (void *)data;
    p -= offsetof(mp_obj_decompio_t, decomp);
    mp_obj_decompio_t *self = (mp_obj_decompio_t *)p;

    const mp_stream_p_t *stream = mp_get_stream(self->src_stream);
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

STATIC mp_obj_t decompio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ);
    mp_obj_decompio_t *o = mp_obj_malloc(mp_obj_decompio_t, type);
    memset(&o->decomp, 0, sizeof(o->decomp));
    o->decomp.readSource = read_src_stream;
    o->src_stream = args[0];
    o->eof = false;

    mp_int_t dict_opt = 0;
    uint dict_sz;
    if (n_args > 1) {
        dict_opt = mp_obj_get_int(args[1]);
    }

    if (dict_opt >= 16) {
        int st = uzlib_gzip_parse_header(&o->decomp);
        if (st != TINF_OK) {
            goto header_error;
        }
        dict_sz = 1 << (dict_opt - 16);
    } else if (dict_opt >= 0) {
        dict_opt = uzlib_zlib_parse_header(&o->decomp);
        if (dict_opt < 0) {
        header_error:
            mp_raise_ValueError(MP_ERROR_TEXT("compression header"));
        }
        // RFC 1950 section 2.2:
        // CINFO is the base-2 logarithm of the LZ77 window size,
        // minus eight (CINFO=7 indicates a 32K window size)
        dict_sz = 1 << (dict_opt + 8);
    } else {
        dict_sz = 1 << -dict_opt;
    }

    uzlib_uncompress_init(&o->decomp, m_new(byte, dict_sz), dict_sz);
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_uint_t decompio_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_decompio_t *o = MP_OBJ_TO_PTR(o_in);
    if (o->eof) {
        return 0;
    }

    o->decomp.dest = buf;
    o->decomp.dest_limit = (byte *)buf + size;
    int st = uzlib_uncompress_chksum(&o->decomp);
    if (st == TINF_DONE) {
        o->eof = true;
    }
    if (st < 0) {
        DEBUG_printf("uncompress error=" INT_FMT "\n", st);
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    return o->decomp.dest - (byte *)buf;
}

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t decompio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
};

STATIC MP_DEFINE_CONST_DICT(decompio_locals_dict, decompio_locals_dict_table);
#endif

STATIC const mp_stream_p_t decompio_stream_p = {
    .read = decompio_read,
};

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC MP_DEFINE_CONST_OBJ_TYPE(
    decompio_type,
    MP_QSTR_DecompIO,
    MP_TYPE_FLAG_NONE,
    make_new, decompio_make_new,
    protocol, &decompio_stream_p,
    locals_dict, &decompio_locals_dict
    );
#endif

STATIC mp_obj_t mod_uzlib_decompress(size_t n_args, const mp_obj_t *args) {
    mp_obj_t data = args[0];
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);

    TINF_DATA *decomp = m_new_obj(TINF_DATA);
    memset(decomp, 0, sizeof(*decomp));
    DEBUG_printf("sizeof(TINF_DATA)=" UINT_FMT "\n", sizeof(*decomp));
    uzlib_uncompress_init(decomp, NULL, 0);
    mp_uint_t dest_buf_size = (bufinfo.len + 15) & ~15;
    byte *dest_buf = m_new(byte, dest_buf_size);

    decomp->dest = dest_buf;
    decomp->dest_limit = dest_buf + dest_buf_size;
    DEBUG_printf("uzlib: Initial out buffer: " UINT_FMT " bytes\n", dest_buf_size);
    decomp->source = bufinfo.buf;
    decomp->source_limit = (byte *)bufinfo.buf + bufinfo.len;

    int st;
    bool is_zlib = true;

    if (n_args > 1 && MP_OBJ_SMALL_INT_VALUE(args[1]) < 0) {
        is_zlib = false;
    }

    if (is_zlib) {
        st = uzlib_zlib_parse_header(decomp);
        if (st < 0) {
            goto error;
        }
    }

    while (1) {
        st = uzlib_uncompress_chksum(decomp);
        if (st < 0) {
            goto error;
        }
        if (st == TINF_DONE) {
            break;
        }
        size_t offset = decomp->dest - dest_buf;
        dest_buf = m_renew(byte, dest_buf, dest_buf_size, dest_buf_size + 256);
        dest_buf_size += 256;
        decomp->dest = dest_buf + offset;
        decomp->dest_limit = decomp->dest + 256;
    }

    mp_uint_t final_sz = decomp->dest - dest_buf;
    DEBUG_printf("uzlib: Resizing from " UINT_FMT " to final size: " UINT_FMT " bytes\n", dest_buf_size, final_sz);
    dest_buf = (byte *)m_renew(byte, dest_buf, dest_buf_size, final_sz);
    mp_obj_t res = mp_obj_new_bytearray_by_ref(final_sz, dest_buf);
    m_del_obj(TINF_DATA, decomp);
    return res;

error:
    mp_raise_type_arg(&mp_type_ValueError, MP_OBJ_NEW_SMALL_INT(st));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_uzlib_decompress_obj, 1, 3, mod_uzlib_decompress);

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t mp_module_uzlib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uzlib) },
    { MP_ROM_QSTR(MP_QSTR_decompress), MP_ROM_PTR(&mod_uzlib_decompress_obj) },
    { MP_ROM_QSTR(MP_QSTR_DecompIO), MP_ROM_PTR(&decompio_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_uzlib_globals, mp_module_uzlib_globals_table);

const mp_obj_module_t mp_module_uzlib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_uzlib_globals,
};


MP_REGISTER_MODULE(MP_QSTR_uzlib, mp_module_uzlib);
#endif

// Source files #include'd here to make sure they're compiled in
// only if module is enabled by config setting.

#include "lib/uzlib/tinflate.c"
#include "lib/uzlib/tinfzlib.c"
#include "lib/uzlib/tinfgzip.c"
#include "lib/uzlib/adler32.c"
#include "lib/uzlib/crc32.c"

#endif // MICROPY_PY_UZLIB
