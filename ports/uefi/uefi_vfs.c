// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// VFS backend over EFI_SIMPLE_FILE_SYSTEM_PROTOCOL / EFI_FILE_PROTOCOL, mounted at
// "/" on the volume our .efi was loaded from (the boot volume). Provides the file
// object (stream protocol) and the VFS object (open/ilistdir/stat/mkdir/remove/
// rename + import_stat), so open(), import-from-disk and the os module all work.

#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/stream.h"
#include "py/objstr.h"
#include "extmod/vfs.h"
#include "uefi_port.h"
#include "efi.h"

#if MICROPY_VFS

#define EFI_PATH_MAX (255)

static int efi_err_to_errno(EFI_STATUS s) {
    if (s == EFI_NOT_FOUND) {
        return MP_ENOENT;
    }
    return MP_EIO;
}

// MicroPython path (utf8, '/' separators) -> EFI CHAR16 path ('\' separators,
// relative to the volume root). Empty/"/" becomes "." (the directory itself).
static void path_to_efi(const char *path, CHAR16 *out, size_t out_len) {
    while (*path == '/') {
        path++; // strip leading slashes (we open relative to root)
    }
    size_t i = 0;
    if (*path == '\0') {
        out[i++] = '.';
    } else {
        for (; path[0] != '\0' && i < out_len - 1; path++) {
            out[i++] = (path[0] == '/') ? '\\' : (CHAR16)(uint8_t)path[0];
        }
    }
    out[i] = 0;
}

// CHAR16 (ASCII subset) -> utf8 cstr into buf; returns length.
static size_t efi_name_to_utf8(const CHAR16 *name, char *buf, size_t buf_len) {
    size_t i = 0;
    for (; name[i] != 0 && i < buf_len - 1; i++) {
        buf[i] = (char)(name[i] & 0xff);
    }
    buf[i] = 0;
    return i;
}

/******************************************************************************/
// VFS object

typedef struct _mp_obj_vfs_efi_t {
    mp_obj_base_t base;
    EFI_FILE_PROTOCOL *root;
    char cwd[EFI_PATH_MAX + 1]; // current dir within the volume, leading '/'
} mp_obj_vfs_efi_t;

// Build the filesystem-absolute path (leading '/') from a possibly-relative path,
// resolving relative paths against the VFS cwd.
static void vfs_efi_resolve(mp_obj_vfs_efi_t *self, const char *path, char *out, size_t out_len) {
    size_t i = 0;
    if (path[0] != '/') {
        for (const char *c = self->cwd; *c && i < out_len - 1; c++) {
            out[i++] = *c;
        }
        if (i > 0 && out[i - 1] != '/' && i < out_len - 1) {
            out[i++] = '/';
        }
    }
    for (; *path && i < out_len - 1; path++) {
        out[i++] = *path;
    }
    out[i] = 0;
}

const mp_obj_type_t mp_type_vfs_efi;
extern const mp_obj_type_t mp_type_vfs_efi_textio;
extern const mp_obj_type_t mp_type_vfs_efi_fileio;

// Open a path on the volume (resolved against cwd). *out is a fresh EFI_FILE
// handle (caller closes).
static EFI_STATUS vfs_efi_open_raw(mp_obj_vfs_efi_t *self, const char *path, uint64_t mode, uint64_t attr, EFI_FILE_PROTOCOL **out) {
    char abs[EFI_PATH_MAX + 1];
    vfs_efi_resolve(self, path, abs, sizeof(abs));
    CHAR16 wpath[EFI_PATH_MAX + 1];
    path_to_efi(abs, wpath, MP_ARRAY_SIZE(wpath));
    return self->root->Open(self->root, out, wpath, mode, attr);
}

/******************************************************************************/
// File object (stream)

typedef struct _mp_obj_vfs_efi_file_t {
    mp_obj_base_t base;
    EFI_FILE_PROTOCOL *file; // NULL once closed
} mp_obj_vfs_efi_file_t;

static void vfs_efi_file_check_open(mp_obj_vfs_efi_file_t *o) {
    if (o->file == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}

static mp_uint_t vfs_efi_file_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_efi_file_t *o = MP_OBJ_TO_PTR(o_in);
    vfs_efi_file_check_open(o);
    UINTN n = size;
    EFI_STATUS s = o->file->Read(o->file, &n, buf);
    if (EFI_ERROR(s)) {
        *errcode = efi_err_to_errno(s);
        return MP_STREAM_ERROR;
    }
    return (mp_uint_t)n;
}

