/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2016 Paul Sokolovsky
 * Copyright (c) 2021-2023 Damien P. George
 * Copyright (c) 2023 Jim Mussared
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

#if MICROPY_PY_GZIP

#include "lib/uzlib/uzlib.h"

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

typedef enum {
    GZIPFILE_MODE_READ = 0,
    GZIPFILE_MODE_WRITE_RAW = 1,
    GZIPFILE_MODE_WRITE_ZLIB = 2,
    GZIPFILE_MODE_WRITE_GZIP = 3,
} gzipfile_mode_t;

typedef struct {
    uzlib_uncomp_t decomp;
    bool eof;
} mp_obj_gzipfile_read_t;

#if MICROPY_PY_GZIP_COMPRESS
typedef struct {
    size_t input_len;
    uint32_t input_checksum;
    struct uzlib_lz77_state lz77;
} mp_obj_gzipfile_write_t;
#endif

typedef struct {
    mp_obj_base_t base;
    mp_obj_t stream;
    #if MICROPY_PY_GZIP_COMPRESS
    gzipfile_mode_t mode;
    #endif
    mp_obj_t hist_obj;
    union {
        mp_obj_gzipfile_read_t read;
        #if MICROPY_PY_GZIP_COMPRESS
        mp_obj_gzipfile_write_t write;
        #endif
    } state[];
} mp_obj_gzipfile_t;

#if MICROPY_PY_GZIP_COMPRESS
STATIC void gzipfile_out_byte(void *data, uint8_t b) {
    mp_obj_gzipfile_t *self = data;
    const mp_stream_p_t *stream = mp_get_stream(self->stream);
    int err;
    mp_uint_t ret = stream->write(self->stream, &b, 1, &err);
    if (ret == MP_STREAM_ERROR) {
        mp_raise_OSError(err);
    }
}
#endif


STATIC int gzipfile_read_stream(uzlib_uncomp_t *data) {
    // data is a pointer to the .decomp member of mp_obj_gzipfile_t. Work
    // backwards to the mp_obj_gzipfile_t pointer.
    uint8_t *p = (void *)data;
    p -= offsetof(mp_obj_gzipfile_t, state[0].read.decomp);
    mp_obj_gzipfile_t *self = (mp_obj_gzipfile_t *)p;

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

STATIC void parse_wbits_hist(mp_obj_gzipfile_t *self, int *wbits, mp_obj_t hist_in, uint8_t **hist, size_t *hist_len) {
    #if MICROPY_PY_GZIP_COMPRESS
    if (self->mode == GZIPFILE_MODE_READ)
    #endif
    {
        if (*wbits >= -15 && *wbits <= -8) {
            // −8 to −15: Uses the absolute value of *wbits as the window size
            // logarithm. The input must be a raw stream with no header or
            // trailer.
            *wbits = -*wbits;
        } else if (*wbits >= 8 && *wbits <= 15) {
            // +8 to +15: The base-two logarithm of the window size. The input
            //  must include a zlib header and trailer.
            int header_wbits = uzlib_zlib_parse_header(&self->state[0].read.decomp);
            if (header_wbits < 0 || header_wbits + 8 != *wbits) { // TODO how does cpython handle this?
                mp_raise_OSError(MP_EINVAL);
            }
        } else if (*wbits >= 24 && *wbits <= 31) {
            // +24 to +31 = 16 + (8 to 15): Uses the low 4 bits of the value
            //  as the window size logarithm. The input must include a gzip
            //  header and trailer.
            *wbits = *wbits & 0xf;
            int gzip_err = uzlib_gzip_parse_header(&self->state[0].read.decomp);
            if (gzip_err != UZLIB_OK) {
                mp_raise_OSError(gzip_err); // gzip_err is already negative.
            }
            // } else if (*wbits >= 40 && *wbits <= 47) {
            //     // +40 to +47 = 32 + (8 to 15): Uses the low 4 bits of the value
            //     //  as the window size logarithm, and automatically accepts
            //     //  either the zlib or gzip format.
            //     *wbits = *wbits & 0xf;
            //     // We don't currently have a way to peek at the input stream.
            //     // Not supported.
        } else if (*wbits == 0) {
            // 0: Automatically determine the window size from the zlib
            // header. Only supported since zlib 1.2.3.5.
            int header_wbits = uzlib_zlib_parse_header(&self->state[0].read.decomp);
            if (header_wbits < 0) {
                mp_raise_OSError(MP_EINVAL);
            }
            *wbits = header_wbits + 8;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("wbits"));
        }
    #if MICROPY_PY_GZIP_COMPRESS
    } else {
        if (*wbits >= -15 && *wbits <= -9) {
            // −9 to −15: Uses the absolute value of *wbits as the window size
            // logarithm, while producing a raw output stream with no header or
            // trailing checksum.
            *wbits = -*wbits;
        } else if (*wbits >= 9 && *wbits <= 15) {
            // +9 to +15: The base-two logarithm of the window size, which
            // therefore ranges between 512 and 32768. Larger values produce
            // better compression at the expense of greater memory usage. The
            // resulting output will include a zlib-specific header and trailer.
            self->mode = GZIPFILE_MODE_WRITE_ZLIB;
            self->state[0].write.input_checksum = 1; // ADLER32
        } else if (*wbits >= 25 && *wbits <= 31) {
            // +25 to +31 = 16 + (9 to 15): Uses the low 4 bits of the value as
            // the window size logarithm, while including a basic gzip header
            // and trailing checksum in the output.
            self->mode = GZIPFILE_MODE_WRITE_GZIP;
            self->state[0].write.input_checksum = ~0; // CRC32
            *wbits = *wbits & 0xf;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("wbits"));
        }
    #endif
    }

    *hist_len = 1 << *wbits;
    if (hist_in == mp_const_none) {
        *hist = m_new(uint8_t, *hist_len);
        self->hist_obj = MP_OBJ_NULL;
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(hist_in, &bufinfo, MP_BUFFER_RW);
        if (bufinfo.len < *hist_len) {
            mp_raise_ValueError(MP_ERROR_TEXT("hist"));
        }
        *hist = bufinfo.buf;
        self->hist_obj = hist_in; // Keep the buffer object live.
    }
}

