/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#if MICROPY_FSUSERMOUNT
#include <string.h>
#include <errno.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "lib/fatfs/ff.h"
#include "extmod/fsusermount.h"

fs_user_mount_t *fatfs_mount_mkfs(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args, bool mkfs) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_readonly, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_mkfs, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };

    // parse args
    mp_obj_t device = pos_args[0];
    mp_obj_t mount_point = pos_args[1];
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 2, pos_args + 2, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get the mount point
    mp_uint_t mnt_len;
    const char *mnt_str = mp_obj_str_get_data(mount_point, &mnt_len);

    if (device == mp_const_none) {
        // umount
        FRESULT res = FR_NO_FILESYSTEM;
        for (size_t i = 0; i < MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount)); ++i) {
            fs_user_mount_t *vfs = MP_STATE_PORT(fs_user_mount)[i];
            if (vfs != NULL && !memcmp(mnt_str, vfs->str, mnt_len + 1)) {
                res = f_mount(NULL, vfs->str, 0);
                if (vfs->flags & FSUSER_FREE_OBJ) {
                    m_del_obj(fs_user_mount_t, vfs);
                }
                MP_STATE_PORT(fs_user_mount)[i] = NULL;
                break;
            }
        }
        if (res != FR_OK) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't umount"));
        }
        return NULL;
    } else {
        // mount
        size_t i = 0;
        for (; i < MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount)); ++i) {
            if (MP_STATE_PORT(fs_user_mount)[i] == NULL) {
                break;
            }
        }
        if (i == MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount))) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "too many devices mounted"));
        }

        // create new object
        fs_user_mount_t *vfs = m_new_obj(fs_user_mount_t);
        vfs->str = mnt_str;
        vfs->len = mnt_len;
        vfs->flags = FSUSER_FREE_OBJ;

        // load block protocol methods
        mp_load_method(device, MP_QSTR_readblocks, vfs->readblocks);
        mp_load_method_maybe(device, MP_QSTR_writeblocks, vfs->writeblocks);
        mp_load_method_maybe(device, MP_QSTR_ioctl, vfs->u.ioctl);
        if (vfs->u.ioctl[0] != MP_OBJ_NULL) {
            // device supports new block protocol, so indicate it
            vfs->flags |= FSUSER_HAVE_IOCTL;
        } else {
            // no ioctl method, so assume the device uses the old block protocol
            mp_load_method_maybe(device, MP_QSTR_sync, vfs->u.old.sync);
            mp_load_method(device, MP_QSTR_count, vfs->u.old.count);
        }

        // Read-only device indicated by writeblocks[0] == MP_OBJ_NULL.
        // User can specify read-only device by:
        //  1. readonly=True keyword argument
        //  2. nonexistent writeblocks method (then writeblocks[0] == MP_OBJ_NULL already)
        if (args[0].u_bool) {
            vfs->writeblocks[0] = MP_OBJ_NULL;
        }

        // Register the vfs object so that it can be found by the FatFS driver using
        // ff_get_ldnumber.  We don't register it any earlier than this point in case there
        // is an exception, in which case there would remain a partially mounted device.
        MP_STATE_PORT(fs_user_mount)[i] = vfs;

        // mount the block device (if mkfs, only pre-mount)
        FRESULT res = f_mount(&vfs->fatfs, vfs->str, !mkfs);
        // check the result
        if (res == FR_OK) {
            if (mkfs) {
                goto mkfs;
            }
        } else if (res == FR_NO_FILESYSTEM && args[1].u_bool) {
mkfs:
            res = f_mkfs(vfs->str, 1, 0);
            if (res != FR_OK) {
mkfs_error:
                MP_STATE_PORT(fs_user_mount)[i] = NULL;
                nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't mkfs"));
            }
            if (mkfs) {
                // If requested to only mkfs, unmount pre-mounted device
                res = f_mount(NULL, vfs->str, 0);
                if (res != FR_OK) {
                    goto mkfs_error;
                }
                MP_STATE_PORT(fs_user_mount)[i] = NULL;
                return NULL;
            }
        } else {
            MP_STATE_PORT(fs_user_mount)[i] = NULL;
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't mount"));
        }

        /*
        if (vfs->writeblocks[0] == MP_OBJ_NULL) {
            printf("mounted read-only");
        } else {
            printf("mounted read-write");
        }
        DWORD nclst;
        FATFS *fatfs;
        f_getfree(vfs->str, &nclst, &fatfs);
        printf(" on %s with %u bytes free\n", vfs->str, (uint)(nclst * fatfs->csize * 512));
        */
        return vfs;
    }
}

STATIC mp_obj_t fatfs_mount(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    fatfs_mount_mkfs(n_args, pos_args, kw_args, false);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(fsuser_mount_obj, 2, fatfs_mount);

mp_obj_t fatfs_umount(mp_obj_t bdev_or_path_in) {
    size_t i = 0;
    if (MP_OBJ_IS_STR(bdev_or_path_in)) {
        mp_uint_t mnt_len;
        const char *mnt_str = mp_obj_str_get_data(bdev_or_path_in, &mnt_len);
        for (; i < MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount)); ++i) {
            fs_user_mount_t *vfs = MP_STATE_PORT(fs_user_mount)[i];
            if (vfs != NULL && !memcmp(mnt_str, vfs->str, mnt_len + 1)) {
                break;
            }
        }
    } else {
        for (; i < MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount)); ++i) {
            fs_user_mount_t *vfs = MP_STATE_PORT(fs_user_mount)[i];
            if (vfs != NULL && bdev_or_path_in == vfs->readblocks[1]) {
                break;
            }
        }
    }

    if (i == MP_ARRAY_SIZE(MP_STATE_PORT(fs_user_mount))) {
        mp_raise_OSError(MP_EINVAL);
    }

    fs_user_mount_t *vfs = MP_STATE_PORT(fs_user_mount)[i];
    FRESULT res = f_mount(NULL, vfs->str, 0);
    if (vfs->flags & FSUSER_FREE_OBJ) {
        m_del_obj(fs_user_mount_t, vfs);
    }
    MP_STATE_PORT(fs_user_mount)[i] = NULL;
    if (res != FR_OK) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't umount"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(fsuser_umount_obj, fatfs_umount);

STATIC mp_obj_t fatfs_mkfs(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    fatfs_mount_mkfs(n_args, pos_args, kw_args, true);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(fsuser_mkfs_obj, 2, fatfs_mkfs);

#endif // MICROPY_FSUSERMOUNT
