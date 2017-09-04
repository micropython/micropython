/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/builtin.h"
#include "py/mphal.h"
#include "fdfile.h"

#if MICROPY_PY_IO

#ifdef _WIN32
#define fsync _commit
#endif

#ifdef MICROPY_CPYTHON_COMPAT
STATIC void check_fd_is_open(const mp_obj_fdfile_t *o) {
    if (o->fd < 0) {
        mp_raise_ValueError("I/O operation on closed file");
    }
}
#else
#define check_fd_is_open(o)
#endif

extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;

STATIC void fdfile_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_fdfile_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<io.%s %d>", mp_obj_get_type_str(self_in), self->fd);
}

STATIC mp_uint_t fdfile_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_fdfile_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    mp_int_t r = read(o->fd, buf, size);
    if (r == -1) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_uint_t fdfile_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_fdfile_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    #if MICROPY_PY_OS_DUPTERM
    if (o->fd <= STDERR_FILENO) {
        mp_hal_stdout_tx_strn(buf, size);
        return size;
    }
    #endif
    mp_int_t r = write(o->fd, buf, size);
    while (r == -1 && errno == EINTR) {
        if (MP_STATE_VM(mp_pending_exception) != MP_OBJ_NULL) {
            mp_obj_t obj = MP_STATE_VM(mp_pending_exception);
            MP_STATE_VM(mp_pending_exception) = MP_OBJ_NULL;
            nlr_raise(obj);
        }
        r = write(o->fd, buf, size);
    }
    if (r == -1) {
        *errcode = errno;
        return MP_STREAM_ERROR;
    }
    return r;
}

STATIC mp_uint_t fdfile_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_fdfile_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    switch (request) {
        case MP_STREAM_SEEK: {
            struct mp_stream_seek_t *s = (struct mp_stream_seek_t*)arg;
            off_t off = lseek(o->fd, s->offset, s->whence);
            if (off == (off_t)-1) {
                *errcode = errno;
                return MP_STREAM_ERROR;
            }
            s->offset = off;
            return 0;
        }
        case MP_STREAM_FLUSH:
            if (fsync(o->fd) < 0) {
                *errcode = errno;
                return MP_STREAM_ERROR;
            }
            return 0;
        default:
            *errcode = EINVAL;
            return MP_STREAM_ERROR;
    }
}

STATIC mp_obj_t fdfile_close(mp_obj_t self_in) {
    mp_obj_fdfile_t *self = MP_OBJ_TO_PTR(self_in);
    close(self->fd);
#ifdef MICROPY_CPYTHON_COMPAT
    self->fd = -1;
#endif
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fdfile_close_obj, fdfile_close);

STATIC mp_obj_t fdfile___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return fdfile_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fdfile___exit___obj, 4, 4, fdfile___exit__);

STATIC mp_obj_t fdfile_fileno(mp_obj_t self_in) {
    mp_obj_fdfile_t *self = MP_OBJ_TO_PTR(self_in);
    check_fd_is_open(self);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fdfile_fileno_obj, fdfile_fileno);

// Note: encoding is ignored for now; it's also not a valid kwarg for CPython's FileIO,
// but by adding it here we can use one single mp_arg_t array for open() and FileIO's constructor
STATIC const mp_arg_t file_open_args[] = {
    { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_r)} },
    { MP_QSTR_buffering, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    { MP_QSTR_encoding, MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
};
#define FILE_OPEN_NUM_ARGS MP_ARRAY_SIZE(file_open_args)

STATIC mp_obj_t fdfile_open(const mp_obj_type_t *type, mp_arg_val_t *args) {
    mp_obj_fdfile_t *o = m_new_obj(mp_obj_fdfile_t);
    const char *mode_s = mp_obj_str_get_str(args[1].u_obj);

    int mode_rw = 0, mode_x = 0;
    while (*mode_s) {
        switch (*mode_s++) {
            case 'r':
                mode_rw = O_RDONLY;
                break;
            case 'w':
                mode_rw = O_WRONLY;
                mode_x = O_CREAT | O_TRUNC;
                break;
            case 'a':
                mode_rw = O_WRONLY;
                mode_x = O_CREAT | O_APPEND;
                break;
            case '+':
                mode_rw = O_RDWR;
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

    mp_obj_t fid = args[0].u_obj;

    if (MP_OBJ_IS_SMALL_INT(fid)) {
        o->fd = MP_OBJ_SMALL_INT_VALUE(fid);
        return MP_OBJ_FROM_PTR(o);
    }

    const char *fname = mp_obj_str_get_str(fid);
    int fd = open(fname, mode_x | mode_rw, 0644);
    if (fd == -1) {
        mp_raise_OSError(errno);
    }
    o->fd = fd;
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t fdfile_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, FILE_OPEN_NUM_ARGS, file_open_args, arg_vals);
    return fdfile_open(type, arg_vals);
}

STATIC const mp_rom_map_elem_t rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&fdfile_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&fdfile_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&fdfile___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict, rawfile_locals_dict_table);

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t fileio_stream_p = {
    .read = fdfile_read,
    .write = fdfile_write,
    .ioctl = fdfile_ioctl,
};

const mp_obj_type_t mp_type_fileio = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = fdfile_print,
    .make_new = fdfile_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &fileio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict,
};
#endif

STATIC const mp_stream_p_t textio_stream_p = {
    .read = fdfile_read,
    .write = fdfile_write,
    .ioctl = fdfile_ioctl,
    .is_text = true,
};

const mp_obj_type_t mp_type_textio = {
    { &mp_type_type },
    .name = MP_QSTR_TextIOWrapper,
    .print = fdfile_print,
    .make_new = fdfile_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &textio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict,
};

// Factory function for I/O stream classes
mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    // TODO: analyze buffering args and instantiate appropriate type
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kwargs, FILE_OPEN_NUM_ARGS, file_open_args, arg_vals);
    return fdfile_open(&mp_type_textio, arg_vals);
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);

const mp_obj_fdfile_t mp_sys_stdin_obj  = { .base = {&mp_type_textio}, .fd = STDIN_FILENO };
const mp_obj_fdfile_t mp_sys_stdout_obj = { .base = {&mp_type_textio}, .fd = STDOUT_FILENO };
const mp_obj_fdfile_t mp_sys_stderr_obj = { .base = {&mp_type_textio}, .fd = STDERR_FILENO };

#endif // MICROPY_PY_IO
