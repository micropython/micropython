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

// ROMFS filesystem format
// =======================
//
// ROMFS is a flexible and extensible filesystem format designed to represent a
// directory hierarchy with files, where those files are read-only and their data
// can be memory mapped.
//
// Concepts:
// - varuint: An unsigned integer that is encoded in a variable number of bytes. It is
//   stored big-endian with the high bit of the byte set if there are following bytes.
// - record: A variable sized element with a type.  It is stored as two varuint's and then
//   a payload.  The first varuint is the record kind and the second varuint is the
//   payload length (which may be zero bytes long).
//
// A ROMFS filesystem is a record with record kind 0x14a6b1, chosen so the encoded value
// is 0xd2-0xcd-0x31 which is "RM1" with the first two bytes having their high bit set.
// If the ROMFS record's payload is non-empty then it contains records.
//
// Record types:
// - 0 = unused, can be used to detect corruption of the filesystem.
// - 1 = padding/comments, can contain any data in their payload.
// - 2 = verbatim data, used to store file data.
// - 3 = indirect data, pointer to offset within the ROMFS payload.
// - 4 = a directory: payload contains a varuint which is the length of the directory
//       name in bytes, then the name, then optional nested records for the contents
//       of the directory (including optional metadata).
// - 5 = a file: payload contains a varuint which is the length of the filename in bytes
//       then the name, then optional nested records.
//
// Remarks:
// - A varuint can be padded if needed by prepending with one or more 0x80 bytes.  This
//   padding does not change any semantics.
// - The size of the ROMFS record (including kind and length and payload) must be a
//   multiple of 2 (because it's not possible to add a padding record of one byte).
// - File data can be optionally aligned using padding records and/or indirect data
//   records.
// - There is no limit to the size of directory/file names or file data.
//
// Unknown record types must be skipped over.  They may in the future add optional
// features, while still retaining backwards compatibility.  Such features may be:
// - Alignment requirements of the ROMFS record.
// - Timestamps on directories/files.
// - A precomputed hash of a file, or other metadata.
// - An optimised lookup table indexing the directory hierarchy.

#include <string.h>

#include "py/bc.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/vfs_rom.h"

#if MICROPY_VFS_ROM

#define ROMFS_SIZE_MIN (4)
#define ROMFS_HEADER_BYTE0 (0x80 | 'R')
#define ROMFS_HEADER_BYTE1 (0x80 | 'M')
#define ROMFS_HEADER_BYTE2 (0x00 | '1')

// Values for `record_kind_t`.
#define ROMFS_RECORD_KIND_UNUSED (0)
#define ROMFS_RECORD_KIND_PADDING (1)
#define ROMFS_RECORD_KIND_DATA_VERBATIM (2)
#define ROMFS_RECORD_KIND_DATA_POINTER (3)
#define ROMFS_RECORD_KIND_DIRECTORY (4)
#define ROMFS_RECORD_KIND_FILE (5)
#define ROMFS_RECORD_KIND_FILESYSTEM (0x14a6b1)

typedef mp_uint_t record_kind_t;

struct _mp_obj_vfs_rom_t {
    mp_obj_base_t base;
    mp_obj_t memory;
    const uint8_t *filesystem;
    const uint8_t *filesystem_end;
};

// Returns 0 for success, -1 for failure.
static int mp_decode_uint_checked(const uint8_t **ptr, const uint8_t *ptr_max, mp_uint_t *value_out) {
    mp_uint_t unum = 0;
    byte val;
    const uint8_t *p = *ptr;
    do {
        if (p >= ptr_max) {
            return -1;
        }
        val = *p++;
        unum = (unum << 7) | (val & 0x7f);
    } while ((val & 0x80) != 0);
    *ptr = p;
    *value_out = unum;
    return 0;
}

