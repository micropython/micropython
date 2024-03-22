/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
 * Copyright (c) 2014-2016 Paul Sokolovsky
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

#include "py/objstr.h"
#include "py/stream.h"
#include "py/runtime.h"

// This file defines generic Python stream read/write methods which
// dispatch to the underlying stream interface of an object.

// TODO: should be in mpconfig.h
#define DEFAULT_BUFFER_SIZE 256

static mp_obj_t stream_readall(mp_obj_t self_in);

// Returns error condition in *errcode, if non-zero, return value is number of bytes written
// before error condition occurred. If *errcode == 0, returns total bytes written (which will
// be equal to input size).
mp_uint_t mp_stream_rw(mp_obj_t stream, void *buf_, mp_uint_t size, int *errcode, byte flags) {
    byte *buf = buf_;
    typedef mp_uint_t (*io_func_t)(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode);
    io_func_t io_func;
    const mp_stream_p_t *stream_p = mp_get_stream(stream);
    if (flags & MP_STREAM_RW_WRITE) {
        io_func = (io_func_t)stream_p->write;
    } else {
        io_func = stream_p->read;
    }

    *errcode = 0;
    mp_uint_t done = 0;
    while (size > 0) {
        mp_uint_t out_sz = io_func(stream, buf, size, errcode);
        // For read, out_sz == 0 means EOF. For write, it's unspecified
        // what it means, but we don't make any progress, so returning
        // is still the best option.
        if (out_sz == 0) {
            return done;
        }
        if (out_sz == MP_STREAM_ERROR) {
            // If we read something before getting EAGAIN, don't leak it
            if (mp_is_nonblocking_error(*errcode) && done != 0) {
                *errcode = 0;
            }
            return done;
        }
        if (flags & MP_STREAM_RW_ONCE) {
            return out_sz;
        }

        buf += out_sz;
        size -= out_sz;
        done += out_sz;
    }
    return done;
}

mp_off_t mp_stream_seek(mp_obj_t stream, mp_off_t offset, int whence, int *errcode) {
    struct mp_stream_seek_t seek_s;
    seek_s.offset = offset;
    seek_s.whence = whence;
    const mp_stream_p_t *stream_p = mp_get_stream(stream);
    mp_uint_t res = stream_p->ioctl(MP_OBJ_FROM_PTR(stream), MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, errcode);
    if (res == MP_STREAM_ERROR) {
        return (mp_off_t)-1;
    }
    return seek_s.offset;
}

const mp_stream_p_t *mp_get_stream_raise(mp_obj_t self_in, int flags) {
    const mp_obj_type_t *type = mp_obj_get_type(self_in);
    if (MP_OBJ_TYPE_HAS_SLOT(type, protocol)) {
        const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(type, protocol);
        if (!((flags & MP_STREAM_OP_READ) && stream_p->read == NULL)
            && !((flags & MP_STREAM_OP_WRITE) && stream_p->write == NULL)
            && !((flags & MP_STREAM_OP_IOCTL) && stream_p->ioctl == NULL)) {
            return stream_p;
        }
    }
    // CPython: io.UnsupportedOperation, OSError subclass
    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("stream operation not supported"));
}

static mp_obj_t stream_read_generic(size_t n_args, const mp_obj_t *args, byte flags) {
    // What to do if sz < -1?  Python docs don't specify this case.
    // CPython does a readall, but here we silently let negatives through,
    // and they will cause a MemoryError.
    mp_int_t sz;
    if (n_args == 1 || ((sz = mp_obj_get_int(args[1])) == -1)) {
        return stream_readall(args[0]);
    }

    const mp_stream_p_t *stream_p = mp_get_stream(args[0]);

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
            int error;
            mp_uint_t out_sz = mp_stream_read_exactly(args[0], p, more_bytes, &error);
            if (error != 0) {
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
                mp_raise_OSError(error);
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

        return mp_obj_new_str_from_vstr(&vstr);
    }
    #endif

    vstr_t vstr;
    vstr_init_len(&vstr, sz);
    int error;
    mp_uint_t out_sz = mp_stream_rw(args[0], vstr.buf, sz, &error, flags);
    if (error != 0) {
        vstr_clear(&vstr);
        if (mp_is_nonblocking_error(error)) {
            // https://docs.python.org/3.4/library/io.html#io.RawIOBase.read
            // "If the object is in non-blocking mode and no bytes are available,
            // None is returned."
            // This is actually very weird, as naive truth check will treat
            // this as EOF.
            return mp_const_none;
        }
        mp_raise_OSError(error);
    } else {
        vstr.len = out_sz;
        if (stream_p->is_text) {
            return mp_obj_new_str_from_vstr(&vstr);
        } else {
            return mp_obj_new_bytes_from_vstr(&vstr);
        }
    }
}

