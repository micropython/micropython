/**
 * @file module_io.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

struct file_obj_t {
    mp_obj_base_t base;
    struct fs_file_t file;
};

static mp_obj_t file_open(const mp_obj_type_t *type_p,
                          mp_arg_val_t *args_p);

static const mp_arg_t file_open_allowed_args[] = {
    {
        MP_QSTR_file,
        MP_ARG_OBJ | MP_ARG_REQUIRED,
        { .u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) }
    },
    {
        MP_QSTR_mode,
        MP_ARG_OBJ,
        { .u_obj = MP_OBJ_NEW_QSTR(MP_QSTR_r) }
    },
    {
        MP_QSTR_encoding,
        MP_ARG_OBJ | MP_ARG_KW_ONLY,
        { .u_rom_obj = MP_ROM_PTR(&mp_const_none_obj) }
    },
};

static void file_obj_print(const mp_print_t *print_p,
                           mp_obj_t self_in,
                           mp_print_kind_t kind)
{
    mp_printf(print_p,
              "<io.%s %p>",
              mp_obj_get_type_str(self_in),
              MP_OBJ_TO_PTR(self_in));
}

static mp_uint_t file_obj_read(mp_obj_t self_in,
                               void *buf_p,
                               mp_uint_t size,
                               int *errcode_p)
{
    struct file_obj_t *self_p;
    ssize_t res;

    self_p = MP_OBJ_TO_PTR(self_in);
    res = fs_read(&self_p->file, buf_p, size);

    if (res < 0) {
        *errcode_p = res;

        return (MP_STREAM_ERROR);
    }

    return (res);
}

static mp_uint_t file_obj_write(mp_obj_t self_in,
                                const void *buf_p,
                                mp_uint_t size,
                                int *errcode_p)
{
    struct file_obj_t *self_p;
    ssize_t res;

    self_p = MP_OBJ_TO_PTR(self_in);
    res = fs_write(&self_p->file, buf_p, size);

    if (res < 0) {
        *errcode_p = res;

        return (MP_STREAM_ERROR);
    }

    return (res);
}

static mp_obj_t file_obj_flush(mp_obj_t self_in)
{
    mp_not_implemented("file_obj_flush");

    return (mp_const_none);
}

static mp_obj_t file_obj_close(mp_obj_t self_in)
{
    struct file_obj_t *self_p;

    self_p = MP_OBJ_TO_PTR(self_in);
    fs_close(&self_p->file);

    return (mp_const_none);
}

static mp_obj_t file_obj___exit__(size_t n_args, const mp_obj_t *args_p)
{
    return (file_obj_close(args_p[0]));
}

static mp_uint_t file_obj_ioctl(mp_obj_t o_in,
                                mp_uint_t request,
                                uintptr_t arg,
                                int *errcode_p)
{
    struct file_obj_t *self_p;
    struct mp_stream_seek_t *s_p;

    self_p = MP_OBJ_TO_PTR(o_in);
    s_p = (struct mp_stream_seek_t*)(uintptr_t)arg;

    if (request == MP_STREAM_SEEK) {
        if (fs_seek(&self_p->file, s_p->offset, s_p->whence) != 0) {
            *errcode_p = EINVAL;

            return (MP_STREAM_ERROR);
        }

        s_p->offset = fs_tell(&self_p->file);
    } else {
        *errcode_p = EINVAL;

        return (MP_STREAM_ERROR);
    }

    return (0);
}

static mp_obj_t file_obj_make_new(const mp_obj_type_t *type_p,
                                  size_t n_args,
                                  size_t n_kw,
                                  const mp_obj_t *args_p)
{
    mp_arg_val_t args[membersof(file_open_allowed_args)];

    mp_arg_parse_all_kw_array(n_args,
                              n_kw,
                              args_p,
                              membersof(file_open_allowed_args),
                              file_open_allowed_args,
                              args);

    return (file_open(type_p, args));
}

static MP_DEFINE_CONST_FUN_OBJ_1(file_obj_flush_obj, file_obj_flush);
static MP_DEFINE_CONST_FUN_OBJ_1(file_obj_close_obj, file_obj_close);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(file_obj___exit___obj, 4, 4, file_obj___exit__);

static const mp_rom_map_elem_t rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readall), MP_ROM_PTR(&mp_stream_readall_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&file_obj_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&file_obj_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&file_obj_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&file_obj___exit___obj) },
};

static MP_DEFINE_CONST_DICT(rawfile_locals_dict, rawfile_locals_dict_table);

#if MICROPY_PY_IO_FILEIO == 1

/**
 * The FileIO stream.
 */
static const mp_stream_p_t fileio_stream = {
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
};

/**
 * The FileIO class.
 */
const mp_obj_type_t class_fileio = {
    { &mp_type_type },
    .name = MP_QSTR_FileIO,
    .print = file_obj_print,
    .make_new = file_obj_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &fileio_stream,
    .locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict,
};

#endif

/**
 * The TextIOWrapper stream.
 */
static const mp_stream_p_t textio_stream = {
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
    .is_text = true,
};

/**
 * The TextIOWrapper class.
 */
static const mp_obj_type_t class_textio = {
    { &mp_type_type },
    .name = MP_QSTR_TextIOWrapper,
    .print = file_obj_print,
    .make_new = file_obj_make_new,
    .getiter = mp_identity,
    .iternext = mp_stream_unbuffered_iter,
    .protocol = &textio_stream,
    .locals_dict = (mp_obj_dict_t*)&rawfile_locals_dict,
};

/**
 * Helper function to open a file.
 */
static mp_obj_t file_open(const mp_obj_type_t *type_p,
                          mp_arg_val_t *args_p)
{
    int mode;
    const char *mode_p;
    struct file_obj_t *obj_p;
    const char *fname_p;
    int res;

    mode = 0;
    mode_p = mp_obj_str_get_str(args_p[1].u_obj);

    while (*mode_p) {
        switch (*mode_p++) {

        case 'r':
            mode |= (FS_READ);
            break;

        case 'w':
            mode |= (FS_WRITE | FS_CREAT | FS_TRUNC);
            break;

        case 'a':
            mode |= (FS_WRITE | FS_APPEND);
            break;

        case '+':
            mode |= (FS_READ | FS_WRITE);
            break;

#if MICROPY_PY_IO_FILEIO == 1

        case 'b':
            type_p = &class_fileio;
            break;

#endif

        case 't':
            type_p = &class_textio;
            break;
        }
    }

    obj_p = m_new_obj_with_finaliser(struct file_obj_t);
    obj_p->base.type = type_p;

    fname_p = mp_obj_str_get_str(args_p[0].u_obj);
    res = fs_open(&obj_p->file, fname_p, mode);

    if (res != 0) {
        m_del_obj(struct file_obj_t, obj_p);
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError,
                                            MP_OBJ_NEW_SMALL_INT(res)));
    }

    return (MP_OBJ_FROM_PTR(obj_p));
}

/**
 * The builtin open() function.
 *
 * open(path, mode="r")
 */
static mp_obj_t builtin_open(mp_uint_t n_args,
                             const mp_obj_t *args_p,
                             mp_map_t *kwargs_p)
{
    mp_arg_val_t args[membersof(file_open_allowed_args)];

    mp_arg_parse_all(n_args,
                     args_p,
                     kwargs_p,
                     membersof(file_open_allowed_args),
                     file_open_allowed_args,
                     args);

    return (file_open(&class_textio, args));
}

MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, builtin_open);
