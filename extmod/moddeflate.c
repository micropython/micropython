/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Jim Mussared
 *
 * Based on extmod/modzlib.c
 * Copyright (c) 2014-2016 Paul Sokolovsky
 * Copyright (c) 2021-2023 Damien P. George
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

#if MICROPY_PY_DEFLATE

#include "lib/uzlib/uzlib.h"

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

typedef enum {
    DEFLATEIO_FORMAT_MIN = 0,
    DEFLATEIO_FORMAT_AUTO = DEFLATEIO_FORMAT_MIN, // Read mode this means auto-detect zlib/gzip, write mode this means RAW.
    DEFLATEIO_FORMAT_RAW = 1,
    DEFLATEIO_FORMAT_ZLIB = 2,
    DEFLATEIO_FORMAT_GZIP = 3,
    DEFLATEIO_FORMAT_MAX = DEFLATEIO_FORMAT_GZIP,
} deflateio_format_t;

// This is used when the wbits is unset in the DeflateIO constructor. Default
// to the smallest window size (faster compression, less RAM usage, etc).
const int DEFLATEIO_DEFAULT_WBITS = 8;

typedef struct {
    void *window;
    uzlib_uncomp_t decomp;
    bool eof;
} mp_obj_deflateio_read_t;

#if MICROPY_PY_DEFLATE_COMPRESS
typedef struct {
    void *window;
    size_t input_len;
    uint32_t input_checksum;
    uzlib_lz77_state_t lz77;
} mp_obj_deflateio_write_t;
#endif

typedef struct {
    mp_obj_base_t base;
    mp_obj_t stream;
    uint8_t format : 2;
    uint8_t window_bits : 4;
    bool close : 1;
    mp_obj_deflateio_read_t *read;
    #if MICROPY_PY_DEFLATE_COMPRESS
    mp_obj_deflateio_write_t *write;
    #endif
} mp_obj_deflateio_t;

