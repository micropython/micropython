#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "extmod/vfs.h"

#include "macutil.h"
#include "vfs_mac.h"
#include "debug_print.h"
#include "uart_core.h"

#define MAC_O_RDONLY (1)
#define MAC_O_WRONLY (2)
#define MAC_O_RDWR (3)
#define MAC_O_TRUNC (4)
#define MAC_O_APPEND (8)
#define MAC_O_CREAT (16)

// I have no idea whether mac is guaranteed not to generate these as file handles...
#define STDIN_FILENO (0)
#define STDOUT_FILENO (1)
#define STDERR_FILENO (2)

typedef struct _mp_obj_vfs_mac_t {
    mp_obj_base_t base;
    INTEGER volRefNum;
    size_t root_len;
    bool readonly;
} mp_obj_vfs_mac_t;

typedef struct _mp_obj_vfs_mac_file_t {
    mp_obj_base_t base;
    INTEGER fd;
    INTEGER volRefNum;
    int mode;
} mp_obj_vfs_mac_file_t;

static Byte *vfs_mac_get_path_cstr(Byte *pName, size_t pName_size, const char *path) {
    while (*path == '/') {
        path++;
    }
    pstr_from_cstr(pName, pName_size, "/");
    pstr_cat_cstr(pName, pName_size, path);
    for (int i = 1; i <= pName[0]; i++) {
        if (pName[i] == '/') {
            pName[i] = ':';
        }
    }
    return pName;
}

static Byte *vfs_mac_get_path_str(Byte *pName, size_t pName_size, mp_obj_t obj) {
    return vfs_mac_get_path_cstr(pName, pName_size, mp_obj_str_get_str(obj));
}

static VCB *getVolumeByName(mp_obj_t name) {
    size_t str_len;
    const char *str_data = mp_obj_str_get_data(name, &str_len);
    VCB *vol = (VCB *)LMGetVCBQHdr().qHead;
    while (vol) {
        if (PSTR_LEN(vol->vcbVN) == str_len &&
            memcmp(PSTR_DATA(vol->vcbVRefNum), str_data, str_len) == 0) {
            return vol;
        }
        vol = (VCB *)vol->qLink;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("volume not found (by name)"));
}

static VCB *getVolumeByVolumeReference(INTEGER vn) {
    VCB *vol = (VCB *)LMGetVCBQHdr().qHead;
    while (vol) {
        if (vol->vcbVRefNum == vn) {
            return vol;
        }
        vol = (VCB *)vol->qLink;
    }
    mp_raise_ValueError(MP_ERROR_TEXT("volume not found (by ref)"));
}


static void check_fd_is_open(const mp_obj_vfs_mac_file_t *o) {
    if (o->fd < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("I/O operation on closed file"));
    }
}

static void vfs_mac_file_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_vfs_mac_file_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->fd <= STDERR_FILENO) {
        mp_printf(print, "<io.%s %d>",
            mp_obj_get_type_str(self_in), self->fd);
    } else {
        VCB *vol = getVolumeByVolumeReference(self->volRefNum);
        mp_printf(print, "<io.%s %d on %.*s>",
            mp_obj_get_type_str(self_in), self->fd,
            (int)PSTR_LEN(vol->vcbVN), PSTR_DATA(vol->vcbVN));
    }
}

