/*
* This file is part of the MicroPython project, http://micropython.org/
*
* The MIT License (MIT)
*
* Copyright (c) 2025 MASSDRIVER EI (massdriver.space)
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

#include "py/mpconfig.h"

#if CONFIG_FILE_SYSTEM

#if !MICROPY_VFS
#error "with CONFIG_FILE_SYSTEM enabled, must also enable MICROPY_VFS"
#endif

#include <zephyr/fs/fs.h>
#include <zephyr/fs/fs_sys.h>

#include <string.h>
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "shared/timeutils/timeutils.h"

// Declare each FSTAB entry
#define FOREACH_FS_DEFINE(fs) FS_FSTAB_DECLARE_ENTRY(fs);

#define FOREACH_FSTAB_DEFINE(n) DT_FOREACH_CHILD(n, FOREACH_FS_DEFINE)

DT_FOREACH_STATUS_OKAY(zephyr_fstab, FOREACH_FSTAB_DEFINE)

// Add all FSTAB entries to a table for us to use dynamically
#define FOREACH_FS(fs) FS_FSTAB_ENTRY(fs)

#define FOREACH_FSTAB(n) DT_FOREACH_CHILD(n, FOREACH_FS)

static struct fs_mount_t *const zephyr_fs_mounts[] = {
    &DT_FOREACH_STATUS_OKAY(zephyr_fstab, FOREACH_FSTAB),
};

#define zephyr_fs_mounts_size sizeof(zephyr_fs_mounts) / sizeof(struct fs_mount_t *)

typedef struct _zephyr_fs_obj_t {
    mp_obj_base_t base;
    struct fs_mount_t *mount;
    vstr_t cur_dir;
    vstr_t root_dir;
} zephyr_fs_obj_t;

const char *zephyr_fs_make_path(zephyr_fs_obj_t *self, mp_obj_t path_in) {
    const char *path = mp_obj_str_get_str(path_in);

    if (path[0] != '/') {
        size_t l = vstr_len(&self->root_dir);
        size_t lc = vstr_len(&self->cur_dir);
        vstr_add_str(&self->root_dir, "/");
        vstr_add_strn(&self->root_dir, self->cur_dir.buf, lc);
        vstr_add_str(&self->root_dir, path);
        path = vstr_null_terminated_str(&self->root_dir);
        self->root_dir.len = l;
    } else {
        size_t l = vstr_len(&self->root_dir);
        vstr_add_str(&self->root_dir, path);
        path = vstr_null_terminated_str(&self->root_dir);
        self->root_dir.len = l;
    }
    return path;
}

typedef struct _zephyr_file_obj_t {
    mp_obj_base_t base;
    struct fs_file_t fp;
} zephyr_file_obj_t;

static void file_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_printf(print, "<io.%s %p>", mp_obj_get_type_str(self_in), MP_OBJ_TO_PTR(self_in));
}

static mp_uint_t file_obj_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    zephyr_file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ssize_t err;

    err = fs_read(&self->fp, buf, size);
    if (err < 0) {
        *errcode = -err;
        return MP_STREAM_ERROR;
    }
    return err;
}

static mp_uint_t file_obj_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    zephyr_file_obj_t *self = MP_OBJ_TO_PTR(self_in);
    ssize_t err;

    err = fs_write(&self->fp, buf, size);
    if (err < 0) {
        *errcode = -err;
        return MP_STREAM_ERROR;
    }
    return err;
}

static mp_uint_t file_obj_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    zephyr_file_obj_t *self = MP_OBJ_TO_PTR(o_in);
    int err = -EINVAL;

    if (request == MP_STREAM_SEEK) {
        struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)(uintptr_t)arg;

        MP_STATIC_ASSERT(FS_SEEK_SET == MP_SEEK_SET);
        MP_STATIC_ASSERT(FS_SEEK_CUR == MP_SEEK_CUR);
        MP_STATIC_ASSERT(FS_SEEK_END == MP_SEEK_END);

        err = fs_seek(&self->fp, s->offset, s->whence);

        if (err < 0) {
            *errcode = -err;
            return MP_STREAM_ERROR;
        }

        off_t tell = fs_tell(&self->fp);
        if (tell < 0) {
            *errcode = -err;
            return MP_STREAM_ERROR;
        }
        s->offset = tell;
        return 0;

    } else if (request == MP_STREAM_FLUSH) {
        err = fs_sync(&self->fp);
        if (err < 0) {
            *errcode = -err;
            return MP_STREAM_ERROR;
        }
        return 0;

    } else if (request == MP_STREAM_CLOSE) {
        err = fs_close(&self->fp);
        if (err < 0) {
            *errcode = -err;
            return MP_STREAM_ERROR;
        }
        return 0;

    } else {
        *errcode = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

static const mp_rom_map_elem_t zephyr_fs_rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};

static MP_DEFINE_CONST_DICT(zephyr_fs_rawfile_locals_dict, zephyr_fs_rawfile_locals_dict_table);

static const mp_stream_p_t zephyr_fs_fileio_stream_p = {
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_zephyr_fs_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, file_obj_print,
    protocol, &zephyr_fs_fileio_stream_p,
    locals_dict, &zephyr_fs_rawfile_locals_dict
    );

static const mp_stream_p_t zephyr_fs_textio_stream_p = {
    .read = file_obj_read,
    .write = file_obj_write,
    .ioctl = file_obj_ioctl,
    .is_text = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_zephyr_fs_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, file_obj_print,
    protocol, &zephyr_fs_textio_stream_p,
    locals_dict, &zephyr_fs_rawfile_locals_dict
    );

// Factory function for I/O stream classes
static mp_obj_t zephyr_fs_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    const mp_obj_type_t *type = &mp_type_zephyr_fs_textio;
    int flags = 0;
    int err;
    bool x_mode = false;
    const char *mode_str = mp_obj_str_get_str(mode_in);
    const char *path = zephyr_fs_make_path(self, path_in);

    for (; *mode_str; ++mode_str) {
        int new_flags = 0;
        switch (*mode_str) {
            case 'r':
                new_flags = FS_O_READ;
                break;
            case 'w':
                new_flags = FS_O_WRITE | FS_O_CREATE | FS_O_TRUNC;
                break;
            case 'x':
                new_flags = FS_O_WRITE;
                x_mode = true;
                break;
            case 'a':
                new_flags = FS_O_WRITE | FS_O_CREATE | FS_O_APPEND;
                break;
            case '+':
                flags |= FS_O_RDWR;
                break;
            case 'b':
                type = &mp_type_zephyr_fs_fileio;
                break;
            case 't':
                type = &mp_type_zephyr_fs_textio;
                break;
        }
        if (new_flags) {
            if (flags) {
                mp_raise_ValueError(NULL);
            }
            flags = new_flags;
        }
    }

    zephyr_file_obj_t *o = mp_obj_malloc_with_finaliser(zephyr_file_obj_t, type);

    err = fs_open(&o->fp, path, flags);
    if (x_mode && err < 0) {
        err = fs_open(&o->fp, path, flags | FS_O_CREATE);
    } else if (x_mode) {
        fs_close(&o->fp);
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("file %s Already exists: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }
    if (err < 0) {
        m_del_obj(zephyr_file_obj_t, o);
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error opening file %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }

    return MP_OBJ_FROM_PTR(o);
}
MP_DEFINE_CONST_FUN_OBJ_3(zephyr_fs_open_obj, zephyr_fs_open);

typedef struct _mp_zephyr_fs_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_fun_1_t finaliser;
    bool is_str;
    struct fs_dir_t dir;
} mp_zephyr_fs_ilistdir_it_t;

static mp_obj_t mp_zephyr_fs_ilistdir_it_iternext(mp_obj_t self_in) {
    mp_zephyr_fs_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);
    int err;

    for (;;) {
        struct fs_dirent stats;
        err = fs_readdir(&self->dir, &stats);
        char *fn = stats.name;
        if (err < 0 || stats.name[0] == 0) {
            // stop on error or end of dir
            break;
        }

        // Note that Zephyr FS also already filters . and .., so we don't need to

        // make 4-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));
        if (self->is_str) {
            t->items[0] = mp_obj_new_str_from_cstr(fn);
        } else {
            t->items[0] = mp_obj_new_bytes((const byte *)fn, strlen(fn));
        }
        if (stats.type == FS_DIR_ENTRY_DIR) {
            // dir
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
        } else {
            // file
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
        }
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0);         // no inode number
        t->items[3] = mp_obj_new_int_from_uint(stats.size);

        return MP_OBJ_FROM_PTR(t);
    }

    // ignore error because we may be closing a second time
    fs_closedir(&self->dir);

    return MP_OBJ_STOP_ITERATION;
}

static mp_obj_t mp_zephyr_fs_ilistdir_it_del(mp_obj_t self_in) {
    mp_zephyr_fs_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);
    // ignore result / error because we may be closing a second time.
    fs_closedir(&self->dir);
    return mp_const_none;
}

static mp_obj_t zephyr_fs_ilistdir_func(size_t n_args, const mp_obj_t *args) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    bool is_str_type = true;
    const char *path;
    int err;

    if (n_args == 2) {
        if (mp_obj_get_type(args[1]) == &mp_type_bytes) {
            is_str_type = false;
        }
        path = zephyr_fs_make_path(self, args[1]);
    } else {
        path = "";
    }

    // Create a new iterator object to list the dir
    mp_zephyr_fs_ilistdir_it_t *iter = mp_obj_malloc_with_finaliser(mp_zephyr_fs_ilistdir_it_t, &mp_type_polymorph_iter_with_finaliser);
    iter->iternext = mp_zephyr_fs_ilistdir_it_iternext;
    iter->finaliser = mp_zephyr_fs_ilistdir_it_del;
    iter->is_str = is_str_type;

    err = fs_opendir(&iter->dir, path);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("could not open directory %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }

    return MP_OBJ_FROM_PTR(iter);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_fs_ilistdir_obj, 1, 2, zephyr_fs_ilistdir_func);

static mp_obj_t zephyr_fs_mkdir(mp_obj_t vfs_in, mp_obj_t path_o) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path = zephyr_fs_make_path(self, path_o);
    int err;

    err = fs_mkdir(path);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("could not mkdir %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_fs_mkdir_obj, zephyr_fs_mkdir);


static mp_obj_t zephyr_fs_chdir(mp_obj_t vfs_in, mp_obj_t path_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path = mp_obj_str_get_str(path_in);
    size_t lc = vstr_len(&self->cur_dir);
    size_t l = vstr_len(&self->root_dir);
    vstr_t chdir;
    struct fs_dirent stats;
    int err;


    vstr_init(&chdir, 32);
    vstr_add_strn(&chdir, self->root_dir.buf, l);
    if (path[0] != '/') {
        vstr_add_byte(&chdir, '/');
        vstr_add_strn(&chdir, self->cur_dir.buf, lc);
    }
    vstr_add_str(&chdir, path);
    vstr_add_byte(&chdir, '/');

        #define CWD_LEN (vstr_len(&chdir))
    size_t to = 1;
    size_t from = 1;
    char *cwd = vstr_str(&chdir);
    while (from < CWD_LEN) {
        for (; from < CWD_LEN && cwd[from] == '/'; ++from) {
            // Scan for the start
        }
        if (from > to) {
            // Found excessive slash chars, squeeze them out
            vstr_cut_out_bytes(&chdir, to, from - to);
            from = to;
        }
        for (; from < CWD_LEN && cwd[from] != '/'; ++from) {
            // Scan for the next /
        }
        if ((from - to) == 1 && cwd[to] == '.') {
            // './', ignore
            vstr_cut_out_bytes(&chdir, to, ++from - to);
            from = to;
        } else if ((from - to) == 2 && cwd[to] == '.' && cwd[to + 1] == '.') {
            // '../', skip back
            if (to > 1) {
                // Only skip back if not at the tip
                for (--to; to > 1 && cwd[to - 1] != '/'; --to) {
                    // Skip back
                }
            }
            vstr_cut_out_bytes(&chdir, to, ++from - to);
            from = to;
        } else {
            // Normal element, keep it and just move the offset
            to = ++from;
        }
    }

    err = fs_stat(vstr_null_terminated_str(&chdir), &stats);
    vstr_cut_out_bytes(&chdir, 0, l + 1);
    lc = vstr_len(&chdir);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("could not chdir to %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }

    vstr_reset(&self->cur_dir);
    vstr_add_strn(&self->cur_dir, chdir.buf, lc);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_fs_chdir_obj, zephyr_fs_chdir);

static mp_obj_t zephyr_fs_getcwd(mp_obj_t vfs_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    char *path;

    if (vstr_len(&self->cur_dir) == 0) {
        return MP_OBJ_NEW_QSTR(MP_QSTR__slash_);
    } else {
        size_t l = vstr_len(&self->root_dir);
        size_t lc = vstr_len(&self->cur_dir);
        vstr_add_str(&self->root_dir, "/");
        vstr_add_strn(&self->root_dir, self->cur_dir.buf, lc);
        path = vstr_null_terminated_str(&self->root_dir);
        self->root_dir.len = l;
        return mp_obj_new_str_from_cstr(path + l);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_fs_getcwd_obj, zephyr_fs_getcwd);

static mp_obj_t zephyr_fs_rmdir(mp_obj_t vfs_in, mp_obj_t path_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path = zephyr_fs_make_path(self, path_in);
    struct fs_dirent stats;
    int err;

    err = fs_stat(path, &stats);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error getting stats of Zephyr File System at %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }
    if (stats.type == FS_DIR_ENTRY_FILE) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("not for removing files"));
        return mp_const_none;
    }
    err = fs_unlink(path);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error removing directory at %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_fs_rmdir_obj, zephyr_fs_rmdir);

static mp_obj_t zephyr_fs_remove(mp_obj_t vfs_in, mp_obj_t path_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *path = zephyr_fs_make_path(self, path_in);
    struct fs_dirent stats;
    int err;

    err = fs_stat(path, &stats);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error getting stats of Zephyr File System at %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }
    if (stats.type == FS_DIR_ENTRY_DIR) {
        mp_raise_msg_varg(&mp_type_OSError, MP_ERROR_TEXT("not for removing directories"));
        return mp_const_none;
    }
    err = fs_unlink(path);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error removing file at %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_fs_remove_obj, zephyr_fs_remove);

static mp_obj_t zephyr_fs_rename(mp_obj_t vfs_in, mp_obj_t path_in, mp_obj_t path_out) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    const char *old_path = zephyr_fs_make_path(self, path_in);
    const char *new_path = zephyr_fs_make_path(self, path_out);
    int err;

    err = fs_rename(old_path, new_path);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error renaming file from %s to %s: %q"), old_path, new_path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_fs_rename_obj, zephyr_fs_rename);

static mp_import_stat_t zephyr_fs_import_stat(void *vfs_in, const char *path) {
    zephyr_fs_obj_t *self = vfs_in;
    struct fs_dirent stats;
    assert(self != NULL);
    const char *ppath = zephyr_fs_make_path(self, mp_obj_new_str_from_cstr(path));
    int err;

    err = fs_stat(ppath, &stats);
    if (err == 0) {
        if (stats.type == FS_DIR_ENTRY_DIR) {
            return MP_IMPORT_STAT_DIR;
        } else {
            return MP_IMPORT_STAT_FILE;
        }
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

static mp_obj_t zephyr_fs_stat(mp_obj_t vfs_in, mp_obj_t path_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    int err;
    struct fs_dirent stats;
    const char *path = zephyr_fs_make_path(self, path_in);

    err = fs_stat(path, &stats);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error getting stats of Zephyr File System at %s: %q"), path, mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    mp_int_t mode = 0;
    if (stats.type == FS_DIR_ENTRY_DIR) {
        mode |= MP_S_IFDIR;
    } else {
        mode |= MP_S_IFREG;
    }
    t->items[0] = MP_OBJ_NEW_SMALL_INT(mode);     // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0);     // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0);     // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0);     // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0);     // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0);     // st_gid
    t->items[6] = mp_obj_new_int_from_uint(stats.size);     // st_size
    t->items[7] = mp_obj_new_int_from_uint(0);     // st_atime
    t->items[8] = mp_obj_new_int_from_uint(0);     // st_mtime
    t->items[9] = mp_obj_new_int_from_uint(0);     // st_ctime

    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_fs_stat_obj, zephyr_fs_stat);

static mp_obj_t zephyr_fs_statvfs(mp_obj_t vfs_in, mp_obj_t path_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(vfs_in);
    (void)path_in;
    int err;
    struct fs_statvfs stats;
    unsigned long bsize;

    err = fs_statvfs(self->mount->mnt_point, &stats);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error getting stats of Zephyr File System: %q"), mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }

    if (stats.f_frsize > stats.f_bsize) {
        bsize = stats.f_frsize;
    } else {
        bsize = stats.f_bsize;
    }

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));

    t->items[0] = MP_OBJ_NEW_SMALL_INT(bsize);     // f_bsize
    t->items[1] = MP_OBJ_NEW_SMALL_INT(stats.f_frsize);     // f_frsize
    t->items[2] = MP_OBJ_NEW_SMALL_INT(stats.f_blocks);     // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(stats.f_bfree);     // f_bfree
    t->items[4] = t->items[3];     // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0);     // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0);     // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0);     // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0);     // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(MAX_FILE_NAME);     // f_namemax.

    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(zephyr_fs_statvfs_obj, zephyr_fs_statvfs);

static mp_obj_t zephyr_fs_umount(mp_obj_t self_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int err;

    err = fs_unmount(self->mount);
    if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error un-mounting Zephyr File System: %q"), mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_fs_umount_obj, zephyr_fs_umount);

static mp_obj_t zephyr_fs_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int err;
    (void)readonly;
    (void)mkfs;

    err = fs_mount(self->mount);
    if (err == -EBUSY) {
        err = fs_unmount(self->mount);
        if (err < 0) {
            mp_raise_msg_varg(&mp_type_OSError,
                MP_ERROR_TEXT("error un-mounting Zephyr File System: %q"), mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
            return mp_const_none;
        }
        err = fs_mount(self->mount);
    }

    if (err == -EROFS) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("read only filesystem that requires formatting"));
        return mp_const_none;
    } else if (err < 0) {
        mp_raise_msg_varg(&mp_type_OSError,
            MP_ERROR_TEXT("error mounting Zephyr File System: %q"), mp_errno_to_str(MP_OBJ_NEW_SMALL_INT(-err)));
        return mp_const_none;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(zephyr_fs_mount_obj, zephyr_fs_mount);

#if CONFIG_FILE_SYSTEM_MKFS
static mp_obj_t zephyr_fs_mkfs(mp_obj_t self_in) {
    zephyr_fs_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Yes, they correspond indirectly
    if (self->mount->fs->mkfs((int)self->mount->storage_dev, self->mount->fs_data, self->mount->flags) < 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("error formatting Zephyr File System"));
    }

    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(zephyr_fs_mkfs_obj, zephyr_fs_mkfs);
#endif

static mp_obj_t zephyr_fs_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    if (!mp_obj_is_str(args[0])) {
        mp_raise_ValueError(MP_ERROR_TEXT("argument must be a zephyr FSTAB mountpoint string"));
        return mp_const_none;
    }

    const char *_mountpoint = mp_obj_str_get_str(args[0]);
    int i = 0;

    for (; i < zephyr_fs_mounts_size; i++) {
        if (strcmp(_mountpoint, zephyr_fs_mounts[i]->mnt_point) == 0) {
            break;
        }
    }

    if (i == zephyr_fs_mounts_size) {
        mp_raise_ValueError(MP_ERROR_TEXT("couldn't find zephyr mountpoint"));
        return mp_const_none;
    }

    if (zephyr_fs_mounts[i]->fs == 0) {
        if (fs_mount(zephyr_fs_mounts[i]) != 0) {
            mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("FS is invalid, try adding automount to fstab"));
            return mp_const_none;
        }
    }

    // create new object
    zephyr_fs_obj_t *fs = mp_obj_malloc(zephyr_fs_obj_t, type);
    fs->mount = zephyr_fs_mounts[i];
    vstr_init(&fs->cur_dir, 32);
    vstr_init(&fs->root_dir, 128);
    vstr_add_str(&fs->root_dir, zephyr_fs_mounts[i]->mnt_point);

    return MP_OBJ_FROM_PTR(fs);
}

static mp_obj_t zephyr_fs_fstab(void) {
    mp_obj_t list = mp_obj_new_list(0, NULL);

    for (int i = 0; i < zephyr_fs_mounts_size; i++) {
        mp_obj_list_append(list, mp_obj_new_str_from_cstr(zephyr_fs_mounts[i]->mnt_point));
    }

    return list;
}
static MP_DEFINE_CONST_FUN_OBJ_0(zephyr_fs_fstab_fun_obj, zephyr_fs_fstab);
static MP_DEFINE_CONST_STATICMETHOD_OBJ(zephyr_fs_fstab_obj, MP_ROM_PTR(&zephyr_fs_fstab_fun_obj));

static const mp_rom_map_elem_t zephyr_fs_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&zephyr_fs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&zephyr_fs_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&zephyr_fs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&zephyr_fs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&zephyr_fs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&zephyr_fs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&zephyr_fs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&zephyr_fs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&zephyr_fs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&zephyr_fs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&zephyr_fs_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&zephyr_fs_ilistdir_obj) },
    #if CONFIG_FILE_SYSTEM_MKFS
    { MP_ROM_QSTR(MP_QSTR_mkfs), MP_ROM_PTR(&zephyr_fs_mkfs_obj) },
    #endif
    // Not part of the VFS API, used to list available File Systems
    { MP_ROM_QSTR(MP_QSTR_fstab), MP_ROM_PTR(&zephyr_fs_fstab_obj) },
};
static MP_DEFINE_CONST_DICT(zephyr_fs_locals_dict, zephyr_fs_locals_dict_table);

static const mp_vfs_proto_t zephyr_fs_proto = {
    .import_stat = zephyr_fs_import_stat,
};

const mp_obj_type_t zephyr_filesystem_type;

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_filesystem_type,
    MP_QSTR_FileSystem,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_fs_make_new,
    protocol, &zephyr_fs_proto,
    locals_dict, &zephyr_fs_locals_dict
    );

#endif // CONFIG_FILE_SYSTEM
