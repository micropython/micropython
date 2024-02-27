/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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

#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "extmod/vfs_posix.h"

#if MICROPY_VFS_POSIX

#include <fcntl.h>
#include <unistd.h>

#ifdef _WIN32
#define fsync _commit
#else
#include <poll.h>
#endif

typedef struct _mp_obj_vfs_posix_file_t {
    mp_obj_base_t base;
    int fd;
} mp_obj_vfs_posix_file_t;

#if MICROPY_CPYTHON_COMPAT
static void check_fd_is_open(const mp_obj_vfs_posix_file_t *o) {
    if (o->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}
#else
#define check_fd_is_open(o)
#endif

static void vfs_posix_file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_vfs_posix_file_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<io.%s %d>", mp_obj_get_type_str(self_in), self->fd);
}

mp_obj_t mp_vfs_posix_file_open(const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in) {
    const char *mode_s = mp_obj_str_get_str(mode_in);

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
            case 'b':
                type = &mp_type_vfs_posix_fileio;
                break;
            case 't':
                type = &mp_type_vfs_posix_textio;
                break;
        }
    }

    mp_obj_vfs_posix_file_t *o = mp_obj_malloc_with_finaliser(mp_obj_vfs_posix_file_t, type);
    o->fd = -1; // In case open() fails below, initialise this as a "closed" file object.

    mp_obj_t fid = file_in;

    if (mp_obj_is_small_int(fid)) {
        o->fd = MP_OBJ_SMALL_INT_VALUE(fid);
        return MP_OBJ_FROM_PTR(o);
    }

    const char *fname = mp_obj_str_get_str(fid);
    int fd;
    MP_HAL_RETRY_SYSCALL(fd, open(fname, mode_x | mode_rw, 0644), mp_raise_OSError(err));
    o->fd = fd;
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t vfs_posix_file_fileno(mp_obj_t self_in) {
    mp_obj_vfs_posix_file_t *self = MP_OBJ_TO_PTR(self_in);
    check_fd_is_open(self);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_posix_file_fileno_obj, vfs_posix_file_fileno);

static mp_uint_t vfs_posix_file_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_posix_file_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    ssize_t r;
    MP_HAL_RETRY_SYSCALL(r, read(o->fd, buf, size), {
        *errcode = err;
        return MP_STREAM_ERROR;
    });
    return (mp_uint_t)r;
}

static mp_uint_t vfs_posix_file_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_posix_file_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    #if MICROPY_PY_OS_DUPTERM
    if (o->fd <= STDERR_FILENO) {
        mp_hal_stdout_tx_strn(buf, size);
        return size;
    }
    #endif
    ssize_t r;
    MP_HAL_RETRY_SYSCALL(r, write(o->fd, buf, size), {
        *errcode = err;
        return MP_STREAM_ERROR;
    });
    return (mp_uint_t)r;
}

static mp_uint_t vfs_posix_file_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_vfs_posix_file_t *o = MP_OBJ_TO_PTR(o_in);

    if (request != MP_STREAM_CLOSE) {
        check_fd_is_open(o);
    }

    switch (request) {
        case MP_STREAM_FLUSH: {
            int ret;
            // fsync(stdin/stdout/stderr) may fail with EINVAL (or ENOTSUP on macos or EBADF
            // on windows), because the OS doesn't buffer these except for instance when they
            // are redirected from/to file, but don't propagate that error out.  Because data
            // is not buffered by us, and stdin/out/err.flush() should just be a no-op.
            #if defined(__APPLE__)
            #define VFS_POSIX_STREAM_STDIO_ERR_CATCH (err == EINVAL || err == ENOTSUP)
            #elif defined(_MSC_VER)
            #define VFS_POSIX_STREAM_STDIO_ERR_CATCH (err == EINVAL || err == EBADF)
            #else
            #define VFS_POSIX_STREAM_STDIO_ERR_CATCH (err == EINVAL)
            #endif
            MP_HAL_RETRY_SYSCALL(ret, fsync(o->fd), {
                if (VFS_POSIX_STREAM_STDIO_ERR_CATCH
                    && (o->fd == STDIN_FILENO || o->fd == STDOUT_FILENO || o->fd == STDERR_FILENO)) {
                    return 0;
                }
                *errcode = err;
                return MP_STREAM_ERROR;
            });
            return 0;
        }
        case MP_STREAM_SEEK: {
            struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)arg;
            MP_THREAD_GIL_EXIT();
            off_t off = lseek(o->fd, s->offset, s->whence);
            MP_THREAD_GIL_ENTER();
            if (off == (off_t)-1) {
                *errcode = errno;
                return MP_STREAM_ERROR;
            }
            s->offset = off;
            return 0;
        }
        case MP_STREAM_CLOSE:
            if (o->fd >= 0) {
                MP_THREAD_GIL_EXIT();
                close(o->fd);
                MP_THREAD_GIL_ENTER();
            }
            o->fd = -1;
            return 0;
        case MP_STREAM_GET_FILENO:
            return o->fd;
        #if MICROPY_PY_SELECT && !MICROPY_PY_SELECT_POSIX_OPTIMISATIONS
        case MP_STREAM_POLL: {
            #ifdef _WIN32
            mp_raise_NotImplementedError(MP_ERROR_TEXT("poll on file not available on win32"));
            #else
            mp_uint_t ret = 0;
            uint8_t pollevents = 0;
            if (arg & MP_STREAM_POLL_RD) {
                pollevents |= POLLIN;
            }
            if (arg & MP_STREAM_POLL_WR) {
                pollevents |= POLLOUT;
            }
            struct pollfd pfd = { .fd = o->fd, .events = pollevents };
            if (poll(&pfd, 1, 0) > 0) {
                if (pfd.revents & POLLIN) {
                    ret |= MP_STREAM_POLL_RD;
                }
                if (pfd.revents & POLLOUT) {
                    ret |= MP_STREAM_POLL_WR;
                }
                if (pfd.revents & POLLERR) {
                    ret |= MP_STREAM_POLL_ERR;
                }
                if (pfd.revents & POLLHUP) {
                    ret |= MP_STREAM_POLL_HUP;
                }
                if (pfd.revents & POLLNVAL) {
                    ret |= MP_STREAM_POLL_NVAL;
                }
            }
            return ret;
            #endif
        }
        #endif
        default:
            *errcode = EINVAL;
            return MP_STREAM_ERROR;
    }
}

