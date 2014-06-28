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

#include <string.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objstr.h"
#include "runtime.h"
#include "stream.h"
#if MICROPY_STREAMS_NON_BLOCK
#include <errno.h>
#endif

// This file defines generic Python stream read/write methods which
// dispatch to the underlying stream interface of an object.

// TODO: should be in mpconfig.h
#define DEFAULT_BUFFER_SIZE 256

STATIC mp_obj_t stream_readall(mp_obj_t self_in);

#if MICROPY_STREAMS_NON_BLOCK
// TODO: This is POSIX-specific (but then POSIX is the only real thing,
// and anything else just emulates it, right?)
#define is_nonblocking_error(errno) ((errno) == EAGAIN || (errno) == EWOULDBLOCK)
#else
#define is_nonblocking_error(errno) (0)
#endif

#define STREAM_CONTENT_TYPE(stream) (((stream)->is_bytes) ? &mp_type_bytes : &mp_type_str)

STATIC mp_obj_t stream_read(uint n_args, const mp_obj_t *args) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)args[0];
    if (o->type->stream_p == NULL || o->type->stream_p->read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    machine_int_t sz;
    if (n_args == 1 || ((sz = mp_obj_get_int(args[1])) == -1)) {
        return stream_readall(args[0]);
    }

    #if MICROPY_PY_BUILTINS_STR_UNICODE
    if (!o->type->stream_p->is_bytes) {
        mp_not_implemented("Reading from unicode text streams by character count");
    }
    #endif

    byte *buf = m_new(byte, sz);
    int error;
    machine_int_t out_sz = o->type->stream_p->read(o, buf, sz, &error);
    if (out_sz == -1) {
        if (is_nonblocking_error(error)) {
            // https://docs.python.org/3.4/library/io.html#io.RawIOBase.read
            // "If the object is in non-blocking mode and no bytes are available,
            // None is returned."
            // This is actually very weird, as naive truth check will treat
            // this as EOF.
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
    } else {
        mp_obj_t s = mp_obj_new_str_of_type(STREAM_CONTENT_TYPE(o->type->stream_p), buf, out_sz); // will reallocate to use exact size
        m_free(buf, sz);
        return s;
    }
}

STATIC mp_obj_t stream_write(mp_obj_t self_in, mp_obj_t arg) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p == NULL || o->type->stream_p->write == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);

    int error;
    machine_int_t out_sz = o->type->stream_p->write(self_in, bufinfo.buf, bufinfo.len, &error);
    if (out_sz == -1) {
        if (is_nonblocking_error(error)) {
            // http://docs.python.org/3/library/io.html#io.RawIOBase.write
            // "None is returned if the raw stream is set not to block and
            // no single byte could be readily written to it."
            // This is for consistency with read() behavior, still weird,
            // see abobe.
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
    } else {
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

STATIC mp_obj_t stream_readall(mp_obj_t self_in) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p == NULL || o->type->stream_p->read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    int total_size = 0;
    vstr_t *vstr = vstr_new_size(DEFAULT_BUFFER_SIZE);
    char *buf = vstr_str(vstr);
    char *p = buf;
    int error;
    int current_read = DEFAULT_BUFFER_SIZE;
    while (true) {
        machine_int_t out_sz = o->type->stream_p->read(self_in, p, current_read, &error);
        if (out_sz == -1) {
            if (is_nonblocking_error(error)) {
                // With non-blocking streams, we read as much as we can.
                // If we read nothing, return None, just like read().
                // Otherwise, return data read so far.
                if (total_size == 0) {
                    return mp_const_none;
                }
                break;
            }
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
        }
        if (out_sz == 0) {
            break;
        }
        total_size += out_sz;
        if (out_sz < current_read) {
            current_read -= out_sz;
            p += out_sz;
        } else {
            current_read = DEFAULT_BUFFER_SIZE;
            p = vstr_extend(vstr, current_read);
            if (p == NULL) {
                // TODO
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError/*&mp_type_RuntimeError*/, "Out of memory"));
            }
        }
    }

    mp_obj_t s = mp_obj_new_str_of_type(STREAM_CONTENT_TYPE(o->type->stream_p), (byte*)vstr->buf, total_size);
    vstr_free(vstr);
    return s;
}

// Unbuffered, inefficient implementation of readline() for raw I/O files.
STATIC mp_obj_t stream_unbuffered_readline(uint n_args, const mp_obj_t *args) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)args[0];
    if (o->type->stream_p == NULL || o->type->stream_p->read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    machine_int_t max_size = -1;
    if (n_args > 1) {
        max_size = MP_OBJ_SMALL_INT_VALUE(args[1]);
    }

    vstr_t *vstr;
    if (max_size != -1) {
        vstr = vstr_new_size(max_size);
    } else {
        vstr = vstr_new();
    }

    int error;
    while (max_size == -1 || max_size-- != 0) {
        char *p = vstr_add_len(vstr, 1);
        if (p == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "out of memory"));
        }

        machine_int_t out_sz = o->type->stream_p->read(o, p, 1, &error);
        if (out_sz == -1) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
        }
        if (out_sz == 0) {
            // Back out previously added byte
            // TODO: This is a bit hacky, does it supported by vstr API contract?
            // Consider, what's better - read a char and get OutOfMemory (so read
            // char is lost), or allocate first as we do.
            vstr_add_len(vstr, -1);
            break;
        }
        if (*p == '\n') {
            break;
        }
    }
    // TODO need a string creation API that doesn't copy the given data
    mp_obj_t ret = mp_obj_new_str_of_type(STREAM_CONTENT_TYPE(o->type->stream_p), (byte*)vstr->buf, vstr->len);
    vstr_free(vstr);
    return ret;
}

// TODO take an optional extra argument (what does it do exactly?)
STATIC mp_obj_t stream_unbuffered_readlines(mp_obj_t self) {
    mp_obj_t lines = mp_obj_new_list(0, NULL);
    for (;;) {
        mp_obj_t line = stream_unbuffered_readline(1, &self);
        if (!mp_obj_is_true(line)) {
            break;
        }
        mp_obj_list_append(lines, line);
    }
    return lines;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_unbuffered_readlines_obj, stream_unbuffered_readlines);

mp_obj_t mp_stream_unbuffered_iter(mp_obj_t self) {
    mp_obj_t l_in = stream_unbuffered_readline(1, &self);
    if (mp_obj_is_true(l_in)) {
        return l_in;
    }
    return MP_OBJ_STOP_ITERATION;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read_obj, 1, 2, stream_read);
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_readall_obj, stream_readall);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_unbuffered_readline_obj, 1, 2, stream_unbuffered_readline);
MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_write_obj, stream_write);