static mp_uint_t vfs_efi_file_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_efi_file_t *o = MP_OBJ_TO_PTR(o_in);
    vfs_efi_file_check_open(o);
    UINTN n = size;
    EFI_STATUS s = o->file->Write(o->file, &n, (void *)buf);
    if (EFI_ERROR(s)) {
        *errcode = efi_err_to_errno(s);
        return MP_STREAM_ERROR;
    }
    return (mp_uint_t)n;
}

static mp_uint_t vfs_efi_file_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_vfs_efi_file_t *o = MP_OBJ_TO_PTR(o_in);
    switch (request) {
        case MP_STREAM_FLUSH:
            vfs_efi_file_check_open(o);
            o->file->Flush(o->file);
            return 0;
        case MP_STREAM_SEEK: {
            vfs_efi_file_check_open(o);
            struct mp_stream_seek_t *seek = (struct mp_stream_seek_t *)(uintptr_t)arg;
            UINT64 pos;
            if (seek->whence == MP_SEEK_SET) {
                pos = (UINT64)seek->offset;
            } else if (seek->whence == MP_SEEK_CUR) {
                UINT64 cur;
                o->file->GetPosition(o->file, &cur);
                pos = cur + seek->offset;
            } else { // MP_SEEK_END
                o->file->SetPosition(o->file, 0xFFFFFFFFFFFFFFFFULL); // seek to EOF
                UINT64 end;
                o->file->GetPosition(o->file, &end);
                pos = end + seek->offset;
            }
            EFI_STATUS s = o->file->SetPosition(o->file, pos);
            if (EFI_ERROR(s)) {
                *errcode = efi_err_to_errno(s);
                return MP_STREAM_ERROR;
            }
            seek->offset = pos;
            return 0;
        }
        case MP_STREAM_CLOSE:
            if (o->file != NULL) {
                o->file->Close(o->file);
                o->file = NULL;
            }
            return 0;
        default:
            *errcode = MP_EINVAL;
            return MP_STREAM_ERROR;
    }
}

static mp_obj_t vfs_efi_file_make_new(mp_obj_vfs_efi_t *vfs, const char *path, const char *mode) {
    bool write = false, append = false, create = false, plus = false, text = true;
    for (; *mode; mode++) {
        switch (*mode) {
            case 'w':
                write = true;
                create = true;
                break;
            case 'a':
                append = true;
                create = true;
                break;
            case 'r':
                break;
            case '+':
                plus = true;
                break;
            case 'b':
                text = false;
                break;
            case 't':
                text = true;
                break;
        }
    }

    uint64_t efimode = EFI_FILE_MODE_READ;
    if (write || append || plus) {
        efimode |= EFI_FILE_MODE_WRITE;
    }
    if (create) {
        efimode |= EFI_FILE_MODE_CREATE;
    }

    // "w" truncates: delete any existing file first, then create fresh.
    if (write) {
        EFI_FILE_PROTOCOL *existing;
        if (!EFI_ERROR(vfs_efi_open_raw(vfs, path, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0, &existing))) {
            existing->Delete(existing); // deletes and closes
        }
    }

    EFI_FILE_PROTOCOL *file;
    EFI_STATUS s = vfs_efi_open_raw(vfs, path, efimode, 0, &file);
    if (EFI_ERROR(s)) {
        mp_raise_OSError(efi_err_to_errno(s));
    }
    if (append) {
        file->SetPosition(file, 0xFFFFFFFFFFFFFFFFULL); // seek to end
    }

    const mp_obj_type_t *type = text ? &mp_type_vfs_efi_textio : &mp_type_vfs_efi_fileio;
    mp_obj_vfs_efi_file_t *o = mp_obj_malloc_with_finaliser(mp_obj_vfs_efi_file_t, type);
    o->file = file;
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t vfs_efi_file_fileno(mp_obj_t self_in) {
    (void)self_in;
    return MP_OBJ_NEW_SMALL_INT(-1);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_efi_file_fileno_obj, vfs_efi_file_fileno);

static const mp_rom_map_elem_t vfs_efi_file_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&vfs_efi_file_fileno_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_readlines), MP_ROM_PTR(&mp_stream_unbuffered_readlines_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_seek), MP_ROM_PTR(&mp_stream_seek_obj) },
    { MP_ROM_QSTR(MP_QSTR_tell), MP_ROM_PTR(&mp_stream_tell_obj) },
    { MP_ROM_QSTR(MP_QSTR_flush), MP_ROM_PTR(&mp_stream_flush_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&mp_identity_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&mp_stream___exit___obj) },
};
static MP_DEFINE_CONST_DICT(vfs_efi_file_locals_dict, vfs_efi_file_locals_dict_table);

