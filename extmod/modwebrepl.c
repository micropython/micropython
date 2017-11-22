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
#include "py/builtin.h"
#ifdef MICROPY_PY_WEBREPL_DELAY
#include "py/mphal.h"
#endif
#include "extmod/modwebsocket.h"
#include "genhdr/mpversion.h"

#if MICROPY_PY_WEBREPL

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

struct webrepl_file {
    char sig[2];
    char type;
    char flags;
    uint64_t offset;
    uint32_t size;
    uint16_t fname_len;
    char fname[64];
} __attribute__((packed));

enum { PUT_FILE = 1, GET_FILE, GET_VER };
enum { STATE_PASSWD, STATE_NORMAL };

typedef struct _mp_obj_webrepl_t {
    mp_obj_base_t base;
    mp_obj_t sock;
    byte state;
    byte hdr_to_recv;
    uint32_t data_to_recv;
    struct webrepl_file hdr;
    mp_obj_t cur_file;
} mp_obj_webrepl_t;

// These get passed to functions which aren't force-l32, so can't be const
STATIC char passwd_prompt[] = "Password: ";
STATIC char connected_prompt[] = "\r\nWebREPL connected\r\n>>> ";
STATIC char denied_prompt[] = "\r\nAccess denied\r\n";

STATIC char webrepl_passwd[10];

STATIC void write_webrepl(mp_obj_t websock, const void *buf, size_t len) {
    const mp_stream_p_t *sock_stream = mp_get_stream_raise(websock, MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    int err;
    int old_opts = sock_stream->ioctl(websock, MP_STREAM_SET_DATA_OPTS, FRAME_BIN, &err);
    sock_stream->write(websock, buf, len, &err);
    sock_stream->ioctl(websock, MP_STREAM_SET_DATA_OPTS, old_opts, &err);
}

#define SSTR(s) s, sizeof(s) - 1
STATIC void write_webrepl_str(mp_obj_t websock, const char *str, int sz) {
    int err;
    const mp_stream_p_t *sock_stream = mp_get_stream_raise(websock, MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    sock_stream->write(websock, str, sz, &err);
}

STATIC void write_webrepl_resp(mp_obj_t websock, uint16_t code) {
    char buf[4] = {'W', 'B', code & 0xff, code >> 8};
    write_webrepl(websock, buf, sizeof(buf));
}

STATIC mp_obj_t webrepl_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);
    mp_get_stream_raise(args[0], MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    DEBUG_printf("sizeof(struct webrepl_file) = %lu\n", sizeof(struct webrepl_file));
    mp_obj_webrepl_t *o = m_new_obj(mp_obj_webrepl_t);
    o->base.type = type;
    o->sock = args[0];
    o->hdr_to_recv = sizeof(struct webrepl_file);
    o->data_to_recv = 0;
    o->state = STATE_PASSWD;
    write_webrepl_str(args[0], SSTR(passwd_prompt));
    return o;
}

STATIC int write_file_chunk(mp_obj_webrepl_t *self) {
    const mp_stream_p_t *file_stream =
        mp_get_stream_raise(self->cur_file, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);
    byte readbuf[2 + 256];
    int err;
    mp_uint_t out_sz = file_stream->read(self->cur_file, readbuf + 2, sizeof(readbuf) - 2, &err);
    if (out_sz == MP_STREAM_ERROR) {
        return out_sz;
    }
    readbuf[0] = out_sz;
    readbuf[1] = out_sz >> 8;
    DEBUG_printf("webrepl: Sending %d bytes of file\n", out_sz);
    write_webrepl(self->sock, readbuf, 2 + out_sz);
    return out_sz;
}

STATIC void handle_op(mp_obj_webrepl_t *self) {

    // Handle operations not requiring opened file

    switch (self->hdr.type) {
        case GET_VER: {
            static char ver[] = {MICROPY_VERSION_MAJOR, MICROPY_VERSION_MINOR, MICROPY_VERSION_MICRO};
            write_webrepl(self->sock, ver, sizeof(ver));
            self->hdr_to_recv = sizeof(struct webrepl_file);
            return;
        }
    }

    // Handle operations requiring opened file

    mp_obj_t open_args[2] = {
        mp_obj_new_str(self->hdr.fname, strlen(self->hdr.fname)),
        MP_OBJ_NEW_QSTR(MP_QSTR_rb)
    };

    if (self->hdr.type == PUT_FILE) {
        open_args[1] = MP_OBJ_NEW_QSTR(MP_QSTR_wb);
    }

    self->cur_file = mp_builtin_open(2, open_args, (mp_map_t*)&mp_const_empty_map);

    #if 0
    struct mp_stream_seek_t seek = { .offset = self->hdr.offset, .whence = 0 };
    int err;
    mp_uint_t res = file_stream->ioctl(self->cur_file, MP_STREAM_SEEK, (uintptr_t)&seek, &err);
    assert(res != MP_STREAM_ERROR);
    #endif

    write_webrepl_resp(self->sock, 0);

    if (self->hdr.type == PUT_FILE) {
        self->data_to_recv = self->hdr.size;
    } else if (self->hdr.type == GET_FILE) {
        self->data_to_recv = 1;
    }
}

STATIC mp_uint_t _webrepl_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode);