static record_kind_t extract_record(const uint8_t **fs, const uint8_t **fs_next, const uint8_t *fs_max) {
    mp_uint_t record_kind;
    if (mp_decode_uint_checked(fs, fs_max, &record_kind) != 0) {
        return ROMFS_RECORD_KIND_UNUSED;
    }
    mp_uint_t record_len;
    if (mp_decode_uint_checked(fs, fs_max, &record_len) != 0) {
        return ROMFS_RECORD_KIND_UNUSED;
    }
    *fs_next = *fs + record_len;
    return record_kind;
}

// Returns 0 for success, a negative integer for failure.
static int extract_data(mp_obj_vfs_rom_t *self, const uint8_t *fs, const uint8_t *fs_top, size_t *size_out, const uint8_t **data_out) {
    while (fs < fs_top) {
        const uint8_t *fs_next;
        record_kind_t record_kind = extract_record(&fs, &fs_next, fs_top);
        if (record_kind == ROMFS_RECORD_KIND_UNUSED) {
            // Corrupt filesystem.
            break;
        } else if (record_kind == ROMFS_RECORD_KIND_DATA_VERBATIM) {
            // Verbatim data.
            if (size_out != NULL) {
                *size_out = fs_next - fs;
                *data_out = fs;
            }
            return 0;
        } else if (record_kind == ROMFS_RECORD_KIND_DATA_POINTER) {
            // Pointer to data.
            mp_uint_t size;
            if (mp_decode_uint_checked(&fs, fs_next, &size) != 0) {
                break;
            }
            mp_uint_t offset;
            if (mp_decode_uint_checked(&fs, fs_next, &offset) != 0) {
                break;
            }
            if (size_out != NULL) {
                *size_out = size;
                *data_out = self->filesystem + offset;
            }
            return 0;
        } else {
            // Skip this record.
            fs = fs_next;
        }
    }
    return -MP_EIO;
}

// Searches for `path` in the filesystem.
// `path` must be null-terminated.
mp_import_stat_t mp_vfs_rom_search_filesystem(mp_obj_vfs_rom_t *self, const char *path, size_t *size_out, const uint8_t **data_out) {
    const uint8_t *fs = self->filesystem;
    const uint8_t *fs_top = self->filesystem_end;
    size_t path_len = strlen(path);
    if (*path == '/') {
        // An optional slash at the start of the path enters the top-level filesystem.
        ++path;
        --path_len;
    }
    while (path_len > 0 && fs < fs_top) {
        const uint8_t *fs_next;
        record_kind_t record_kind = extract_record(&fs, &fs_next, fs_top);
        if (record_kind == ROMFS_RECORD_KIND_UNUSED) {
            // Corrupt filesystem.
            return MP_IMPORT_STAT_NO_EXIST;
        } else if (record_kind == ROMFS_RECORD_KIND_DIRECTORY || record_kind == ROMFS_RECORD_KIND_FILE) {
            // A directory or file record.
            mp_uint_t name_len;
            if (mp_decode_uint_checked(&fs, fs_next, &name_len) != 0) {
                // Corrupt filesystem.
                return MP_IMPORT_STAT_NO_EXIST;
            }
            if ((name_len == path_len
                 || (name_len < path_len && path[name_len] == '/'))
                && memcmp(path, fs, name_len) == 0) {
                // Name matches, so enter this record.
                fs += name_len;
                fs_top = fs_next;
                path += name_len;
                path_len -= name_len;
                if (record_kind == ROMFS_RECORD_KIND_DIRECTORY) {
                    // Continue searching in this directory.
                    if (*path == '/') {
                        ++path;
                        --path_len;
                    }
                } else {
                    // Return this file.
                    if (path_len != 0) {
                        return MP_IMPORT_STAT_NO_EXIST;
                    }
                    if (extract_data(self, fs, fs_top, size_out, data_out) != 0) {
                        // Corrupt filesystem.
                        return MP_IMPORT_STAT_NO_EXIST;
                    }
                    return MP_IMPORT_STAT_FILE;
                }
            } else {
                // Skip this directory/file record.
                fs = fs_next;
            }
        } else {
            // Skip this record.
            fs = fs_next;
        }
    }
    if (path_len == 0) {
        if (size_out != NULL) {
            *size_out = fs_top - fs;
            *data_out = fs;
        }
        return MP_IMPORT_STAT_DIR;
    }
    return MP_IMPORT_STAT_NO_EXIST;
}