static const mp_stream_p_t vfs_efi_fileio_stream_p = {
    .read = vfs_efi_file_read,
    .write = vfs_efi_file_write,
    .ioctl = vfs_efi_file_ioctl,
};
MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_efi_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    protocol, &vfs_efi_fileio_stream_p,
    locals_dict, &vfs_efi_file_locals_dict
    );

static const mp_stream_p_t vfs_efi_textio_stream_p = {
    .read = vfs_efi_file_read,
    .write = vfs_efi_file_write,
    .ioctl = vfs_efi_file_ioctl,
    .is_text = true,
};
MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_efi_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    protocol, &vfs_efi_textio_stream_p,
    locals_dict, &vfs_efi_file_locals_dict
    );

/******************************************************************************/
// VFS methods

static mp_import_stat_t mp_vfs_efi_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_efi_t *self = self_in;
    EFI_FILE_PROTOCOL *f;
    if (EFI_ERROR(vfs_efi_open_raw(self, path, EFI_FILE_MODE_READ, 0, &f))) {
        return MP_IMPORT_STAT_NO_EXIST;
    }
    // GetInfo to learn if it's a directory.
    uint8_t info[sizeof(EFI_FILE_INFO) + 256];
    UINTN info_size = sizeof(info);
    EFI_GUID info_id = EFI_FILE_INFO_ID;
    mp_import_stat_t ret = MP_IMPORT_STAT_FILE;
    if (!EFI_ERROR(f->GetInfo(f, &info_id, &info_size, info))) {
        if (((EFI_FILE_INFO *)info)->Attribute & EFI_FILE_DIRECTORY) {
            ret = MP_IMPORT_STAT_DIR;
        }
    }
    f->Close(f);
    return ret;
}

static mp_obj_t vfs_efi_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    (void)self_in;
    (void)readonly;
    (void)mkfs;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_efi_mount_obj, vfs_efi_mount);

static mp_obj_t vfs_efi_umount(mp_obj_t self_in) {
    (void)self_in;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_efi_umount_obj, vfs_efi_umount);

static mp_obj_t vfs_efi_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    return vfs_efi_file_make_new(self, mp_obj_str_get_str(path_in), mp_obj_str_get_str(mode_in));
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_efi_open_obj, vfs_efi_open);

// ilistdir iterator
typedef struct _vfs_efi_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    EFI_FILE_PROTOCOL *dir; // NULL when exhausted
} vfs_efi_ilistdir_it_t;

static mp_obj_t vfs_efi_ilistdir_it_iternext(mp_obj_t self_in) {
    vfs_efi_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->dir == NULL) {
        return MP_OBJ_STOP_ITERATION;
    }
    for (;;) {
        uint8_t info[sizeof(EFI_FILE_INFO) + 2 * (EFI_PATH_MAX + 1)];
        UINTN n = sizeof(info);
        EFI_STATUS s = self->dir->Read(self->dir, &n, info);
        if (EFI_ERROR(s) || n == 0) {
            self->dir->Close(self->dir);
            self->dir = NULL;
            return MP_OBJ_STOP_ITERATION;
        }
        EFI_FILE_INFO *fi = (EFI_FILE_INFO *)info;
        char name[EFI_PATH_MAX + 1];
        size_t name_len = efi_name_to_utf8(fi->FileName, name, sizeof(name));
        if (name[0] == '.' && (name[1] == 0 || (name[1] == '.' && name[2] == 0))) {
            continue; // skip . and ..
        }
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));
        t->items[0] = self->is_str ? mp_obj_new_str(name, name_len) : mp_obj_new_bytes((const byte *)name, name_len);
        t->items[1] = MP_OBJ_NEW_SMALL_INT((fi->Attribute & EFI_FILE_DIRECTORY) ? MP_S_IFDIR : MP_S_IFREG);
        t->items[2] = MP_OBJ_NEW_SMALL_INT(0);
        return MP_OBJ_FROM_PTR(t);
    }
}

static mp_obj_t vfs_efi_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_FILE_PROTOCOL *dir;
    EFI_STATUS s = vfs_efi_open_raw(self, mp_obj_str_get_str(path_in), EFI_FILE_MODE_READ, 0, &dir);
    if (EFI_ERROR(s)) {
        mp_raise_OSError(efi_err_to_errno(s));
    }
    vfs_efi_ilistdir_it_t *iter = mp_obj_malloc_with_finaliser(vfs_efi_ilistdir_it_t, &mp_type_polymorph_iter_with_finaliser);
    iter->iternext = vfs_efi_ilistdir_it_iternext;
    iter->is_str = mp_obj_get_type(path_in) == &mp_type_str;
    iter->dir = dir;
    return MP_OBJ_FROM_PTR(iter);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_efi_ilistdir_obj, vfs_efi_ilistdir);