STATIC mp_uint_t webrepl_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    mp_uint_t out_sz;
    do {
        out_sz = _webrepl_read(self_in, buf, size, errcode);
    } while (out_sz == -2);
    return out_sz;
}

STATIC mp_uint_t _webrepl_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    // We know that os.dupterm always calls with size = 1
    assert(size == 1);
    mp_obj_webrepl_t *self = self_in;
    const mp_stream_p_t *sock_stream = mp_get_stream_raise(self->sock, MP_STREAM_OP_READ);
    mp_uint_t out_sz = sock_stream->read(self->sock, buf, size, errcode);
    //DEBUG_printf("webrepl: Read %d initial bytes from websocket\n", out_sz);
    if (out_sz == 0 || out_sz == MP_STREAM_ERROR) {
        return out_sz;
    }

    if (self->state == STATE_PASSWD) {
        char c = *(char*)buf;
        if (c == '\r' || c == '\n') {
            self->hdr.fname[self->data_to_recv] = 0;
            DEBUG_printf("webrepl: entered password: %s\n", self->hdr.fname);

            if (strcmp(self->hdr.fname, webrepl_passwd) != 0) {
                write_webrepl_str(self->sock, SSTR(denied_prompt));
                return 0;
            }

            self->state = STATE_NORMAL;
            self->data_to_recv = 0;
            write_webrepl_str(self->sock, SSTR(connected_prompt));
        } else if (self->data_to_recv < 10) {
            self->hdr.fname[self->data_to_recv++] = c;
        }
        return -2;
    }

    // If last read data belonged to text record (== REPL)
    int err;
    if (sock_stream->ioctl(self->sock, MP_STREAM_GET_DATA_OPTS, 0, &err) == 1) {
        return out_sz;
    }

    DEBUG_printf("webrepl: received bin data, hdr_to_recv: %d, data_to_recv=%d\n", self->hdr_to_recv, self->data_to_recv);

    if (self->hdr_to_recv != 0) {
        char *p = (char*)&self->hdr + sizeof(self->hdr) - self->hdr_to_recv;
        *p++ = *(char*)buf;
        if (--self->hdr_to_recv != 0) {
            mp_uint_t hdr_sz = sock_stream->read(self->sock, p, self->hdr_to_recv, errcode);
            if (hdr_sz == MP_STREAM_ERROR) {
                return hdr_sz;
            }
            self->hdr_to_recv -= hdr_sz;
            if (self->hdr_to_recv != 0) {
                return -2;
            }
        }

        DEBUG_printf("webrepl: op: %d, file: %s, chunk @%x, sz=%d\n", self->hdr.type, self->hdr.fname, (uint32_t)self->hdr.offset, self->hdr.size);

        handle_op(self);

        return -2;
    }

    if (self->data_to_recv != 0) {
        static byte filebuf[512];
        filebuf[0] = *(byte*)buf;
        mp_uint_t buf_sz = 1;
        if (--self->data_to_recv != 0) {
            size_t to_read = MIN(sizeof(filebuf) - 1, self->data_to_recv);
            mp_uint_t sz = sock_stream->read(self->sock, filebuf + 1, to_read, errcode);
            if (sz == MP_STREAM_ERROR) {
                return sz;
            }
            self->data_to_recv -= sz;
            buf_sz += sz;
        }

        if (self->hdr.type == PUT_FILE) {
            DEBUG_printf("webrepl: Writing %lu bytes to file\n", buf_sz);
            int err;
            mp_uint_t res = mp_stream_write_exactly(self->cur_file, filebuf, buf_sz, &err);
            if (err != 0 || res != buf_sz) {
                assert(0);
            }
        } else if (self->hdr.type == GET_FILE) {
            assert(buf_sz == 1);
            assert(self->data_to_recv == 0);
            assert(filebuf[0] == 0);
            mp_uint_t out_sz = write_file_chunk(self);
            if (out_sz != 0) {
                self->data_to_recv = 1;
            }
        }

        if (self->data_to_recv == 0) {
            mp_stream_close(self->cur_file);
            self->hdr_to_recv = sizeof(struct webrepl_file);
            DEBUG_printf("webrepl: Finished file operation %d\n", self->hdr.type);
            write_webrepl_resp(self->sock, 0);
        }

        #ifdef MICROPY_PY_WEBREPL_DELAY
        // Some platforms may have broken drivers and easily gets
        // overloaded with modest traffic WebREPL file transfers
        // generate. The basic workaround is a crude rate control
        // done in such way.
        mp_hal_delay_ms(MICROPY_PY_WEBREPL_DELAY);
        #endif
    }

    return -2;
}