static mp_obj_t stream_read(size_t n_args, const mp_obj_t *args) {
    return stream_read_generic(n_args, args, MP_STREAM_RW_READ);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read_obj, 1, 2, stream_read);

static mp_obj_t stream_read1(size_t n_args, const mp_obj_t *args) {
    return stream_read_generic(n_args, args, MP_STREAM_RW_READ | MP_STREAM_RW_ONCE);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read1_obj, 1, 2, stream_read1);

mp_obj_t mp_stream_write(mp_obj_t self_in, const void *buf, size_t len, byte flags) {
    int error;
    mp_uint_t out_sz = mp_stream_rw(self_in, (void *)buf, len, &error, flags);
    if (error != 0) {
        if (mp_is_nonblocking_error(error)) {
            // http://docs.python.org/3/library/io.html#io.RawIOBase.write
            // "None is returned if the raw stream is set not to block and
            // no single byte could be readily written to it."
            return mp_const_none;
        }
        mp_raise_OSError(error);
    } else {
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}

// This is used to adapt a stream object to an mp_print_t interface
void mp_stream_write_adaptor(void *self, const char *buf, size_t len) {
    mp_stream_write(MP_OBJ_FROM_PTR(self), buf, len, MP_STREAM_RW_WRITE);
}

static mp_obj_t stream_write_method(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);
    size_t max_len = (size_t)-1;
    size_t off = 0;
    if (n_args == 3) {
        max_len = mp_obj_get_int_truncated(args[2]);
    } else if (n_args == 4) {
        off = mp_obj_get_int_truncated(args[2]);
        max_len = mp_obj_get_int_truncated(args[3]);
        if (off > bufinfo.len) {
            off = bufinfo.len;
        }
    }
    bufinfo.len -= off;
    return mp_stream_write(args[0], (byte *)bufinfo.buf + off, MIN(bufinfo.len, max_len), MP_STREAM_RW_WRITE);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_write_obj, 2, 4, stream_write_method);

static mp_obj_t stream_write1_method(mp_obj_t self_in, mp_obj_t arg) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(arg, &bufinfo, MP_BUFFER_READ);
    return mp_stream_write(self_in, bufinfo.buf, bufinfo.len, MP_STREAM_RW_WRITE | MP_STREAM_RW_ONCE);
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_stream_write1_obj, stream_write1_method);

static mp_obj_t stream_readinto(size_t n_args, const mp_obj_t *args) {
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
    mp_uint_t out_sz = mp_stream_read_exactly(args[0], bufinfo.buf, len, &error);
    if (error != 0) {
        if (mp_is_nonblocking_error(error)) {
            return mp_const_none;
        }
        mp_raise_OSError(error);
    } else {
        return MP_OBJ_NEW_SMALL_INT(out_sz);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_readinto_obj, 2, 3, stream_readinto);

static mp_obj_t stream_readall(mp_obj_t self_in) {
    const mp_stream_p_t *stream_p = mp_get_stream(self_in);

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
            mp_raise_OSError(error);
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
        }
    }

    vstr.len = total_size;
    if (stream_p->is_text) {
        return mp_obj_new_str_from_vstr(&vstr);
    } else {
        return mp_obj_new_bytes_from_vstr(&vstr);
    }
}