STATIC mp_obj_t gzipfile_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Unsupported from CPython: filename, compresslevel, mtime=None
    // MicroPython extras: wbits, hist

    // Parse args
    enum { ARG_fileobj, ARG_mode, ARG_wbits, ARG_hist };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_fileobj, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_mode, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_wbits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 25} }, // Default to gzip with smallest window.
        { MP_QSTR_hist, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t mode_len;
    const char *mode = mp_obj_str_get_data(args[ARG_mode].u_obj, &mode_len);

    mp_obj_gzipfile_t *self;
    if (memchr(mode, 'r', mode_len)) {
        self = m_new_obj_var(mp_obj_gzipfile_t, mp_obj_gzipfile_read_t, 1);
        #if MICROPY_PY_GZIP_COMPRESS
        self->mode = GZIPFILE_MODE_READ;
    } else if (memchr(mode, 'w', mode_len)) {
        self = m_new_obj_var(mp_obj_gzipfile_t, mp_obj_gzipfile_write_t, 1);
        self->mode = GZIPFILE_MODE_WRITE_RAW;
        #endif
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid mode"));
    }

    self->base.type = type;
    self->stream = args[ARG_fileobj].u_obj;

    #if MICROPY_PY_GZIP_COMPRESS
    if (self->mode == GZIPFILE_MODE_READ)
    #endif
    {
        mp_get_stream_raise(self->stream, MP_STREAM_OP_READ);

        memset(&self->state[0].read.decomp, 0, sizeof(self->state[0].read.decomp));
        self->state[0].read.decomp.source_read_cb = gzipfile_read_stream;
        self->state[0].read.eof = false;

        // Parse "wbits" argument and set up mode and history state.
        uint8_t *hist;
        size_t hist_len;
        int wbits = args[ARG_wbits].u_int;
        parse_wbits_hist(self, &wbits, args[ARG_hist].u_obj, &hist, &hist_len);

        uzlib_uncompress_init(&self->state[0].read.decomp, hist, hist_len);
    #if MICROPY_PY_GZIP_COMPRESS
    } else {
        const mp_stream_p_t *stream = mp_get_stream_raise(self->stream, MP_STREAM_OP_WRITE);

        self->state[0].write.input_len = 0;

        // Parse "wbits" argument and set up mode and history state.
        uint8_t *hist;
        size_t hist_len;
        int wbits = args[ARG_wbits].u_int;
        parse_wbits_hist(self, &wbits, args[ARG_hist].u_obj, &hist, &hist_len);

        uzlib_lz77_init(&self->state[0].write.lz77, hist, hist_len);
        self->state[0].write.lz77.outbuf.dest_write_data = self;
        self->state[0].write.lz77.outbuf.dest_write_cb = gzipfile_out_byte;

        // Write header if needed.
        mp_uint_t ret = 0;
        int err;
        if (self->mode == GZIPFILE_MODE_WRITE_ZLIB) {
            // -----CMF------  ----------FLG---------------
            // CINFO(5) CM(3)  FLEVEL(2) FDICT(1) FCHECK(5)
            uint8_t buf[] = { 0x08, 0x80 };     // CM=2 (deflate), FLEVEL=2 (default), FDICT=0 (no dictionary)
            buf[0] |= (wbits - 8) << 4;     // base-2 logarithm of the LZ77 window size, minus eight.
            buf[1] |= 31 - ((buf[0] * 256 + buf[1]) % 31);     // (CMF*256 + FLG) % 31 == 0.
            ret = stream->write(self->stream, buf, sizeof(buf), &err);
        } else if (self->mode == GZIPFILE_MODE_WRITE_GZIP) {
            // ID1(8) ID2(8) CM(8) ---FLG--- MTIME(32) XFL(8) OS(8)
            // FLG: x x x FCOMMENT FNAME FEXTRA FHCRC FTEXT
            uint8_t buf[] = { 0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03 };     // MTIME=0, XFL=4 (fastest), OS=3 (unix)
            ret = stream->write(self->stream, buf, sizeof(buf), &err);
        }
        if (ret == MP_STREAM_ERROR) {
            mp_raise_OSError(err);
        }

        // Write starting block.
        zlib_start_block(&self->state[0].write.lz77.outbuf);
    #endif // MICROPY_PY_GZIP_COMPRESS
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_uint_t gzipfile_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_gzipfile_t *self = MP_OBJ_TO_PTR(o_in);
    #if MICROPY_PY_GZIP_COMPRESS
    if (self->mode != GZIPFILE_MODE_READ) {
        mp_raise_OSError(MP_EBADF);
    }
    #endif
    if (self->state[0].read.eof) {
        return 0;
    }

    self->state[0].read.decomp.dest = buf;
    self->state[0].read.decomp.dest_limit = (uint8_t *)buf + size;
    int st = uzlib_uncompress_chksum(&self->state[0].read.decomp);
    if (st == UZLIB_DONE) {
        self->state[0].read.eof = true;
    }
    if (st < 0) {
        DEBUG_printf("uncompress error=" INT_FMT "\n", st);
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
    return self->state[0].read.decomp.dest - (uint8_t *)buf;
}

#if MICROPY_PY_GZIP_COMPRESS
STATIC mp_uint_t gzipfile_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_gzipfile_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->mode == GZIPFILE_MODE_READ) {
        mp_raise_OSError(MP_EBADF);
    }
    self->state[0].write.input_len += size;
    if (self->mode == GZIPFILE_MODE_WRITE_ZLIB) {
        self->state[0].write.input_checksum = uzlib_adler32(buf, size, self->state[0].write.input_checksum);
    } else if (self->mode == GZIPFILE_MODE_WRITE_GZIP) {
        self->state[0].write.input_checksum = uzlib_crc32(buf, size, self->state[0].write.input_checksum);
    }
    uzlib_lz77_compress(&self->state[0].write.lz77, buf, size);
    return size;
}
#endif

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

