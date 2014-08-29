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
#include <errno.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "stream.h"
#include "file.h"
#include "ff.h"

extern const mp_obj_type_t mp_type_fileio;
extern const mp_obj_type_t mp_type_textio;

// this table converts from FRESULT to POSIX errno
const byte fresult_to_errno_table[20] = {
    [FR_OK] = 0,
    [FR_DISK_ERR] = EIO,
    [FR_INT_ERR] = EIO,
    [FR_NOT_READY] = EBUSY,
    [FR_NO_FILE] = ENOENT,
    [FR_NO_PATH] = ENOENT,
    [FR_INVALID_NAME] = EINVAL,
    [FR_DENIED] = EACCES,
    [FR_EXIST] = EEXIST,
    [FR_INVALID_OBJECT] = EINVAL,
    [FR_WRITE_PROTECTED] = EROFS,
    [FR_INVALID_DRIVE] = ENODEV,
    [FR_NOT_ENABLED] = ENODEV,
    [FR_NO_FILESYSTEM] = ENODEV,
    [FR_MKFS_ABORTED] = EIO,
    [FR_TIMEOUT] = EIO,
    [FR_LOCKED] = EIO,
    [FR_NOT_ENOUGH_CORE] = ENOMEM,
    [FR_TOO_MANY_OPEN_FILES] = EMFILE,
    [FR_INVALID_PARAMETER] = EINVAL,
};

typedef struct _pyb_file_obj_t {
    mp_obj_base_t base;
    FIL fp;
} pyb_file_obj_t;

void file_obj_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "<io.%s %p>", mp_obj_get_type_str(self_in), self_in);
}

STATIC mp_uint_t file_obj_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    pyb_file_obj_t *self = self_in;
    UINT sz_out;
    FRESULT res = f_read(&self->fp, buf, size, &sz_out);
    if (res != FR_OK) {
        *errcode = fresult_to_errno_table[res];
        return MP_STREAM_ERROR;
    }
    return sz_out;
}

STATIC mp_uint_t file_obj_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    pyb_file_obj_t *self = self_in;
    UINT sz_out;
    FRESULT res = f_write(&self->fp, buf, size, &sz_out);
    if (res != FR_OK) {
        *errcode = fresult_to_errno_table[res];
        return MP_STREAM_ERROR;
    }
    return sz_out;
}

STATIC mp_obj_t file_obj_flush(mp_obj_t self_in) {
    pyb_file_obj_t *self = self_in;
    f_sync(&self->fp);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(file_obj_flush_obj, file_obj_flush);

mp_obj_t file_obj_close(mp_obj_t self_in) {
    pyb_file_obj_t *self = self_in;
    f_close(&self->fp);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(file_obj_close_obj, file_obj_close);

mp_obj_t file_obj___exit__(mp_uint_t n_args, const mp_obj_t *args) {
    return file_obj_close(args[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(file_obj___exit___obj, 4, 4, file_obj___exit__);

mp_obj_t file_obj_seek(mp_uint_t n_args, const mp_obj_t *args) {
    pyb_file_obj_t *self = args[0];
    mp_int_t offset = mp_obj_get_int(args[1]);
    mp_int_t whence = 0;
    if (n_args == 3) {
        whence = mp_obj_get_int(args[2]);
    }

    switch (whence) {
        case 0: // SEEK_SET
            f_lseek(&self->fp, offset);
            break;

        case 1: // SEEK_CUR
            if (offset != 0) {
                goto error;
            }
            // no-operation
            break;

        case 2: // SEEK_END
            if (offset != 0) {
                goto error;
            }
            f_lseek(&self->fp, f_size(&self->fp));
            break;

        default:
            goto error;
    }

    return mp_obj_new_int_from_uint(f_tell(&self->fp));

error:
    // A bad whence is a ValueError, while offset!=0 is an io.UnsupportedOperation.
    // But the latter inherits ValueError (as well as IOError), so we just raise ValueError.
    nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid whence and/or offset"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(file_obj_seek_obj, 2, 3, file_obj_seek);

mp_obj_t file_obj_tell(mp_obj_t self_in) {
    pyb_file_obj_t *self = self_in;
    return mp_obj_new_int_from_uint(f_tell(&self->fp));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(file_obj_tell_obj, file_obj_tell);

STATIC mp_obj_t file_obj_make_new(mp_obj_t type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    const char *fname = mp_obj_str_get_str(args[0]);

    int mode = 0;
    if (n_args == 1) {
        mode = FA_READ;
    } else {
        const char *mode_s = mp_obj_str_get_str(args[1]);
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
                    type = (mp_obj_t)&mp_type_fileio;
                    break;
                #endif
                case 't':
                    type = (mp_obj_t)&mp_type_textio;
                    break;
            }
        }
    }

    pyb_file_obj_t *o = m_new_obj_with_finaliser(pyb_file_obj_t);
    o->base.type = type;

    FRESULT res = f_open(&o->fp, fname, mode);
    if (res != FR_OK) {
        m_del_obj(pyb_file_obj_t, o);
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(fresult_to_errno_table[res])));
    }

    // for 'a' mode, we must begin at the end of the file
    if ((mode & FA_OPEN_ALWAYS) != 0) {
        f_lseek(&o->fp, f_size(&o->fp));
    }

    return o;
}

// TODO gc hook to close the file if not already closed

STATIC const mp_map_elem_t rawfile_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_read), (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readall), (mp_obj_t)&mp_stream_readall_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), (mp_obj_t)&mp_stream_unbuffered_readline_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_readlines), (mp_obj_t)&mp_stream_unbuffered_readlines_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_write), (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flush), (mp_obj_t)&file_obj_flush_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&file_obj_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_seek), (mp_obj_t)&file_obj_seek_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_tell), (mp_obj_t)&file_obj_tell_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&file_obj_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___enter__), (mp_obj_t)&mp_identity_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___exit__), (mp_obj_t)&file_obj___exit___obj },
};

STATIC MP_DEFINE_CONST_DICT(rawfile_locals_dict, rawfile_locals_dict_table);

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t fileio_stream_p = {
    .read = file_obj_read,
    .write = file_obj_write,
};

const mp_obj_type_t mp_type_fileio = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = file_obj_print,
    .make_new = file_obj_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &fileio_stream_p,
    .locals_dict = (mp_obj_t)&rawfile_locals_dict,
};
#endif

STATIC const mp_stream_p_t textio_stream_p = {
    .read = file_obj_read,
    .write = file_obj_write,
    .is_text = true,
};

const mp_obj_type_t mp_type_textio = {
    { &mp_type_type },
    .name = MP_QSTR_TextIOWrapper,
    .print = file_obj_print,
    .make_new = file_obj_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .stream_p = &textio_stream_p,
    .locals_dict = (mp_obj_t)&rawfile_locals_dict,
};

// Factory function for I/O stream classes
STATIC mp_obj_t pyb_io_open(mp_uint_t n_args, const mp_obj_t *args) {
    // TODO: analyze mode and buffering args and instantiate appropriate type
    return file_obj_make_new((mp_obj_t)&mp_type_textio, n_args, 0, args);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_builtin_open_obj, 1, 2, pyb_io_open);