static const mp_rom_map_elem_t vfs_posix_rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&vfs_posix_file_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};

static MP_DEFINE_CONST_DICT(vfs_posix_rawfile_locals_dict, vfs_posix_rawfile_locals_dict_table);

static const mp_stream_p_t vfs_posix_fileio_stream_p = {
    .read = vfs_posix_file_read,
    .write = vfs_posix_file_write,
    .ioctl = vfs_posix_file_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_posix_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, vfs_posix_file_print,
    protocol, &vfs_posix_fileio_stream_p,
    locals_dict, &vfs_posix_rawfile_locals_dict
    );

static const mp_stream_p_t vfs_posix_textio_stream_p = {
    .read = vfs_posix_file_read,
    .write = vfs_posix_file_write,
    .ioctl = vfs_posix_file_ioctl,
    .is_text = true,
};

#if MICROPY_PY_SYS_STDIO_BUFFER

mp_obj_vfs_posix_file_t mp_sys_stdin_buffer_obj = {{&mp_type_vfs_posix_fileio}, STDIN_FILENO};
mp_obj_vfs_posix_file_t mp_sys_stdout_buffer_obj = {{&mp_type_vfs_posix_fileio}, STDOUT_FILENO};
mp_obj_vfs_posix_file_t mp_sys_stderr_buffer_obj = {{&mp_type_vfs_posix_fileio}, STDERR_FILENO};

// Forward declarations.
mp_obj_vfs_posix_file_t mp_sys_stdin_obj;
mp_obj_vfs_posix_file_t mp_sys_stdout_obj;
mp_obj_vfs_posix_file_t mp_sys_stderr_obj;

static void vfs_posix_textio_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    if (dest[0] != MP_OBJ_NULL) {
        // These objects are read-only.
        return;
    }

    if (attr == MP_QSTR_buffer) {
        // Implement the `buffer` attribute only on std{in,out,err} instances.
        if (MP_OBJ_TO_PTR(self_in) == &mp_sys_stdin_obj) {
            dest[0] = MP_OBJ_FROM_PTR(&mp_sys_stdin_buffer_obj);
            return;
        }
        if (MP_OBJ_TO_PTR(self_in) == &mp_sys_stdout_obj) {
            dest[0] = MP_OBJ_FROM_PTR(&mp_sys_stdout_buffer_obj);
            return;
        }
        if (MP_OBJ_TO_PTR(self_in) == &mp_sys_stderr_obj) {
            dest[0] = MP_OBJ_FROM_PTR(&mp_sys_stderr_buffer_obj);
            return;
        }
    }

    // Any other attribute - forward to locals dict.
    dest[1] = MP_OBJ_SENTINEL;
};

#define VFS_POSIX_TEXTIO_TYPE_ATTR attr, vfs_posix_textio_attr,

#else

#define VFS_POSIX_TEXTIO_TYPE_ATTR

#endif // MICROPY_PY_SYS_STDIO_BUFFER

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_posix_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, vfs_posix_file_print,
    protocol, &vfs_posix_textio_stream_p,
    VFS_POSIX_TEXTIO_TYPE_ATTR
    locals_dict, &vfs_posix_rawfile_locals_dict
    );

mp_obj_vfs_posix_file_t mp_sys_stdin_obj = {{&mp_type_vfs_posix_textio}, STDIN_FILENO};
mp_obj_vfs_posix_file_t mp_sys_stdout_obj = {{&mp_type_vfs_posix_textio}, STDOUT_FILENO};
mp_obj_vfs_posix_file_t mp_sys_stderr_obj = {{&mp_type_vfs_posix_textio}, STDERR_FILENO};

#endif // MICROPY_VFS_POSIX