static int deflateio_read_stream(void *data) {
    mp_obj_deflateio_t *self = data;
    const mp_stream_p_t *stream = mp_get_stream(self->stream);
    int err;
    byte c;
    mp_uint_t out_sz = stream->read(self->stream, &c, 1, &err);
    if (out_sz == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
    if (out_sz == 0) {
        mp_raise_type(&mp_type_EOFError);
    }
    return c;
}

static bool deflateio_init_read(mp_obj_deflateio_t *self) {
    if (self->read) {
        return true;
    }

    mp_get_stream_raise(self->stream, MP_STREAM_OP_READ);

    self->read = m_new_obj(mp_obj_deflateio_read_t);
    memset(&self->read->decomp, 0, sizeof(self->read->decomp));
    self->read->decomp.source_read_data = self;
    self->read->decomp.source_read_cb = deflateio_read_stream;
    self->read->eof = false;

    // Don't modify self->window_bits as it may also be used for write.
    int wbits = self->window_bits;

    if (self->format == DEFLATEIO_FORMAT_RAW) {
        if (wbits == 0) {
            // The docs recommends always setting wbits explicitly when using
            // RAW, but we still allow a default.
            wbits = DEFLATEIO_DEFAULT_WBITS;
        }
    } else {
        // Parse the header if we're in NONE/ZLIB/GZIP modes.
        int header_wbits;
        int header_type = uzlib_parse_zlib_gzip_header(&self->read->decomp, &header_wbits);
        if (header_type < 0) {
            // Stream header was invalid.
            return false;
        }
        if ((self->format == DEFLATEIO_FORMAT_ZLIB && header_type != UZLIB_HEADER_ZLIB) || (self->format == DEFLATEIO_FORMAT_GZIP && header_type != UZLIB_HEADER_GZIP)) {
            // Not what we expected.
            return false;
        }
        // header_wbits will either be 15 (gzip) or 8-15 (zlib).
        if (wbits == 0 || header_wbits < wbits) {
            // If the header specified something lower, then use that instead.
            // No point doing a bigger allocation than we need to.
            wbits = header_wbits;
        }
    }

    size_t window_len = (size_t)1 << wbits;
    self->read->window = m_new(uint8_t, window_len);

    uzlib_uncompress_init(&self->read->decomp, self->read->window, window_len);

    return true;
}

#if MICROPY_PY_DEFLATE_COMPRESS
static void deflateio_out_byte(void *data, uint8_t b) {
    mp_obj_deflateio_t *self = data;
    const mp_stream_p_t *stream = mp_get_stream(self->stream);
    int err;
    mp_uint_t ret = stream->write(self->stream, &b, 1, &err);
    if (ret == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
}

static bool deflateio_init_write(mp_obj_deflateio_t *self) {
    if (self->write) {
        return true;
    }

    const mp_stream_p_t *stream = mp_get_stream_raise(self->stream, MP_STREAM_OP_WRITE);

    int wbits = self->window_bits;
    if (wbits == 0) {
        // Same default wbits for all formats.
        wbits = DEFLATEIO_DEFAULT_WBITS;
    }

    // Allocate the large window before allocating the mp_obj_deflateio_write_t, in case the
    // window allocation fails the mp_obj_deflateio_t object will remain in a consistent state.
    size_t window_len = 1 << wbits;
    uint8_t *window = m_new(uint8_t, window_len);

    self->write = m_new_obj(mp_obj_deflateio_write_t);
    self->write->window = window;
    self->write->input_len = 0;

    uzlib_lz77_init(&self->write->lz77, self->write->window, window_len);
    self->write->lz77.dest_write_data = self;
    self->write->lz77.dest_write_cb = deflateio_out_byte;

    // Write header if needed.
    mp_uint_t ret = 0;
    int err;
    if (self->format == DEFLATEIO_FORMAT_ZLIB) {
        // -----CMF------  ----------FLG---------------
        // CINFO(5) CM(3)  FLEVEL(2) FDICT(1) FCHECK(5)
        uint8_t buf[] = { 0x08, 0x80 }; // CM=2 (deflate), FLEVEL=2 (default), FDICT=0 (no dictionary)
        buf[0] |= MAX(wbits - 8, 1) << 4; // base-2 logarithm of the LZ77 window size, minus eight.
        buf[1] |= 31 - ((buf[0] * 256 + buf[1]) % 31); // (CMF*256 + FLG) % 31 == 0.
        ret = stream->write(self->stream, buf, sizeof(buf), &err);

        self->write->input_checksum = 1; // ADLER32
    } else if (self->format == DEFLATEIO_FORMAT_GZIP) {
        // ID1(8) ID2(8) CM(8) ---FLG--- MTIME(32) XFL(8) OS(8)
        // FLG: x x x FCOMMENT FNAME FEXTRA FHCRC FTEXT
        uint8_t buf[] = { 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03 }; // MTIME=0, XFL=4 (fastest), OS=3 (unix)
        ret = stream->write(self->stream, buf, sizeof(buf), &err);

        self->write->input_checksum = ~0; // CRC32
    }
    if (ret == MP_STREAM_ERROR) {
        return false;
    }

    // Write starting block.
    uzlib_start_block(&self->write->lz77);

    return true;
}
#endif

static mp_obj_t deflateio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args_in) {
    // args: stream, format=NONE, wbits=0, close=False
    mp_arg_check_num(n_args, n_kw, 1, 4, false);

    mp_int_t format = n_args > 1 ? mp_obj_get_int(args_in[1]) : DEFLATEIO_FORMAT_AUTO;
    mp_int_t wbits = n_args > 2 ? mp_obj_get_int(args_in[2]) : 0;

    if (format < DEFLATEIO_FORMAT_MIN || format > DEFLATEIO_FORMAT_MAX) {
        mp_raise_ValueError(MP_ERROR_TEXT("format"));
    }
    if (wbits != 0 && (wbits < 5 || wbits > 15)) {
        mp_raise_ValueError(MP_ERROR_TEXT("wbits"));
    }

    mp_obj_deflateio_t *self = mp_obj_malloc(mp_obj_deflateio_t, type);
    self->stream = args_in[0];
    self->format = format;
    self->window_bits = wbits;
    self->read = NULL;
    #if MICROPY_PY_DEFLATE_COMPRESS
    self->write = NULL;
    #endif
    self->close = n_args > 3 ? mp_obj_is_true(args_in[3]) : false;

    return MP_OBJ_FROM_PTR(self);
}

static mp_uint_t deflateio_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_deflateio_t *self = MP_OBJ_TO_PTR(o_in);

    if (self->stream == MP_OBJ_NULL || !deflateio_init_read(self)) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    if (self->read->eof) {
        return 0;
    }

    self->read->decomp.dest = buf;
    self->read->decomp.dest_limit = (uint8_t *)buf + size;
    int st = uzlib_uncompress_chksum(&self->read->decomp);
    if (st == UZLIB_DONE) {
        self->read->eof = true;
    }
    if (st < 0) {
        DEBUG_printf("uncompress error=" INT_FMT "\n", st);
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    return self->read->decomp.dest - (uint8_t *)buf;
}

#if MICROPY_PY_DEFLATE_COMPRESS
static mp_uint_t deflateio_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_deflateio_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->stream == MP_OBJ_NULL || !deflateio_init_write(self)) {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }

    self->write->input_len += size;
    if (self->format == DEFLATEIO_FORMAT_ZLIB) {
        self->write->input_checksum = uzlib_adler32(buf, size, self->write->input_checksum);
    } else if (self->format == DEFLATEIO_FORMAT_GZIP) {
        self->write->input_checksum = uzlib_crc32(buf, size, self->write->input_checksum);
    }

    uzlib_lz77_compress(&self->write->lz77, buf, size);
    return size;
}