mp_obj_t mp_vfs_mac_file_open(const mp_obj_vfs_mac_t *fs, const mp_obj_type_t *type, mp_obj_t file_in, mp_obj_t mode_in) {
    const char *mode_s = mp_obj_str_get_str(mode_in);

    int mode_rw = 0, mode_x = 0;
    while (*mode_s) {
        switch (*mode_s++) {
            case 'r':
                mode_rw = MAC_O_RDONLY;
                break;
            case 'w':
                mode_rw = MAC_O_WRONLY;
                mode_x = MAC_O_CREAT | MAC_O_TRUNC;
                break;
            case 'a':
                mode_rw = MAC_O_WRONLY;
                mode_x = MAC_O_CREAT | MAC_O_APPEND;
                break;
            case '+':
                mode_rw = MAC_O_RDWR;
                break;
            case 'b':
                type = &mp_type_vfs_mac_fileio;
                break;
            case 't':
                type = &mp_type_vfs_mac_textio;
                break;
        }
    }

    mp_obj_vfs_mac_file_t *o = mp_obj_malloc_with_finaliser(mp_obj_vfs_mac_file_t, type);
    o->fd = -1; // In case open() fails below, initialise this as a "closed" file object.

    mp_obj_t fid = file_in;

    if (mp_obj_is_small_int(fid)) {
        o->fd = MP_OBJ_SMALL_INT_VALUE(fid);
        return MP_OBJ_FROM_PTR(o);
    }

    Str255 pName;
    vfs_mac_get_path_str(pName, sizeof(pName), fid);
    INTEGER fd;
    OSErr err = noErr;

    while (true) {
        OSErr err = FSOpen(pName, fs->volRefNum, &fd);
        if (err == fnfErr && (mode_x & MAC_O_CREAT)) {
            err = Create(pName, fs->volRefNum, 'mupy', type == &mp_type_vfs_mac_textio ? 'text' : 'bin ');
            check_mac_err(err);
            mode_x &= ~MAC_O_CREAT;
            continue;
        }
        check_mac_err(err);
        break;
    }

    if (mode_x & MAC_O_TRUNC) {
        err = SetEOF(fd, 0);
    } else if (mode_x & MAC_O_APPEND) {
        err = SetFPos(fd, fsFromLEOF, 0);
    }
    if (err != noErr) {
        FSClose(fd);
        raise_mac_err(err);
    }
    o->fd = fd;
    o->mode = mode_rw;
    o->volRefNum = fs->volRefNum;
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t vfs_mac_file_fileno(mp_obj_t self_in) {
    mp_obj_vfs_mac_file_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(self->fd);
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_mac_file_fileno_obj, vfs_mac_file_fileno);

static mp_uint_t vfs_mac_file_read(mp_obj_t o_in, void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_mac_file_t *o = MP_OBJ_TO_PTR(o_in);

    check_fd_is_open(o);
    long count = (long)size;

    if (o->fd == STDIN_FILENO) {
        mp_uint_t result = size;
        char *data = buf;
        while (size--) {
            *data++ = mp_hal_stdin_rx_chr();
        }
        return result;
    }

    if (o->fd <= STDERR_FILENO) {
        return 0;
    }

    // As far as I could tell, reading past EOF is an error. It leaves the file advanced
    // to the EOF position so "try again if EOF is signaled, only using the right length"
    // doesn't work. Instead, check the current position & EOF position, limiting the size
    // argument every time.
    LONGINT pos_cur, pos_eof;
    OSErr err = GetFPos(o->fd, &pos_cur);
    if (err != noErr) {
        goto out_err;
    }
    err = GetEOF(o->fd, &pos_eof);
    if (err != noErr) {
        goto out_err;
    }
    LONGINT max_count = pos_eof - pos_cur;
    if (max_count < count) {
        count = max_count;
    }

    err = FSRead(o->fd, &count, buf);

    if (err == noErr) {
        return count;
    }
out_err:
    *errcode = convert_mac_err(err);
    return MP_STREAM_ERROR;
}

static mp_uint_t vfs_mac_file_write(mp_obj_t o_in, const void *buf, mp_uint_t size, int *errcode) {
    mp_obj_vfs_mac_file_t *o = MP_OBJ_TO_PTR(o_in);

    if (o->fd <= STDERR_FILENO) {
        mp_hal_stdout_tx_strn(buf, size);
        return size;
    }

    check_fd_is_open(o);
    long count = (long)size;
    OSErr err = FSWrite(o->fd, &count, buf);

    if (err == noErr) {
        return count;
    }
    *errcode = convert_mac_err(err);
    return MP_STREAM_ERROR;
}
static mp_uint_t vfs_mac_file_ioctl(mp_obj_t o_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    mp_obj_vfs_mac_file_t *o = MP_OBJ_TO_PTR(o_in);

    if (request != MP_STREAM_CLOSE) {
        check_fd_is_open(o);
    }

    switch (request) {
        case MP_STREAM_POLL: {
            if (o->fd != STDIN_FILENO) {
                *errcode = EINVAL;
                return MP_STREAM_ERROR;
            }
            mp_uint_t ret = 0;
            if (mp_hal_stdin_available()) {
                ret |= arg & MP_STREAM_POLL_RD;
            }
            return ret;
        }
        case MP_STREAM_FLUSH: {
            if (o->fd <= STDERR_FILENO) {
                return 0;
            }
            INTEGER err = FlushVol(NULL, o->volRefNum);
            if (err == noErr) {
                *errcode = convert_mac_err(err);
                break;
            }
            return 0;
        }
        case MP_STREAM_SEEK: {
            struct mp_stream_seek_t *s = (struct mp_stream_seek_t *)arg;
            INTEGER posMode =
                s->whence == MP_SEEK_CUR ? fsFromMark :
                s->whence == MP_SEEK_SET ? fsFromStart : fsFromMark;
            OSErr err = SetFPos(o->fd, posMode, (long)s->offset);
            // TODO: seek that enlarges a file [need to call Allocate()?]
            if (err == noErr) {
                *errcode = convert_mac_err(err);
                break;
            }
            return 0;
        }
        case MP_STREAM_CLOSE:
            if (o->fd >= 0) {
                int fd = o->fd;
                o->fd = -1;
                MP_THREAD_GIL_EXIT();
                OSErr err = FSClose(fd);
                OSErr err1 = FlushVol(NULL, o->volRefNum);
                if (err != noErr) {
                    *errcode = convert_mac_err(err);
                    break;
                }
                if (err1 != noErr) {
                    *errcode = convert_mac_err(err1);
                    break;
                }
                MP_THREAD_GIL_ENTER();
            }
            return 0;
        case MP_STREAM_GET_FILENO:
            return o->fd;

        default:
            *errcode = EINVAL;
    }
    return MP_STREAM_ERROR;
}

static const mp_rom_map_elem_t vfs_mac_rawfile_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_fileno), MP_ROM_PTR(&vfs_mac_file_fileno_obj) },
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

