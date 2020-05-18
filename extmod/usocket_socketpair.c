/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"

#if MICROPY_PY_USOCKET_SOCKETPAIR

typedef struct _mp_obj_socketpair_t {
    mp_obj_base_t base;
    bool blocking;
    volatile size_t avail;
    uint8_t buf[1];
} mp_obj_socketpair_t;

STATIC mp_uint_t socketpair_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_socketpair_t *self = MP_OBJ_TO_PTR(self_in);
    switch (request) {
        case MP_STREAM_POLL: {
            uintptr_t flags = arg;
            if (self->avail) {
                return MP_STREAM_POLL_RD & flags;
            } else {
                return MP_STREAM_POLL_WR & flags;
            }
        }
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

STATIC mp_obj_t socketpair_recv_into(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_obj_socketpair_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len == 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    if (!self->avail) {
        if (!self->blocking) {
            mp_raise_OSError(MP_EAGAIN);
        }
        while (!self->avail) {
            MICROPY_EVENT_POLL_HOOK;
        }
    }

    ((uint8_t *)bufinfo.buf)[0] = self->buf[0];
    self->avail = 0;

    return MP_OBJ_NEW_SMALL_INT(1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpair_recv_into_obj, socketpair_recv_into);

STATIC mp_obj_t socketpair_send(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_obj_socketpair_t *self = MP_OBJ_TO_PTR(self_in);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len == 0) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    if (self->avail) {
        return MP_OBJ_NEW_SMALL_INT(0);
    }

    self->buf[0] = ((uint8_t *)bufinfo.buf)[0];
    self->avail = 1;

    return MP_OBJ_NEW_SMALL_INT(1);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpair_send_obj, socketpair_send);

STATIC mp_obj_t socketpair_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    mp_obj_socketpair_t *self = MP_OBJ_TO_PTR(self_in);
    self->blocking = mp_obj_is_true(flag_in);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(socketpair_setblocking_obj, socketpair_setblocking);

STATIC const mp_rom_map_elem_t socketpair_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_recv_into), MP_ROM_PTR(&socketpair_recv_into_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&socketpair_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&socketpair_setblocking_obj) },
};
STATIC MP_DEFINE_CONST_DICT(socketpair_locals_dict, socketpair_locals_dict_table);

STATIC const mp_stream_p_t socketpair_stream_p = {
    .ioctl = socketpair_ioctl,
};

STATIC const mp_obj_type_t mp_type_socketpair = {
    { &mp_type_type },
    .name = MP_QSTR_socketpair,
    .protocol = &socketpair_stream_p,
    .locals_dict = (mp_obj_dict_t *)&socketpair_locals_dict,
};

STATIC mp_obj_t socketpair_new(void) {
    mp_obj_socketpair_t *self = m_new_obj(mp_obj_socketpair_t);
    self->base.type = &mp_type_socketpair;
    self->blocking = true;
    self->avail = 0;
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t mod_socket_socketpair(void) {
    mp_obj_t s = socketpair_new();
    mp_obj_t tuple[2] = { s, s };
    return mp_obj_new_tuple(2, tuple);
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_socket_socketpair_obj, mod_socket_socketpair);

#endif // MICROPY_PY_USOCKET_SOCKETPAIR
