/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/vfs_littlefs.h"

#if MICROPY_VFS && MICROPY_VFS_LITTLEFS

typedef struct _vfs_littlefs_file_t {
    mp_obj_base_t base;
    mp_obj_vfs_littlefs_t *vfs;
    lfs_file_t file;
    struct lfs_file_config cfg;
    uint8_t file_buffer[0];
} vfs_littlefs_file_t;

STATIC void vfs_littlefs_file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    (void)kind;
    mp_printf(print, "<io.%s>", mp_obj_get_type_str(self_in));
}

mp_obj_t mp_vfs_littlefs_file_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);

    int flags = 0;
    const mp_obj_type_t *type = &mp_type_vfs_littlefs_textio;
    const char *mode_str = mp_obj_str_get_str(mode_in);
    for (; *mode_str; ++mode_str) {
        int new_flags = 0;
        switch (*mode_str) {
            case 'r':
                new_flags = LFS_O_RDONLY;
                break;
            case 'w':
                new_flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
                break;
            case 'x':
                new_flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_EXCL;
                break;
            case 'a':
                new_flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
                break;
            case '+':
                flags |= LFS_O_RDWR;
                break;
            #if MICROPY_PY_IO_FILEIO
            case 'b':
                type = &mp_type_vfs_littlefs_fileio;
                break;
            #endif
            case 't':
                type = &mp_type_vfs_littlefs_textio;
                break;
        }
        if (new_flags) {
            if (flags) {
                mp_raise_ValueError(NULL);
            }
            flags = new_flags;
        }
    }
    if (flags == 0) {
        flags = LFS_O_RDONLY;
    }

    vfs_littlefs_file_t *o = m_new_obj_var_with_finaliser(vfs_littlefs_file_t, uint8_t, self->lfs.cfg->prog_size);
    o->base.type = type;
    o->vfs = self;
    #if !MICROPY_GC_CONSERVATIVE_CLEAR
    memset(&o->file, 0, sizeof(o->file));
    memset(&o->cfg, 0, sizeof(o->cfg));
    #endif
    o->cfg.buffer = &o->file_buffer[0];

    const char *path = mp_obj_vfs_littlefs_make_path(self, path_in);
    int ret = lfs_file_opencfg(&self->lfs, &o->file, path, flags, &o->cfg);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t vfs_littlefs_file___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(vfs_littlefs_file___exit___obj, 4, 4, vfs_littlefs_file___exit__);

STATIC mp_uint_t vfs_littlefs_file_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    vfs_littlefs_file_t *self = MP_OBJ_TO_PTR(self_in);
    lfs_ssize_t sz = lfs_file_read(&self->vfs->lfs, &self->file, buf, size);
    if (sz < 0) {
        *errcode = -sz;
        return MP_STREAM_ERROR;
    }
    return sz;
}

STATIC mp_uint_t vfs_littlefs_file_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    vfs_littlefs_file_t *self = MP_OBJ_TO_PTR(self_in);
    lfs_ssize_t sz = lfs_file_write(&self->vfs->lfs, &self->file, buf, size);
    if (sz < 0) {
        *errcode = -sz;
        return MP_STREAM_ERROR;
    }
    return sz;
}

STATIC mp_uint_t vfs_littlefs_file_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    vfs_littlefs_file_t *self = MP_OBJ_TO_PTR(self_in);

    if (request == MP_STREAM_SEEK) {
        struct mp_stream_seek_t *s = (struct mp_stream_seek_t*)(uintptr_t)arg;
        int res = lfs_file_seek(&self->vfs->lfs, &self->file, s->offset, s->whence);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        res = lfs_file_tell(&self->vfs->lfs, &self->file);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        s->offset = res;
        return 0;
    } else if (request == MP_STREAM_FLUSH) {
        int res = lfs_file_sync(&self->vfs->lfs, &self->file);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        return 0;
    } else if (request == MP_STREAM_CLOSE) {
        int res = lfs_file_close(&self->vfs->lfs, &self->file);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        return 0;
    } else {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

STATIC const mp_rom_map_elem_t vfs_littlefs_file_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&vfs_littlefs_file___exit___obj) },
};
STATIC MP_DEFINE_CONST_DICT(vfs_littlefs_file_locals_dict, vfs_littlefs_file_locals_dict_table);

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t vfs_littlefs_fileio_stream_p = {
    .read = vfs_littlefs_file_read,
    .write = vfs_littlefs_file_write,
    .ioctl = vfs_littlefs_file_ioctl,
};

const mp_obj_type_t mp_type_vfs_littlefs_fileio = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = vfs_littlefs_file_print,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &vfs_littlefs_fileio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&vfs_littlefs_file_locals_dict,
};
#endif

STATIC const mp_stream_p_t vfs_littlefs_textio_stream_p = {
    .read = vfs_littlefs_file_read,
    .write = vfs_littlefs_file_write,
    .ioctl = vfs_littlefs_file_ioctl,
    .is_text = true,
};

const mp_obj_type_t mp_type_vfs_littlefs_textio = {
    { &mp_type_type },
    .name = MP_QSTR_TextIOWrapper,
    .print = vfs_littlefs_file_print,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &vfs_littlefs_textio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&vfs_littlefs_file_locals_dict,
};

#endif // MICROPY_VFS && MICROPY_VFS_LITTLEFS
