/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "stream.h"

#ifdef _WIN32
#define fsync _commit
#endif

typedef struct _mp_obj_fdfile_t {
    mp_obj_base_t base;
    int fd;
} mp_obj_fdfile_t;

#ifdef MICROPY_CPYTHON_COMPAT
void check_fd_is_open(const mp_obj_fdfile_t *o) {
    if (o->fd < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "I/O operation on closed file"));
    }
}
#else
#define check_fd_is_open(o)
#endif

extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;

STATIC void fdfile_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_fdfile_t *self = self_in;
    print(env, "<io.%s %d>", mp_obj_get_type_str(self), self->fd);
}

STATIC mp_uint_t fdfile_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_fdfile_t *o = o_in;
    check_fd_is_open(o);
    mp_int_t r = read(o->fd, buf, size);
    if (r == -1) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_uint_t fdfile_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_fdfile_t *o = o_in;
    check_fd_is_open(o);
    mp_int_t r = write(o->fd, buf, size);
    if (r == -1) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_obj_t fdfile_flush(mp_obj_t self_in) {
    mp_obj_fdfile_t *self = self_in;
    check_fd_is_open(self);
    fsync(self->fd);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fdfile_flush_obj, fdfile_flush);

STATIC mp_obj_t fdfile_close(mp_obj_t self_in) {
    mp_obj_fdfile_t *self = self_in;
    close(self->fd);
#ifdef MICROPY_CPYTHON_COMPAT
    self->fd = -1;
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fdfile_close_obj, fdfile_close);

mp_obj_t fdfile___exit__(uint n_args, const mp_obj_t *args) {
    return fdfile_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fdfile___exit___obj, 4, 4, fdfile___exit__);

STATIC mp_obj_t fdfile_fileno(mp_obj_t self_in) {
    mp_obj_fdfile_t *self = self_in;
    check_fd_is_open(self);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fdfile_fileno_obj, fdfile_fileno);

STATIC mp_obj_t fdfile_make_new(mp_obj_t type_in, uint n_args, uint n_kw, const mp_obj_t *args) {
    mp_obj_fdfile_t *o = m_new_obj(mp_obj_fdfile_t);
    mp_const_obj_t type = type_in;

    const char *mode_s;
    if (n_args > 1) {
        mode_s = mp_obj_str_get_str(args[1]);
    } else {
        mode_s = "r";
    }

    int mode = 0;
    while (*mode_s) {
        switch (*mode_s++) {
            // Note: these assume O_RDWR = O_RDONLY | O_WRONLY
            case 'r':
                mode |= O_RDONLY;
                break;
            case 'w':
                mode |= O_WRONLY | O_CREAT | O_TRUNC;
                break;
            case 'a':
                mode |= O_APPEND;
                break;
            case '+':
                mode |= O_RDWR;
                break;
            #if MICROPY_PY_IO_FILEIO
            // If we don't have io.FileIO, then files are in text mode implicitly
            case 'b':
                type = &mp_type_fileio;
                break;
            case 't':
                type = &mp_type_textio;
                break;
            #endif
        }
    }

    o->base.type = type;

    if (MP_OBJ_IS_SMALL_INT(args[0])) {
        o->fd = MP_OBJ_SMALL_INT_VALUE(args[0]);
        return o;
    }

    const char *fname = mp_obj_str_get_str(args[0]);
    int fd = open(fname, mode, 0644);
    if (fd == -1) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(errno)));
    }
    o->fd = fd;
    return o;
}

STATIC const mp_map_elem_t rawfile_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_fileno), (mp_obj_t)&fdfile_fileno_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_readlines), (mp_obj_t)&mp_stream_unbuffered_readlines_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flush), (mp_obj_t)&fdfile_flush_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&fdfile_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___enter__), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___exit__), (mp_obj_t)&fdfile___exit___obj },
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict, rawfile_locals_dict_table);

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t fileio_stream_p = {
    .read = fdfile_read,
    .write = fdfile_write,
};

const mp_obj_type_t mp_type_fileio = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = fdfile_print,
    .make_new = fdfile_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &fileio_stream_p,
    .locals_dict = (mp_obj_t)&rawfile_locals_dict,
};
#endif

STATIC const mp_stream_p_t textio_stream_p = {
    .read = fdfile_read,
    .write = fdfile_write,
    .is_text = true,
};

const mp_obj_type_t mp_type_textio = {
    { &mp_type_type },
    .name = MP_QSTR_TextIOWrapper,
    .print = fdfile_print,
    .make_new = fdfile_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &textio_stream_p,
    .locals_dict = (mp_obj_t)&rawfile_locals_dict,
};

// Factory function for I/O stream classes
mp_obj_t mp_builtin_open(uint n_args, const mp_obj_t *args) {
    // TODO: analyze mode and buffering args and instantiate appropriate type
    return fdfile_make_new((mp_obj_t)&mp_type_textio, n_args, 0, args);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_open_obj, 1, 2, mp_builtin_open);

const mp_obj_fdfile_t mp_sys_stdin_obj  = { .base = {&mp_type_textio}, .fd = STDIN_FILENO };
const mp_obj_fdfile_t mp_sys_stdout_obj = { .base = {&mp_type_textio}, .fd = STDOUT_FILENO };
const mp_obj_fdfile_t mp_sys_stderr_obj = { .base = {&mp_type_textio}, .fd = STDERR_FILENO };
