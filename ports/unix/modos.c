/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 Paul Sokolovsky
 * Copyright (c) 2014-2018 Damien P. George
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#ifdef _MSC_VER
#include <direct.h> // For mkdir
#endif
#include "py/mpconfig.h"

#include "py/runtime.h"
#include "py/objtuple.h"
#include "py/mphal.h"
#include "py/mpthread.h"
#include "extmod/vfs.h"
#include "extmod/misc.h"

#ifdef __ANDROID__
#define USE_STATFS 1
#endif

STATIC mp_obj_t mod_os_stat(mp_obj_t path_in) {
    struct stat sb;
    const char *path = mp_obj_str_get_str(path_in);

    int res;
    MP_HAL_RETRY_SYSCALL(res, stat(path, &sb), mp_raise_OSError(err));

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(sb.st_mode);
    t->items[1] = mp_obj_new_int_from_uint(sb.st_ino);
    t->items[2] = mp_obj_new_int_from_uint(sb.st_dev);
    t->items[3] = mp_obj_new_int_from_uint(sb.st_nlink);
    t->items[4] = mp_obj_new_int_from_uint(sb.st_uid);
    t->items[5] = mp_obj_new_int_from_uint(sb.st_gid);
    t->items[6] = mp_obj_new_int_from_uint(sb.st_size);
    t->items[7] = mp_obj_new_int_from_uint(sb.st_atime);
    t->items[8] = mp_obj_new_int_from_uint(sb.st_mtime);
    t->items[9] = mp_obj_new_int_from_uint(sb.st_ctime);
    return MP_OBJ_FROM_PTR(t);
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
    const char *path = mp_obj_str_get_str(path_in);

    int res;
    MP_HAL_RETRY_SYSCALL(res, STATVFS(path, &sb), mp_raise_OSError(err));

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
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
    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_statvfs_obj, mod_os_statvfs);
#endif

STATIC mp_obj_t mod_os_remove(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);

    // Note that POSIX requires remove() to be able to delete a directory
    // too (act as rmdir()). This is POSIX extenstion to ANSI C semantics
    // of that function. But Python remove() follows ANSI C, and explicitly
    // required to raise exception on attempt to remove a directory. Thus,
    // call POSIX unlink() here.
    MP_THREAD_GIL_EXIT();
    int r = unlink(path);
    MP_THREAD_GIL_ENTER();

    RAISE_ERRNO(r, errno);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_remove_obj, mod_os_remove);

STATIC mp_obj_t mod_os_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path = mp_obj_str_get_str(old_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);

    MP_THREAD_GIL_EXIT();
    int r = rename(old_path, new_path);
    MP_THREAD_GIL_ENTER();

    RAISE_ERRNO(r, errno);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_os_rename_obj, mod_os_rename);

STATIC mp_obj_t mod_os_rmdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);

    MP_THREAD_GIL_EXIT();
    int r = rmdir(path);
    MP_THREAD_GIL_ENTER();

    RAISE_ERRNO(r, errno);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_rmdir_obj, mod_os_rmdir);

STATIC mp_obj_t mod_os_system(mp_obj_t cmd_in) {
    const char *cmd = mp_obj_str_get_str(cmd_in);

    MP_THREAD_GIL_EXIT();
    int r = system(cmd);
    MP_THREAD_GIL_ENTER();

    RAISE_ERRNO(r, errno);

    return MP_OBJ_NEW_SMALL_INT(r);
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_system_obj, mod_os_system);

STATIC mp_obj_t mod_os_getenv(mp_obj_t var_in) {
    const char *s = getenv(mp_obj_str_get_str(var_in));
    if (s == NULL) {
        return mp_const_none;
    }
    return mp_obj_new_str(s, strlen(s));
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_getenv_obj, mod_os_getenv);

STATIC mp_obj_t mod_os_putenv(mp_obj_t key_in, mp_obj_t value_in) {
    const char *key = mp_obj_str_get_str(key_in);
    const char *value = mp_obj_str_get_str(value_in);
    int ret;

    #if _WIN32
    ret = _putenv_s(key, value);
    #else
    ret = setenv(key, value, 1);
    #endif

    if (ret == -1) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_os_putenv_obj, mod_os_putenv);