static mp_obj_t vfs_rom_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_vfs_rom_t *self = m_new_obj(mp_obj_vfs_rom_t);
    self->base.type = type;
    self->memory = args[0];

    // Get the ROMFS memory region.
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(self->memory, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len < ROMFS_SIZE_MIN) {
        mp_raise_OSError(MP_ENODEV);
    }
    self->filesystem = bufinfo.buf;

    // Verify it is a ROMFS.
    if (!(self->filesystem[0] == ROMFS_HEADER_BYTE0
          && self->filesystem[1] == ROMFS_HEADER_BYTE1
          && self->filesystem[2] == ROMFS_HEADER_BYTE2)) {
        mp_raise_OSError(MP_ENODEV);
    }

    // The ROMFS is a record itself, so enter into it and compute its limit.
    record_kind_t record_kind = extract_record(&self->filesystem, &self->filesystem_end, self->filesystem + bufinfo.len);
    if (record_kind != ROMFS_RECORD_KIND_FILESYSTEM) {
        mp_raise_OSError(MP_ENODEV);
    }

    // Check the filesystem is within the limits of the input buffer.
    if (self->filesystem_end > (const uint8_t *)bufinfo.buf + bufinfo.len) {
        mp_raise_OSError(MP_ENODEV);
    }

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t vfs_rom_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    (void)self_in;
    (void)readonly;
    if (mp_obj_is_true(mkfs)) {
        mp_raise_OSError(MP_EPERM);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_rom_mount_obj, vfs_rom_mount);

// mp_vfs_rom_file_open is implemented in vfs_rom_file.c.
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_rom_open_obj, mp_vfs_rom_file_open);

static mp_obj_t vfs_rom_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_rom_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_vfs_rom_get_path_str(self, path_in);
    if (path[0] == '/' && path[1] == '\0') {
        // Allow chdir to the root of the filesystem.
    } else {
        // Don't allow chdir to any subdirectory (not currently implemented).
        mp_raise_OSError(MP_EOPNOTSUPP);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_rom_chdir_obj, vfs_rom_chdir);

typedef struct _vfs_rom_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    mp_obj_vfs_rom_t *vfs_rom;
    bool is_str;
    const uint8_t *index;
    const uint8_t *index_top;
} vfs_rom_ilistdir_it_t;

static mp_obj_t vfs_rom_ilistdir_it_iternext(mp_obj_t self_in) {
    vfs_rom_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    while (self->index < self->index_top) {
        const uint8_t *index_next;
        record_kind_t record_kind = extract_record(&self->index, &index_next, self->index_top);
        uint32_t type;
        mp_uint_t name_len;
        size_t data_len;
        if (record_kind == ROMFS_RECORD_KIND_UNUSED) {
            // Corrupt filesystem.
            self->index = self->index_top;
            break;
        } else if (record_kind == ROMFS_RECORD_KIND_DIRECTORY || record_kind == ROMFS_RECORD_KIND_FILE) {
            // A directory or file record.
            if (mp_decode_uint_checked(&self->index, index_next, &name_len) != 0) {
                // Corrupt filesystem.
                self->index = self->index_top;
                break;
            }
            if (record_kind == ROMFS_RECORD_KIND_DIRECTORY) {
                // A directory.
                type = MP_S_IFDIR;
                data_len = index_next - self->index - name_len;
            } else {
                // A file.
                type = MP_S_IFREG;
                const uint8_t *data_value;
                if (extract_data(self->vfs_rom, self->index + name_len, index_next, &data_len, &data_value) != 0) {
                    // Corrupt filesystem.
                    break;
                }
            }
        } else {
            // Skip this record.
            self->index = index_next;
            continue;
        }

        const uint8_t *name_str = self->index;
        self->index = index_next;

        // Make 4-tuple with info about this entry: (name, attr, inode, size)
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(4, NULL));

        if (self->is_str) {
            t->items[0] = mp_obj_new_str((const char *)name_str, name_len);
        } else {
            t->items[0] = mp_obj_new_bytes(name_str, name_len);
        }

        t->items[1] = MP_OBJ_NEW_SMALL_INT(type);
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
        t->items[3] = mp_obj_new_int(data_len);

        return MP_OBJ_FROM_PTR(t);
    }

    return MP_OBJ_STOP_ITERATION;
}