STATIC mp_uint_t gzipfile_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {

    if (request == MP_STREAM_CLOSE) {
        #if MICROPY_PY_GZIP_COMPRESS
        mp_obj_gzipfile_t *self = MP_OBJ_TO_PTR(self_in);
        if (self->mode != GZIPFILE_MODE_READ && self->stream != MP_OBJ_NULL) {
            zlib_finish_block(&self->state[0].write.lz77.outbuf);

            const mp_stream_p_t *stream = mp_get_stream(self->stream);

            // Write footer if needed.
            if (self->mode != GZIPFILE_MODE_WRITE_RAW) {
                char footer[8];
                size_t footer_len;
                if (self->mode == GZIPFILE_MODE_WRITE_ZLIB) {
                    put_be32(&footer[0], self->state[0].write.input_checksum);
                    footer_len = 4;
                } else { // GZIPFILE_MODE_WRITE_GZIP
                    put_le32(&footer[0], ~self->state[0].write.input_checksum);
                    put_le32(&footer[4], self->state[0].write.input_len);
                    footer_len = 8;
                }
                mp_uint_t ret = stream->write(self->stream, footer, footer_len, errcode);
                if (ret == MP_STREAM_ERROR) {
                    self->stream = MP_OBJ_NULL;
                    return ret;
                }
            }

            // Don't close the stream (the caller may still want to write to it, or in
            // the case of BytesIO call getvalue), instead just free the reference to it.
            self->stream = MP_OBJ_NULL;
        }
        #endif

        return 0;
    } else {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

STATIC const mp_stream_p_t gzipfile_stream_p = {
    .read = gzipfile_read,
    #if MICROPY_PY_GZIP_COMPRESS
    .write = gzipfile_write,
    #endif
    .ioctl = gzipfile_ioctl,
};

STATIC const mp_rom_map_elem_t gzipfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    #if MICROPY_PY_GZIP_COMPRESS
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};
STATIC MP_DEFINE_CONST_DICT(gzipfile_locals_dict, gzipfile_locals_dict_table);

STATIC MP_DEFINE_CONST_OBJ_TYPE(
    gzipfile_type,
    MP_QSTR_GzipFile,
    MP_TYPE_FLAG_NONE,
    make_new, gzipfile_make_new,
    protocol, &gzipfile_stream_p,
    locals_dict, &gzipfile_locals_dict
    );

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_rom_map_elem_t mp_module_gzip_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_gzip) },
    { MP_ROM_QSTR(MP_QSTR_GzipFile), MP_ROM_PTR(&gzipfile_type) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_gzip_globals, mp_module_gzip_globals_table);

const mp_obj_module_t mp_module_gzip = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_gzip_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_gzip, mp_module_gzip);
#endif

// Source files #include'd here to make sure they're compiled in
// only if module is enabled by config setting.

#include "lib/uzlib/tinflate.c"
#include "lib/uzlib/tinfzlib.c"
#include "lib/uzlib/tinfgzip.c"
#include "lib/uzlib/adler32.c"
#include "lib/uzlib/crc32.c"

#if MICROPY_PY_GZIP_COMPRESS
#include "lib/uzlib/defl_static.c"
#include "lib/uzlib/lz77.c"
#endif

#endif // MICROPY_PY_GZIP
