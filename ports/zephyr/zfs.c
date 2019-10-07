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

#include "zfs.h"
#include "py/runtime.h"

mp_import_stat_t zfs_import_stat(const char *path) {
    struct fs_dirent entry;

    if (fs_stat(path, &entry)) {
        return MP_IMPORT_STAT_NO_EXIST;
    } else if (entry.type == FS_DIR_ENTRY_DIR) {
        return MP_IMPORT_STAT_DIR;
    } else {
        return MP_IMPORT_STAT_FILE;
    }
}

STATIC mp_obj_t zfs_ilistdir_it_iternext(mp_obj_t self_in) {
    zfs_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);
    struct fs_dirent entry;
    int res;

    for (;;) {
        res = fs_readdir(&self->dir, &entry);
        if (res || entry.name[0] == 0) {
            break;
        }

        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));
        if (self->is_str) {
            t->items[0] = mp_obj_new_str(entry.name, strlen(entry.name));
        } else {
            t->items[0] = mp_obj_new_bytes((const byte*)entry.name, strlen(entry.name));
        }

        if (entry.type == FS_DIR_ENTRY_DIR) {
            t->items[1] = MP_OBJ_NEW_SMALL_INT(ZFS_IFDIR);
        } else {
            t->items[1] = MP_OBJ_NEW_SMALL_INT(ZFS_IFREG);
        }
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // no inode number
        t->items[3] = mp_obj_new_int_from_uint(entry.size);

        return MP_OBJ_FROM_PTR(t);
    }

    fs_closedir(&self->dir);

    return MP_OBJ_STOP_ITERATION;
}

mp_obj_t zfs_ilistdir(size_t n_args, const mp_obj_t *args) {
    bool is_str_type = true;
    const char *path;
    if (n_args == 1) {
        if (mp_obj_get_type(args[0]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = mp_obj_str_get_str(args[0]);
    } else {
        path = "";
    }

    zfs_ilistdir_it_t *iter = m_new_obj(zfs_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = zfs_ilistdir_it_iternext;
    iter->is_str = is_str_type;

    if (fs_opendir(&iter->dir, path)) {
        mp_raise_ValueError("Could not open dir");
    }

    return MP_OBJ_FROM_PTR(iter);
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zfs_ilistdir_obj, 0, 1, zfs_ilistdir);

mp_obj_t zfs_listdir(size_t n_args, const mp_obj_t *args) {
    mp_obj_t iter = zfs_ilistdir(n_args, args);
    mp_obj_t dir_list = mp_obj_new_list(0, NULL);
    mp_obj_t next;
    while ((next = mp_iternext(iter)) != MP_OBJ_STOP_ITERATION) {
        mp_obj_list_append(dir_list, mp_obj_subscr(next, MP_OBJ_NEW_SMALL_INT(0), MP_OBJ_SENTINEL));
    }
    return dir_list;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zfs_listdir_obj, 0, 1, zfs_listdir);

mp_obj_t zfs_mkdir(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);

    if (fs_mkdir(path)) {
        mp_raise_ValueError("Could not mkdir");
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(zfs_mkdir_obj, zfs_mkdir);

mp_obj_t zfs_remove(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);

    if (fs_unlink(path)) {
        mp_raise_ValueError("Could not unlink");
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_1(zfs_remove_obj, zfs_remove);

mp_obj_t zfs_rename(mp_obj_t old_path_in, mp_obj_t new_path_in) {
    const char *old_path = mp_obj_str_get_str(old_path_in);
    const char *new_path = mp_obj_str_get_str(new_path_in);

    if (fs_rename(old_path, new_path)) {
        mp_raise_ValueError("Could not rename");
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(zfs_rename_obj, zfs_rename);

mp_obj_t zfs_stat(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    struct fs_dirent entry;

    if (fs_stat(path, &entry)) {
        mp_raise_ValueError("Could not get status");
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));

    if (entry.type == FS_DIR_ENTRY_DIR) {
        t->items[0] = MP_OBJ_NEW_SMALL_INT(ZFS_IFDIR);
    } else {
        t->items[0] = MP_OBJ_NEW_SMALL_INT(ZFS_IFREG);
    }

    for (int i = 1; i <= 9; i++) {
        if (i == 5) {
            t->items[i] = mp_obj_new_int_from_uint(entry.size);
        } else {
            t->items[i] = MP_OBJ_NEW_SMALL_INT(0);
        }
    }

    return MP_OBJ_FROM_PTR(t);
}

MP_DEFINE_CONST_FUN_OBJ_1(zfs_stat_obj, zfs_stat);

mp_obj_t zfs_statvfs(mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    struct fs_statvfs stat;

    if (fs_statvfs(path, &stat)) {
        mp_raise_ValueError("Could not read stats");
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));

    t->items[0] = MP_OBJ_NEW_SMALL_INT(stat.f_bsize);
    t->items[1] = MP_OBJ_NEW_SMALL_INT(stat.f_frsize);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(stat.f_blocks);
    t->items[3] = MP_OBJ_NEW_SMALL_INT(stat.f_bfree);
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(MAX_FILE_NAME);

    return MP_OBJ_FROM_PTR(t);
}

MP_DEFINE_CONST_FUN_OBJ_1(zfs_statvfs_obj, zfs_statvfs);

mp_obj_t zfs_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_readonly, ARG_mkfs };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_readonly, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_false_obj)} },
        { MP_QSTR_mkfs, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_PTR(&mp_const_false_obj)} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // create new object
    zfs_mount_t *mount = m_new_obj(zfs_mount_t);

    mp_obj_t zfs_obj = pos_args[0];
    mp_obj_type_t *zfs_type = mp_obj_get_type(zfs_obj);

#ifdef CONFIG_FAT_FILESYSTEM_ELM
    if (zfs_type == &zfs_fat_type) {
        zfs_fat_t *zfs_fat = zfs_obj;
        mount->fs_mount.type = FS_FATFS;
        mount->fs_mount.fs_data = &zfs_fat->fat_fs;
    }
#endif

#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
    if (zfs_type == &zfs_little_type) {
        zfs_little_t *zfs_little = zfs_obj;
        mount->fs_mount.type = FS_LITTLEFS;
        mount->fs_mount.fs_data = &zfs_little->littlefs;
        mount->fs_mount.storage_dev = (void *)DT_FLASH_AREA_STORAGE_ID;
    }
#endif

    // get the mount point
    size_t mnt_len;
    const char *mnt_str = mp_obj_str_get_data(pos_args[1], &mnt_len);

    mount->fs_mount.mnt_point = mnt_str;
    if (fs_mount(&mount->fs_mount)) {
        mp_raise_ValueError("Could not mount");
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW(zfs_mount_obj, 2, zfs_mount);

STATIC const mp_rom_map_elem_t zfs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&zfs_mount_obj) },
};