static mp_obj_t vfs_rom_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_rom_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_rom_ilistdir_it_t *iter = m_new_obj(vfs_rom_ilistdir_it_t);
    iter->base.type = &mp_type_polymorph_iter;
    iter->iternext = vfs_rom_ilistdir_it_iternext;
    iter->vfs_rom = self;
    iter->is_str = mp_obj_get_type(path_in) == &mp_type_str;
    const char *path = mp_vfs_rom_get_path_str(self, path_in);
    size_t size;
    if (mp_vfs_rom_search_filesystem(self, path, &size, &iter->index) != MP_IMPORT_STAT_DIR) {
        mp_raise_OSError(MP_ENOENT);
    }
    iter->index_top = iter->index + size;
    return MP_OBJ_FROM_PTR(iter);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_rom_ilistdir_obj, vfs_rom_ilistdir);

static mp_obj_t vfs_rom_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_rom_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = mp_vfs_rom_get_path_str(self, path_in);
    size_t file_size;
    const uint8_t *file_data;
    mp_import_stat_t stat = mp_vfs_rom_search_filesystem(self, path, &file_size, &file_data);
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
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_rom_stat_obj, vfs_rom_stat);

static mp_obj_t vfs_rom_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_rom_t *self = MP_OBJ_TO_PTR(self_in);
    (void)path_in;
    size_t filesystem_len = self->filesystem_end - self->filesystem;
    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    t->items[0] = MP_OBJ_NEW_SMALL_INT(1); // f_bsize
    t->items[1] = MP_OBJ_NEW_SMALL_INT(0); // f_frsize
    t->items[2] = mp_obj_new_int_from_uint(filesystem_len); // f_blocks
    t->items[3] = MP_OBJ_NEW_SMALL_INT(0); // f_bfree
    t->items[4] = MP_OBJ_NEW_SMALL_INT(0); // f_bavail
    t->items[5] = MP_OBJ_NEW_SMALL_INT(0); // f_files
    t->items[6] = MP_OBJ_NEW_SMALL_INT(0); // f_ffree
    t->items[7] = MP_OBJ_NEW_SMALL_INT(0); // f_favail
    t->items[8] = MP_OBJ_NEW_SMALL_INT(0); // f_flags
    t->items[9] = MP_OBJ_NEW_SMALL_INT(32767); // f_namemax
    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_rom_statvfs_obj, vfs_rom_statvfs);

static const mp_rom_map_elem_t vfs_rom_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_rom_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_rom_open_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_rom_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_rom_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_rom_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_rom_statvfs_obj) },
};
static MP_DEFINE_CONST_DICT(vfs_rom_locals_dict, vfs_rom_locals_dict_table);

static mp_import_stat_t mp_vfs_rom_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_rom_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_vfs_rom_search_filesystem(self, path, NULL, NULL);
}

static const mp_vfs_proto_t vfs_rom_proto = {
    .import_stat = mp_vfs_rom_import_stat,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_rom,
    MP_QSTR_VfsRom,
    MP_TYPE_FLAG_NONE,
    make_new, vfs_rom_make_new,
    protocol, &vfs_rom_proto,
    locals_dict, &vfs_rom_locals_dict
    );

#endif // MICROPY_VFS_ROM
