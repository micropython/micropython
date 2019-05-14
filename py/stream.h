/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_STREAM_H
#define MICROPY_INCLUDED_PY_STREAM_H

#include "py/obj.h"
#include "py/mperrno.h"

#define MP_STREAM_ERROR ((mp_uint_t)-1)

// Stream ioctl request codes
#define MP_STREAM_FLUSH (1)
#define MP_STREAM_SEEK  (2)
#define MP_STREAM_POLL  (3)
#define MP_STREAM_CLOSE         (4)
#define MP_STREAM_TIMEOUT       (5)  // Get/set timeout (single op)
#define MP_STREAM_GET_OPTS      (6)  // Get stream options
#define MP_STREAM_SET_OPTS      (7)  // Set stream options
#define MP_STREAM_GET_DATA_OPTS (8)  // Get data/message options
#define MP_STREAM_SET_DATA_OPTS (9)  // Set data/message options
#define MP_STREAM_GET_FILENO    (10) // Get fileno of underlying file

// These poll ioctl values are compatible with Linux
#define MP_STREAM_POLL_RD  (0x0001)
#define MP_STREAM_POLL_WR  (0x0004)
#define MP_STREAM_POLL_ERR (0x0008)
#define MP_STREAM_POLL_HUP (0x0010)

// Argument structure for MP_STREAM_SEEK
struct mp_stream_seek_t {
    // If whence == MP_SEEK_SET, offset should be treated as unsigned.
    // This allows dealing with full-width stream sizes (16, 32, 64,
    // etc. bits). For other seek types, should be treated as signed.
    mp_off_t offset;
    int whence;
};

// seek ioctl "whence" values
#define MP_SEEK_SET (0)
#define MP_SEEK_CUR (1)
#define MP_SEEK_END (2)

// Stream protocol
typedef struct _mp_stream_p_t {
    // On error, functions should return MP_STREAM_ERROR and fill in *errcode (values
    // are implementation-dependent, but will be exposed to user, e.g. via exception).
    mp_uint_t (*read)(mp_obj_t obj, void *buf, mp_uint_t size, int *errcode);
    mp_uint_t (*write)(mp_obj_t obj, const void *buf, mp_uint_t size, int *errcode);
    mp_uint_t (*ioctl)(mp_obj_t obj, mp_uint_t request, uintptr_t arg, int *errcode);
    mp_uint_t is_text : 1; // default is bytes, set this for text stream
} mp_stream_p_t;

MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_read1_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_readinto_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_unbuffered_readline_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_stream_unbuffered_readlines_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_write_obj);
MP_DECLARE_CONST_FUN_OBJ_2(mp_stream_write1_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_stream_close_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_seek_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_stream_tell_obj);
MP_DECLARE_CONST_FUN_OBJ_1(mp_stream_flush_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(mp_stream_ioctl_obj);

// these are for mp_get_stream_raise and can be or'd together
#define MP_STREAM_OP_READ (1)
#define MP_STREAM_OP_WRITE (2)
#define MP_STREAM_OP_IOCTL (4)

// Object is assumed to have a non-NULL stream protocol with valid r/w/ioctl methods
static inline const mp_stream_p_t *mp_get_stream(mp_const_obj_t self) {
    return (const mp_stream_p_t*)((const mp_obj_base_t*)MP_OBJ_TO_PTR(self))->type->protocol;
}

const mp_stream_p_t *mp_get_stream_raise(mp_obj_t self_in, int flags);
mp_obj_t mp_stream_close(mp_obj_t stream);

// Iterator which uses mp_stream_unbuffered_readline_obj
mp_obj_t mp_stream_unbuffered_iter(mp_obj_t self);

mp_obj_t mp_stream_write(mp_obj_t self_in, const void *buf, size_t len, byte flags);

// C-level helper functions
#define MP_STREAM_RW_READ  0
#define MP_STREAM_RW_WRITE 2
#define MP_STREAM_RW_ONCE  1
mp_uint_t mp_stream_rw(mp_obj_t stream, void *buf, mp_uint_t size, int *errcode, byte flags);
#define mp_stream_write_exactly(stream, buf, size, err) mp_stream_rw(stream, (byte*)buf, size, err, MP_STREAM_RW_WRITE)
#define mp_stream_read_exactly(stream, buf, size, err) mp_stream_rw(stream, buf, size, err, MP_STREAM_RW_READ)

void mp_stream_write_adaptor(void *self, const char *buf, size_t len);

#if MICROPY_STREAMS_POSIX_API
// Functions with POSIX-compatible signatures
// "stream" is assumed to be a pointer to a concrete object with the stream protocol
ssize_t mp_stream_posix_write(void *stream, const void *buf, size_t len);
ssize_t mp_stream_posix_read(void *stream, void *buf, size_t len);
off_t mp_stream_posix_lseek(void *stream, off_t offset, int whence);
int mp_stream_posix_fsync(void *stream);
#endif

#if MICROPY_STREAMS_NON_BLOCK
#define mp_is_nonblocking_error(errno) ((errno) == MP_EAGAIN || (errno) == MP_EWOULDBLOCK)
#else
#define mp_is_nonblocking_error(errno) (0)
#endif

#endif // MICROPY_INCLUDED_PY_STREAM_H
