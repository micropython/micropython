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
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "extmod/moduwebsocket.h"

#if MICROPY_PY_UWEBSOCKET

enum { FRAME_HEADER, FRAME_OPT, PAYLOAD, CONTROL };

enum { BLOCKING_WRITE = 0x80 };

typedef struct _mp_obj_websocket_t {
    mp_obj_base_t base;
    mp_obj_t sock;
    uint32_t msg_sz;
    byte mask[4];
    byte state;
    byte to_recv;
    byte mask_pos;
    byte buf_pos;
    byte buf[6];
    byte opts;
    // Copy of last data frame flags
    byte ws_flags;
    // Copy of current frame flags
    byte last_flags;
} mp_obj_websocket_t;

STATIC mp_uint_t websocket_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode);

STATIC mp_obj_t websocket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    mp_obj_websocket_t *o = m_new_obj(mp_obj_websocket_t);
    o->base.type = type;
    o->sock = args[0];
    o->state = FRAME_HEADER;
    o->to_recv = 2;
    o->mask_pos = 0;
    o->buf_pos = 0;
    o->opts = FRAME_TXT;
    if (n_args > 1 && args[1] == mp_const_true) {
        o->opts |= BLOCKING_WRITE;
    }
    return  MP_OBJ_FROM_PTR(o);
}

STATIC mp_uint_t websocket_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_websocket_t *self =  MP_OBJ_TO_PTR(self_in);
    const mp_stream_p_t *stream_p = mp_get_stream(self->sock);
    while (1) {
        if (self->to_recv != 0) {
            mp_uint_t out_sz = stream_p->read(self->sock, self->buf + self->buf_pos, self->to_recv, errcode);
            if (out_sz == 0 || out_sz == MP_STREAM_ERROR) {
                return out_sz;
            }
            self->buf_pos += out_sz;
            self->to_recv -= out_sz;
            if (self->to_recv != 0) {
                *errcode = MP_EAGAIN;
                return MP_STREAM_ERROR;
            }
        }

        switch (self->state) {
            case FRAME_HEADER: {
                // TODO: Split frame handling below is untested so far, so conservatively disable it
                assert(self->buf[0] & 0x80);

                // "Control frames MAY be injected in the middle of a fragmented message."
                // So, they must be processed before data frames (and not alter
                // self->ws_flags)
                byte frame_type = self->buf[0];
                self->last_flags = frame_type;
                frame_type &= FRAME_OPCODE_MASK;

                if ((self->buf[0] & FRAME_OPCODE_MASK) == FRAME_CONT) {
                    // Preserve previous frame type
                    self->ws_flags = (self->ws_flags & FRAME_OPCODE_MASK) | (self->buf[0] & ~FRAME_OPCODE_MASK);
                } else {
                    self->ws_flags = self->buf[0];
                }

                // Reset mask in case someone will use "simplified" protocol
                // without masks.
                memset(self->mask, 0, sizeof(self->mask));

                int to_recv = 0;
                size_t sz = self->buf[1] & 0x7f;
                if (sz == 126) {
                    // Msg size is next 2 bytes
                    to_recv += 2;
                } else if (sz == 127) {
                    // Msg size is next 8 bytes
                    assert(0);
                }
                if (self->buf[1] & 0x80) {
                    // Next 4 bytes is mask
                    to_recv += 4;
                }

                self->buf_pos = 0;
                self->to_recv = to_recv;
                self->msg_sz = sz; // May be overridden by FRAME_OPT
                if (to_recv != 0) {
                    self->state = FRAME_OPT;
                } else {
                    if (frame_type >= FRAME_CLOSE) {
                        self->state = CONTROL;
                    } else {
                        self->state = PAYLOAD;
                    }
                }
                continue;
            }

            case FRAME_OPT: {
                if ((self->buf_pos & 3) == 2) {
                    // First two bytes are message length
                    self->msg_sz = (self->buf[0] << 8) | self->buf[1];
                }
                if (self->buf_pos >= 4) {
                    // Last 4 bytes is mask
                    memcpy(self->mask, self->buf + self->buf_pos - 4, 4);
                }
                self->buf_pos = 0;
                if ((self->last_flags & FRAME_OPCODE_MASK) >= FRAME_CLOSE) {
                    self->state = CONTROL;
                } else {
                    self->state = PAYLOAD;
                }
                continue;
            }

            case PAYLOAD:
            case CONTROL: {
                mp_uint_t out_sz = 0;
                if (self->msg_sz == 0) {
                    // In case message had zero payload
                    goto no_payload;
                }

                size_t sz = MIN(size, self->msg_sz);
                out_sz = stream_p->read(self->sock, buf, sz, errcode);
                if (out_sz == 0 || out_sz == MP_STREAM_ERROR) {
                    return out_sz;
                }

                sz = out_sz;
                for (byte *p = buf; sz--; p++) {
                    *p ^= self->mask[self->mask_pos++ & 3];
                }

                self->msg_sz -= out_sz;
                if (self->msg_sz == 0) {
                    byte last_state;
no_payload:
                    last_state = self->state;
                    self->state = FRAME_HEADER;
                    self->to_recv = 2;
                    self->mask_pos = 0;
                    self->buf_pos = 0;

                    // Handle control frame
                    if (last_state == CONTROL) {
                        byte frame_type = self->last_flags & FRAME_OPCODE_MASK;
                        if (frame_type == FRAME_CLOSE) {
                            static const char close_resp[2] = {0x88, 0};
                            int err;
                            websocket_write(self_in, close_resp, sizeof(close_resp), &err);
                            return 0;
                        }

                        //DEBUG_printf("Finished receiving ctrl message %x, ignoring\n", self->last_flags);
                        continue;
                    }
                }

                if (out_sz != 0) {
                    return out_sz;
                }
                // Empty (data) frame received is not EOF
                continue;
            }

        }
    }
}

