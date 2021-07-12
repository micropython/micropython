// SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
// SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"
#if MICROPY_VFS && MICROPY_VFS_FAT

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "lib/oofatfs/ff.h"
#include "extmod/vfs_fat.h"
#include "supervisor/filesystem.h"

// this table converts from FRESULT to POSIX errno
const byte fresult_to_errno_table[20] = {
    [FR_OK] = 0,
    [FR_DISK_ERR] = MP_EIO,
    [FR_INT_ERR] = MP_EIO,
    [FR_NOT_READY] = MP_EBUSY,
    [FR_NO_FILE] = MP_ENOENT,
    [FR_NO_PATH] = MP_ENOENT,
    [FR_INVALID_NAME] = MP_EINVAL,
    [FR_DENIED] = MP_EACCES,
    [FR_EXIST] = MP_EEXIST,
    [FR_INVALID_OBJECT] = MP_EINVAL,
    [FR_WRITE_PROTECTED] = MP_EROFS,
    [FR_INVALID_DRIVE] = MP_ENODEV,
    [FR_NOT_ENABLED] = MP_ENODEV,
    [FR_NO_FILESYSTEM] = MP_ENODEV,
    [FR_MKFS_ABORTED] = MP_EIO,
    [FR_TIMEOUT] = MP_EIO,
    [FR_LOCKED] = MP_EIO,
    [FR_NOT_ENOUGH_CORE] = MP_ENOMEM,
    [FR_TOO_MANY_OPEN_FILES] = MP_EMFILE,
    [FR_INVALID_PARAMETER] = MP_EINVAL,
};

STATIC void file_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_printf(print, "<io.%q %p>", mp_obj_get_type_qstr(self_in), MP_OBJ_TO_PTR(self_in));
}

STATIC mp_uint_t file_obj_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    pyb_file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    UINT sz_out;
    FRESULT res = f_read(&self->fp, buf, size, &sz_out);
    if (res != FR_OK) {
        *errcode = fresult_to_errno_table[res];
        return MP_STREAM_ERROR;
    }
    return sz_out;
}

STATIC mp_uint_t file_obj_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    pyb_file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    UINT sz_out;
    FRESULT res = f_write(&self->fp, buf, size, &sz_out);
    if (res != FR_OK) {
        *errcode = fresult_to_errno_table[res];
        return MP_STREAM_ERROR;
    }
    if (sz_out != size) {
        // The FatFS documentation says that this means disk full.
        *errcode = MP_ENOSPC;
        return MP_STREAM_ERROR;
    }
    return sz_out;
}


STATIC mp_obj_t file_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(file_obj___exit___obj, 4, 4, file_obj___exit__);

STATIC mp_uint_t file_obj_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    pyb_file_obj_t *self = MP_OBJ_TO_PTR(o_in);

    if (request == MP_STREAM_SEEK) {
        struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)(uintptr_t)arg;

        switch (s->whence) {
            case 0: // SEEK_SET
                f_lseek(&self->fp, s->offset);
                break;

            case 1: // SEEK_CUR
                f_lseek(&self->fp, f_tell(&self->fp) + s->offset);
                break;

            case 2: // SEEK_END
                f_lseek(&self->fp, f_size(&self->fp) + s->offset);
                break;
        }

        s->offset = f_tell(&self->fp);
        return 0;

    } else if (request == MP_STREAM_FLUSH) {
        FRESULT res = f_sync(&self->fp);
        if (res != FR_OK) {
            *errcode = fresult_to_errno_table[res];
            return MP_STREAM_ERROR;
        }
        return 0;

    } else if (request == MP_STREAM_CLOSE) {
        // if fs==NULL then the file is closed and in that case this method is a no-op
        if (self->fp.obj.fs != NULL) {
            FRESULT res = f_close(&self->fp);
            if (res != FR_OK) {
                *errcode = fresult_to_errno_table[res];
                return MP_STREAM_ERROR;
            }
        }
        return 0;

    } else {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

// Note: encoding is ignored for now; it's also not a valid kwarg for CPython's FileIO,
// but by adding it here we can use one single mp_arg_t array for open() and FileIO's constructor
STATIC const mp_arg_t file_open_args[] = {
    { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_NONE} },
    { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_r)} },
    { MP_QSTR_encoding, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_NONE} },
};
#define FILE_OPEN_NUM_ARGS MP_ARRAY_SIZE(file_open_args)

