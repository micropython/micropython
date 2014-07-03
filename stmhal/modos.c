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

#include <stdint.h>
#include <string.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "systick.h"
#include "rng.h"
#include "storage.h"
#include "ff.h"
#include "portmodules.h"

#if _USE_LFN
static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
#endif

STATIC mp_obj_t os_listdir(uint n_args, const mp_obj_t *args) {
    const mp_obj_type_t *local_str_type = &mp_type_str;
    const char *path;
    if (n_args == 1) {
        if (mp_obj_get_type(args[0]) == &mp_type_bytes) {
            local_str_type = &mp_type_bytes;
        }
        path = mp_obj_str_get_str(args[0]);
    } else {
        path = "0:";
    }

    FRESULT res;
    FILINFO fno;
    DIR dir;
#if _USE_LFN
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif

    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res != FR_OK) {
        // TODO should be mp_type_FileNotFoundError
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "No such file or directory: '%s'", path));
    }

    mp_obj_t dir_list = mp_obj_new_list(0, NULL);

    uint path_len = strlen(path);
    if (path[path_len - 1] == '/') {
        path_len--;
    }

    for (;;) {
        res = f_readdir(&dir, &fno);                   /* Read a directory item */
        if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
        if (fno.fname[0] == '.' && fno.fname[1] == 0) continue;             /* Ignore . entry */
        if (fno.fname[0] == '.' && fno.fname[1] == '.' && fno.fname[2] == 0) continue;             /* Ignore .. entry */

#if _USE_LFN
        char *fn = *fno.lfname ? fno.lfname : fno.fname;
#else
        char *fn = fno.fname;
#endif

        if (fno.fattrib & AM_DIR) {                    /* It is a directory */
        } else {                                       /* It is a file. */
        }

        // make a string object for this entry
        byte *data;
        uint fn_len = strlen(fn);
        mp_obj_t entry_o = mp_obj_str_builder_start(local_str_type, path_len + 1 + fn_len, &data);
        memcpy(data, path, path_len);
        data[path_len] = '/';
        memcpy(data + path_len + 1, fn, fn_len);

        // add the entry to the list
        mp_obj_list_append(dir_list, mp_obj_str_builder_end(entry_o));
    }

    f_closedir(&dir);

    return dir_list;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(os_listdir_obj, 0, 1, os_listdir);

STATIC mp_obj_t os_mkdir(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    FRESULT res = f_mkdir(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        case FR_EXIST:
            // TODO should be FileExistsError
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "File exists: '%s'", path));
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error creating directory '%s'", path));
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(os_mkdir_obj, os_mkdir);

STATIC mp_obj_t os_remove(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    // TODO check that path is actually a file before trying to unlink it
    FRESULT res = f_unlink(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error removing file '%s'", path));
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(os_remove_obj, os_remove);

STATIC mp_obj_t os_rmdir(mp_obj_t path_o) {
    const char *path = mp_obj_str_get_str(path_o);
    // TODO check that path is actually a directory before trying to unlink it
    FRESULT res = f_unlink(path);
    switch (res) {
        case FR_OK:
            return mp_const_none;
        default:
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "Error removing directory '%s'", path));
    }
}

MP_DEFINE_CONST_FUN_OBJ_1(os_rmdir_obj, os_rmdir);

STATIC mp_obj_t os_sync(void) {
    storage_flush();
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(os_sync_obj, os_sync);

STATIC mp_obj_t os_urandom(mp_obj_t num) {
    mp_int_t n = mp_obj_get_int(num);
    byte *data;
    mp_obj_t o = mp_obj_str_builder_start(&mp_type_bytes, n, &data);
    for (int i = 0; i < n; i++) {
        data[i] = rng_get();
    }
    return mp_obj_str_builder_end(o);
}

MP_DEFINE_CONST_FUN_OBJ_1(os_urandom_obj, os_urandom);

STATIC const mp_map_elem_t os_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_os) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_listdir), (mp_obj_t)&os_listdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkdir), (mp_obj_t)&os_mkdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_remove), (mp_obj_t)&os_remove_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rmdir), (mp_obj_t)&os_rmdir_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unlink), (mp_obj_t)&os_remove_obj }, // unlink aliases to remove

    { MP_OBJ_NEW_QSTR(MP_QSTR_sync), (mp_obj_t)&os_sync_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_sep), MP_OBJ_NEW_QSTR(MP_QSTR__slash_) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_urandom), (mp_obj_t)&os_urandom_obj },
};

STATIC const mp_obj_dict_t os_module_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(os_module_globals_table),
        .alloc = MP_ARRAY_SIZE(os_module_globals_table),
        .table = (mp_map_elem_t*)os_module_globals_table,
    },
};

const mp_obj_module_t os_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_os,
    .globals = (mp_obj_dict_t*)&os_module_globals,
};