static mp_obj_t vfs_efi_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_FILE_PROTOCOL *f;
    EFI_STATUS s = vfs_efi_open_raw(self, mp_obj_str_get_str(path_in), EFI_FILE_MODE_READ, 0, &f);
    if (EFI_ERROR(s)) {
        mp_raise_OSError(efi_err_to_errno(s));
    }
    uint8_t info[sizeof(EFI_FILE_INFO) + 2 * (EFI_PATH_MAX + 1)];
    UINTN info_size = sizeof(info);
    EFI_GUID info_id = EFI_FILE_INFO_ID;
    EFI_STATUS gs = f->GetInfo(f, &info_id, &info_size, info);
    f->Close(f);
    if (EFI_ERROR(gs)) {
        mp_raise_OSError(efi_err_to_errno(gs));
    }
    EFI_FILE_INFO *fi = (EFI_FILE_INFO *)info;
    mp_obj_t t[10] = {0};
    t[0] = MP_OBJ_NEW_SMALL_INT((fi->Attribute & EFI_FILE_DIRECTORY) ? MP_S_IFDIR : MP_S_IFREG);
    for (int i = 1; i <= 5; i++) {
        t[i] = MP_OBJ_NEW_SMALL_INT(0);
    }
    t[6] = mp_obj_new_int_from_uint((mp_uint_t)fi->FileSize);
    for (int i = 7; i <= 9; i++) {
        t[i] = MP_OBJ_NEW_SMALL_INT(0);
    }
    return mp_obj_new_tuple(10, t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_efi_stat_obj, vfs_efi_stat);

static mp_obj_t vfs_efi_mkdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_FILE_PROTOCOL *dir;
    EFI_STATUS s = vfs_efi_open_raw(self, mp_obj_str_get_str(path_in),
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY, &dir);
    if (EFI_ERROR(s)) {
        mp_raise_OSError(efi_err_to_errno(s));
    }
    dir->Close(dir);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_efi_mkdir_obj, vfs_efi_mkdir);

static mp_obj_t vfs_efi_getcwd(mp_obj_t self_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_str_from_cstr(self->cwd);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_efi_getcwd_obj, vfs_efi_getcwd);

static mp_obj_t vfs_efi_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    char abs[EFI_PATH_MAX + 1];
    vfs_efi_resolve(self, mp_obj_str_get_str(path_in), abs, sizeof(abs));
    // Validate the target exists and is a directory.
    CHAR16 wpath[EFI_PATH_MAX + 1];
    path_to_efi(abs, wpath, MP_ARRAY_SIZE(wpath));
    EFI_FILE_PROTOCOL *d;
    if (EFI_ERROR(self->root->Open(self->root, &d, wpath, EFI_FILE_MODE_READ, 0))) {
        mp_raise_OSError(MP_ENOENT);
    }
    uint8_t info[sizeof(EFI_FILE_INFO) + 256];
    UINTN sz = sizeof(info);
    EFI_GUID id = EFI_FILE_INFO_ID;
    int isdir = !EFI_ERROR(d->GetInfo(d, &id, &sz, info)) &&
        (((EFI_FILE_INFO *)info)->Attribute & EFI_FILE_DIRECTORY);
    d->Close(d);
    if (!isdir) {
        mp_raise_OSError(MP_ENOTDIR);
    }
    size_t n = strlen(abs);
    if (n >= sizeof(self->cwd)) {
        n = sizeof(self->cwd) - 1;
    }
    memcpy(self->cwd, abs, n);
    self->cwd[n] = 0;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_efi_chdir_obj, vfs_efi_chdir);

static mp_obj_t vfs_efi_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    (void)path_in;
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    uint8_t info[sizeof(EFI_FILE_SYSTEM_INFO) + 2 * (EFI_PATH_MAX + 1)];
    UINTN info_size = sizeof(info);
    EFI_GUID info_id = EFI_FILE_SYSTEM_INFO_ID;
    if (EFI_ERROR(self->root->GetInfo(self->root, &info_id, &info_size, info))) {
        mp_raise_OSError(MP_EIO);
    }
    EFI_FILE_SYSTEM_INFO *fsi = (EFI_FILE_SYSTEM_INFO *)info;
    mp_uint_t bs = fsi->BlockSize ? fsi->BlockSize : 512;
    mp_obj_t t[10] = {0};
    t[0] = MP_OBJ_NEW_SMALL_INT(bs);                                  // f_bsize
    t[1] = MP_OBJ_NEW_SMALL_INT(bs);                                  // f_frsize
    t[2] = mp_obj_new_int_from_uint((mp_uint_t)(fsi->VolumeSize / bs)); // f_blocks
    t[3] = mp_obj_new_int_from_uint((mp_uint_t)(fsi->FreeSpace / bs));  // f_bfree
    t[4] = t[3];                                                      // f_bavail
    for (int i = 5; i <= 8; i++) {
        t[i] = MP_OBJ_NEW_SMALL_INT(0);
    }
    t[9] = MP_OBJ_NEW_SMALL_INT(EFI_PATH_MAX);                        // f_namemax
    return mp_obj_new_tuple(10, t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_efi_statvfs_obj, vfs_efi_statvfs);