// Unbuffered, inefficient implementation of readline() for raw I/O files.
static mp_obj_t stream_unbuffered_readline(size_t n_args, const mp_obj_t *args) {
    const mp_stream_p_t *stream_p = mp_get_stream(args[0]);

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
        int error;
        mp_uint_t out_sz = stream_p->read(args[0], p, 1, &error);
        if (out_sz == MP_STREAM_ERROR) {
            if (mp_is_nonblocking_error(error)) {
                if (vstr.len == 1) {
                    // We just incremented it, but otherwise we read nothing
                    // and immediately got EAGAIN. This case is not well
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
            mp_raise_OSError(error);
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

    if (stream_p->is_text) {
        return mp_obj_new_str_from_vstr(&vstr);
    } else {
        return mp_obj_new_bytes_from_vstr(&vstr);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_unbuffered_readline_obj, 1, 2, stream_unbuffered_readline);

// TODO take an optional extra argument (what does it do exactly?)
static mp_obj_t stream_unbuffered_readlines(mp_obj_t self) {
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

mp_obj_t mp_stream_close(mp_obj_t stream) {
    const mp_stream_p_t *stream_p = mp_get_stream(stream);
    int error;
    mp_uint_t res = stream_p->ioctl(stream, MP_STREAM_CLOSE, 0, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_close_obj, mp_stream_close);

static mp_obj_t mp_stream___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream___exit___obj, 4, 4, mp_stream___exit__);

static mp_obj_t stream_seek(size_t n_args, const mp_obj_t *args) {
    // TODO: Could be uint64
    mp_off_t offset = mp_obj_get_int(args[1]);
    int whence = SEEK_SET;
    if (n_args == 3) {
        whence = mp_obj_get_int(args[2]);
    }

    // In POSIX, it's error to seek before end of stream, we enforce it here.
    if (whence == SEEK_SET && offset < 0) {
        mp_raise_OSError(MP_EINVAL);
    }

    int error;
    mp_off_t res = mp_stream_seek(args[0], offset, whence, &error);
    if (res == (mp_off_t)-1) {
        mp_raise_OSError(error);
    }

    // TODO: Could be uint64
    return mp_obj_new_int_from_uint(res);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_seek_obj, 2, 3, stream_seek);

static mp_obj_t stream_tell(mp_obj_t self) {
    mp_obj_t offset = MP_OBJ_NEW_SMALL_INT(0);
    mp_obj_t whence = MP_OBJ_NEW_SMALL_INT(SEEK_CUR);
    const mp_obj_t args[3] = {self, offset, whence};
    return stream_seek(3, args);
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_tell_obj, stream_tell);

static mp_obj_t stream_flush(mp_obj_t self) {
    const mp_stream_p_t *stream_p = mp_get_stream(self);
    int error;
    mp_uint_t res = stream_p->ioctl(self, MP_STREAM_FLUSH, 0, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mp_stream_flush_obj, stream_flush);

static mp_obj_t stream_ioctl(size_t n_args, const mp_obj_t *args) {
    mp_buffer_info_t bufinfo;
    uintptr_t val = 0;
    if (n_args > 2) {
        if (mp_get_buffer(args[2], &bufinfo, MP_BUFFER_WRITE)) {
            val = (uintptr_t)bufinfo.buf;
        } else {
            val = mp_obj_get_int_truncated(args[2]);
        }
    }

    const mp_stream_p_t *stream_p = mp_get_stream(args[0]);
    int error;
    mp_uint_t res = stream_p->ioctl(args[0], mp_obj_get_int(args[1]), val, &error);
    if (res == MP_STREAM_ERROR) {
        mp_raise_OSError(error);
    }

    return mp_obj_new_int(res);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_ioctl_obj, 2, 3, stream_ioctl);

#if MICROPY_STREAMS_POSIX_API
/*
 * POSIX-like functions
 *
 * These functions have POSIX-compatible signature (except for "void *stream"
 * first argument instead of "int fd"). They are useful to port existing
 * POSIX-compatible software to work with MicroPython streams.
 */

#include <errno.h>

ssize_t mp_stream_posix_write(void *stream, const void *buf, size_t len) {
    mp_obj_base_t *o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    mp_uint_t out_sz = stream_p->write(MP_OBJ_FROM_PTR(stream), buf, len, &errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

ssize_t mp_stream_posix_read(void *stream, void *buf, size_t len) {
    mp_obj_base_t *o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    mp_uint_t out_sz = stream_p->read(MP_OBJ_FROM_PTR(stream), buf, len, &errno);
    if (out_sz == MP_STREAM_ERROR) {
        return -1;
    } else {
        return out_sz;
    }
}

off_t mp_stream_posix_lseek(void *stream, off_t offset, int whence) {
    mp_off_t res = mp_stream_seek(MP_OBJ_FROM_PTR(stream), offset, whence, &errno);
    if (res == (mp_off_t)-1) {
        return -1;
    }
    return res;
}

int mp_stream_posix_fsync(void *stream) {
    mp_obj_base_t *o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    mp_uint_t res = stream_p->ioctl(MP_OBJ_FROM_PTR(stream), MP_STREAM_FLUSH, 0, &errno);
    if (res == MP_STREAM_ERROR) {
        return -1;
    }
    return res;
}

#endif
