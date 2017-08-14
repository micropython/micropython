/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <assert.h>
#include <string.h>

#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stream.h"

#if MICROPY_PY_IO

extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;

#if MICROPY_PY_IO_BUFFEREDWRITER
typedef struct _mp_obj_bufwriter_t {
    mp_obj_base_t base;
    mp_obj_t stream;
    size_t alloc;
    size_t len;
    byte buf[0];
} mp_obj_bufwriter_t;

STATIC mp_obj_t bufwriter_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 2, 2, false);
    size_t alloc = mp_obj_get_int(args[1]);
    mp_obj_bufwriter_t *o = m_new_obj_var(mp_obj_bufwriter_t, byte, alloc);
    o->base.type = type;
    o->stream = args[0];
    o->alloc = alloc;
    o->len = 0;
    return o;
}

STATIC mp_uint_t bufwriter_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_bufwriter_t *self = MP_OBJ_TO_PTR(self_in);

    mp_uint_t org_size = size;

    while (size > 0) {
        mp_uint_t rem = self->alloc - self->len;
        if (size < rem) {
            memcpy(self->buf + self->len, buf, size);
            self->len += size;
            return org_size;
        }

        // Buffer flushing policy here is to flush entire buffer all the time.
        // This allows e.g. to have a block device as backing storage and write
        // entire block to it. memcpy below is not ideal and could be optimized
        // in some cases. But the way it is now it at least ensures that buffer
        // is word-aligned, to guard against obscure cases when it matters, e.g.
        // https://github.com/micropython/micropython/issues/1863
        memcpy(self->buf + self->len, buf, rem);
        buf = (byte*)buf + rem;
        size -= rem;
        mp_uint_t out_sz = mp_stream_write_exactly(self->stream, self->buf, self->alloc, errcode);
        if (*errcode != 0) {
            return MP_STREAM_ERROR;
        }
        // TODO: try to recover from a case of non-blocking stream, e.g. move
        // remaining chunk to the beginning of buffer.
        assert(out_sz == self->alloc);
        self->len = 0;
    }

    return org_size;
}

STATIC mp_obj_t bufwriter_flush(mp_obj_t self_in) {
    mp_obj_bufwriter_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->len != 0) {
        int err;
        mp_uint_t out_sz = mp_stream_write_exactly(self->stream, self->buf, self->len, &err);
        // TODO: try to recover from a case of non-blocking stream, e.g. move
        // remaining chunk to the beginning of buffer.
        assert(out_sz == self->len);
        self->len = 0;
        if (err != 0) {
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(err)));
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bufwriter_flush_obj, bufwriter_flush);

STATIC const mp_map_elem_t bufwriter_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flush), (mp_obj_t)&bufwriter_flush_obj },
};
STATIC MP_DEFINE_CONST_DICT(bufwriter_locals_dict, bufwriter_locals_dict_table);

STATIC const mp_stream_p_t bufwriter_stream_p = {
    .write = bufwriter_write,
};

STATIC const mp_obj_type_t bufwriter_type = {
    { &mp_type_type },
    .name = MP_QSTR_BufferedWriter,
    .make_new = bufwriter_make_new,
    .protocol = &bufwriter_stream_p,
    .locals_dict = (mp_obj_t)&bufwriter_locals_dict,
};
#endif // MICROPY_PY_IO_BUFFEREDWRITER

STATIC const mp_rom_map_elem_t mp_module_io_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uio) },
    // Note: mp_builtin_open_obj should be defined by port, it's not
    // part of the core.
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },
    #if MICROPY_PY_IO_FILEIO
    { MP_ROM_QSTR(MP_QSTR_FileIO), MP_ROM_PTR(&mp_type_fileio) },
    #if MICROPY_CPYTHON_COMPAT
    { MP_ROM_QSTR(MP_QSTR_TextIOWrapper), MP_ROM_PTR(&mp_type_textio) },
    #endif
    #endif
    { MP_ROM_QSTR(MP_QSTR_StringIO), MP_ROM_PTR(&mp_type_stringio) },
    #if MICROPY_PY_IO_BYTESIO
    { MP_ROM_QSTR(MP_QSTR_BytesIO), MP_ROM_PTR(&mp_type_bytesio) },
    #endif
    #if MICROPY_PY_IO_BUFFEREDWRITER
    { MP_ROM_QSTR(MP_QSTR_BufferedWriter), MP_ROM_PTR(&bufwriter_type) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_io_globals, mp_module_io_globals_table);

const mp_obj_module_t mp_module_io = {
    .base = { &mp_type_module },
    .name = MP_QSTR_uio,
    .globals = (mp_obj_dict_t*)&mp_module_io_globals,
};

#endif
