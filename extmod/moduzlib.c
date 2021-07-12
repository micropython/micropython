// Copyright (c) 2014-2016 Paul Sokolovsky
// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
//
// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"

#include "supervisor/shared/translate.h"

#if MICROPY_PY_UZLIB

#define UZLIB_CONF_PARANOID_CHECKS (1)
#include "../lib/uzlib/src/tinf.h"

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

STATIC mp_obj_t decompio_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 1, 2, false);
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ);
    mp_obj_decompio_t *o = m_new_obj(mp_obj_decompio_t);
    o->base.type = type;
    memset(&o->decomp, 0, sizeof(o->decomp));
    o->decomp.readSource = read_src_stream;
    o->src_stream = args[0];
    o->eof = false;

    mp_int_t dict_opt = 0;
    int dict_sz;
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
        dict_sz = 1 << dict_opt;
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
    o->decomp.dest_limit = (unsigned char *)buf + size;
    int st = uzlib_uncompress_chksum(&o->decomp);
    if (st == TINF_DONE) {
        o->eof = true;
    }
    if (st < 0) {
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
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = decompio_read,
};

#if !MICROPY_ENABLE_DYNRUNTIME
STATIC const mp_obj_type_t decompio_type = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_DecompIO,
    .make_new = decompio_make_new,
    .locals_dict = (void *)&decompio_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .protocol = &decompio_stream_p,
        ),
};
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
    DEBUG_printf("uzlib: Initial out buffer: " UINT_FMT " bytes\n", decomp->destSize);
    decomp->source = bufinfo.buf;
    decomp->source_limit = (unsigned char *)bufinfo.buf + bufinfo.len;
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
        decomp->dest_limit = dest_buf + offset + 256;
    }

    mp_uint_t final_sz = decomp->dest - dest_buf;
    DEBUG_printf("uzlib: Resizing from " UINT_FMT " to final size: " UINT_FMT " bytes\n", dest_buf_size, final_sz);
    dest_buf = (byte *)m_renew(byte, dest_buf, dest_buf_size, final_sz);
    mp_obj_t res = mp_obj_new_bytearray_by_ref(final_sz, dest_buf);
    m_del_obj(TINF_DATA, decomp);
    return res;

error:
    mp_raise_arg1(&mp_type_ValueError, MP_OBJ_NEW_SMALL_INT(st));
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
#endif

// Source files #include'd here to make sure they're compiled in
// only if module is enabled by config setting.

#pragma GCC diagnostic ignored "-Wsign-compare"
#include "../lib/uzlib/src/tinflate.c"
#include "../lib/uzlib/src/tinfzlib.c"
#include "../lib/uzlib/src/tinfgzip.c"
#include "../lib/uzlib/src/adler32.c"
#include "../lib/uzlib/src/crc32.c"

#endif // MICROPY_PY_UZLIB