static mp_obj_t vfs_efi_remove(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_efi_t *self = MP_OBJ_TO_PTR(self_in);
    EFI_FILE_PROTOCOL *f;
    EFI_STATUS s = vfs_efi_open_raw(self, mp_obj_str_get_str(path_in), EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0, &f);
    if (EFI_ERROR(s)) {
        mp_raise_OSError(efi_err_to_errno(s));
    }
    f->Delete(f); // deletes and closes
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_efi_remove_obj, vfs_efi_remove);

static const mp_rom_map_elem_t vfs_efi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_efi_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&vfs_efi_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_efi_open_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&vfs_efi_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_efi_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_efi_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_efi_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_efi_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&vfs_efi_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&vfs_efi_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&vfs_efi_remove_obj) },
};
static MP_DEFINE_CONST_DICT(vfs_efi_locals_dict, vfs_efi_locals_dict_table);

static const mp_vfs_proto_t vfs_efi_proto = {
    .import_stat = mp_vfs_efi_import_stat,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_efi,
    MP_QSTR_VfsEfi,
    MP_TYPE_FLAG_NONE,
    protocol, &vfs_efi_proto,
    locals_dict, &vfs_efi_locals_dict
    );

/******************************************************************************/
// Boot-volume location + mount at startup.

MP_REGISTER_ROOT_POINTER(struct _mp_obj_vfs_efi_t *uefi_vfs_boot);

// Build a VFS object from a SimpleFileSystem interface pointer (OpenVolume →
// fresh EFI_FILE root). Returned as an mp_obj_t (void* in the header); Python
// hands it to os.mount. Raises OSError if the volume can't be opened.
void *mp_uefi_vfs_from_simple_fs(void *fs_iface) {
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs = (EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *)fs_iface;
    EFI_FILE_PROTOCOL *root;
    EFI_STATUS s = fs->OpenVolume(fs, &root);
    if (EFI_ERROR(s)) {
        mp_raise_OSError(efi_err_to_errno(s));
    }
    mp_obj_vfs_efi_t *vfs = mp_obj_malloc(mp_obj_vfs_efi_t, &mp_type_vfs_efi);
    vfs->root = root;
    vfs->cwd[0] = '/';
    vfs->cwd[1] = 0;
    return MP_OBJ_FROM_PTR(vfs);
}

// Locate the EFI_FILE root of the volume our image was loaded from, create a VFS
// object, and mount it at "/". Best-effort: a missing/again filesystem is non-fatal.
void mp_uefi_vfs_mount_boot(void) {
    EFI_BOOT_SERVICES *bs = mp_uefi_st->BootServices;

    EFI_GUID li_guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;
    EFI_LOADED_IMAGE_PROTOCOL *li;
    if (EFI_ERROR(bs->HandleProtocol(mp_uefi_image, &li_guid, (void **)&li))) {
        return;
    }
    EFI_GUID fs_guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *fs;
    if (EFI_ERROR(bs->HandleProtocol(li->DeviceHandle, &fs_guid, (void **)&fs))) {
        return;
    }
    EFI_FILE_PROTOCOL *root;
    if (EFI_ERROR(fs->OpenVolume(fs, &root))) {
        return;
    }

    mp_obj_vfs_efi_t *vfs = mp_obj_malloc(mp_obj_vfs_efi_t, &mp_type_vfs_efi);
    vfs->root = root;
    vfs->cwd[0] = '/';
    vfs->cwd[1] = 0;
    MP_STATE_VM(uefi_vfs_boot) = vfs;

    mp_vfs_mount_t *mnt = m_new_obj(mp_vfs_mount_t);
    mnt->str = "/";
    mnt->len = 1;
    mnt->obj = MP_OBJ_FROM_PTR(vfs);
    mnt->next = MP_STATE_VM(vfs_mount_table);
    MP_STATE_VM(vfs_mount_table) = mnt;
    MP_STATE_VM(vfs_cur) = mnt;
}

#endif // MICROPY_VFS
