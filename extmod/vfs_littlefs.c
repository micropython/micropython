/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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

#include "py/runtime.h"
#include "py/stream.h"
#include "py/binary.h"
#include "py/objarray.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/vfs_littlefs.h"

#if MICROPY_VFS && MICROPY_VFS_LITTLEFS

size_t strspn(const char *s, const char *accept) {
    const char *ss = s;
    while (*s && strchr(accept, *s) != NULL) {
        ++s;
    }
    return s - ss;
}

size_t strcspn(const char *s, const char *reject) {
    const char *ss = s;
    while (*s && strchr(reject, *s) == NULL) {
        ++s;
    }
    return s - ss;
}

STATIC int dev_ioctl(const struct lfs_config *c, int cmd, int arg) {
    mp_obj_t dest[4];
    mp_load_method(MP_OBJ_FROM_PTR(c->context), MP_QSTR_ioctl, dest);
    dest[2] = MP_OBJ_NEW_SMALL_INT(cmd);
    dest[3] = MP_OBJ_NEW_SMALL_INT(arg);
    return mp_obj_get_int(mp_call_method_n_kw(2, 0, dest));
}

STATIC int dev_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    mp_obj_vfs_littlefs_t *vfs = c->context;

    mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, size, buffer};

    mp_obj_t dest[5];
    mp_load_method(MP_OBJ_FROM_PTR(vfs), MP_QSTR_readblocks, dest);
    dest[2] = MP_OBJ_NEW_SMALL_INT(block);
    dest[3] = MP_OBJ_NEW_SMALL_INT(off);
    dest[4] = MP_OBJ_FROM_PTR(&ar);
    mp_call_method_n_kw(3, 0, dest);

    // TODO handle error return

    return 0;
}

STATIC int dev_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
    mp_obj_vfs_littlefs_t *vfs = c->context;

    mp_obj_array_t ar = {{&mp_type_bytearray}, BYTEARRAY_TYPECODE, 0, size, (void*)buffer};

    mp_obj_t dest[5];
    mp_load_method(MP_OBJ_FROM_PTR(vfs), MP_QSTR_writeblocks, dest);
    dest[2] = MP_OBJ_NEW_SMALL_INT(block);
    dest[3] = MP_OBJ_NEW_SMALL_INT(off);
    dest[4] = MP_OBJ_FROM_PTR(&ar);
    mp_call_method_n_kw(3, 0, dest);

    return 0;
}

STATIC int dev_erase(const struct lfs_config *c, lfs_block_t block) {
    dev_ioctl(c, 4, block); // erase
    // TODO handle error return
    return 0;
}

STATIC int dev_sync(const struct lfs_config *c) {
    dev_ioctl(c, 0, 0); // sync
    // TODO handle error return
    return 0;
}

STATIC void init_config(struct lfs_config *config, mp_obj_t bdev) {
    config->context = MP_OBJ_TO_PTR(bdev);

    config->read = dev_read;
    config->prog = dev_prog;
    config->erase = dev_erase;
    config->sync = dev_sync;

    int bs = dev_ioctl(config, 1, 0); // get block size
    int bc = dev_ioctl(config, 2, 0); // get block count
    int rws = dev_ioctl(config, 3, 0); // get read/write size

    config->read_size = rws;
    config->prog_size = rws;
    config->block_size = bs;
    config->block_count = bc;
    config->lookahead = 128;

    config->read_buffer = m_new(uint8_t, config->read_size);
    config->prog_buffer = m_new(uint8_t, config->prog_size);
    config->lookahead_buffer = m_new(uint8_t, config->lookahead / 8);
    config->file_buffer = m_new(uint8_t, config->prog_size);
}

const char *mp_obj_vfs_littlefs_make_path(mp_obj_vfs_littlefs_t *self, mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);
    if (path[0] != '/') {
        size_t l = vstr_len(&self->cur_dir);
        if (l > 0) {
            vstr_add_str(&self->cur_dir, path);
            path = vstr_null_terminated_str(&self->cur_dir);
            self->cur_dir.len = l;
        }
    }
    return path;
}

STATIC mp_obj_t vfs_littlefs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    (void)type;
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_vfs_littlefs_t *self = m_new0(mp_obj_vfs_littlefs_t, 1);
    self->base.type = &mp_type_vfs_littlefs;
    vstr_init(&self->cur_dir, 16);
    vstr_add_byte(&self->cur_dir, '/');
    init_config(&self->config, args[0]);
    int ret = lfs_mount(&self->lfs, &self->config);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t vfs_littlefs_mkfs(mp_obj_t bdev) {
    struct lfs_config config;
    lfs_t lfs;
    init_config(&config, bdev);
    int ret = lfs_format(&lfs, &config);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_littlefs_mkfs_fun_obj, vfs_littlefs_mkfs);