static inline void put_le32(char *buf, uint32_t value) {
    buf[0] = value & 0xff;
    buf[1] = value >> 8 & 0xff;
    buf[2] = value >> 16 & 0xff;
    buf[3] = value >> 24 & 0xff;
}

static inline void put_be32(char *buf, uint32_t value) {
    buf[3] = value & 0xff;
    buf[2] = value >> 8 & 0xff;
    buf[1] = value >> 16 & 0xff;
    buf[0] = value >> 24 & 0xff;
}
#endif

static mp_uint_t deflateio_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    if (request == MP_STREAM_CLOSE) {
        mp_obj_deflateio_t *self = MP_OBJ_TO_PTR(self_in);

        mp_uint_t ret = 0;

        if (self->stream != MP_OBJ_NULL) {
            #if MICROPY_PY_DEFLATE_COMPRESS
            if (self->write) {
                uzlib_finish_block(&self->write->lz77);

                const mp_stream_p_t *stream = mp_get_stream(self->stream);

                // Write footer if needed.
                if (self->format == DEFLATEIO_FORMAT_ZLIB || self->format == DEFLATEIO_FORMAT_GZIP) {
                    char footer[8];
                    size_t footer_len;
                    if (self->format == DEFLATEIO_FORMAT_ZLIB) {
                        put_be32(&footer[0], self->write->input_checksum);
                        footer_len = 4;
                    } else { // DEFLATEIO_FORMAT_GZIP
                        put_le32(&footer[0], ~self->write->input_checksum);
                        put_le32(&footer[4], self->write->input_len);
                        footer_len = 8;
                    }
                    if (stream->write(self->stream, footer, footer_len, errcode) == MP_STREAM_ERROR) {
                        ret = MP_STREAM_ERROR;
                    }
                }
            }
            #endif

            // Only close the stream if required. e.g. when using io.BytesIO
            // it needs to stay open so that getvalue() can be called.
            if (self->close) {
                mp_stream_close(self->stream);
            }

            // Either way, free the reference to the stream.
            self->stream = MP_OBJ_NULL;
        }

        return ret;
    } else {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

static const mp_stream_p_t deflateio_stream_p = {
    .read = deflateio_read,
    #if MICROPY_PY_DEFLATE_COMPRESS
    .write = deflateio_write,
    #endif
    .ioctl = deflateio_ioctl,
};

#if !MICROPY_ENABLE_DYNRUNTIME
static const mp_rom_map_elem_t deflateio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    #if MICROPY_PY_DEFLATE_COMPRESS
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};
static MP_DEFINE_CONST_DICT(deflateio_locals_dict, deflateio_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    deflateio_type,
    MP_QSTR_DeflateIO,
    MP_TYPE_FLAG_NONE,
    make_new, deflateio_make_new,
    protocol, &deflateio_stream_p,
    locals_dict, &deflateio_locals_dict
    );

static const mp_rom_map_elem_t mp_module_deflate_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_deflate) },
    { MP_ROM_QSTR(MP_QSTR_DeflateIO), MP_ROM_PTR(&deflateio_type) },
    { MP_ROM_QSTR(MP_QSTR_AUTO), MP_ROM_INT(DEFLATEIO_FORMAT_AUTO) },
    { MP_ROM_QSTR(MP_QSTR_RAW), MP_ROM_INT(DEFLATEIO_FORMAT_RAW) },
    { MP_ROM_QSTR(MP_QSTR_ZLIB), MP_ROM_INT(DEFLATEIO_FORMAT_ZLIB) },
    { MP_ROM_QSTR(MP_QSTR_GZIP), MP_ROM_INT(DEFLATEIO_FORMAT_GZIP) },
};
static MP_DEFINE_CONST_DICT(mp_module_deflate_globals, mp_module_deflate_globals_table);

const mp_obj_module_t mp_module_deflate = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_deflate_globals,
};

MP_REGISTER_MODULE(MP_QSTR_deflate, mp_module_deflate);
#endif // !MICROPY_ENABLE_DYNRUNTIME

// Source files #include'd here to make sure they're compiled in
// only if the module is enabled.

#include "lib/uzlib/tinflate.c"
#include "lib/uzlib/header.c"
#include "lib/uzlib/adler32.c"
#include "lib/uzlib/crc32.c"

#if MICROPY_PY_DEFLATE_COMPRESS
#include "lib/uzlib/lz77.c"
#endif

#endif // MICROPY_PY_DEFLATE
