/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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

// This file should be compiled when included from vfs_lfs.c.
#if defined(LFS_BUILD_VERSION)

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/binary.h"
#include "py/objarray.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "shared/timeutils/timeutils.h"

#if !MICROPY_ENABLE_FINALISER
#error "MICROPY_VFS_LFS requires MICROPY_ENABLE_FINALISER"
#endif

static int MP_VFS_LFSx(dev_ioctl)(const struct LFSx_API (config) * c, int cmd, int arg, bool must_return_int) {
    mp_obj_t ret = mp_vfs_blockdev_ioctl(c->context, cmd, arg);
    int ret_i = 0;
    if (must_return_int || ret != mp_const_none) {
        ret_i = mp_obj_get_int(ret);
    }
    return ret_i;
}

static int MP_VFS_LFSx(dev_read)(const struct LFSx_API (config) * c, LFSx_API(block_t) block, LFSx_API(off_t) off, void *buffer, LFSx_API(size_t) size) {
    return mp_vfs_blockdev_read_ext(c->context, block, off, size, buffer);
}

static int MP_VFS_LFSx(dev_prog)(const struct LFSx_API (config) * c, LFSx_API(block_t) block, LFSx_API(off_t) off, const void *buffer, LFSx_API(size_t) size) {
    return mp_vfs_blockdev_write_ext(c->context, block, off, size, buffer);
}

static int MP_VFS_LFSx(dev_erase)(const struct LFSx_API (config) * c, LFSx_API(block_t) block) {
    return MP_VFS_LFSx(dev_ioctl)(c, MP_BLOCKDEV_IOCTL_BLOCK_ERASE, block, true);
}

static int MP_VFS_LFSx(dev_sync)(const struct LFSx_API (config) * c) {
    return MP_VFS_LFSx(dev_ioctl)(c, MP_BLOCKDEV_IOCTL_SYNC, 0, false);
}

static void MP_VFS_LFSx(init_config)(MP_OBJ_VFS_LFSx * self, mp_obj_t bdev, size_t read_size, size_t prog_size, size_t lookahead) {
    self->blockdev.flags = MP_BLOCKDEV_FLAG_FREE_OBJ;
    mp_vfs_blockdev_init(&self->blockdev, bdev);

    struct LFSx_API (config) * config = &self->config;
    memset(config, 0, sizeof(*config));

    config->context = &self->blockdev;

    config->read = MP_VFS_LFSx(dev_read);
    config->prog = MP_VFS_LFSx(dev_prog);
    config->erase = MP_VFS_LFSx(dev_erase);
    config->sync = MP_VFS_LFSx(dev_sync);

    MP_VFS_LFSx(dev_ioctl)(config, MP_BLOCKDEV_IOCTL_INIT, 1, false); // initialise block device
    int bs = MP_VFS_LFSx(dev_ioctl)(config, MP_BLOCKDEV_IOCTL_BLOCK_SIZE, 0, true); // get block size
    int bc = MP_VFS_LFSx(dev_ioctl)(config, MP_BLOCKDEV_IOCTL_BLOCK_COUNT, 0, true); // get block count
    self->blockdev.block_size = bs;

    config->read_size = read_size;
    config->prog_size = prog_size;
    config->block_size = bs;
    config->block_count = bc;

    #if LFS_BUILD_VERSION == 1
    config->lookahead = lookahead;
    config->read_buffer = m_new(uint8_t, config->read_size);
    config->prog_buffer = m_new(uint8_t, config->prog_size);
    config->lookahead_buffer = m_new(uint8_t, config->lookahead / 8);
    #else
    config->block_cycles = 100;
    config->cache_size = MIN(config->block_size, (4 * MAX(read_size, prog_size)));
    config->lookahead_size = lookahead;
    config->read_buffer = m_new(uint8_t, config->cache_size);
    config->prog_buffer = m_new(uint8_t, config->cache_size);
    config->lookahead_buffer = m_new(uint8_t, config->lookahead_size);
    #ifdef LFS2_MULTIVERSION
    // This can be set to override the on-disk lfs version.
    // eg. for compat with lfs2 < v2.6 add the following to make:
    // CFLAGS += '-DLFS2_MULTIVERSION=0x00020000'
    config->disk_version = LFS2_MULTIVERSION;
    #endif
    #endif
}

