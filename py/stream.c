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
#include <unistd.h>

#include "py/nlr.h"
#include "py/objstr.h"
#include "py/stream.h"

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

#define STREAM_CONTENT_TYPE(stream) (((stream)->is_text) ? &mp_type_str : &mp_type_bytes)

const mp_stream_p_t *mp_get_stream_raise(mp_obj_t self_in, int flags) {
    mp_obj_base_t *o = (mp_obj_base_t*)MP_OBJ_TO_PTR(self_in);
    const mp_stream_p_t *stream_p = o->type->stream_p;
    if (stream_p == NULL
        || ((flags & MP_STREAM_OP_READ) && stream_p->read == NULL)
        || ((flags & MP_STREAM_OP_WRITE) && stream_p->write == NULL)
        || ((flags & MP_STREAM_OP_IOCTL) && stream_p->ioctl == NULL)) {
        // CPython: io.UnsupportedOperation, OSError subclass
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "stream operation not supported"));
    }
    return stream_p;
}

STATIC mp_obj_t stream_read(size_t n_args, const mp_obj_t *args) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(args[0], MP_STREAM_OP_READ);

    // What to do if sz < -1?  Python docs don't specify this case.
    // CPython does a readall, but here we silently let negatives through,
    // and they will cause a MemoryError.
    mp_int_t sz;
    if (n_args == 1 || ((sz = mp_obj_get_int(args[1])) == -1)) {
        return stream_readall(args[0]);
    }

    #if MICROPY_PY_BUILTINS_STR_UNICODE
    if (stream_p->is_text) {
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
            mp_uint_t out_sz = stream_p->read(args[0], p, more_bytes, &error);
            if (out_sz == MP_STREAM_ERROR) {
                vstr_cut_tail_bytes(&vstr, more_bytes);
                if (mp_is_nonblocking_error(error)) {
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
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
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

        return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
    }
    #endif

    vstr_t vstr;
    vstr_init_len(&vstr, sz);
    int error;
    mp_uint_t out_sz = stream_p->read(args[0], vstr.buf, sz, &error);
    if (out_sz == MP_STREAM_ERROR) {
        vstr_clear(&vstr);
        if (mp_is_nonblocking_error(error)) {
            // https://docs.python.org/3.4/library/io.html#io.RawIOBase.read
            // "If the object is in non-blocking mode and no bytes are available,
            // None is returned."
            // This is actually very weird, as naive truth check will treat
            // this as EOF.
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    } else {
        vstr.len = out_sz;
        return mp_obj_new_str_from_vstr(STREAM_CONTENT_TYPE(stream_p), &vstr);
    }
}

mp_obj_t mp_stream_write(mp_obj_t self_in, const void *buf, size_t len) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(self_in, MP_STREAM_OP_WRITE);

    int error;
    mp_uint_t out_sz = stream_p->write(self_in, buf, len, &error);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(error)) {
            // http://docs.python.org/3/library/io.html#io.RawIOBase.write
            // "None is returned if the raw stream is set not to block and
            // no single byte could be readily written to it."
            // This is for consistency with read() behavior, still weird,
            // see abobe.
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    } else {
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

// XXX hack
void mp_stream_write_adaptor(void *self, const char *buf, size_t len) {
    mp_stream_write(MP_OBJ_FROM_PTR(self), buf, len);
}

STATIC mp_obj_t stream_write_method(mp_obj_t self_in, mp_obj_t arg) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    return mp_stream_write(self_in, bufinfo.buf, bufinfo.len);
}

STATIC mp_obj_t stream_readinto(size_t n_args, const mp_obj_t *args) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(args[0], MP_STREAM_OP_READ);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);

    // CPython extension: if 2nd arg is provided, that's max len to read,
    // instead of full buffer. Similar to
    // https://docs.python.org/3/library/socket.html#socket.socket.recv_into
    mp_uint_t len = bufinfo.len;
    if (n_args > 2) {
        len = mp_obj_get_int(args[2]);
        if (len > bufinfo.len) {
            len = bufinfo.len;
        }
    }

    int error;
    mp_uint_t out_sz = stream_p->read(args[0], bufinfo.buf, len, &error);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(error)) {
            return mp_const_none;
        }
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    } else {
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

STATIC mp_obj_t stream_readall(mp_obj_t self_in) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(self_in, MP_STREAM_OP_READ);

    mp_uint_t total_size = 0;
    vstr_t vstr;
    vstr_init(&vstr, DEFAULT_BUFFER_SIZE);
    char *p = vstr.buf;
    mp_uint_t current_read = DEFAULT_BUFFER_SIZE;
    while (true) {
        int error;
        mp_uint_t out_sz = stream_p->read(self_in, p, current_read, &error);
        if (out_sz == MP_STREAM_ERROR) {
            if (mp_is_nonblocking_error(error)) {
                // With non-blocking streams, we read as much as we can.
                // If we read nothing, return None, just like read().
                // Otherwise, return data read so far.
                if (total_size == 0) {
                    return mp_const_none;
                }
                break;
            }
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
        }
        if (out_sz == 0) {
            break;
        }
        total_size += out_sz;
        if (out_sz < current_read) {
            current_read -= out_sz;
            p += out_sz;
        } else {
            p = vstr_extend(&vstr, DEFAULT_BUFFER_SIZE);
            current_read = DEFAULT_BUFFER_SIZE;
            if (p == NULL) {
                // TODO
                nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError/*&mp_type_RuntimeError*/, "Out of memory"));
            }
        }
    }

    vstr.len = total_size;
    return mp_obj_new_str_from_vstr(STREAM_CONTENT_TYPE(stream_p), &vstr);
}

