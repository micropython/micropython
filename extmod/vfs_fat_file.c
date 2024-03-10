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
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(vfs_fat_rawfile_locals_dict, vfs_fat_rawfile_locals_dict_table);

STATIC const mp_stream_p_t vfs_fat_fileio_stream_p = {
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_fat_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, file_obj_print,
    protocol, &vfs_fat_fileio_stream_p,
    locals_dict, &vfs_fat_rawfile_locals_dict
    );

STATIC const mp_stream_p_t vfs_fat_textio_stream_p = {
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
    .is_text = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_fat_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, file_obj_print,
    protocol, &vfs_fat_textio_stream_p,
    locals_dict, &vfs_fat_rawfile_locals_dict
    );

// Factory function for I/O stream classes
STATIC mp_obj_t fat_vfs_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    fs_user_mount_t *self = MP_OBJ_TO_PTR(self_in);

    const mp_obj_type_t *type = &mp_type_vfs_fat_textio;
    int mode = 0;
    const char *mode_s = mp_obj_str_get_str(mode_in);
    uint32_t rwxa_count = 0;
    uint32_t bt_count = 0;
    uint32_t plus_count = 0;
    bool bad_mode = false;
    while (*mode_s) {
        switch (*mode_s++) {
            case 'r':
                mode |= FA_READ;
                rwxa_count++;
                break;
            case 'w':
                mode |= FA_WRITE | FA_CREATE_ALWAYS;
                rwxa_count++;
                break;
            case 'x':
                mode |= FA_WRITE | FA_CREATE_NEW;
                rwxa_count++;
                break;
            case 'a':
                mode |= FA_WRITE | FA_OPEN_ALWAYS;
                rwxa_count++;
                break;
            case '+':
                mode |= FA_READ | FA_WRITE;
                plus_count++;
                break;
            case 'b':
                bt_count++;
                type = &mp_type_vfs_fat_fileio;
                break;
            case 't':
                bt_count++;
                type = &mp_type_vfs_fat_textio;
                break;
            default:
                bad_mode = true;
                break;
        }
    }

    if (rwxa_count != 1 || plus_count > 1 || bt_count > 1 || bad_mode) {
        mp_arg_error_invalid(MP_QSTR_mode);
    }

    assert(self != NULL);
    if ((mode & FA_WRITE) != 0 && !filesystem_is_writable_by_python(self)) {
        mp_raise_OSError(MP_EROFS);
    }


    pyb_file_obj_t *o = m_new_obj_with_finaliser(pyb_file_obj_t);
    o->base.type = type;

    const char *fname = mp_obj_str_get_str(path_in);
    FRESULT res = f_open(&self->fatfs, &o->fp, fname, mode);
    if (res != FR_OK) {
        m_del_obj(pyb_file_obj_t, o);
        mp_raise_OSError_errno_str(fresult_to_errno_table[res], path_in);
    }
    // CIRCUITPY-CHANGE: does fast seek.
    // If we're reading, turn on fast seek.
    if (mode == FA_READ) {
        // One call to determine how much space we need.
        DWORD temp_table[2];
        temp_table[0] = 2;
        o->fp.cltbl = temp_table;
        f_lseek(&o->fp, CREATE_LINKMAP);
        DWORD size = (temp_table[0] + 1) * 2;

        // Now allocate the size and construct the map.
        o->fp.cltbl = m_malloc_maybe(size * sizeof(DWORD));
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
MP_DEFINE_CONST_FUN_OBJ_3(fat_vfs_open_obj, fat_vfs_open);

#endif // MICROPY_VFS && MICROPY_VFS_FAT
