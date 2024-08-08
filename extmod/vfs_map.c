/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Damien P. George
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

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/vfs_map.h"

#if MICROPY_VFS_MAP

#define MAGIC_LEN (2)
#define MAGIC_BYTE0 ('M')
#define MAGIC_BYTE1 ('F')

#define GET_LE16(p) ((p)[0] | (p)[1] << 8)
#define GET_LE32(p) ((p)[0] | (p)[1] << 8 | (p)[2] << 16 | (p)[3] << 24)

struct _mp_obj_vfs_map_t {
    mp_obj_base_t base;
    mp_obj_t memory;
    mp_obj_t device;
    const uint8_t *filesystem;
};

mp_import_stat_t mp_vfs_map_search_filesystem(mp_obj_vfs_map_t *self, const char *path, size_t *size_out, const uint8_t **data_out) {
    if (!(self->filesystem[0] == MAGIC_BYTE0 && self->filesystem[1] == MAGIC_BYTE1)) {
        return MP_IMPORT_STAT_NO_EXIST;
    }
    if (path[0] == '/') {
        ++path;
    }
    size_t path_len = strlen(path);
    const uint8_t *fs = self->filesystem + MAGIC_LEN;
    for (;;) {
        uint16_t nlen = GET_LE16(fs);
        fs += 2;
        if (nlen == 0) {
            return MP_IMPORT_STAT_NO_EXIST;
        }
        uint32_t flen;
        mp_import_stat_t type;
        if (nlen & 0x8000) {
            // A directory.
            nlen &= 0x7fff;
            flen = 0;
            type = MP_IMPORT_STAT_DIR;
        } else {
            // A file.
            flen = GET_LE32(fs);
            fs += 4;
            type = MP_IMPORT_STAT_FILE;
        }
        if (path_len == nlen && memcmp(path, fs, path_len) == 0) {
            if (size_out != NULL) {
                *size_out = flen;
                *data_out = fs + nlen;
            }
            return type;
        }
        fs += nlen + flen;
    }
}

static inline const char *vfs_map_get_path_str(mp_obj_vfs_map_t *self, mp_obj_t path) {
    return mp_obj_str_get_str(path);
}

static mp_obj_t vfs_map_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 2, false);

    mp_obj_vfs_map_t *self = m_new_obj(mp_obj_vfs_map_t);
    self->base.type = type;
    self->memory = args[0];
    self->device = n_args == 1 ? mp_const_none : args[1];

    mp_buffer_info_t bufinfo;
    if (mp_get_buffer(self->memory, &bufinfo, MP_BUFFER_READ)) {
        self->filesystem = bufinfo.buf;
    } else {
        self->filesystem = (const uint8_t *)(uintptr_t)mp_obj_get_int(self->memory);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t vfs_map_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    (void)self_in;
    (void)readonly;
    if (mp_obj_is_true(mkfs)) {
        mp_raise_OSError(MP_EPERM);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_map_mount_obj, vfs_map_mount);

static mp_obj_t vfs_map_umount(mp_obj_t self_in) {
    (void)self_in;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_map_umount_obj, vfs_map_umount);

// mp_vfs_map_file_open is implemented in vfs_map_file.c.
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_map_open_obj, mp_vfs_map_file_open);

static mp_obj_t vfs_map_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_map_chdir_obj, vfs_map_chdir);

typedef struct _vfs_map_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    const char *path;
    const uint8_t *index;
} vfs_map_ilistdir_it_t;

static mp_obj_t vfs_map_ilistdir_it_iternext(mp_obj_t self_in) {
    vfs_map_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    if (self->index == NULL) {
        return MP_OBJ_STOP_ITERATION;
    }

    const char *path = self->path;
    if (path[0] == '/' || (path[0] == '.' && path[1] == '\0')) {
        ++path;
    }
    size_t path_len = strlen(path);

    for (;;) {
        uint16_t nlen = GET_LE16(self->index);
        self->index += 2;
        if (nlen == 0) {
            self->index = NULL;
            return MP_OBJ_STOP_ITERATION;
        }

        uint32_t flen;
        uint32_t type;
        if (nlen & 0x8000) {
            // dir
            nlen &= 0x7fff;
            flen = 0;
            type = MP_S_IFDIR;
        } else {
            // file
            flen = GET_LE32(self->index);
            self->index += 4;
            type = MP_S_IFREG;
        }

        const uint8_t *nstr = self->index;
        self->index += nlen + flen;

        if ((path_len == 0 && memchr(nstr, '/', nlen) == NULL)
            || (path_len < nlen && nstr[path_len] == '/' && memcmp(path, nstr, path_len) == 0 && memchr(nstr + path_len + 1, '/', nlen - path_len - 1) == NULL)) {
            // Make 4-tuple with info about this entry: (name, attr, inode, size)
            mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));

            if (path_len > 0) {
                nstr += path_len + 1;
                nlen -= path_len + 1;
            }

            if (self->is_str) {
                t->items[0] = mp_obj_new_str((const char *)nstr, nlen);
            } else {
                t->items[0] = mp_obj_new_bytes(nstr, nlen);
            }

            t->items[1] = MP_OBJ_NEW_SMALL_INT(type);
            t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
            t->items[3] = mp_obj_new_int(flen);

            return MP_OBJ_FROM_PTR(t);
        }
    }
}