static MP_DEFINE_CONST_DICT(vfs_mac_rawfile_locals_dict, vfs_mac_rawfile_locals_dict_table);

static const mp_stream_p_t vfs_mac_fileio_stream_p = {
    .read = vfs_mac_file_read,
    .write = vfs_mac_file_write,
    .ioctl = vfs_mac_file_ioctl,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_mac_fileio,
    MP_QSTR_FileIO,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, vfs_mac_file_print,
    protocol, &vfs_mac_fileio_stream_p,
    locals_dict, &vfs_mac_rawfile_locals_dict
    );

static const mp_stream_p_t vfs_mac_textio_stream_p = {
    .read = vfs_mac_file_read,
    .write = vfs_mac_file_write,
    .ioctl = vfs_mac_file_ioctl,
    .is_text = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_mac_textio,
    MP_QSTR_TextIOWrapper,
    MP_TYPE_FLAG_ITER_IS_STREAM,
    print, &vfs_mac_file_print,
    protocol, &vfs_mac_textio_stream_p,
    locals_dict, &vfs_mac_rawfile_locals_dict
    );

static mp_obj_t vfs_mac_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    INTEGER volRefNum;

    if (n_args == 0) {
        GetVol(NULL, &volRefNum);
    } else if (mp_obj_is_small_int(args[0])) {
        volRefNum = MP_OBJ_SMALL_INT_VALUE(args[0]);
    } else {
        volRefNum = getVolumeByName(args[0])->vcbVRefNum;
    }

    // create new object
    mp_obj_vfs_mac_t *vfs = mp_obj_malloc(mp_obj_vfs_mac_t, type);
    vfs->volRefNum = volRefNum;

    return MP_OBJ_FROM_PTR(vfs);
}