STATIC MP_DEFINE_CONST_STATICMETHOD_OBJ(vfs_littlefs_mkfs_obj, MP_ROM_PTR(&vfs_littlefs_mkfs_fun_obj));

// Implementation of mp_vfs_littlefs_file_open is provided in vfs_littlefs_file.c
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_littlefs_open_obj, mp_vfs_littlefs_file_open);

typedef struct _mp_vfs_littlefs_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    mp_obj_vfs_littlefs_t *vfs;
    lfs_dir_t dir;
} mp_vfs_littlefs_ilistdir_it_t;

STATIC mp_obj_t mp_vfs_littlefs_ilistdir_it_iternext(mp_obj_t self_in) {
    mp_vfs_littlefs_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    struct lfs_info info;
    for (;;) {
        int ret = lfs_dir_read(&self->vfs->lfs, &self->dir, &info);
        if (ret == 0) {
            lfs_dir_close(&self->vfs->lfs, &self->dir);
            return MP_OBJ_STOP_ITERATION;
        }
        if (!(info.name[0] == '.' && (info.name[1] == '\0'
            || (info.name[1] == '.' && info.name[2] == '\0')))) {
            break;
        }
    }

    // make 3-tuple with info about this entry
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
    if (self->is_str) {
        t->items[0] = mp_obj_new_str(info.name, strlen(info.name));
    } else {
        t->items[0] = mp_obj_new_bytes((const byte*)info.name, strlen(info.name));
    }
    t->items[1] = MP_OBJ_NEW_SMALL_INT(info.type == LFS_TYPE_REG ? MP_S_IFREG : MP_S_IFDIR);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // no inode number

    return MP_OBJ_FROM_PTR(t);
}

