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
#include <string.h>

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "ff.h"
#include "ffconf.h"

/// \module fatfs - functions relate to the FatFS file system used on the pyboard.
///
/// The `fatfs` module contains functions specific to the FatFS file system,
/// which are not otherise covered by the generic modules (like `os`).

#if _USE_LABEL
/// \function label(path [label])
/// Get or set the label associated with a volume.
/// If no label is provided, then the label for the volume associated with
/// `path` is retrieved.
/// If a label is provided, then the label for the volume is set.
///
/// Note: If you have USB Mass Storage enabled, then setting a volume label
///       needs to happen before the volume is shared via USB Mass Storage.
/// boot.py is a place where you can set the volume label before USB Mass
/// Storage starts.
///
/// Example (added to boot.py): 
///     import fatfs
///     fatfs.label('/flash', 'internal')
///     print("/flash is now labelled as '%s'" % fatfs.label('/flash'))
///
/// Now, under ubuntu 14.04, the volume will be mounted as /media/username/INTERNAL
/// (fatfs automatically capitalizes the volume)
STATIC mp_obj_t fatfs_label(uint n_args, const mp_obj_t *arg) {
    const char *path = mp_obj_str_get_str(arg[0]);
    FRESULT res;
    char label[32];
    if (n_args == 1) {
        // get
        DWORD serial;
        res = f_getlabel(path, label, &serial);
        if (res != FR_OK) {
            nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "No such file or directory: '%s'", path));
        }
        return mp_obj_new_str(label, strlen(label), false);
    }

    const char *label_in = mp_obj_str_get_str(arg[1]);
    snprintf(label, sizeof(label), "%s/%s", path, label_in);
    res = f_setlabel(label);
    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "No such file or directory: '%s'", path));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(fatfs_label_obj, 1, 2, fatfs_label);

/// \function serial(path)
/// Returrns the serial number of the indicated volume.
STATIC mp_obj_t fatfs_serial(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    char label[16];
    DWORD serial;
    FRESULT res = f_getlabel(path, label, &serial);
    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "No such file or directory: '%s'", path));
    }
    return mp_obj_new_int_from_uint(serial);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(fatfs_serial_obj, fatfs_serial);
#endif // __USE_LABEL

STATIC const mp_map_elem_t fatfs_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_fatfs) },

#if _USE_LABEL
    { MP_OBJ_NEW_QSTR(MP_QSTR_label), (mp_obj_t)&fatfs_label_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_serial), (mp_obj_t)&fatfs_serial_obj },
#endif
};

STATIC const mp_obj_dict_t fatfs_module_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(fatfs_module_globals_table),
        .alloc = MP_ARRAY_SIZE(fatfs_module_globals_table),
        .table = (mp_map_elem_t*)fatfs_module_globals_table,
    },
};

const mp_obj_module_t fatfs_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_fatfs,
    .globals = (mp_obj_dict_t*)&fatfs_module_globals,
};
