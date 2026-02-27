/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

#include "py/builtin.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared/runtime/semihosting.h"
#include "extmod/vfs.h"

#include "vfs_semihosting.h"

#if MICROPY_VFS_SEMIHOSTING

#if !MICROPY_ENABLE_FINALISER
#error "The semihosting VFS requires MICROPY_ENABLE_FINALISER"
#endif

#include <unistd.h>

// This VFS is here mostly to support persistent code loading from the host's
// filesystem.  Most of the limitations here stem from the functions exposed
// by the semihosting specification, with the most visible being the lack of
// directory creation and file enumeration function calls.  This code doesn't
// have security in mind, so path management is cut down to its bare minimum
// needed to get file I/O working.

typedef struct _mp_obj_vfs_semihosting_t {
    mp_obj_base_t base;
    vstr_t root;
    size_t root_len;
    bool readonly;
    vstr_t cwd;
} mp_obj_vfs_semihosting_t;

static void trim_separators(vstr_t *vstr) {
    while (vstr_len(vstr) > 0) {
        if (vstr->buf[vstr_len(vstr) - 1] != '/') {
            break;
        }
        vstr_cut_tail_bytes(vstr, 1);
    }
}

static mp_obj_t build_path(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_semihosting_t *self = MP_OBJ_TO_PTR(self_in);
    size_t path_length = 0;
    const char *path = mp_obj_str_get_data(path_in, &path_length);
    vstr_t *host_path = vstr_new(self->root_len + vstr_len(&self->cwd) + path_length + 2);
    vstr_add_strn(host_path, self->root.buf, self->root_len);
    if (path[0] != '/') {
        vstr_add_str(host_path, self->cwd.buf);
    }
    vstr_add_str(host_path, path);
    trim_separators(host_path);
    mp_obj_t host_path_in = mp_obj_new_str_from_vstr(host_path);
    vstr_free(host_path);
    return host_path_in;
}

static mp_obj_t vfs_semihosting_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    mp_obj_vfs_semihosting_t *vfs = mp_obj_malloc(mp_obj_vfs_semihosting_t, type);
    vstr_init(&vfs->root, 0);
    if (n_args == 1) {
        size_t root_length = 0;
        const char *root = mp_obj_str_get_data(args[0], &root_length);
        vstr_add_strn(&vfs->root, root, root_length);
    }
    trim_separators(&vfs->root);
    vfs->root_len = vfs->root.len;
    vfs->readonly = false;
    vstr_init(&vfs->cwd, 1);
    vstr_add_char(&vfs->cwd, '/');

    return MP_OBJ_FROM_PTR(vfs);
}

static mp_obj_t vfs_semihosting_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    mp_obj_vfs_semihosting_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_true(readonly)) {
        self->readonly = true;
    }
    if (mp_obj_is_true(mkfs)) {
        mp_raise_OSError(MP_EPERM);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_semihosting_mount_obj, vfs_semihosting_mount);

static mp_obj_t vfs_semihosting_umount(mp_obj_t self_in) {
    (void)self_in;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_semihosting_umount_obj, vfs_semihosting_umount);

static mp_obj_t vfs_semihosting_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    return mp_vfs_semihosting_file_open(&mp_type_vfs_semihosting_textio, build_path(self_in, path_in), mode_in);
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_semihosting_open_obj, vfs_semihosting_open);

static mp_obj_t vfs_semihosting_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_semihosting_t *self = MP_OBJ_TO_PTR(self_in);
    size_t path_length = 0;
    const char *path = mp_obj_str_get_data(path_in, &path_length);
    if (path[0] != '/') {
        vstr_add_strn(&self->cwd, path, path_length);
    } else {
        vstr_clear(&self->cwd);
        vstr_init(&self->cwd, path_length + 1);
        vstr_add_strn(&self->cwd, path, path_length);
    }
    trim_separators(&self->cwd);
    vstr_add_char(&self->cwd, '/');
    return MP_OBJ_NEW_SMALL_INT(0);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_chdir_obj, vfs_semihosting_chdir);