STATIC mp_obj_t mod_os_unsetenv(mp_obj_t key_in) {
    const char *key = mp_obj_str_get_str(key_in);
    int ret;

    #if _WIN32
    ret = _putenv_s(key, "");
    #else
    ret = unsetenv(key);
    #endif

    if (ret == -1) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_os_unsetenv_obj, mod_os_unsetenv);

STATIC mp_obj_t mod_os_mkdir(mp_obj_t path_in) {
    // TODO: Accept mode param
    const char *path = mp_obj_str_get_str(path_in);
    MP_THREAD_GIL_EXIT();
    #ifdef _WIN32
    int r = mkdir(path);
    #else
    int r = mkdir(path, 0777);
    #endif
    MP_THREAD_GIL_ENTER();
    RAISE_ERRNO(r, errno);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_os_mkdir_obj, mod_os_mkdir);

typedef struct _mp_obj_listdir_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    DIR *dir;
} mp_obj_listdir_t;

STATIC mp_obj_t listdir_next(mp_obj_t self_in) {
    mp_obj_listdir_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->dir == NULL) {
        goto done;
    }
    MP_THREAD_GIL_EXIT();
    struct dirent *dirent = readdir(self->dir);
    if (dirent == NULL) {
        closedir(self->dir);
        MP_THREAD_GIL_ENTER();
        self->dir = NULL;
    done:
        return MP_OBJ_STOP_ITERATION;
    }
    MP_THREAD_GIL_ENTER();

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
    t->items[0] = mp_obj_new_str(dirent->d_name, strlen(dirent->d_name));

    #ifdef _DIRENT_HAVE_D_TYPE
    #ifdef DTTOIF
    t->items[1] = MP_OBJ_NEW_SMALL_INT(DTTOIF(dirent->d_type));
    #else
    if (dirent->d_type == DT_DIR) {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
    } else if (dirent->d_type == DT_REG) {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
    } else {
        t->items[1] = MP_OBJ_NEW_SMALL_INT(dirent->d_type);
    }
    #endif
    #else
    // DT_UNKNOWN should have 0 value on any reasonable system
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0);
    #endif

    #ifdef _DIRENT_HAVE_D_INO
    t->items[2] = MP_OBJ_NEW_SMALL_INT(dirent->d_ino);
    #else
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    #endif
    return MP_OBJ_FROM_PTR(t);
}

STATIC mp_obj_t mod_os_ilistdir(size_t n_args, const mp_obj_t *args) {
    const char *path = ".";
    if (n_args > 0) {
        path = mp_obj_str_get_str(args[0]);
    }
    mp_obj_listdir_t *o = m_new_obj(mp_obj_listdir_t);
    o->base.type = &mp_type_polymorph_iter;
    MP_THREAD_GIL_EXIT();
    o->dir = opendir(path);
    MP_THREAD_GIL_ENTER();
    o->iternext = listdir_next;
    return MP_OBJ_FROM_PTR(o);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_os_ilistdir_obj, 0, 1, mod_os_ilistdir);

STATIC mp_obj_t mod_os_errno(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return MP_OBJ_NEW_SMALL_INT(errno);
    }

    errno = mp_obj_get_int(args[0]);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_os_errno_obj, 0, 1, mod_os_errno);

STATIC const mp_rom_map_elem_t mp_module_os_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },
    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mod_os_errno_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&mod_os_stat_obj) },
    #if MICROPY_PY_OS_STATVFS
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&mod_os_statvfs_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_system), MP_ROM_PTR(&mod_os_system_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mod_os_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&mod_os_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&mod_os_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getenv), MP_ROM_PTR(&mod_os_getenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_putenv), MP_ROM_PTR(&mod_os_putenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_unsetenv), MP_ROM_PTR(&mod_os_unsetenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mod_os_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&mod_os_ilistdir_obj) },
    #if MICROPY_PY_OS_DUPTERM
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_uos_dupterm_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_os_globals, mp_module_os_globals_table);

const mp_obj_module_t mp_module_os = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_os_globals,
};