STATIC mp_obj_t vfs_littlefs_ilistdir_func(size_t n_args, const mp_obj_t *args) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(args[0]);
    bool is_str_type = true;
    const char *path;
    if (n_args == 2) {
        if (mp_obj_get_type(args[1]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = mp_obj_vfs_littlefs_make_path(self, args[1]);
    } else {
        path = vstr_null_terminated_str(&self->cur_dir);
    }

    mp_vfs_littlefs_ilistdir_it_t *iter = m_new_obj(mp_vfs_littlefs_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = mp_vfs_littlefs_ilistdir_it_iternext;
    iter->is_str = is_str_type;
    iter->vfs = self;
    int ret = lfs_dir_open(&self->lfs, &iter->dir, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return MP_OBJ_FROM_PTR(iter);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(vfs_littlefs_ilistdir_obj, 1, 2, vfs_littlefs_ilistdir_func);

STATIC mp_obj_t vfs_littlefs_remove(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_obj_vfs_littlefs_make_path(self, path_in);
    int ret = lfs_remove(&self->lfs, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_littlefs_remove_obj, vfs_littlefs_remove);

STATIC mp_obj_t vfs_littlefs_rmdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_obj_vfs_littlefs_make_path(self, path_in);
    int ret = lfs_remove(&self->lfs, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_littlefs_rmdir_obj, vfs_littlefs_rmdir);

STATIC mp_obj_t vfs_littlefs_rename(mp_obj_t self_in, mp_obj_t path_old_in, mp_obj_t path_new_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path_old = mp_obj_vfs_littlefs_make_path(self, path_old_in);
    vstr_t path_new;
    vstr_init(&path_new, vstr_len(&self->cur_dir));
    vstr_add_strn(&path_new, vstr_str(&self->cur_dir), vstr_len(&self->cur_dir));
    vstr_add_str(&path_new, mp_obj_str_get_str(path_new_in));
    int ret = lfs_rename(&self->lfs, path_old, vstr_null_terminated_str(&path_new));
    vstr_clear(&path_new);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_littlefs_rename_obj, vfs_littlefs_rename);

STATIC mp_obj_t vfs_littlefs_mkdir(mp_obj_t self_in, mp_obj_t path_o) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_obj_vfs_littlefs_make_path(self, path_o);
    int ret = lfs_mkdir(&self->lfs, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_littlefs_mkdir_obj, vfs_littlefs_mkdir);

STATIC mp_obj_t vfs_littlefs_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_obj_str_get_str(path_in);
    if (path[0] == '/') {
        // change absolute
        vstr_reset(&self->cur_dir);
    }
    vstr_add_str(&self->cur_dir, path);
    if (vstr_len(&self->cur_dir) == 1) {
        // at root, it exists
    } else {
        // not at root, check it exists
        struct lfs_info info;
        int ret = lfs_stat(&self->lfs, vstr_null_terminated_str(&self->cur_dir), &info);
        if (ret < 0 || info.type != LFS_TYPE_DIR) {
            mp_raise_OSError(-MP_ENOENT);
        }
        // add trailing / to make it easy to build paths
        vstr_add_byte(&self->cur_dir, '/');
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_littlefs_chdir_obj, vfs_littlefs_chdir);

STATIC mp_obj_t vfs_littlefs_getcwd(mp_obj_t self_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    if (vstr_len(&self->cur_dir) == 1) {
        return MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
    } else {
        // don't include trailing /
        return mp_obj_new_str(self->cur_dir.buf, self->cur_dir.len - 1);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_littlefs_getcwd_obj, vfs_littlefs_getcwd);

STATIC mp_obj_t vfs_littlefs_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_obj_str_get_str(path_in);
    struct lfs_info info;
    int ret = lfs_stat(&self->lfs, path, &info);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(info.type == LFS_TYPE_REG ? MP_S_IFREG : MP_S_IFDIR); // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // st_gid
    t->items[6] = mp_obj_new_int_from_uint(info.size); // st_size
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // st_atime
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // st_mtime
    t->items[9] = MP_OBJ_NEW_SMALL_INT(0); // st_ctime

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_littlefs_stat_obj, vfs_littlefs_stat);

STATIC int lfs_traverse_cb(void *data, lfs_block_t bl) {
    (void)bl;
    uint32_t *n = (uint32_t*)data;
    *n += 1;
    return LFS_ERR_OK;
}

STATIC mp_obj_t vfs_littlefs_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    (void)path_in;
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    uint32_t n_used_blocks = 0;
    int ret = lfs_traverse(&self->lfs, lfs_traverse_cb, &n_used_blocks);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(self->lfs.cfg->block_size); // f_bsize
    t->items[1] = t->items[0]; // f_frsize
    t->items[2] = MP_OBJ_NEW_SMALL_INT(self->lfs.cfg->block_count); // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(self->lfs.cfg->block_count - n_used_blocks); // f_bfree
    t->items[4] = t->items[3]; // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(LFS_NAME_MAX); // f_namemax

    return MP_OBJ_FROM_PTR(t);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(vfs_littlefs_statvfs_obj, vfs_littlefs_statvfs);

STATIC mp_obj_t vfs_littlefs_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    (void)self_in;
    (void)readonly;
    (void)mkfs;
    // already called lfs_mount in vfs_littlefs_make_new
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(vfs_littlefs_mount_obj, vfs_littlefs_mount);

STATIC mp_obj_t vfs_littlefs_umount(mp_obj_t self_in) {
    mp_obj_vfs_littlefs_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = lfs_unmount(&self->lfs);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(vfs_littlefs_umount_obj, vfs_littlefs_umount);

STATIC const mp_rom_map_elem_t vfs_littlefs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&vfs_littlefs_mkfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_littlefs_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_littlefs_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&vfs_littlefs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&vfs_littlefs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_littlefs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&vfs_littlefs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&vfs_littlefs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&vfs_littlefs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_littlefs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_littlefs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_littlefs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&vfs_littlefs_umount_obj) },
};
STATIC MP_DEFINE_CONST_DICT(vfs_littlefs_locals_dict, vfs_littlefs_locals_dict_table);

STATIC mp_import_stat_t vfs_littlefs_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_littlefs_t *self = self_in;
    struct lfs_info info;
    int ret = lfs_stat(&self->lfs, path, &info);
    if (ret == 0) {
        if (info.type == LFS_TYPE_REG) {
            return MP_IMPORT_STAT_FILE;
        } else {
            return MP_IMPORT_STAT_DIR;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

STATIC const mp_vfs_proto_t vfs_littlefs_proto = {
    .import_stat = vfs_littlefs_import_stat,
};

const mp_obj_type_t mp_type_vfs_littlefs = {
    { &mp_type_type },
    .name = MP_QSTR_VfsLittle,
    .make_new = vfs_littlefs_make_new,
    .protocol = &vfs_littlefs_proto,
    .locals_dict = (mp_obj_dict_t*)&vfs_littlefs_locals_dict,
};

#endif // MICROPY_VFS && MICROPY_VFS_LITTLEFS
