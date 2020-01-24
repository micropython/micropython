/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017-2018 Damien P. George
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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mpthread.h"
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"

#if MICROPY_VFS_POSIX

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

typedef struct _mp_obj_vfs_posix_t {
    mp_obj_base_t base;
    vstr_t root;
    size_t root_len;
    bool readonly;
} mp_obj_vfs_posix_t;

STATIC const char *vfs_posix_get_path_str(mp_obj_vfs_posix_t *self, mp_obj_t path) {
    if (self->root_len == 0) {
        return mp_obj_str_get_str(path);
    } else {
        self->root.len = self->root_len;
        vstr_add_str(&self->root, mp_obj_str_get_str(path));
        return vstr_null_terminated_str(&self->root);
    }
}

STATIC mp_obj_t vfs_posix_get_path_obj(mp_obj_vfs_posix_t *self, mp_obj_t path) {
    if (self->root_len == 0) {
        return path;
    } else {
        self->root.len = self->root_len;
        vstr_add_str(&self->root, mp_obj_str_get_str(path));
        return mp_obj_new_str(self->root.buf, self->root.len);
    }
}

STATIC mp_obj_t vfs_posix_fun1_helper(mp_obj_t self_in, mp_obj_t path_in, int (*f)(const char*)) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = f(vfs_posix_get_path_str(self, path_in));
    if (ret != 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}

STATIC mp_import_stat_t mp_vfs_posix_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_posix_t *self = self_in;
    if (self->root_len != 0) {
        self->root.len = self->root_len;
        vstr_add_str(&self->root, path);
        path = vstr_null_terminated_str(&self->root);
    }
    struct stat st;
    if (stat(path, &st) == 0) {
        if (S_ISDIR(st.st_mode)) {
            return MP_IMPORT_STAT_DIR;
        } else if (S_ISREG(st.st_mode)) {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

STATIC mp_obj_t vfs_posix_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);

    mp_obj_vfs_posix_t *vfs = m_new_obj(mp_obj_vfs_posix_t);
    vfs->base.type = type;
    vstr_init(&vfs->root, 0);
    if (n_args == 1) {
        vstr_add_str(&vfs->root, mp_obj_str_get_str(args[0]));
        vstr_add_char(&vfs->root, '/');
    }
    vfs->root_len = vfs->root.len;
    vfs->readonly = false;

    return MP_OBJ_FROM_PTR(vfs);
}

STATIC mp_obj_t vfs_posix_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_true(readonly)) {
        self->readonly = true;
    }
    if (mp_obj_is_true(mkfs)) {
        mp_raise_OSError(MP_EPERM);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_posix_mount_obj, vfs_posix_mount);

STATIC mp_obj_t vfs_posix_umount(mp_obj_t self_in) {
    (void)self_in;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_posix_umount_obj, vfs_posix_umount);

STATIC mp_obj_t vfs_posix_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    const char *mode = mp_obj_str_get_str(mode_in);
    if (self->readonly
        && (strchr(mode, 'w') != NULL || strchr(mode, 'a') != NULL || strchr(mode, '+') != NULL)) {
        mp_raise_OSError(MP_EROFS);
    }
    if (!mp_obj_is_small_int(path_in)) {
        path_in = vfs_posix_get_path_obj(self, path_in);
    }
    return mp_vfs_posix_file_open(&mp_type_textio, path_in, mode_in);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_posix_open_obj, vfs_posix_open);

STATIC mp_obj_t vfs_posix_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    return vfs_posix_fun1_helper(self_in, path_in, chdir);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_chdir_obj, vfs_posix_chdir);

STATIC mp_obj_t vfs_posix_getcwd(mp_obj_t self_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    char buf[MICROPY_ALLOC_PATH_MAX + 1];
    const char *ret = getcwd(buf, sizeof(buf));
    if (ret == NULL) {
        mp_raise_OSError(errno);
    }
    ret += self->root_len;
    return mp_obj_new_str(ret, strlen(ret));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_posix_getcwd_obj, vfs_posix_getcwd);

typedef struct _vfs_posix_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    DIR *dir;
} vfs_posix_ilistdir_it_t;

