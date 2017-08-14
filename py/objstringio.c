/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include "py/nlr.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"

#if MICROPY_PY_IO

typedef struct _mp_obj_stringio_t {
    mp_obj_base_t base;
    vstr_t *vstr;
    // StringIO has single pointer used for both reading and writing
    mp_uint_t pos;
} mp_obj_stringio_t;

#if MICROPY_CPYTHON_COMPAT
STATIC void check_stringio_is_open(const mp_obj_stringio_t *o) {
    if (o->vstr == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "I/O operation on closed file"));
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
    mp_uint_t remaining = o->vstr->len - o->pos;
    if (size > remaining) {
        size = remaining;
    }
    memcpy(buf, o->vstr->buf + o->pos, size);
    o->pos += size;
    return size;
}

STATIC mp_uint_t stringio_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    (void)errcode;
    mp_obj_stringio_t *o = MP_OBJ_TO_PTR(o_in);
    check_stringio_is_open(o);
    mp_uint_t remaining = o->vstr->alloc - o->pos;
    if (size > remaining) {
        // Take all what's already allocated...
        o->vstr->len = o->vstr->alloc;
        // ... and add more
        vstr_add_len(o->vstr, size - remaining);
    }
    memcpy(o->vstr->buf + o->pos, buf, size);
    o->pos += size;
    if (o->pos > o->vstr->len) {
        o->vstr->len = o->pos;
    }
    return size;
}

#define STREAM_TO_CONTENT_TYPE(o) (((o)->base.type == &mp_type_stringio) ? &mp_type_str : &mp_type_bytes)

STATIC mp_obj_t stringio_getvalue(mp_obj_t self_in) {
    mp_obj_stringio_t *self = MP_OBJ_TO_PTR(self_in);
    check_stringio_is_open(self);
    // TODO: Try to avoid copying string
    return mp_obj_new_str_of_type(STREAM_TO_CONTENT_TYPE(self), (byte*)self->vstr->buf, self->vstr->len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(stringio_getvalue_obj, stringio_getvalue);

STATIC mp_obj_t stringio_close(mp_obj_t self_in) {
    mp_obj_stringio_t *self = MP_OBJ_TO_PTR(self_in);
#if MICROPY_CPYTHON_COMPAT
    vstr_free(self->vstr);
    self->vstr = NULL;
#else
    vstr_clear(self->vstr);
    self->vstr->alloc = 0;
    self->vstr->len = 0;
    self->pos = 0;
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(stringio_close_obj, stringio_close);

STATIC mp_obj_t stringio___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return stringio_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(stringio___exit___obj, 4, 4, stringio___exit__);

STATIC mp_obj_stringio_t *stringio_new(const mp_obj_type_t *type) {
    mp_obj_stringio_t *o = m_new_obj(mp_obj_stringio_t);
    o->base.type = type;
    o->vstr = vstr_new();
    o->pos = 0;
    return o;
}

STATIC mp_obj_t stringio_make_new(const mp_obj_type_t *type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)n_kw; // TODO check n_kw==0
    mp_obj_stringio_t *o = stringio_new(type_in);

    if (n_args > 0) {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_READ);
        stringio_write(MP_OBJ_FROM_PTR(o), bufinfo.buf, bufinfo.len, NULL);
        // Cur ptr is always at the beginning of buffer at the construction
        o->pos = 0;
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC const mp_rom_map_elem_t stringio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readall), MP_ROM_PTR(&mp_stream_readall_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&stringio_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_getvalue), MP_ROM_PTR(&stringio_getvalue_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&stringio___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(stringio_locals_dict, stringio_locals_dict_table);

STATIC const mp_stream_p_t stringio_stream_p = {
    .read = stringio_read,
    .write = stringio_write,
    .is_text = true,
};

STATIC const mp_stream_p_t bytesio_stream_p = {
    .read = stringio_read,
    .write = stringio_write,
};

const mp_obj_type_t mp_type_stringio = {
    { &mp_type_type },
    .name = MP_QSTR_StringIO,
    .print = stringio_print,
    .make_new = stringio_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &stringio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&stringio_locals_dict,
};

#if MICROPY_PY_IO_BYTESIO
const mp_obj_type_t mp_type_bytesio = {
    { &mp_type_type },
    .name = MP_QSTR_BytesIO,
    .print = stringio_print,
    .make_new = stringio_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &bytesio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&stringio_locals_dict,
};
#endif

#endif