static mp_obj_t vfs_semihosting_getcwd(mp_obj_t self_in) {
    mp_obj_vfs_semihosting_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_str_from_vstr(&self->cwd);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_semihosting_getcwd_obj, vfs_semihosting_getcwd);

static mp_obj_t vfs_semihosting_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Semihosting VFS cannot list files."));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_ilistdir_obj, vfs_semihosting_ilistdir);

static mp_obj_t vfs_semihosting_mkdir(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Semihosting VFS cannot create directories."));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_mkdir_obj, vfs_semihosting_mkdir);

static mp_obj_t vfs_semihosting_remove(mp_obj_t self_in, mp_obj_t path_in) {
    const char *host_path = mp_obj_str_get_str(build_path(self_in, path_in));
    MP_THREAD_GIL_EXIT();
    int result = mp_semihosting_remove(host_path);
    MP_THREAD_GIL_ENTER();
    if (result != 0) {
        mp_raise_OSError(result);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_remove_obj, vfs_semihosting_remove);

static mp_obj_t vfs_semihosting_rename(mp_obj_t self_in, mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_host_path = mp_obj_str_get_str(build_path(self_in, old_path_in));
    const char *new_host_path = mp_obj_str_get_str(build_path(self_in, new_path_in));
    MP_THREAD_GIL_EXIT();
    int result = mp_semihosting_rename(old_host_path, new_host_path);
    MP_THREAD_GIL_ENTER();
    if (result != 0) {
        mp_raise_OSError(result);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_semihosting_rename_obj, vfs_semihosting_rename);

static mp_obj_t vfs_semihosting_rmdir(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Semihosting VFS cannot remove directories."));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_rmdir_obj, vfs_semihosting_rmdir);

static mp_obj_t vfs_semihosting_stat(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Semihosting VFS cannot obtain information for a file."));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_stat_obj, vfs_semihosting_stat);

#if MICROPY_PY_OS_STATVFS
static mp_obj_t vfs_semihosting_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    mp_raise_NotImplementedError(MP_ERROR_TEXT("Semihosting VFS cannot obtain information for a filesystem."));
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_semihosting_statvfs_obj, vfs_semihosting_statvfs);
#endif

static const mp_rom_map_elem_t vfs_semihosting_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount),    MP_ROM_PTR(&vfs_semihosting_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount),   MP_ROM_PTR(&vfs_semihosting_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_open),     MP_ROM_PTR(&vfs_semihosting_open_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir),    MP_ROM_PTR(&vfs_semihosting_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd),   MP_ROM_PTR(&vfs_semihosting_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_semihosting_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir),    MP_ROM_PTR(&vfs_semihosting_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove),   MP_ROM_PTR(&vfs_semihosting_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename),   MP_ROM_PTR(&vfs_semihosting_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir),    MP_ROM_PTR(&vfs_semihosting_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat),     MP_ROM_PTR(&vfs_semihosting_stat_obj) },
    #if MICROPY_PY_OS_STATVFS
    { MP_ROM_QSTR(MP_QSTR_statvfs),  MP_ROM_PTR(&vfs_semihosting_statvfs_obj) },
    #endif
};
static MP_DEFINE_CONST_DICT(vfs_semihosting_locals_dict, vfs_semihosting_locals_dict_table);

static mp_import_stat_t vfs_semihosting_import_stat(void *self_in, const char *path) {
    // QEMU's semihosting implementation returns ENOENT when opening a
    // directory, so it's not really possible to know whether the given path
    // is a directory or just does not exist.

    (void)self_in;
    MP_THREAD_GIL_EXIT();
    int fd = mp_semihosting_open(path, "r");
    MP_THREAD_GIL_ENTER();
    int errcode = 0;
    if (fd < 0) {
        MP_THREAD_GIL_EXIT();
        errcode = mp_semihosting_errno();
        MP_THREAD_GIL_ENTER();
    } else {
        MP_THREAD_GIL_EXIT();
        mp_semihosting_close(fd);
        MP_THREAD_GIL_ENTER();
    }
    switch (errcode) {
        case ENOENT:
            return MP_IMPORT_STAT_NO_EXIST;
        case 0:
            return MP_IMPORT_STAT_FILE;
        default:
            mp_raise_OSError(errcode);
    }
}

static const mp_vfs_proto_t vfs_semihosting_proto = {
    .import_stat = vfs_semihosting_import_stat,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_semihosting,
    MP_QSTR_VfsSemihosting,
    MP_TYPE_FLAG_NONE,
    make_new, vfs_semihosting_make_new,
    protocol, &vfs_semihosting_proto,
    locals_dict, &vfs_semihosting_locals_dict
    );

typedef struct _mp_obj_vfs_semihosting_file_t {
    mp_obj_base_t base;
    mp_int_t fd;
    mp_int_t pointer;
} mp_obj_vfs_semihosting_file_t;

static void check_fd_is_open(const mp_obj_vfs_semihosting_file_t *o) {
    if (o->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}

static void vfs_semihosting_file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_vfs_semihosting_file_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<io.%s %d>", mp_obj_get_type_str(self_in), self->fd);
}

mp_obj_t mp_vfs_semihosting_file_open(const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in) {
    size_t mode_length = 0;
    const char *mode = mp_obj_str_get_data(mode_in, &mode_length);
    vstr_t *mode_vstr = vstr_new(mode_length);
    for (size_t index = 0; index < mode_length; index++) {
        switch (mode[index]) {
            case 'r':
            case 'w':
            case 'a':
            case '+':
                vstr_add_char(mode_vstr, mode[index]);
                break;
            case 't':
                type = &mp_type_vfs_semihosting_textio;
                break;
            case 'b':
                type = &mp_type_vfs_semihosting_fileio;
                vstr_add_char(mode_vstr, mode[index]);
                break;
            default:
                break;
        }
    }
    mp_obj_vfs_semihosting_file_t *o = mp_obj_malloc_with_finaliser(mp_obj_vfs_semihosting_file_t, type);
    MP_THREAD_GIL_EXIT();
    o->fd = mp_semihosting_open(mp_obj_str_get_str(file_in), vstr_null_terminated_str(mode_vstr));
    MP_THREAD_GIL_ENTER();
    vstr_free(mode_vstr);
    if (o->fd < 0) {
        MP_THREAD_GIL_EXIT();
        int errcode = mp_semihosting_errno();
        MP_THREAD_GIL_ENTER();
        mp_raise_OSError(errcode);
    }
    o->pointer = 0;
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t vfs_semihosting_file_fileno(mp_obj_t self_in) {
    mp_obj_vfs_semihosting_file_t *self = MP_OBJ_TO_PTR(self_in);
    check_fd_is_open(self);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_semihosting_file_fileno_obj, vfs_semihosting_file_fileno);

static mp_obj_t vfs_semihosting_file_tell(mp_obj_t self_in) {
    mp_obj_vfs_semihosting_file_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->pointer);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_semihosting_file_tell_obj, vfs_semihosting_file_tell);

static mp_uint_t vfs_semihosting_file_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_semihosting_file_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    ssize_t r;
    MP_THREAD_GIL_EXIT();
    r = mp_semihosting_read(o->fd, buf, size);
    MP_THREAD_GIL_ENTER();
    if (r < 0) {
        MP_THREAD_GIL_EXIT();
        *errcode = mp_semihosting_errno();
        MP_THREAD_GIL_ENTER();
        return MP_STREAM_ERROR;
    }
    assert(r <= size && "Semihosting read more data than requested.");
    o->pointer += (size - r);
    return (mp_uint_t)(size - r);
}