STATIC mp_obj_t vfs_posix_ilistdir_it_iternext(mp_obj_t self_in) {
    vfs_posix_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->dir == NULL) {
        return MP_OBJ_STOP_ITERATION;
    }

    for (;;) {
        MP_THREAD_GIL_EXIT();
        struct dirent *dirent = readdir(self->dir);
        if (dirent == NULL) {
            closedir(self->dir);
            MP_THREAD_GIL_ENTER();
            self->dir = NULL;
            return MP_OBJ_STOP_ITERATION;
        }
        MP_THREAD_GIL_ENTER();
        const char *fn = dirent->d_name;

        if (fn[0] == '.' && (fn[1] == 0 || fn[1] == '.')) {
            // skip . and ..
            continue;
        }

        // make 3-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));

        if (self->is_str) {
            t->items[0] = mp_obj_new_str(fn, strlen(fn));
        } else {
            t->items[0] = mp_obj_new_bytes((const byte*)fn, strlen(fn));
        }

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
}

STATIC mp_obj_t vfs_posix_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_posix_ilistdir_it_t *iter = m_new_obj(vfs_posix_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = vfs_posix_ilistdir_it_iternext;
    iter->is_str = mp_obj_get_type(path_in) == &mp_type_str;
    const char *path = vfs_posix_get_path_str(self, path_in);
    if (path[0] == '\0') {
        path = ".";
    }
    MP_THREAD_GIL_EXIT();
    iter->dir = opendir(path);
    MP_THREAD_GIL_ENTER();
    if (iter->dir == NULL) {
        mp_raise_OSError(errno);
    }
    return MP_OBJ_FROM_PTR(iter);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_ilistdir_obj, vfs_posix_ilistdir);

typedef struct _mp_obj_listdir_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    DIR *dir;
} mp_obj_listdir_t;

STATIC mp_obj_t vfs_posix_mkdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = vfs_posix_get_path_str(self, path_in);
    MP_THREAD_GIL_EXIT();
    int ret = mkdir(path, 0777);
    MP_THREAD_GIL_ENTER();
    if (ret != 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_mkdir_obj, vfs_posix_mkdir);

STATIC mp_obj_t vfs_posix_remove(mp_obj_t self_in, mp_obj_t path_in) {
    return vfs_posix_fun1_helper(self_in, path_in, unlink);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_remove_obj, vfs_posix_remove);

STATIC mp_obj_t vfs_posix_rename(mp_obj_t self_in, mp_obj_t old_path_in, mp_obj_t new_path_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    const char *old_path = vfs_posix_get_path_str(self, old_path_in);
    const char *new_path = vfs_posix_get_path_str(self, new_path_in);
    MP_THREAD_GIL_EXIT();
    int ret = rename(old_path, new_path);
    MP_THREAD_GIL_ENTER();
    if (ret != 0) {
        mp_raise_OSError(errno);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_posix_rename_obj, vfs_posix_rename);

STATIC mp_obj_t vfs_posix_rmdir(mp_obj_t self_in, mp_obj_t path_in) {
    return vfs_posix_fun1_helper(self_in, path_in, rmdir);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_rmdir_obj, vfs_posix_rmdir);

STATIC mp_obj_t vfs_posix_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    struct stat sb;
    const char *path = vfs_posix_get_path_str(self, path_in);
    MP_THREAD_GIL_EXIT();
    int ret = stat(path, &sb);
    MP_THREAD_GIL_ENTER();
    if (ret != 0) {
        mp_raise_OSError(errno);
    }
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
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
    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_stat_obj, vfs_posix_stat);

#ifdef __ANDROID__
#define USE_STATFS 1
#endif

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

STATIC mp_obj_t vfs_posix_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_posix_t *self = MP_OBJ_TO_PTR(self_in);
    STRUCT_STATVFS sb;
    const char *path = vfs_posix_get_path_str(self, path_in);
    MP_THREAD_GIL_EXIT();
    int ret = STATVFS(path, &sb);
    MP_THREAD_GIL_ENTER();
    if (ret != 0) {
        mp_raise_OSError(errno);
    }
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
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_posix_statvfs_obj, vfs_posix_statvfs);

STATIC const mp_rom_map_elem_t vfs_posix_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_posix_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&vfs_posix_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_posix_open_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_posix_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&vfs_posix_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_posix_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&vfs_posix_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&vfs_posix_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&vfs_posix_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&vfs_posix_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_posix_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_posix_statvfs_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vfs_posix_locals_dict, vfs_posix_locals_dict_table);

STATIC const mp_vfs_proto_t vfs_posix_proto = {
    .import_stat = mp_vfs_posix_import_stat,
};

const mp_obj_type_t mp_type_vfs_posix = {
    { &mp_type_type },
    .name = MP_QSTR_VfsPosix,
    .make_new = vfs_posix_make_new,
    .protocol = &vfs_posix_proto,
    .locals_dict = (mp_obj_dict_t*)&vfs_posix_locals_dict,
};

#endif // MICROPY_VFS_POSIX
