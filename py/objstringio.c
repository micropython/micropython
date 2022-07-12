/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014-2017 Paul Sokolovsky
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

#include "py/objstr.h"
#include "py/objstringio.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_IO

#if MICROPY_CPYTHON_COMPAT
STATIC void check_stringio_is_open(const mp_obj_stringio_t *o) {
    if (o->vstr == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}
#else
#define check_stringio_is_open(o)
#endif

STATIC void stringio_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_stringio_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, self->base.type == &mp_type_stringio ? "<io.StringIO 0x%x>" : "<io.BytesIO 0x%x>", self);
}

STATIC mp_uint_t stringio_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    (void)errcode;
    mp_obj_stringio_t *o = MP_OBJ_TO_PTR(o_in);
    check_stringio_is_open(o);
    if (o->vstr->len <= o->pos) {  // read to EOF, or seeked to EOF or beyond
        return 0;
    }
    mp_uint_t remaining = o->vstr->len - o->pos;
    if (size > remaining) {
        size = remaining;
    }
    memcpy(buf, o->vstr->buf + o->pos, size);
    o->pos += size;
    return size;
}

STATIC void stringio_copy_on_write(mp_obj_stringio_t *o) {
    const void *buf = o->vstr->buf;
    o->vstr->buf = m_new(char, o->vstr->len);
    o->vstr->fixed_buf = false;
    o->ref_obj = MP_OBJ_NULL;
    memcpy(o->vstr->buf, buf, o->vstr->len);
}

STATIC mp_uint_t stringio_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    (void)errcode;
    mp_obj_stringio_t *o = MP_OBJ_TO_PTR(o_in);
    check_stringio_is_open(o);

    if (o->vstr->fixed_buf) {
        stringio_copy_on_write(o);
    }

    mp_uint_t new_pos = o->pos + size;
    if (new_pos < size) {
        // Writing <size> bytes will overflow o->pos beyond limit of mp_uint_t.
        *errcode = MP_EFBIG;
        return MP_STREAM_ERROR;
    }
    mp_uint_t org_len = o->vstr->len;
    if (new_pos > o->vstr->alloc) {
        // Take all what's already allocated...
        o->vstr->len = o->vstr->alloc;
        // ... and add more
        vstr_add_len(o->vstr, new_pos - o->vstr->alloc);
    }
    // If there was a seek past EOF, clear the hole
    if (o->pos > org_len) {
        memset(o->vstr->buf + org_len, 0, o->pos - org_len);
    }
    memcpy(o->vstr->buf + o->pos, buf, size);
    o->pos = new_pos;
    if (new_pos > o->vstr->len) {
        o->vstr->len = new_pos;
    }
    return size;
}

STATIC mp_uint_t stringio_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    (void)errcode;
    mp_obj_stringio_t *o = MP_OBJ_TO_PTR(o_in);
    switch (request) {
        case MP_STREAM_SEEK: {
            struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)arg;
            mp_uint_t ref = 0;
            switch (s->whence) {
                case MP_SEEK_CUR:
                    ref = o->pos;
                    break;
                case MP_SEEK_END:
                    ref = o->vstr->len;
                    break;
            }
            mp_uint_t new_pos = ref + s->offset;

            // For MP_SEEK_SET, offset is unsigned
            if (s->whence != MP_SEEK_SET && s->offset < 0) {
                if (new_pos > ref) {
                    // Negative offset from SEEK_CUR or SEEK_END went past 0.
                    // CPython sets position to 0, POSIX returns an EINVAL error
                    new_pos = 0;
                }
            } else if (new_pos < ref) {
                // positive offset went beyond the limit of mp_uint_t
                *errcode = MP_EINVAL;  // replace with MP_EOVERFLOW when defined
                return MP_STREAM_ERROR;
            }
            s->offset = o->pos = new_pos;
            return 0;
        }
        case MP_STREAM_FLUSH:
            return 0;
        case MP_STREAM_CLOSE:
            #if MICROPY_CPYTHON_COMPAT
            vstr_free(o->vstr);
            o->vstr = NULL;
            #else
            vstr_clear(o->vstr);
            o->vstr->alloc = 0;
            o->vstr->len = 0;
            o->pos = 0;
            #endif
            return 0;
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

#define STREAM_TO_CONTENT_TYPE(o) (((o)->base.type == &mp_type_stringio) ? &mp_type_str : &mp_type_bytes)

STATIC mp_obj_t stringio_getvalue(mp_obj_t self_in) {
    mp_obj_stringio_t *self = MP_OBJ_TO_PTR(self_in);
    check_stringio_is_open(self);
    // TODO: Try to avoid copying string
    return mp_obj_new_str_of_type(STREAM_TO_CONTENT_TYPE(self), (byte *)self->vstr->buf, self->vstr->len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(stringio_getvalue_obj, stringio_getvalue);

STATIC mp_obj_t stringio___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stringio___exit___obj, 4, 4, stringio___exit__);

STATIC mp_obj_stringio_t *stringio_new(const mp_obj_type_t *type) {
    mp_obj_stringio_t *o = mp_obj_malloc(mp_obj_stringio_t, type);
    o->pos = 0;
    o->ref_obj = MP_OBJ_NULL;
    return o;
}

STATIC mp_obj_t stringio_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)n_kw; // TODO check n_kw==0

    mp_uint_t sz = 16;
    bool initdata = false;
    mp_buffer_info_t bufinfo;

    mp_obj_stringio_t *o = stringio_new(type_in);

    if (n_args > 0) {
        if (mp_obj_is_int(args[0])) {
            sz = mp_obj_get_int(args[0]);
        } else {
            mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);

            if (mp_obj_is_str_or_bytes(args[0])) {
                o->vstr = m_new_obj(vstr_t);
                vstr_init_fixed_buf(o->vstr, bufinfo.len, bufinfo.buf);
                o->vstr->len = bufinfo.len;
                o->ref_obj = args[0];
                return MP_OBJ_FROM_PTR(o);
            }

            sz = bufinfo.len;
            initdata = true;
        }
    }

    o->vstr = vstr_new(sz);

    if (initdata) {
        stringio_write(MP_OBJ_FROM_PTR(o), bufinfo.buf, bufinfo.len, NULL);
        // Cur ptr is always at the beginning of buffer at the construction
        o->pos = 0;
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_rom_map_elem_t stringio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_getvalue), MP_ROM_PTR(&stringio_getvalue_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&stringio___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(stringio_locals_dict, stringio_locals_dict_table);

STATIC const mp_stream_p_t stringio_stream_p = {
    .read = stringio_read,
    .write = stringio_write,
    .ioctl = stringio_ioctl,
    .is_text = true,
};

const mp_obj_type_t mp_type_stringio = {
    { &mp_type_type },
    .name = MP_QSTR_StringIO,
    .print = stringio_print,
    .make_new = stringio_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &stringio_stream_p,
    .locals_dict = (mp_obj_dict_t *)&stringio_locals_dict,
};

#if MICROPY_PY_IO_BYTESIO
STATIC const mp_stream_p_t bytesio_stream_p = {
    .read = stringio_read,
    .write = stringio_write,
    .ioctl = stringio_ioctl,
};

const mp_obj_type_t mp_type_bytesio = {
    { &mp_type_type },
    .name = MP_QSTR_BytesIO,
    .print = stringio_print,
    .make_new = stringio_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &bytesio_stream_p,
    .locals_dict = (mp_obj_dict_t *)&stringio_locals_dict,
};
#endif

#endif
