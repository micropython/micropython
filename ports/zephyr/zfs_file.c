/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019, NXP
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

#include "zfs_file.h"
#include "py/runtime.h"
#include "py/stream.h"

STATIC void zfs_file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_printf(print, "<io.%s %p>", mp_obj_get_type_str(self_in), MP_OBJ_TO_PTR(self_in));
}

STATIC mp_uint_t zfs_file_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    zfs_file_t *self = MP_OBJ_TO_PTR(self_in);
    ssize_t sz_out = fs_read(&self->fs_file, buf, size);
    if (sz_out < 0) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    return sz_out;
}

STATIC mp_uint_t zfs_file_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    zfs_file_t *self = MP_OBJ_TO_PTR(self_in);
    ssize_t sz_out = fs_write(&self->fs_file, buf, size);
    if (sz_out < 0) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    }
    if (sz_out != size) {
        *errcode = MP_ENOSPC;
        return MP_STREAM_ERROR;
    }
    return sz_out;
}

STATIC mp_obj_t zfs_file___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return mp_stream_close(args[0]);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zfs_file___exit___obj, 4, 4, zfs_file___exit__);

STATIC mp_uint_t zfs_file_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    zfs_file_t *self = MP_OBJ_TO_PTR(self_in);
    struct mp_stream_seek_t *s = (struct mp_stream_seek_t*)(uintptr_t)arg;
    int res = 0;

    switch (request) {
    case MP_STREAM_SEEK:
        res = fs_seek(&self->fs_file, s->offset, s->whence);
        s->offset = fs_tell(&self->fs_file);
        break;

    case MP_STREAM_FLUSH:
        res = fs_sync(&self->fs_file);
        break;

    case MP_STREAM_CLOSE:
        res = fs_close(&self->fs_file);
        break;

    default:
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
        break;
    }

    if (res) {
        *errcode = MP_EIO;
        return MP_STREAM_ERROR;
    } else {
        return 0;
    }
}

// Note: encoding is ignored for now; it's also not a valid kwarg for CPython's FileIO,
// but by adding it here we can use one single mp_arg_t array for open() and FileIO's constructor
STATIC const mp_arg_t file_open_args[] = {
    { MP_QSTR_file, MP_ARG_OBJ | MP_ARG_REQUIRED, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
    { MP_QSTR_mode, MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_r)} },
    { MP_QSTR_encoding, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_rom_obj = MP_ROM_PTR(&mp_const_none_obj)} },
};
#define FILE_OPEN_NUM_ARGS MP_ARRAY_SIZE(file_open_args)

STATIC mp_obj_t file_open(const mp_obj_type_t *type, mp_arg_val_t *args) {
    const char *filename = mp_obj_str_get_str(args[0].u_obj);
    const char *mode_s = mp_obj_str_get_str(args[1].u_obj);

    while (*mode_s) {
        switch (*mode_s++) {
            #if MICROPY_PY_IO_FILEIO
            case 'b':
                type = &zfs_fileio_type;
                break;
            #endif
            case 't':
                type = &zfs_textio_type;
                break;
        }
    }

    zfs_file_t *file = m_new_obj_with_finaliser(zfs_file_t);
    file->base.type = type;

    if (fs_open(&file->fs_file, filename)) {
        m_del_obj(zfs_file_t, file);
        mp_raise_ValueError("Could not open file");
    }

    // for 'a' mode, we must begin at the end of the file
#if 0
    if ((mode & FA_OPEN_ALWAYS) != 0) {
        f_lseek(&o->fp, f_size(&o->fp));
    }
#endif

    return MP_OBJ_FROM_PTR(file);
}

STATIC mp_uint_t zfs_file_readbyte(void *data) {
    u8_t buf;
    zfs_file_t *file = (zfs_file_t*)data;
    ssize_t sz_out = fs_read(&file->fs_file, &buf, 1);
    if (sz_out < 1) {
        return MP_READER_EOF;
    }
    return buf;
}

STATIC void zfs_file_close(void *data) {
    zfs_file_t *file = (zfs_file_t*)data;
    fs_close(&file->fs_file);
    m_del_obj(zfs_file_t, file);
}

mp_lexer_t *zfs_lexer_new_from_file(const char *filename) {
    zfs_file_t *file = m_new_obj(zfs_file_t);
    file->base.type = &zfs_textio_type;

    if (fs_open(&file->fs_file, filename)) {
        m_del_obj(zfs_file_t, file);
        mp_raise_ValueError("Could not open file");
    }

    mp_reader_t reader;
    reader.data = file;
    reader.readbyte = zfs_file_readbyte;
    reader.close = zfs_file_close;
    return mp_lexer_new(qstr_from_str(filename), reader);
}

mp_obj_t zfs_file_open(mp_obj_t filename, mp_obj_t mode) {
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    arg_vals[0].u_obj = filename;
    arg_vals[1].u_obj = mode;
    arg_vals[2].u_obj = mp_const_none;
    return file_open(&zfs_textio_type, arg_vals);
}

MP_DEFINE_CONST_FUN_OBJ_2(zfs_file_open_obj, zfs_file_open);

STATIC const mp_rom_map_elem_t zfs_rawfile_locals_dict_table[] = {
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
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&zfs_file___exit___obj) },
};

STATIC MP_DEFINE_CONST_DICT(zfs_rawfile_locals_dict, zfs_rawfile_locals_dict_table);

STATIC mp_obj_t zfs_file_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_val_t arg_vals[FILE_OPEN_NUM_ARGS];
    mp_arg_parse_all_kw_array(n_args, n_kw, args, FILE_OPEN_NUM_ARGS, file_open_args, arg_vals);
    return file_open(type, arg_vals);
}

#if MICROPY_PY_IO_FILEIO
STATIC const mp_stream_p_t zfs_fileio_stream_p = {
    .read = zfs_file_read,
    .write = zfs_file_write,
    .ioctl = zfs_file_ioctl,
};

const mp_obj_type_t zfs_fileio_type = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = zfs_file_print,
    .make_new = zfs_file_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &zfs_fileio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&zfs_rawfile_locals_dict,
};
#endif

STATIC const mp_stream_p_t zfs_textio_stream_p = {
    .read = zfs_file_read,
    .write = zfs_file_write,
    .ioctl = zfs_file_ioctl,
    .is_text = true,
};

const mp_obj_type_t zfs_textio_type = {
    { &mp_type_type },
    .name = MP_QSTR_TextIOWrapper,
    .print = zfs_file_print,
    .make_new = zfs_file_make_new,
    .getiter = mp_identity_getiter,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &zfs_textio_stream_p,
    .locals_dict = (mp_obj_dict_t*)&zfs_rawfile_locals_dict,
};