STATIC mp_uint_t websocket_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_websocket_t *self =  MP_OBJ_TO_PTR(self_in);
    assert(size < 0x10000);
    byte header[4] = {0x80 | (self->opts & FRAME_OPCODE_MASK)};
    int hdr_sz;
    if (size < 126) {
        header[1] = size;
        hdr_sz = 2;
    } else {
        header[1] = 126;
        header[2] = size >> 8;
        header[3] = size & 0xff;
        hdr_sz = 4;
    }

    mp_obj_t dest[3];
    if (self->opts & BLOCKING_WRITE) {
        mp_load_method(self->sock, MP_QSTR_setblocking, dest);
        dest[2] = mp_const_true;
        mp_call_method_n_kw(1, 0, dest);
    }

    mp_uint_t out_sz = mp_stream_write_exactly(self->sock, header, hdr_sz, errcode);
    if (*errcode == 0) {
        out_sz = mp_stream_write_exactly(self->sock, buf, size, errcode);
    }

    if (self->opts & BLOCKING_WRITE) {
        dest[2] = mp_const_false;
        mp_call_method_n_kw(1, 0, dest);
    }

    if (*errcode != 0) {
        return MP_STREAM_ERROR;
    }
    return out_sz;
}

STATIC mp_uint_t websocket_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_websocket_t *self = MP_OBJ_TO_PTR(self_in);
    switch (request) {
        case MP_STREAM_CLOSE:
            // TODO: Send close signaling to the other side, otherwise it's
            // abrupt close (connection abort).
            mp_stream_close(self->sock);
            return 0;
        case MP_STREAM_GET_DATA_OPTS:
            return self->ws_flags & FRAME_OPCODE_MASK;
        case MP_STREAM_SET_DATA_OPTS: {
            int cur = self->opts & FRAME_OPCODE_MASK;
            self->opts = (self->opts & ~FRAME_OPCODE_MASK) | (arg & FRAME_OPCODE_MASK);
            return cur;
        }
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

STATIC const mp_rom_map_elem_t websocket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&mp_stream_ioctl_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
};
STATIC MP_DEFINE_CONST_DICT(websocket_locals_dict, websocket_locals_dict_table);

STATIC const mp_stream_p_t websocket_stream_p = {
    .read = websocket_read,
    .write = websocket_write,
    .ioctl = websocket_ioctl,
};

STATIC const mp_obj_type_t websocket_type = {
    { &mp_type_type },
    .name = MP_QSTR_websocket,
    .make_new = websocket_make_new,
    .protocol = &websocket_stream_p,
    .locals_dict = (void*)&websocket_locals_dict,
};

STATIC const mp_rom_map_elem_t uwebsocket_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uwebsocket) },
    { MP_ROM_QSTR(MP_QSTR_websocket), MP_ROM_PTR(&websocket_type) },
};

STATIC MP_DEFINE_CONST_DICT(uwebsocket_module_globals, uwebsocket_module_globals_table);

const mp_obj_module_t mp_module_uwebsocket = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&uwebsocket_module_globals,
};

#endif // MICROPY_PY_UWEBSOCKET