STATIC MP_DEFINE_CONST_DICT(zfs_locals_dict, zfs_locals_dict_table);

#ifdef CONFIG_FAT_FILESYSTEM_ELM
STATIC mp_obj_t zfs_fat_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    zfs_fat_t *self = m_new_obj(zfs_fat_t);
    self->base.type = &zfs_fat_type;
    return MP_OBJ_FROM_PTR(self);
}

const mp_obj_type_t zfs_fat_type = {
    { &mp_type_type },
    .name = MP_QSTR_ZfsFat,
    .make_new = zfs_fat_make_new,
    .locals_dict = (void*)&zfs_locals_dict,
};
#endif

#ifdef CONFIG_FILE_SYSTEM_LITTLEFS
STATIC mp_obj_t zfs_little_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    zfs_little_t *self = m_new_obj(zfs_little_t);
    self->base.type = &zfs_little_type;

    self->littlefs.cfg.read_size = CONFIG_FS_LITTLEFS_READ_SIZE;
    self->littlefs.cfg.prog_size = CONFIG_FS_LITTLEFS_PROG_SIZE;
    self->littlefs.cfg.cache_size = CONFIG_FS_LITTLEFS_CACHE_SIZE;
    self->littlefs.cfg.lookahead_size = CONFIG_FS_LITTLEFS_LOOKAHEAD_SIZE;

    self->littlefs.cfg.read_buffer = m_new(u8_t, CONFIG_FS_LITTLEFS_CACHE_SIZE);
    self->littlefs.cfg.prog_buffer = m_new(u8_t, CONFIG_FS_LITTLEFS_CACHE_SIZE);
    self->littlefs.cfg.lookahead_buffer = m_new(u32_t, CONFIG_FS_LITTLEFS_LOOKAHEAD_SIZE / sizeof(u32_t));

    return MP_OBJ_FROM_PTR(self);
}

const mp_obj_type_t zfs_little_type = {
    { &mp_type_type },
    .name = MP_QSTR_ZfsLittle,
    .make_new = zfs_little_make_new,
    .locals_dict = (void*)&zfs_locals_dict,
};
#endif