static mp_uint_t vfs_semihosting_file_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_semihosting_file_t *o = MP_OBJ_TO_PTR(o_in);
    check_fd_is_open(o);
    #if MICROPY_PY_OS_DUPTERM
    if (o->fd <= STDERR_FILENO) {
        mp_hal_stdout_tx_strn(buf, size);
        return size;
    }
    #endif
    ssize_t r;
    MP_THREAD_GIL_EXIT();
    r = mp_semihosting_write(o->fd, buf, size);
    MP_THREAD_GIL_ENTER();
    if (r == 0) {
        o->pointer += size;
        return (mp_uint_t)size;
    }
    MP_THREAD_GIL_EXIT();
    *errcode = mp_semihosting_errno();
    MP_THREAD_GIL_ENTER();
    return MP_STREAM_ERROR;
}

static mp_uint_t vfs_semihosting_file_seek(mp_obj_vfs_semihosting_file_t *o, uintptr_t arg, int *errcode) {
    // The semihosting specifications followed by RISC-V and Arm do not account
    // for a `whence` parameter, and they only operate on absolute offsets.
    // `whence` support is emulated by computing the current file size and then
    // calculate the effective offset accounting from different starting points.

    MP_THREAD_GIL_EXIT();
    mp_int_t known_size = mp_semihosting_flen(o->fd);
    MP_THREAD_GIL_ENTER();
    if (known_size < 0) {
        MP_THREAD_GIL_EXIT();
        *errcode = mp_semihosting_errno();
        MP_THREAD_GIL_ENTER();
        return MP_STREAM_ERROR;
    }
    struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)arg;
    mp_int_t offset = s->offset;
    switch (s->whence) {
        case MP_SEEK_SET:
            break;
        case MP_SEEK_CUR:
            offset = o->pointer + offset;
            break;
        case MP_SEEK_END:
            offset = known_size + offset;
            break;
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }

    if (offset >= known_size || offset < 0) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t seek_result = mp_semihosting_seek(o->fd, (mp_uint_t)offset);
    MP_THREAD_GIL_ENTER();
    if (seek_result < 0) {
        MP_THREAD_GIL_EXIT();
        *errcode = mp_semihosting_errno();
        MP_THREAD_GIL_ENTER();
        return MP_STREAM_ERROR;
    }
    o->pointer = offset;
    return 0;
}