static mp_obj_t vfs_map_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_map_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_map_ilistdir_it_t *iter = m_new_obj(vfs_map_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = vfs_map_ilistdir_it_iternext;
    iter->is_str = mp_obj_get_type(path_in) == &mp_type_str;
    iter->path = mp_obj_str_get_str(path_in);
    if (!(self->filesystem[0] == MAGIC_BYTE0 && self->filesystem[1] == MAGIC_BYTE1)) {
        mp_raise_OSError(MP_ENOENT);
    }
    if (iter->path[0] == '\0'
        || (iter->path[0] == '.' && iter->path[1] == '\0')
        || (iter->path[0] == '/' && iter->path[1] == '\0')) {
        // pass
    } else if (mp_vfs_map_search_filesystem(self, iter->path, NULL, NULL) != MP_IMPORT_STAT_DIR) {
        mp_raise_OSError(MP_ENOENT);
    }
    iter->index = self->filesystem + MAGIC_LEN;
    return MP_OBJ_FROM_PTR(iter);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_map_ilistdir_obj, vfs_map_ilistdir);

static mp_obj_t vfs_map_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_map_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = vfs_map_get_path_str(self, path_in);
    size_t file_size;
    const uint8_t *file_data;
    mp_import_stat_t stat = mp_vfs_map_search_filesystem(self, path, &file_size, &file_data);
    if (stat == MP_IMPORT_STAT_NO_EXIST) {
        mp_raise_OSError(MP_ENOENT);
    }
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(stat == MP_IMPORT_STAT_FILE ? MP_S_IFREG : MP_S_IFDIR); // st_mode
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // st_ino
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // st_dev
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // st_nlink
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // st_uid
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // st_gid
    t->items[6] = MP_OBJ_NEW_SMALL_INT(file_size); // st_size
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // st_atime
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // st_mtime
    t->items[9] = MP_OBJ_NEW_SMALL_INT(0); // st_ctime
    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_map_stat_obj, vfs_map_stat);

static mp_obj_t vfs_map_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    (void)self_in;
    (void)path_in;
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(0); // f_bsize
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // f_frsize
    t->items[2] = MP_OBJ_NEW_SMALL_INT(0); // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // f_bfree
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(65535); // f_namemax
    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_map_statvfs_obj, vfs_map_statvfs);

static mp_obj_t vfs_map_device(mp_obj_t self_in) {
    mp_obj_vfs_map_t *self = MP_OBJ_TO_PTR(self_in);
    return self->device;
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_map_device_obj, vfs_map_device);

static const mp_rom_map_elem_t vfs_map_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_map_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&vfs_map_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_map_open_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_map_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_map_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_map_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_map_statvfs_obj) },

    { MP_ROM_QSTR(MP_QSTR_device), MP_ROM_PTR(&vfs_map_device_obj) },
};
static MP_DEFINE_CONST_DICT(vfs_map_locals_dict, vfs_map_locals_dict_table);

static mp_import_stat_t mp_vfs_map_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_map_t *self = self_in;
    return mp_vfs_map_search_filesystem(self, path, NULL, NULL);
}

static size_t mp_vfs_map_memmap(void *self_in, const char *path, const void **ptr_out) {
    mp_obj_vfs_map_t *self = self_in;
    size_t size;
    const uint8_t *data;
    mp_import_stat_t stat = mp_vfs_map_search_filesystem(self, path, &size, &data);
    if (stat == MP_IMPORT_STAT_FILE) {
        *ptr_out = data;
        return size;
    } else {
        *ptr_out = NULL;
        return 0;
    }
}

static const mp_vfs_proto_t vfs_map_proto = {
    .import_stat = mp_vfs_map_import_stat,
    .memmap = mp_vfs_map_memmap,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_map,
    MP_QSTR_VfsMap,
    MP_TYPE_FLAG_NONE,
    make_new, vfs_map_make_new,
    protocol, &vfs_map_proto,
    locals_dict, &vfs_map_locals_dict
    );

#endif // MICROPY_VFS_MAP