const char *MP_VFS_LFSx(make_path)(MP_OBJ_VFS_LFSx * self, mp_obj_t path_in) {
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

static mp_obj_t MP_VFS_LFSx(make_new)(const mp_obj_type_t * type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(lfs_make_allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(lfs_make_allowed_args), lfs_make_allowed_args, args);

    MP_OBJ_VFS_LFSx *self = m_new0(MP_OBJ_VFS_LFSx, 1);
    self->base.type = type;
    vstr_init(&self->cur_dir, 16);
    vstr_add_byte(&self->cur_dir, '/');
    #if LFS_BUILD_VERSION == 2
    self->enable_mtime = args[LFS_MAKE_ARG_mtime].u_bool;
    #endif
    MP_VFS_LFSx(init_config)(self, args[LFS_MAKE_ARG_bdev].u_obj,
        args[LFS_MAKE_ARG_readsize].u_int, args[LFS_MAKE_ARG_progsize].u_int, args[LFS_MAKE_ARG_lookahead].u_int);
    int ret = LFSx_API(mount)(&self->lfs, &self->config);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t MP_VFS_LFSx(mkfs)(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_arg_val_t args[MP_ARRAY_SIZE(lfs_make_allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(lfs_make_allowed_args), lfs_make_allowed_args, args);

    MP_OBJ_VFS_LFSx self;
    MP_VFS_LFSx(init_config)(&self, args[LFS_MAKE_ARG_bdev].u_obj,
        args[LFS_MAKE_ARG_readsize].u_int, args[LFS_MAKE_ARG_progsize].u_int, args[LFS_MAKE_ARG_lookahead].u_int);
    int ret = LFSx_API(format)(&self.lfs, &self.config);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(MP_VFS_LFSx(mkfs_fun_obj), 0, MP_VFS_LFSx(mkfs));
static MP_DEFINE_CONST_STATICMETHOD_OBJ(MP_VFS_LFSx(mkfs_obj), MP_ROM_PTR(&MP_VFS_LFSx(mkfs_fun_obj)));

// Implementation of mp_vfs_lfs_file_open is provided in vfs_lfsx_file.c
static MP_DEFINE_CONST_FUN_OBJ_3(MP_VFS_LFSx(open_obj), MP_VFS_LFSx(file_open));

typedef struct MP_VFS_LFSx (_ilistdir_it_t) {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_fun_1_t finaliser;
    bool is_str;
    MP_OBJ_VFS_LFSx *vfs;
    LFSx_API(dir_t) dir;
} MP_VFS_LFSx(ilistdir_it_t);

static mp_obj_t MP_VFS_LFSx(ilistdir_it_iternext)(mp_obj_t self_in) {
    MP_VFS_LFSx(ilistdir_it_t) * self = MP_OBJ_TO_PTR(self_in);

    if (self->vfs == NULL) {
        return MP_OBJ_STOP_ITERATION;
    }

    struct LFSx_API (info) info;
    for (;;) {
        int ret = LFSx_API(dir_read)(&self->vfs->lfs, &self->dir, &info);
        if (ret == 0) {
            LFSx_API(dir_close)(&self->vfs->lfs, &self->dir);
            self->vfs = NULL;
            return MP_OBJ_STOP_ITERATION;
        }
        if (!(info.name[0] == '.' && (info.name[1] == '\0'
                                      || (info.name[1] == '.' && info.name[2] == '\0')))) {
            break;
        }
    }

    // make 4-tuple with info about this entry
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));
    if (self->is_str) {
        t->items[0] = mp_obj_new_str_from_cstr(info.name);
    } else {
        t->items[0] = mp_obj_new_bytes((const byte *)info.name, strlen(info.name));
    }
    t->items[1] = MP_OBJ_NEW_SMALL_INT(info.type == LFSx_MACRO(_TYPE_REG) ? MP_S_IFREG : MP_S_IFDIR);
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // no inode number
    t->items[3] = MP_OBJ_NEW_SMALL_INT(info.size);

    return MP_OBJ_FROM_PTR(t);
}

static mp_obj_t MP_VFS_LFSx(ilistdir_it_del)(mp_obj_t self_in) {
    MP_VFS_LFSx(ilistdir_it_t) * self = MP_OBJ_TO_PTR(self_in);
    if (self->vfs != NULL) {
        LFSx_API(dir_close)(&self->vfs->lfs, &self->dir);
    }
    return mp_const_none;
}

static mp_obj_t MP_VFS_LFSx(ilistdir_func)(size_t n_args, const mp_obj_t *args) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(args[0]);
    bool is_str_type = true;
    const char *path;
    if (n_args == 2) {
        if (mp_obj_get_type(args[1]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = MP_VFS_LFSx(make_path)(self, args[1]);
    } else {
        path = vstr_null_terminated_str(&self->cur_dir);
    }

    MP_VFS_LFSx(ilistdir_it_t) * iter = mp_obj_malloc_with_finaliser(MP_VFS_LFSx(ilistdir_it_t), &mp_type_polymorph_iter_with_finaliser);

    iter->iternext = MP_VFS_LFSx(ilistdir_it_iternext);
    iter->finaliser = MP_VFS_LFSx(ilistdir_it_del);
    iter->is_str = is_str_type;
    int ret = LFSx_API(dir_open)(&self->lfs, &iter->dir, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    iter->vfs = self;
    return MP_OBJ_FROM_PTR(iter);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(MP_VFS_LFSx(ilistdir_obj), 1, 2, MP_VFS_LFSx(ilistdir_func));

static mp_obj_t MP_VFS_LFSx(remove)(mp_obj_t self_in, mp_obj_t path_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    const char *path = MP_VFS_LFSx(make_path)(self, path_in);
    int ret = LFSx_API(remove)(&self->lfs, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(MP_VFS_LFSx(remove_obj), MP_VFS_LFSx(remove));

static mp_obj_t MP_VFS_LFSx(rmdir)(mp_obj_t self_in, mp_obj_t path_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    const char *path = MP_VFS_LFSx(make_path)(self, path_in);
    int ret = LFSx_API(remove)(&self->lfs, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(MP_VFS_LFSx(rmdir_obj), MP_VFS_LFSx(rmdir));

static mp_obj_t MP_VFS_LFSx(rename)(mp_obj_t self_in, mp_obj_t path_old_in, mp_obj_t path_new_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    const char *path_old = MP_VFS_LFSx(make_path)(self, path_old_in);
    const char *path = mp_obj_str_get_str(path_new_in);
    vstr_t path_new;
    vstr_init(&path_new, vstr_len(&self->cur_dir));
    if (path[0] != '/') {
        vstr_add_strn(&path_new, vstr_str(&self->cur_dir), vstr_len(&self->cur_dir));
    }
    vstr_add_str(&path_new, path);
    int ret = LFSx_API(rename)(&self->lfs, path_old, vstr_null_terminated_str(&path_new));
    vstr_clear(&path_new);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(MP_VFS_LFSx(rename_obj), MP_VFS_LFSx(rename));

static mp_obj_t MP_VFS_LFSx(mkdir)(mp_obj_t self_in, mp_obj_t path_o) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    const char *path = MP_VFS_LFSx(make_path)(self, path_o);
    int ret = LFSx_API(mkdir)(&self->lfs, path);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(MP_VFS_LFSx(mkdir_obj), MP_VFS_LFSx(mkdir));

static mp_obj_t MP_VFS_LFSx(chdir)(mp_obj_t self_in, mp_obj_t path_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);

    // Check path exists
    const char *path = MP_VFS_LFSx(make_path)(self, path_in);
    if (path[1] != '\0') {
        // Not at root, check it exists
        struct LFSx_API (info) info;
        int ret = LFSx_API(stat)(&self->lfs, path, &info);
        if (ret < 0 || info.type != LFSx_MACRO(_TYPE_DIR)) {
            mp_raise_OSError(MP_ENOENT);
        }
    }

    // Update cur_dir with new path
    if (path == vstr_str(&self->cur_dir)) {
        self->cur_dir.len = strlen(path);
    } else {
        vstr_reset(&self->cur_dir);
        vstr_add_str(&self->cur_dir, path);
    }

    // If not at root add trailing / to make it easy to build paths
    // and then normalise the path
    if (vstr_len(&self->cur_dir) != 1) {
        vstr_add_byte(&self->cur_dir, '/');

        #define CWD_LEN (vstr_len(&self->cur_dir))
        size_t to = 1;
        size_t from = 1;
        char *cwd = vstr_str(&self->cur_dir);
        while (from < CWD_LEN) {
            for (; from < CWD_LEN && cwd[from] == '/'; ++from) {
                // Scan for the start
            }
            if (from > to) {
                // Found excessive slash chars, squeeze them out
                vstr_cut_out_bytes(&self->cur_dir, to, from - to);
                from = to;
            }
            for (; from < CWD_LEN && cwd[from] != '/'; ++from) {
                // Scan for the next /
            }
            if ((from - to) == 1 && cwd[to] == '.') {
                // './', ignore
                vstr_cut_out_bytes(&self->cur_dir, to, ++from - to);
                from = to;
            } else if ((from - to) == 2 && cwd[to] == '.' && cwd[to + 1] == '.') {
                // '../', skip back
                if (to > 1) {
                    // Only skip back if not at the tip
                    for (--to; to > 1 && cwd[to - 1] != '/'; --to) {
                        // Skip back
                    }
                }
                vstr_cut_out_bytes(&self->cur_dir, to, ++from - to);
                from = to;
            } else {
                // Normal element, keep it and just move the offset
                to = ++from;
            }
        }
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(MP_VFS_LFSx(chdir_obj), MP_VFS_LFSx(chdir));

static mp_obj_t MP_VFS_LFSx(getcwd)(mp_obj_t self_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    if (vstr_len(&self->cur_dir) == 1) {
        return MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
    } else {
        // don't include trailing /
        return mp_obj_new_str(self->cur_dir.buf, self->cur_dir.len - 1);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(MP_VFS_LFSx(getcwd_obj), MP_VFS_LFSx(getcwd));

static mp_obj_t MP_VFS_LFSx(stat)(mp_obj_t self_in, mp_obj_t path_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    const char *path = MP_VFS_LFSx(make_path)(self, path_in);
    struct LFSx_API (info) info;
    int ret = LFSx_API(stat)(&self->lfs, path, &info);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }

    mp_timestamp_t mtime = 0;
    #if LFS_BUILD_VERSION == 2
    uint8_t mtime_buf[8];
    lfs2_ssize_t sz = lfs2_getattr(&self->lfs, path, LFS_ATTR_MTIME, &mtime_buf, sizeof(mtime_buf));
    if (sz == sizeof(mtime_buf)) {
        uint64_t ns = 0;
        for (size_t i = sizeof(mtime_buf); i > 0; --i) {
            ns = ns << 8 | mtime_buf[i - 1];
        }
        // On-disk storage of timestamps uses 1970 as the Epoch, so convert to host's Epoch.
        mtime = timeutils_seconds_since_epoch_from_nanoseconds_since_1970(ns);
    }
    #endif

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(info.type == LFSx_MACRO(_TYPE_REG) ? MP_S_IFREG : MP_S_IFDIR); // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // st_gid
    t->items[6] = mp_obj_new_int_from_uint(info.size); // st_size
    t->items[7] = timeutils_obj_from_timestamp(mtime); // st_atime
    t->items[8] = timeutils_obj_from_timestamp(mtime); // st_mtime
    t->items[9] = timeutils_obj_from_timestamp(mtime); // st_ctime

    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(MP_VFS_LFSx(stat_obj), MP_VFS_LFSx(stat));

static int LFSx_API(traverse_cb)(void *data, LFSx_API(block_t) bl) {
    (void)bl;
    uint32_t *n = (uint32_t *)data;
    *n += 1;
    return LFSx_MACRO(_ERR_OK);
}

static mp_obj_t MP_VFS_LFSx(statvfs)(mp_obj_t self_in, mp_obj_t path_in) {
    (void)path_in;
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    uint32_t n_used_blocks = 0;
    #if LFS_BUILD_VERSION == 1
    int ret = LFSx_API(traverse)(&self->lfs, LFSx_API(traverse_cb), &n_used_blocks);
    #else
    int ret = LFSx_API(fs_traverse)(&self->lfs, LFSx_API(traverse_cb), &n_used_blocks);
    #endif
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
    t->items[9] = MP_OBJ_NEW_SMALL_INT(LFSx_MACRO(_NAME_MAX)); // f_namemax

    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(MP_VFS_LFSx(statvfs_obj), MP_VFS_LFSx(statvfs));

static mp_obj_t MP_VFS_LFSx(mount)(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    (void)mkfs;

    // Make block device read-only if requested.
    if (mp_obj_is_true(readonly)) {
        self->blockdev.writeblocks[0] = MP_OBJ_NULL;
    }

    // Already called LFSx_API(mount) in MP_VFS_LFSx(make_new) so the filesystem is ready.

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(MP_VFS_LFSx(mount_obj), MP_VFS_LFSx(mount));

static mp_obj_t MP_VFS_LFSx(umount)(mp_obj_t self_in) {
    MP_OBJ_VFS_LFSx *self = MP_OBJ_TO_PTR(self_in);
    // LFS unmount never fails
    LFSx_API(unmount)(&self->lfs);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(MP_VFS_LFSx(umount_obj), MP_VFS_LFSx(umount));

static const mp_rom_map_elem_t MP_VFS_LFSx(locals_dict_table)[] = {
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&MP_VFS_LFSx(mkfs_obj)) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&MP_VFS_LFSx(open_obj)) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&MP_VFS_LFSx(ilistdir_obj)) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&MP_VFS_LFSx(mkdir_obj)) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&MP_VFS_LFSx(rmdir_obj)) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&MP_VFS_LFSx(chdir_obj)) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&MP_VFS_LFSx(getcwd_obj)) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&MP_VFS_LFSx(remove_obj)) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&MP_VFS_LFSx(rename_obj)) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&MP_VFS_LFSx(stat_obj)) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&MP_VFS_LFSx(statvfs_obj)) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&MP_VFS_LFSx(mount_obj)) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&MP_VFS_LFSx(umount_obj)) },
};
static MP_DEFINE_CONST_DICT(MP_VFS_LFSx(locals_dict), MP_VFS_LFSx(locals_dict_table));

static mp_import_stat_t MP_VFS_LFSx(import_stat)(void *self_in, const char *path) {
    MP_OBJ_VFS_LFSx *self = self_in;
    struct LFSx_API (info) info;
    mp_obj_str_t path_obj = { { &mp_type_str }, 0, 0, (const byte *)path };
    path = MP_VFS_LFSx(make_path)(self, MP_OBJ_FROM_PTR(&path_obj));
    int ret = LFSx_API(stat)(&self->lfs, path, &info);
    if (ret == 0) {
        if (info.type == LFSx_MACRO(_TYPE_REG)) {
            return MP_IMPORT_STAT_FILE;
        } else {
            return MP_IMPORT_STAT_DIR;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

static const mp_vfs_proto_t MP_VFS_LFSx(proto) = {
    .import_stat = MP_VFS_LFSx(import_stat),
};

#if LFS_BUILD_VERSION == 1
#define VFS_LFSx_QSTR MP_QSTR_VfsLfs1
#else
#define VFS_LFSx_QSTR MP_QSTR_VfsLfs2
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    MP_TYPE_VFS_LFSx,
    VFS_LFSx_QSTR,
    MP_TYPE_FLAG_NONE,
    make_new, MP_VFS_LFSx(make_new),
    protocol, &MP_VFS_LFSx(proto),
    locals_dict, &MP_VFS_LFSx(locals_dict)
    );

#undef VFS_LFSx_QSTR

#endif // defined(LFS_BUILD_VERSION)