static mp_uint_t vfs_semihosting_file_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_vfs_semihosting_file_t *o = MP_OBJ_TO_PTR(o_in);

    if (request != MP_STREAM_CLOSE) {
        check_fd_is_open(o);
    }

    switch (request) {
        case MP_STREAM_FLUSH:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("file flushing not available with semihosting"));
            return 0;

        case MP_STREAM_SEEK:
            return vfs_semihosting_file_seek(o, arg, errcode);

        case MP_STREAM_CLOSE:
            if (o->fd >= 0) {
                MP_THREAD_GIL_EXIT();
                mp_semihosting_close(o->fd);
                MP_THREAD_GIL_ENTER();
            }
            o->fd = -1;
            o->pointer = -1;
            return 0;

        case MP_STREAM_GET_FILENO:
            return o->fd;

        case MP_STREAM_POLL:
            mp_raise_NotImplementedError(MP_ERROR_TEXT("poll on file not available with semihosting"));
            return 0;

        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

static const mp_rom_map_elem_t vfs_semihosting_rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno),    MP_ROM_PTR(&vfs_semihosting_file_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),      MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto),  MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline),  MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write),     MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek),      MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell),      MP_ROM_PTR(&vfs_semihosting_file_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush),     MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close),     MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__),   MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),  MP_ROM_PTR(&mp_stream___exit___obj) },
};

static MP_DEFINE_CONST_DICT(vfs_semihosting_rawfile_locals_dict, vfs_semihosting_rawfile_locals_dict_table);

static const mp_stream_p_t vfs_semihosting_fileio_stream_p = {
    .read = vfs_semihosting_file_read,
    .write = vfs_semihosting_file_write,
    .ioctl = vfs_semihosting_file_ioctl,
};

static const mp_stream_p_t vfs_semihosting_textio_stream_p = {
    .read = vfs_semihosting_file_read,
    .write = vfs_semihosting_file_write,
    .ioctl = vfs_semihosting_file_ioctl,
    .is_text = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_semihosting_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, vfs_semihosting_file_print,
    protocol, &vfs_semihosting_fileio_stream_p,
    locals_dict, &vfs_semihosting_rawfile_locals_dict
    );

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_semihosting_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, vfs_semihosting_file_print,
    protocol, &vfs_semihosting_textio_stream_p,
    locals_dict, &vfs_semihosting_rawfile_locals_dict
    );

#endif
