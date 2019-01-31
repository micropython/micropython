/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#include "py/binary.h"
#include "py/objarray.h"
#include "py/objstringio.h"
#include "py/frozenmod.h"

#if MICROPY_PY_IO

extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;

#if MICROPY_PY_IO_IOBASE

STATIC const mp_obj_type_t mp_type_iobase;

STATIC const mp_obj_base_t iobase_singleton = {&mp_type_iobase};

STATIC mp_obj_t iobase_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    (void)n_args;
    (void)n_kw;
    (void)args;
    return MP_OBJ_FROM_PTR(&iobase_singleton);
}

STATIC mp_uint_t iobase_read_write(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode, qstr qst) {
    mp_obj_t dest[3];
    mp_load_method(obj, qst, dest);
    mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, size, buf};
    dest[2] = MP_OBJ_FROM_PTR(&ar);
    mp_obj_t ret = mp_call_method_n_kw(1, 0, dest);
    if (ret == mp_const_none) {
        *errcode = MP_EAGAIN;
        return MP_STREAM_ERROR;
    } else {
        return mp_obj_get_int(ret);
    }
}
STATIC mp_uint_t iobase_read(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode) {
    return iobase_read_write(obj, buf, size, errcode, MP_QSTR_readinto);
}

STATIC mp_uint_t iobase_write(mp_obj_t obj, const void *buf, mp_uint_t size, int *errcode) {
    return iobase_read_write(obj, (void*)buf, size, errcode, MP_QSTR_write);
}

STATIC mp_uint_t iobase_ioctl(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_t dest[4];
    mp_load_method(obj, MP_QSTR_ioctl, dest);
    dest[2] = mp_obj_new_int_from_uint(request);
    dest[3] = mp_obj_new_int_from_uint(arg);
    mp_int_t ret = mp_obj_get_int(mp_call_method_n_kw(2, 0, dest));
    if (ret >= 0) {
        return ret;
    } else {
        *errcode = -ret;
        return MP_STREAM_ERROR;
    }
}

STATIC const mp_stream_p_t iobase_p = {
    .read = iobase_read,
    .write = iobase_write,
    .ioctl = iobase_ioctl,
};

STATIC const mp_obj_type_t mp_type_iobase = {
    { &mp_type_type },
    .name = MP_QSTR_IOBase,
    .make_new = iobase_make_new,
    .protocol = &iobase_p,
};

#endif // MICROPY_PY_IO_IOBASE

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
            mp_raise_OSError(err);
        }
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bufwriter_flush_obj, bufwriter_flush);

STATIC const mp_rom_map_elem_t bufwriter_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&bufwriter_flush_obj) },
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
    .locals_dict = (mp_obj_dict_t*)&bufwriter_locals_dict,
};
#endif // MICROPY_PY_IO_BUFFEREDWRITER

#if MICROPY_PY_IO_RESOURCE_STREAM
STATIC mp_obj_t resource_stream(mp_obj_t package_in, mp_obj_t path_in) {
    VSTR_FIXED(path_buf, MICROPY_ALLOC_PATH_MAX);
    size_t len;

    // As an extension to pkg_resources.resource_stream(), we support
    // package parameter being None, the path_in is interpreted as a
    // raw path.
    if (package_in != mp_const_none) {
        mp_obj_t args[5];
        args[0] = package_in;
        args[1] = mp_const_none; // TODO should be globals
        args[2] = mp_const_none; // TODO should be locals
        args[3] = mp_const_true; // Pass sentinel "non empty" value to force returning of leaf module
        args[4] = MP_OBJ_NEW_SMALL_INT(0);

        // TODO lookup __import__ and call that instead of going straight to builtin implementation
        mp_obj_t pkg = mp_builtin___import__(5, args);

        mp_obj_t dest[2];
        mp_load_method_maybe(pkg, MP_QSTR___path__, dest);
        if (dest[0] == MP_OBJ_NULL) {
            mp_raise_TypeError(NULL);
        }

        const char *path = mp_obj_str_get_data(dest[0], &len);
        vstr_add_strn(&path_buf, path, len);
        vstr_add_byte(&path_buf, '/');
    }

    const char *path = mp_obj_str_get_data(path_in, &len);
    vstr_add_strn(&path_buf, path, len);

    len = path_buf.len;
    const char *data = mp_find_frozen_str(path_buf.buf, &len);
    if (data != NULL) {
        mp_obj_stringio_t *o = m_new_obj(mp_obj_stringio_t);
        o->base.type = &mp_type_bytesio;
        o->vstr = m_new_obj(vstr_t);
        vstr_init_fixed_buf(o->vstr, len + 1, (char*)data);
        o->vstr->len = len;
        o->pos = 0;
        return MP_OBJ_FROM_PTR(o);
    }

    mp_obj_t path_out = mp_obj_new_str(path_buf.buf, path_buf.len);
    return mp_builtin_open(1, &path_out, (mp_map_t*)&mp_const_empty_map);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(resource_stream_obj, resource_stream);
#endif

STATIC const mp_rom_map_elem_t mp_module_io_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uio) },
    // Note: mp_builtin_open_obj should be defined by port, it's not
    // part of the core.
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },
    #if MICROPY_PY_IO_IOBASE
    { MP_ROM_QSTR(MP_QSTR_IOBase), MP_ROM_PTR(&mp_type_iobase) },
    #endif
    #if MICROPY_PY_IO_RESOURCE_STREAM
    { MP_ROM_QSTR(MP_QSTR_resource_stream), MP_ROM_PTR(&resource_stream_obj) },
    #endif
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
    .globals = (mp_obj_dict_t*)&mp_module_io_globals,
};

#endif