static mp_obj_t volumes(mp_obj_t self_in) {
    mp_obj_t result = mp_obj_list_make_new(&mp_type_list, 0, 0, NULL);
    VCB *vol = (VCB *)LMGetVCBQHdr().qHead;
    mp_obj_t args[3];
    mp_load_method(result, MP_QSTR_append, args);
    while (vol) {
        args[2] = new_str_from_pstr(vol->vcbVN);
        mp_call_method_n_kw(1, 0, args);
        vol = (VCB *)vol->qLink;
    }
    return result;
}
MP_DEFINE_CONST_FUN_OBJ_1(volumes_obj, volumes);
MP_DEFINE_CONST_STATICMETHOD_OBJ(volumes_fun_obj, &volumes_obj);

static mp_obj_t vfs_mac_mount(mp_obj_t self_in, mp_obj_t readonly, mp_obj_t mkfs) {
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    if (mp_obj_is_true(readonly)) {
        self->readonly = true;
    }
    if (mp_obj_is_true(mkfs)) {
        mp_raise_OSError(MP_EPERM);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_mac_mount_obj, vfs_mac_mount);

static mp_obj_t vfs_mac_umount(mp_obj_t self_in) {
    (void)self_in;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_mac_umount_obj, vfs_mac_umount);

static mp_obj_t vfs_mac_open(mp_obj_t self_in, mp_obj_t path_in, mp_obj_t mode_in) {
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    const char *mode = mp_obj_str_get_str(mode_in);
    if (self->readonly
        && (strchr(mode, 'w') != NULL || strchr(mode, 'a') != NULL || strchr(mode, '+') != NULL)) {
        mp_raise_OSError(MP_EROFS);
    }

    return mp_vfs_mac_file_open(self, &mp_type_vfs_mac_textio, path_in, mode_in);
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_mac_open_obj, vfs_mac_open);

static mp_import_stat_t mp_vfs_mac_import_stat(void *self_in, const char *path) {
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    Str255 pName;
    vfs_mac_get_path_cstr(pName, sizeof(pName), path);
    CInfoPBRec pb = {
        .hFileInfo.ioVRefNum = self->volRefNum,
        .hFileInfo.ioNamePtr = (StringPtr)pName,
    };
    OSErr err;
    err = PBGetCatInfoSync(&pb);
    MP_THREAD_GIL_ENTER();


    if (err != noErr) {
        return MP_IMPORT_STAT_NO_EXIST;
    }
    bool is_dir = pb.hFileInfo.ioFlAttrib & (1 << 4);
    if (is_dir) {
        return MP_IMPORT_STAT_DIR;
    } else {
        return MP_IMPORT_STAT_FILE;
    }
}

static mp_obj_t vfs_mac_chdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_raise_NotImplementedError(NULL); // TODO
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_chdir_obj, vfs_mac_chdir);

static mp_obj_t vfs_mac_getcwd(mp_obj_t self_in) {
    mp_raise_NotImplementedError(NULL); // TODO
}
static MP_DEFINE_CONST_FUN_OBJ_1(vfs_mac_getcwd_obj, vfs_mac_getcwd);

typedef struct _vfs_mac_ilistdir_it_t {
    mp_obj_base_t base;
    mp_fun_1_t iternext;
    bool is_str;
    INTEGER volRefNum;
    INTEGER index;
    INTEGER dirId;
} vfs_mac_ilistdir_it_t;

INTEGER get_dirid(INTEGER volRefNum, Byte *dirname) {
    MP_THREAD_GIL_EXIT();
    CInfoPBRec pb = {
        .hFileInfo.ioVRefNum = volRefNum,
        .hFileInfo.ioNamePtr = (StringPtr)dirname,
    };
    OSErr err;
    err = PBGetCatInfoSync(&pb);
    MP_THREAD_GIL_ENTER();

    check_mac_err(err);
    bool is_dir = pb.hFileInfo.ioFlAttrib & (1 << 4);

    if (is_dir) {
        return pb.hFileInfo.ioDirID;
    }
    mp_raise_OSError(MP_ENOTDIR);
}