// Unbuffered, inefficient implementation of readline() for raw I/O files.
STATIC mp_obj_t stream_unbuffered_readline(size_t n_args, const mp_obj_t *args) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(args[0], MP_STREAM_OP_READ);

    mp_int_t max_size = -1;
    if (n_args > 1) {
        max_size = MP_OBJ_SMALL_INT_VALUE(args[1]);
    }

    vstr_t vstr;
    if (max_size != -1) {
        vstr_init(&vstr, max_size);
    } else {
        vstr_init(&vstr, 16);
    }

    while (max_size == -1 || max_size-- != 0) {
        char *p = vstr_add_len(&vstr, 1);
        if (p == NULL) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_MemoryError, "out of memory"));
        }

        int error;
        mp_uint_t out_sz = stream_p->read(args[0], p, 1, &error);
        if (out_sz == MP_STREAM_ERROR) {
            if (mp_is_nonblocking_error(error)) {
                if (vstr.len == 1) {
                    // We just incremented it, but otherwise we read nothing
                    // and immediately got EAGAIN. This is case is not well
                    // specified in
                    // https://docs.python.org/3/library/io.html#io.IOBase.readline
                    // unlike similar case for read(). But we follow the latter's
                    // behavior - return None.
                    vstr_clear(&vstr);
                    return mp_const_none;
                } else {
                    goto done;
                }
            }
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
        }
        if (out_sz == 0) {
done:
            // Back out previously added byte
            // Consider, what's better - read a char and get OutOfMemory (so read
            // char is lost), or allocate first as we do.
            vstr_cut_tail_bytes(&vstr, 1);
            break;
        }
        if (*p == '\n') {
            break;
        }
    }

    return mp_obj_new_str_from_vstr(STREAM_CONTENT_TYPE(stream_p), &vstr);
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

STATIC mp_obj_t stream_seek(size_t n_args, const mp_obj_t *args) {
    const mp_stream_p_t *stream_p = mp_get_stream_raise(args[0], MP_STREAM_OP_IOCTL);

    struct mp_stream_seek_t seek_s;
    // TODO: Could be uint64
    seek_s.offset = mp_obj_get_int(args[1]);
    seek_s.whence = 0;
    if (n_args == 3) {
        seek_s.whence = mp_obj_get_int(args[2]);
    }

    int error;
    mp_uint_t res = stream_p->ioctl(args[0], MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &error);
    if (res == MP_STREAM_ERROR) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error)));
    }

    // TODO: Could be uint64
    return mp_obj_new_int_from_uint(seek_s.offset);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_seek_obj, 2, 3, stream_seek);

STATIC mp_obj_t stream_tell(mp_obj_t self) {
    mp_obj_t offset = MP_OBJ_NEW_SMALL_INT(0);
    mp_obj_t whence = MP_OBJ_NEW_SMALL_INT(SEEK_CUR);
    const mp_obj_t args[3] = {self, offset, whence};
    return stream_seek(3, args);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_tell_obj, stream_tell);

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read_obj, 1, 2, stream_read);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_readinto_obj, 2, 3, stream_readinto);
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_readall_obj, stream_readall);
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_unbuffered_readline_obj, 1, 2, stream_unbuffered_readline);
MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_write_obj, stream_write_method);