STATIC mp_uint_t webrepl_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_webrepl_t *self = self_in;
    if (self->state == STATE_PASSWD) {
        // Don't forward output until passwd is entered
        return size;
    }
    const mp_stream_p_t *stream_p = mp_get_stream_raise(self->sock, MP_STREAM_OP_WRITE);
    return stream_p->write(self->sock, buf, size, errcode);
}

STATIC mp_obj_t webrepl_close(mp_obj_t self_in) {
    mp_obj_webrepl_t *self = MP_OBJ_TO_PTR(self_in);
    // TODO: This is a place to do cleanup
    return mp_stream_close(self->sock);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(webrepl_close_obj, webrepl_close);

STATIC mp_obj_t webrepl_set_password(mp_obj_t passwd_in) {
    size_t len;
    const char *passwd = mp_obj_str_get_data(passwd_in, &len);
    if (len > sizeof(webrepl_passwd) - 1) {
        mp_raise_ValueError(NULL);
    }
    strcpy(webrepl_passwd, passwd);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(webrepl_set_password_obj, webrepl_set_password);

STATIC const mp_rom_map_elem_t webrepl_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&webrepl_close_obj) },
};
STATIC MP_DEFINE_CONST_DICT(webrepl_locals_dict, webrepl_locals_dict_table);

STATIC const mp_stream_p_t webrepl_stream_p = {
    .read = webrepl_read,
    .write = webrepl_write,
};

STATIC const mp_obj_type_t webrepl_type = {
    { &mp_type_type },
    .name = MP_QSTR__webrepl,
    .make_new = webrepl_make_new,
    .protocol = &webrepl_stream_p,
    .locals_dict = (mp_obj_dict_t*)&webrepl_locals_dict,
};

STATIC const mp_rom_map_elem_t webrepl_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__webrepl) },
    { MP_ROM_QSTR(MP_QSTR__webrepl), MP_ROM_PTR(&webrepl_type) },
    { MP_ROM_QSTR(MP_QSTR_password), MP_ROM_PTR(&webrepl_set_password_obj) },
};

STATIC MP_DEFINE_CONST_DICT(webrepl_module_globals, webrepl_module_globals_table);

const mp_obj_module_t mp_module_webrepl = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&webrepl_module_globals,
};

#endif // MICROPY_PY_WEBREPL
