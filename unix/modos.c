/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "py/mpconfig.h"

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/objtuple.h"

#ifdef __ANDROID__
#define USE_STATFS 1
#endif

#define RAISE_ERRNO(err_flag, error_val) \
    { if (err_flag == -1) \
        { nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error_val))); } }

STATIC mp_obj_t mod_os_stat(mp_obj_t path_in) {
    struct stat sb;
    mp_uint_t len;
    const char *path = mp_obj_str_get_data(path_in, &len);

    int res = stat(path, &sb);
    RAISE_ERRNO(res, errno);

    mp_obj_tuple_t *t = mp_obj_new_tuple(10, NULL);
    t->items[0] = MP_OBJ_NEW_SMALL_INT(sb.st_mode);
    t->items[1] = MP_OBJ_NEW_SMALL_INT(sb.st_ino);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(sb.st_dev);
    t->items[3] = MP_OBJ_NEW_SMALL_INT(sb.st_nlink);
    t->items[4] = MP_OBJ_NEW_SMALL_INT(sb.st_uid);
    t->items[5] = MP_OBJ_NEW_SMALL_INT(sb.st_gid);
    t->items[6] = MP_OBJ_NEW_SMALL_INT(sb.st_size);
    t->items[7] = MP_OBJ_NEW_SMALL_INT(sb.st_atime);
    t->items[8] = MP_OBJ_NEW_SMALL_INT(sb.st_mtime);
    t->items[9] = MP_OBJ_NEW_SMALL_INT(sb.st_ctime);
    return t;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_stat_obj, mod_os_stat);

#if MICROPY_PY_OS_STATVFS

#if USE_STATFS
#include <sys/vfs.h>
#define STRUCT_STATVFS struct statfs
#define STATVFS statfs
#define F_FAVAIL sb.f_ffree
#define F_NAMEMAX sb.f_namelen
#define F_FLAG sb.f_flags
#else
#include <sys/statvfs.h>
#define STRUCT_STATVFS struct statvfs
#define STATVFS statvfs
#define F_FAVAIL sb.f_favail
#define F_NAMEMAX sb.f_namemax
#define F_FLAG sb.f_flag
#endif

STATIC mp_obj_t mod_os_statvfs(mp_obj_t path_in) {
    STRUCT_STATVFS sb;
    mp_uint_t len;
    const char *path = mp_obj_str_get_data(path_in, &len);

    int res = STATVFS(path, &sb);
    RAISE_ERRNO(res, errno);

    mp_obj_tuple_t *t = mp_obj_new_tuple(10, NULL);
    t->items[0] = MP_OBJ_NEW_SMALL_INT(sb.f_bsize);
    t->items[1] = MP_OBJ_NEW_SMALL_INT(sb.f_frsize);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(sb.f_blocks);
    t->items[3] = MP_OBJ_NEW_SMALL_INT(sb.f_bfree);
    t->items[4] = MP_OBJ_NEW_SMALL_INT(sb.f_bavail);
    t->items[5] = MP_OBJ_NEW_SMALL_INT(sb.f_files);
    t->items[6] = MP_OBJ_NEW_SMALL_INT(sb.f_ffree);
    t->items[7] = MP_OBJ_NEW_SMALL_INT(F_FAVAIL);
    t->items[8] = MP_OBJ_NEW_SMALL_INT(F_FLAG);
    t->items[9] = MP_OBJ_NEW_SMALL_INT(F_NAMEMAX);
    return t;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_statvfs_obj, mod_os_statvfs);
#endif

STATIC mp_obj_t mod_os_unlink(mp_obj_t path_in) {
    mp_uint_t len;
    const char *path = mp_obj_str_get_data(path_in, &len);

    int r = unlink(path);

    RAISE_ERRNO(r, errno);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_unlink_obj, mod_os_unlink);

STATIC mp_obj_t mod_os_system(mp_obj_t cmd_in) {
    const char *cmd = mp_obj_str_get_str(cmd_in);

    int r = system(cmd);

    RAISE_ERRNO(r, errno);

    return MP_OBJ_NEW_SMALL_INT(r);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_system_obj, mod_os_system);

STATIC mp_obj_t mod_os_getenv(mp_obj_t var_in) {
    const char *s = getenv(mp_obj_str_get_str(var_in));
    if (s == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(s, strlen(s), false);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_getenv_obj, mod_os_getenv);

STATIC mp_obj_t mod_os_mkdir(mp_obj_t path_in) {
    // TODO: Accept mode param
    const char *path = mp_obj_str_get_str(path_in);
    #ifdef _WIN32
    int r = mkdir(path);
    #else
    int r = mkdir(path, 0777);
    #endif
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_mkdir_obj, mod_os_mkdir);

STATIC const mp_map_elem_t mp_module_os_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR__os) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stat), (mp_obj_t)&mod_os_stat_obj },
    #if MICROPY_PY_OS_STATVFS
    { MP_OBJ_NEW_QSTR(MP_QSTR_statvfs), (mp_obj_t)&mod_os_statvfs_obj },
    #endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_system), (mp_obj_t)&mod_os_system_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_unlink),(mp_obj_t)&mod_os_unlink_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_getenv),(mp_obj_t)&mod_os_getenv_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_mkdir),(mp_obj_t)&mod_os_mkdir_obj},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_os_globals, mp_module_os_globals_table);

const mp_obj_module_t mp_module_os = {
    .base = { &mp_type_module },
    .name = MP_QSTR__os,
    .globals = (mp_obj_dict_t*)&mp_module_os_globals,
};