STATIC mp_obj_t file_open(fs_user_mount_t *vfs, const mp_obj_type_t *type, mp_arg_val_t *args) {
    int mode = 0;
    const char *mode_s = mp_obj_str_get_str(args[1].u_obj);
    // TODO make sure only one of r, w, x, a, and b, t are specified
    while (*mode_s) {
        switch (*mode_s++) {
            case 'r':
                mode |= FA_READ;
                break;
            case 'w':
                mode |= FA_WRITE | FA_CREATE_ALWAYS;
                break;
            case 'x':
                mode |= FA_WRITE | FA_CREATE_NEW;
                break;
            case 'a':
                mode |= FA_WRITE | FA_OPEN_ALWAYS;
                break;
            case '+':
                mode |= FA_READ | FA_WRITE;
                break;
            #if MICROPY_PY_IO_FILEIO
            case 'b':
                type = &mp_type_vfs_fat_fileio;
                break;
            #endif
            case 't':
                type = &mp_type_vfs_fat_textio;
                break;
        }
    }
    assert(vfs != NULL);
    if ((mode & FA_WRITE) != 0 && !filesystem_is_writable_by_python(vfs)) {
        mp_raise_OSError(MP_EROFS);
    }

    pyb_file_obj_t *o = m_new_obj_with_finaliser(pyb_file_obj_t);
    o->base.type = type;

    const char *fname = mp_obj_str_get_str(args[0].u_obj);
    FRESULT res = f_open(&vfs->fatfs, &o->fp, fname, mode);
    if (res != FR_OK) {
        m_del_obj(pyb_file_obj_t, o);
        mp_raise_OSError_errno_str(fresult_to_errno_table[res], args[0].u_obj);
    }
    // If we're reading, turn on fast seek.
    if (mode == FA_READ) {
        // One call to determine how much space we need.
        DWORD temp_table[2];
        temp_table[0] = 2;
        o->fp.cltbl = temp_table;
        f_lseek(&o->fp, CREATE_LINKMAP);
        DWORD size = (temp_table[0] + 1) * 2;

        // Now allocate the size and construct the map.
        o->fp.cltbl = m_malloc_maybe(size * sizeof(DWORD), false);
        if (o->fp.cltbl != NULL) {
            o->fp.cltbl[0] = size;
            res = f_lseek(&o->fp, CREATE_LINKMAP);
            if (res != FR_OK) {
                o->fp.cltbl = NULL;
            }
        }
    }

    // for 'a' mode, we must begin at the end of the file
    if ((mode & FA_OPEN_ALWAYS) != 0) {
        f_lseek(&o->fp, f_size(&o->fp));
    }

    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t file_obj_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    mp_arg_parse_all(n_args, args, kw_args, FILE_OPEN_NUM_ARGS, file_open_args, arg_vals);
    return file_open(NULL, type, arg_vals);
}

// TODO gc hook to close the file if not already closed

STATIC const mp_rom_map_elem_t vfs_fat_rawfile_locals_dict_table[] = {
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
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&file_obj___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(vfs_fat_rawfile_locals_dict, vfs_fat_rawfile_locals_dict_table);

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t vfs_fat_fileio_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
};

const mp_obj_type_t mp_type_vfs_fat_fileio = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_FileIO,
    .print = file_obj_print,
    .make_new = file_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&vfs_fat_rawfile_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &vfs_fat_fileio_stream_p,
        ),
};
#endif

STATIC const mp_stream_p_t vfs_fat_textio_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
    .is_text = true,
};

const mp_obj_type_t mp_type_vfs_fat_textio = {
    { &mp_type_type },
    .flags = MP_TYPE_FLAG_EXTENDED,
    .name = MP_QSTR_TextIOWrapper,
    .print = file_obj_print,
    .make_new = file_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&vfs_fat_rawfile_locals_dict,
    MP_TYPE_EXTENDED_FIELDS(
        .getiter = mp_identity_getiter,
        .iternext = mp_stream_unbuffered_iter,
        .protocol = &vfs_fat_textio_stream_p,
        ),
};

// Factory function for I/O stream classes
STATIC mp_obj_t fatfs_builtin_open_self(mp_obj_t self_in, mp_obj_t path, mp_obj_t mode) {
    // TODO: analyze buffering args and instantiate appropriate type
    fs_user_mount_t *self = MP_OBJ_TO_PTR(self_in);
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    arg_vals[0].u_obj = path;
    arg_vals[1].u_obj = mode;
    arg_vals[2].u_obj = mp_const_none;
    return file_open(self, &mp_type_vfs_fat_textio, arg_vals);
}
MP_DEFINE_CONST_FUN_OBJ_3(fat_vfs_open_obj, fatfs_builtin_open_self);

#endif // MICROPY_VFS && MICROPY_VFS_FAT
