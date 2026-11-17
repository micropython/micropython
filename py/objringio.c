/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Andrew Leech
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

#include "ringbuf.h"
#include "py/mpconfig.h"

#if MICROPY_PY_MICROPYTHON_RINGIO

#include "py/runtime.h"
#include "py/stream.h"

typedef struct _micropython_ringio_obj_t {
    mp_obj_base_t base;
    ringbuf_t ringbuffer;
} micropython_ringio_obj_t;

static mp_obj_t micropython_ringio_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_buffer_info_t bufinfo = {NULL, 0, 0};

    if (!mp_get_buffer(args[0], &bufinfo, MP_BUFFER_RW)) {
        bufinfo.len = mp_obj_get_int(args[0]) + 1;
        bufinfo.buf = m_new(uint8_t, bufinfo.len);
    }
    if (bufinfo.len < 2 || bufinfo.len > UINT16_MAX) {
        mp_raise_ValueError(NULL);
    }
    micropython_ringio_obj_t *self = mp_obj_malloc(micropython_ringio_obj_t, type);
    self->ringbuffer.buf = bufinfo.buf;
    self->ringbuffer.size = bufinfo.len;
    self->ringbuffer.iget = self->ringbuffer.iput = 0;
    return MP_OBJ_FROM_PTR(self);
}

static mp_uint_t micropython_ringio_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    micropython_ringio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size = MIN(size, ringbuf_avail(&self->ringbuffer));
    ringbuf_memcpy_get_internal(&(self->ringbuffer), buf_in, size);
    *errcode = 0;
    return size;
}

static mp_uint_t micropython_ringio_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    micropython_ringio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    size = MIN(size, ringbuf_free(&self->ringbuffer));
    ringbuf_memcpy_put_internal(&(self->ringbuffer), buf_in, size);
    *errcode = 0;
    return size;
}

static mp_uint_t micropython_ringio_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    micropython_ringio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    switch (request) {
        case MP_STREAM_POLL: {
            mp_uint_t ret = 0;
            if ((arg & MP_STREAM_POLL_RD) && ringbuf_avail(&self->ringbuffer) > 0) {
                ret |= MP_STREAM_POLL_RD;
            }
            if ((arg & MP_STREAM_POLL_WR) && ringbuf_free(&self->ringbuffer) > 0) {
                ret |= MP_STREAM_POLL_WR;
            }
            return ret;
        }
        case MP_STREAM_CLOSE:
            return 0;
    }
    *errcode = MP_EINVAL;
    return MP_STREAM_ERROR;
}

static mp_obj_t micropython_ringio_any(mp_obj_t self_in) {
    micropython_ringio_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(ringbuf_avail(&self->ringbuffer));
}
static MP_DEFINE_CONST_FUN_OBJ_1(micropython_ringio_any_obj, micropython_ringio_any);

static const mp_rom_map_elem_t micropython_ringio_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_any), MP_ROM_PTR(&micropython_ringio_any_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },

};
static MP_DEFINE_CONST_DICT(micropython_ringio_locals_dict, micropython_ringio_locals_dict_table);

static const mp_stream_p_t ringio_stream_p = {
    .read = micropython_ringio_read,
    .write = micropython_ringio_write,
    .ioctl = micropython_ringio_ioctl,
    .is_text = false,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_ringio,
    MP_QSTR_RingIO,
    MP_TYPE_FLAG_NONE,
    make_new, micropython_ringio_make_new,
    protocol, &ringio_stream_p,
    locals_dict, &micropython_ringio_locals_dict
    );

#endif // MICROPY_PY_MICROPYTHON_RINGIO
