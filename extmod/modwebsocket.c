/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
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
#include <stdint.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/stream.h"

#if MICROPY_PY_WEBSOCKET

enum { FRAME_HEADER, FRAME_OPT, PAYLOAD };

typedef struct _mp_obj_websocket_t {
    mp_obj_base_t base;
    mp_obj_t sock;
    uint32_t mask;
    byte state;
    byte to_recv;
    byte mask_pos;
    byte buf[4];
} mp_obj_websocket_t;

STATIC mp_obj_t websocket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    assert(n_args == 1);
    mp_obj_websocket_t *o = m_new_obj(mp_obj_websocket_t);
    o->base.type = type;
    o->sock = args[0];
    o->state = FRAME_HEADER;
    o->to_recv = 2;
    o->mask_pos = 0;
    return o;
}

STATIC mp_uint_t websocket_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_websocket_t *self = self_in;
    assert(size < 126);
    byte header[] = {0x81, size};

    mp_uint_t out_sz = mp_stream_writeall(self->sock, header, sizeof(header), errcode);
    if (out_sz == MP_STREAM_ERROR) {
        return MP_STREAM_ERROR;
    }
    return mp_stream_writeall(self->sock, buf, size, errcode);
}

STATIC const mp_map_elem_t websocket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
};
STATIC MP_DEFINE_CONST_DICT(websocket_locals_dict, websocket_locals_dict_table);

STATIC const mp_stream_p_t websocket_stream_p = {
//    .read = websocket_read,
    .write = websocket_write,
};

STATIC const mp_obj_type_t websocket_type = {
    { &mp_type_type },
    .name = MP_QSTR_websocket,
    .make_new = websocket_make_new,
    .stream_p = &websocket_stream_p,
    .locals_dict = (mp_obj_t)&websocket_locals_dict,
};

STATIC const mp_map_elem_t websocket_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_websocket) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_websocket), (mp_obj_t)&websocket_type },
};

STATIC MP_DEFINE_CONST_DICT(websocket_module_globals, websocket_module_globals_table);

const mp_obj_module_t mp_module_websocket = {
    .base = { &mp_type_module },
    .name = MP_QSTR_websocket,
    .globals = (mp_obj_dict_t*)&websocket_module_globals,
};

#endif // MICROPY_PY_WEBSOCKET
