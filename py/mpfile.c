/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Dave Hylands
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

#include "py/mpfile.h"

#include "py/builtin.h"
#include "py/misc.h"
#include "py/runtime.h"

#include <string.h>

#if MICROPY_PY_FILE_LIKE

STATIC const mp_obj_type_t mp_file_type;
STATIC mp_obj_t mp___del__(mp_obj_t self);

mp_file_t *mp_file_from_file_obj(mp_obj_t file_obj) {
    mp_file_t *file = m_new_obj_with_finaliser(mp_file_t);
    memset(file, 0, sizeof(*file));
    file->base.type = &mp_file_type;
    file->file_obj = file_obj;
    file->readinto_fn = mp_const_none;
    file->seek_fn = mp_const_none;
    file->tell_fn = mp_const_none;

    return file;
}

mp_file_t *mp_open(const char *filename, const char *mode) {
    mp_obj_t filename_obj = mp_obj_new_str(filename, strlen(filename), false);
    mp_obj_t mode_obj = mp_obj_new_str(mode, strlen(mode), true);
    mp_obj_t args[2] = { filename_obj, mode_obj };
    return mp_file_from_file_obj(mp_builtin_open(2, args, (mp_map_t *)&mp_const_empty_map));
}

mp_int_t mp_readinto(mp_file_t *file, void *buf, size_t num_bytes) {
    if (file->readinto_fn == mp_const_none) {
        file->readinto_fn = mp_load_attr(file->file_obj, MP_QSTR_readinto);
    }
    mp_obj_t bytearray = mp_obj_new_bytearray_by_ref(num_bytes, buf);
    mp_obj_t bytes_read = mp_call_function_1(file->readinto_fn, bytearray);
    if (bytes_read == mp_const_none) {
        return 0;
    }
    return mp_obj_get_int(bytes_read);
}

off_t mp_seek(mp_file_t *file, off_t offset, int whence) {
    if (file->seek_fn == mp_const_none) {
        file->seek_fn = mp_load_attr(file->file_obj, MP_QSTR_seek);
    }
    return mp_obj_get_int(mp_call_function_2(file->seek_fn,
                                             MP_OBJ_NEW_SMALL_INT(offset),
                                             MP_OBJ_NEW_SMALL_INT(whence)));
}

off_t mp_tell(mp_file_t *file) {
    if (file->tell_fn == mp_const_none) {
        file->tell_fn = mp_load_attr(file->file_obj, MP_QSTR_tell);
    }
    return mp_obj_get_int(mp_call_function_0(file->tell_fn));
}

void mp_close(mp_file_t *file) {
    mp_obj_t close_fn = mp_load_attr(file->file_obj, MP_QSTR_close);
    file->file_obj = mp_const_none;
    file->readinto_fn = mp_const_none;
    file->seek_fn = mp_const_none;
    file->tell_fn = mp_const_none;
    mp_call_function_0(close_fn);
}

STATIC void mp_file_print(const mp_print_t *print, mp_obj_t self, mp_print_kind_t kind) {
    (void)kind;
    mp_printf(print, "<mp_file %p>", self);
}

STATIC mp_obj_t mp___del__(mp_obj_t self) {
    mp_close(MP_OBJ_TO_PTR(self));              
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp___del___obj, mp___del__);

STATIC const mp_rom_map_elem_t mp_file_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp___del___obj) },
};
STATIC MP_DEFINE_CONST_DICT(mp_file_locals_dict, mp_file_locals_dict_table);

STATIC const mp_obj_type_t mp_file_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_mp_file,
    .print = mp_file_print,
    .locals_dict = (mp_obj_dict_t *)&mp_file_locals_dict,
};

#endif // MICROPY_PY_FILE_LIKE