static mp_obj_t vfs_mac_ilistdir_it_iternext(mp_obj_t self_in) {
    vfs_mac_ilistdir_it_t *self = MP_OBJ_TO_PTR(self_in);

    while (self->index != 0) {
        int index = self->index++;
        MP_THREAD_GIL_EXIT();
        Str255 pName;
        CInfoPBRec pb = {
            .hFileInfo.ioVRefNum = self->volRefNum,
            .hFileInfo.ioNamePtr = (StringPtr)pName,
            .hFileInfo.ioFVersNum = 0,
            .hFileInfo.ioFDirIndex = index,
            .hFileInfo.ioDirID = self->dirId,
        };
        OSErr err;
        err = PBGetCatInfoSync(&pb);
        MP_THREAD_GIL_ENTER();

        DPRINTF("PBGetCatInfoSync(%d) -> %d", index, err);

        if (err == fnfErr) {
            self->index = 0;
            break;
        }
        if (err != noErr) {
            continue;
        }

        bool is_dir = pb.hFileInfo.ioFlAttrib & (1 << 4);

        // make 3-tuple with info about this entry
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));

        if (self->is_str) {
            t->items[0] = new_str_from_pstr(pName);
        } else {
            t->items[0] = new_bytes_from_pstr(pName);
        }

        if (is_dir) {
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFDIR);
        } else {
            t->items[1] = MP_OBJ_NEW_SMALL_INT(MP_S_IFREG);
        }

        t->items[2] = MP_OBJ_NEW_SMALL_INT(index);

        return MP_OBJ_FROM_PTR(t);
    }
    return MP_OBJ_STOP_ITERATION;
}

static mp_obj_t vfs_mac_ilistdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    vfs_mac_ilistdir_it_t *iter = mp_obj_malloc(vfs_mac_ilistdir_it_t, &mp_type_polymorph_iter);
    iter->iternext = vfs_mac_ilistdir_it_iternext;
    iter->is_str = mp_obj_get_type(path_in) == &mp_type_str;
    iter->volRefNum = self->volRefNum;
    iter->index = 1;

    Str255 pName;
    vfs_mac_get_path_str(pName, sizeof(pName), path_in);
    iter->dirId = get_dirid(self->volRefNum, pName);

    return MP_OBJ_FROM_PTR(iter);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_ilistdir_obj, vfs_mac_ilistdir);

static mp_obj_t vfs_mac_mkdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_raise_NotImplementedError(NULL); // TODO
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_mkdir_obj, vfs_mac_mkdir);

static mp_obj_t vfs_mac_remove(mp_obj_t self_in, mp_obj_t path_in) {
    mp_raise_NotImplementedError(NULL); // TODO
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_remove_obj, vfs_mac_remove);

static mp_obj_t vfs_mac_rename(mp_obj_t self_in, mp_obj_t old_path_in, mp_obj_t new_path_in) {
    mp_raise_NotImplementedError(NULL); // TODO
}
static MP_DEFINE_CONST_FUN_OBJ_3(vfs_mac_rename_obj, vfs_mac_rename);


static mp_obj_t vfs_mac_rmdir(mp_obj_t self_in, mp_obj_t path_in) {
    mp_raise_NotImplementedError(NULL); // TODO
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_rmdir_obj, vfs_mac_rmdir);

