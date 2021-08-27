/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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

STATIC void MP_VFS_LFSx(check_open)(MP_OBJ_VFS_LFSx_FILE * self) {
    if (self->vfs == NULL) {
        mp_raise_ValueError(NULL);
    }
}

STATIC void MP_VFS_LFSx(file_print)(const mp_print_t * print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)self_in;
    (void)kind;
    mp_printf(print, "<io.%s>", mp_obj_get_type_str(self_in));
}

mp_obj_t MP_VFS_LFSx(file_open)(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);

    int flags = 0;
    const mp_obj_type_t *type = &MP_TYPE_VFS_LFSx_(_textio);
    const char *mode_str = mp_obj_str_get_str(mode_in);
    for (; *mode_str; ++mode_str) {
        int new_flags = 0;
        switch (*mode_str) {
            case 'r':
                new_flags = LFSx_MACRO(_O_RDONLY);
                break;
            case 'w':
                new_flags = LFSx_MACRO(_O_WRONLY) | LFSx_MACRO(_O_CREAT) | LFSx_MACRO(_O_TRUNC);
                break;
            case 'x':
                new_flags = LFSx_MACRO(_O_WRONLY) | LFSx_MACRO(_O_CREAT) | LFSx_MACRO(_O_EXCL);
                break;
            case 'a':
                new_flags = LFSx_MACRO(_O_WRONLY) | LFSx_MACRO(_O_CREAT) | LFSx_MACRO(_O_APPEND);
                break;
            case '+':
                flags |= LFSx_MACRO(_O_RDWR);
                break;
            #if MICROPY_PY_IO_FILEIO
            case 'b':
                type = &MP_TYPE_VFS_LFSx_(_fileio);
                break;
            #endif
            case 't':
                type = &MP_TYPE_VFS_LFSx_(_textio);
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
        flags = LFSx_MACRO(_O_RDONLY);
    }

    #if LFS_BUILD_VERSION == 1
    MP_OBJ_VFS_LFSx_FILE *o = m_new_obj_var_with_finaliser(MP_OBJ_VFS_LFSx_FILE, uint8_t, self->lfs.cfg->prog_size);
    #else
    MP_OBJ_VFS_LFSx_FILE *o = m_new_obj_var_with_finaliser(MP_OBJ_VFS_LFSx_FILE, uint8_t, self->lfs.cfg->cache_size);
    #endif
    o->base.type = type;
    o->vfs = self;
    #if !MICROPY_GC_CONSERVATIVE_CLEAR
    memset(&o->file, 0, sizeof(o->file));
    memset(&o->cfg, 0, sizeof(o->cfg));
    #endif
    o->cfg.buffer = &o->file_buffer[0];

    #if LFS_BUILD_VERSION == 2
    if (self->enable_mtime) {
        lfs_get_mtime(&o->mtime[0]);
        o->attrs[0].type = LFS_ATTR_MTIME;
        o->attrs[0].buffer = &o->mtime[0];
        o->attrs[0].size = sizeof(o->mtime);
        o->cfg.attrs = &o->attrs[0];
        o->cfg.attr_count = MP_ARRAY_SIZE(o->attrs);
    }
    #endif

    const char *path = MP_VFS_LFSx(make_path)(self, path_in);
    int ret = LFSx_API(file_opencfg)(&self->lfs, &o->file, path, flags, &o->cfg);
    if (ret < 0) {
        o->vfs = NULL;
        mp_raise_OSError(-ret);
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t MP_VFS_LFSx(file___exit__)(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(MP_VFS_LFSx(file___exit___obj), 4, 4, MP_VFS_LFSx(file___exit__));

STATIC mp_uint_t MP_VFS_LFSx(file_read)(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    MP_OBJ_VFS_LFSx_FILE *self = MP_OBJ_TO_PTR(self_in);
    MP_VFS_LFSx(check_open)(self);
    LFSx_API(ssize_t) sz = LFSx_API(file_read)(&self->vfs->lfs, &self->file, buf, size);
    if (sz < 0) {
        *errcode = -sz;
        return MP_STREAM_ERROR;
    }
    return sz;
}

STATIC mp_uint_t MP_VFS_LFSx(file_write)(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    MP_OBJ_VFS_LFSx_FILE *self = MP_OBJ_TO_PTR(self_in);
    MP_VFS_LFSx(check_open)(self);
    #if LFS_BUILD_VERSION == 2
    if (self->vfs->enable_mtime) {
        lfs_get_mtime(&self->mtime[0]);
    }
    #endif
    LFSx_API(ssize_t) sz = LFSx_API(file_write)(&self->vfs->lfs, &self->file, buf, size);
    if (sz < 0) {
        *errcode = -sz;
        return MP_STREAM_ERROR;
    }
    return sz;
}

STATIC mp_uint_t MP_VFS_LFSx(file_ioctl)(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    MP_OBJ_VFS_LFSx_FILE *self = MP_OBJ_TO_PTR(self_in);

    if (request != MP_STREAM_CLOSE) {
        MP_VFS_LFSx(check_open)(self);
    }

    if (request == MP_STREAM_SEEK) {
        struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)(uintptr_t)arg;
        int res = LFSx_API(file_seek)(&self->vfs->lfs, &self->file, s->offset, s->whence);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        res = LFSx_API(file_tell)(&self->vfs->lfs, &self->file);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        s->offset = res;
        return 0;
    } else if (request == MP_STREAM_FLUSH) {
        int res = LFSx_API(file_sync)(&self->vfs->lfs, &self->file);
        if (res < 0) {
            *errcode = -res;
            return MP_STREAM_ERROR;
        }
        return 0;
    } else if (request == MP_STREAM_CLOSE) {
        if (self->vfs == NULL) {
            return 0;
        }
        int res = LFSx_API(file_close)(&self->vfs->lfs, &self->file);
        self->vfs = NULL; // indicate a closed file
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

STATIC const mp_rom_map_elem_t MP_VFS_LFSx(file_locals_dict_table)[] = {
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
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&MP_VFS_LFSx(file___exit___obj)) },
};
STATIC MP_DEFINE_CONST_DICT(MP_VFS_LFSx(file_locals_dict), MP_VFS_LFSx(file_locals_dict_table));

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t MP_VFS_LFSx(fileio_stream_p) = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = MP_VFS_LFSx(file_read),
    .write = MP_VFS_LFSx(file_write),
    .ioctl = MP_VFS_LFSx(file_ioctl),
};

const mp_obj_type_t MP_TYPE_VFS_LFSx_(_fileio) = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_FileIO,
    .print = MP_VFS_LFSx(file_print),
    .locals_dict = (mp_obj_dict_t *)&MP_VFS_LFSx(file_locals_dict),
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &MP_VFS_LFSx(fileio_stream_p),
        ),
};
#endif

STATIC const mp_stream_p_t MP_VFS_LFSx(textio_stream_p) = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = MP_VFS_LFSx(file_read),
    .write = MP_VFS_LFSx(file_write),
    .ioctl = MP_VFS_LFSx(file_ioctl),
    .is_text = true,
};

const mp_obj_type_t MP_TYPE_VFS_LFSx_(_textio) = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_TextIOWrapper,
    .print = MP_VFS_LFSx(file_print),
    .locals_dict = (mp_obj_dict_t *)&MP_VFS_LFSx(file_locals_dict),
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &MP_VFS_LFSx(textio_stream_p),
        ),
};
