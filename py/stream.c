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
#if defined(__MINGW32__) && !defined(__MINGW64_VERSION_MAJOR)
#define EWOULDBLOCK 140
#endif
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

#define STREAM_CONTENT_TYPE(stream) (((stream)->is_text) ? &mp_type_str : &mp_type_bytes)

STATIC mp_obj_t stream_read(uint n_args, const mp_obj_t *args) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)args[0];
    if (o->type->stream_p == NULL || o->type->stream_p->read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    // What to do if sz < -1?  Python docs don't specify this case.
    // CPython does a readall, but here we silently let negatives through,
    // and they will cause a MemoryError.
    mp_int_t sz;
    if (n_args == 1 || ((sz = mp_obj_get_int(args[1])) == -1)) {
        return stream_readall(args[0]);
    }

    #if MICROPY_PY_BUILTINS_STR_UNICODE
    if (o->type->stream_p->is_text) {
        // We need to read sz number of unicode characters.  Because we don't have any
        // buffering, and because the stream API can only read bytes, we must read here
        // in units of bytes and must never over read.  If we want sz chars, then reading
        // sz bytes will never over-read, so we follow this approach, in a loop to keep
        // reading until we have exactly enough chars.  This will be 1 read for text
        // with ASCII-only chars, and about 2 reads for text with a couple of non-ASCII
        // chars.  For text with lots of non-ASCII chars, it'll be pretty inefficient
        // in time and memory.

        vstr_t vstr;
        vstr_init(&vstr, sz);
        mp_uint_t more_bytes = sz;
        mp_uint_t last_buf_offset = 0;
        while (more_bytes > 0) {
            char *p = vstr_add_len(&vstr, more_bytes);
            if (p == NULL) {
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "out of memory"));
            }
            int error;
            mp_uint_t out_sz = o->type->stream_p->read(o, p, more_bytes, &error);
            if (out_sz == MP_STREAM_ERROR) {
                vstr_cut_tail_bytes(&vstr, more_bytes);
                if (is_nonblocking_error(error)) {
                    // With non-blocking streams, we read as much as we can.
                    // If we read nothing, return None, just like read().
                    // Otherwise, return data read so far.
                    // TODO what if we have read only half a non-ASCII char?
                    if (vstr.len == 0) {
                        vstr_clear(&vstr);
                        return mp_const_none;
                    }
                    break;
                }
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
            }

            if (out_sz < more_bytes) {
                // Finish reading.
                // TODO what if we have read only half a non-ASCII char?
                vstr_cut_tail_bytes(&vstr, more_bytes - out_sz);
                if (out_sz == 0) {
                    break; 
                }
            }

            // count chars from bytes just read
            for (mp_uint_t off = last_buf_offset;;) {
                byte b = vstr.buf[off];
                int n;
                if (!UTF8_IS_NONASCII(b)) {
                    // 1-byte ASCII char
                    n = 1;
                } else if ((b & 0xe0) == 0xc0) {
                    // 2-byte char
                    n = 2;
                } else if ((b & 0xf0) == 0xe0) {
                    // 3-byte char
                    n = 3;
                } else if ((b & 0xf8) == 0xf0) {
                    // 4-byte char
                    n = 4;
                } else {
                    // TODO
                    n = 5;
                }
                if (off + n <= vstr.len) {
                    // got a whole char in n bytes
                    off += n;
                    sz -= 1;
                    last_buf_offset = off;
                    if (off >= vstr.len) {
                        more_bytes = sz;
                        break;
                    }
                } else {
                    // didn't get a whole char, so work out how many extra bytes are needed for
                    // this partial char, plus bytes for additional chars that we want
                    more_bytes = (off + n - vstr.len) + (sz - 1);
                    break;
                }
            }
        }

        mp_obj_t ret = mp_obj_new_str_of_type(&mp_type_str, (byte*)vstr.buf, vstr.len);
        vstr_clear(&vstr);
        return ret;
    }
    #endif

    byte *buf = m_new(byte, sz);
    int error;
    mp_uint_t out_sz = o->type->stream_p->read(o, buf, sz, &error);
    if (out_sz == MP_STREAM_ERROR) {
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

mp_obj_t mp_stream_write(mp_obj_t self_in, const void *buf, mp_uint_t len) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p == NULL || o->type->stream_p->write == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    int error;
    mp_uint_t out_sz = o->type->stream_p->write(self_in, buf, len, &error);
    if (out_sz == MP_STREAM_ERROR) {
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

STATIC mp_obj_t stream_write_method(mp_obj_t self_in, mp_obj_t arg) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    return mp_stream_write(self_in, bufinfo.buf, bufinfo.len);
}

STATIC mp_obj_t stream_readall(mp_obj_t self_in) {
    struct _mp_obj_base_t *o = (struct _mp_obj_base_t *)self_in;
    if (o->type->stream_p == NULL || o->type->stream_p->read == NULL) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "Operation not supported"));
    }

    int total_size = 0;
    vstr_t *vstr = vstr_new_size(DEFAULT_BUFFER_SIZE);
    char *p = vstr_str(vstr);
    int error;
    int current_read = DEFAULT_BUFFER_SIZE;
    while (true) {
        mp_uint_t out_sz = o->type->stream_p->read(self_in, p, current_read, &error);
        if (out_sz == MP_STREAM_ERROR) {
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

    mp_int_t max_size = -1;
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

        mp_uint_t out_sz = o->type->stream_p->read(o, p, 1, &error);
        if (out_sz == MP_STREAM_ERROR) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d]", error));
        }
        if (out_sz == 0) {
            // Back out previously added byte
            // Consider, what's better - read a char and get OutOfMemory (so read
            // char is lost), or allocate first as we do.
            vstr_cut_tail_bytes(vstr, 1);
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
MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_write_obj, stream_write_method);