static mp_obj_t vfs_mac_stat(mp_obj_t self_in, mp_obj_t path_in) {
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    Str255 pName;
    vfs_mac_get_path_str(pName, sizeof(pName), path_in);

    MP_THREAD_GIL_EXIT();
    CInfoPBRec pb = {
        .hFileInfo.ioVRefNum = self->volRefNum,
        .hFileInfo.ioNamePtr = (StringPtr)pName,
    };
    OSErr err;
    err = PBGetCatInfoSync(&pb);
    MP_THREAD_GIL_ENTER();
    check_mac_err(err);

    mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(10, NULL));
    bool is_dir = pb.hFileInfo.ioFlAttrib & (1 << 4);
    t->items[0] = MP_OBJ_NEW_SMALL_INT(is_dir ? MP_S_IFDIR | 0777 : MP_S_IFREG | 0666);
    t->items[1] = mp_obj_new_int_from_uint(pb.hFileInfo.ioFDirIndex);
    t->items[2] = mp_obj_new_int_from_uint(self->volRefNum);
    t->items[3] = mp_obj_new_int_from_uint(is_dir ? 2 : 1);
    t->items[4] = mp_obj_new_int_from_uint(0); // uid
    t->items[5] = mp_obj_new_int_from_uint(0); // gid
    t->items[6] = mp_obj_new_int_from_uint(is_dir ? 512 : pb.hFileInfo.ioFlPyLen); // data fork only
    t->items[7] = mp_obj_new_int_from_uint(from_mac_timestamp(pb.hFileInfo.ioFlCrDat)); // atime
    t->items[8] = mp_obj_new_int_from_uint(from_mac_timestamp(pb.hFileInfo.ioFlMdDat)); // mtime
    t->items[9] = mp_obj_new_int_from_uint(from_mac_timestamp(pb.hFileInfo.ioFlCrDat)); // ctime
    return MP_OBJ_FROM_PTR(t);
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_stat_obj, vfs_mac_stat);

static mp_obj_t vfs_mac_statvfs(mp_obj_t self_in, mp_obj_t path_in) {
    mp_raise_NotImplementedError(NULL); // TODO
    #if 0
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    const char *path = vfs_mac_get_path_cstr(self, path_in);
    STRUCT_STATVFS sb;
    int ret;
    MP_HAL_RETRY_SYSCALL(ret, STATVFS(path, &sb), mp_raise_OSError(err));
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
    #endif
}
static MP_DEFINE_CONST_FUN_OBJ_2(vfs_mac_statvfs_obj, vfs_mac_statvfs);


static const mp_rom_map_elem_t vfs_mac_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_volumes), MP_ROM_PTR(&volumes_fun_obj) },
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&vfs_mac_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&vfs_mac_umount_obj) },
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&vfs_mac_open_obj) },

    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&vfs_mac_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&vfs_mac_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&vfs_mac_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&vfs_mac_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&vfs_mac_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&vfs_mac_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&vfs_mac_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&vfs_mac_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&vfs_mac_statvfs_obj) },
};
static MP_DEFINE_CONST_DICT(vfs_mac_locals_dict, vfs_mac_locals_dict_table);

static const mp_vfs_proto_t vfs_mac_proto = {
    .import_stat = mp_vfs_mac_import_stat,
};

static void vfs_mac_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_vfs_mac_t *self = MP_OBJ_TO_PTR(self_in);
    VCB *vol = getVolumeByVolumeReference(self->volRefNum);
    mp_printf(print, "<VfsMac %d %.*s>",
        self->volRefNum, (int)PSTR_LEN(vol->vcbVN), PSTR_DATA(vol->vcbVN));
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_vfs_mac,
    MP_QSTR_VfsMac,
    MP_TYPE_FLAG_NONE,
    make_new, vfs_mac_make_new,
    protocol, &vfs_mac_proto,
    locals_dict, &vfs_mac_locals_dict,
    print, &vfs_mac_print
    );

mp_obj_vfs_mac_file_t mp_sys_stdin_obj = {{&mp_type_vfs_mac_textio}, STDIN_FILENO};
mp_obj_vfs_mac_file_t mp_sys_stdout_obj = {{&mp_type_vfs_mac_textio}, STDOUT_FILENO};
mp_obj_vfs_mac_file_t mp_sys_stderr_obj = {{&mp_type_vfs_mac_textio}, STDERR_FILENO};
