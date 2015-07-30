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

#include "py/nlr.h"
#include "py/runtime.h"
#include "lib/fatfs/ff.h"
#include "fsusermount.h"

STATIC mp_obj_t pyb_mount(mp_uint_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
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
        if (MP_STATE_PORT(fs_user_mount) != NULL) {
            res = f_mount(NULL, MP_STATE_PORT(fs_user_mount)->str, 0);
            m_del_obj(fs_user_mount_t, MP_STATE_PORT(fs_user_mount));
            MP_STATE_PORT(fs_user_mount) = NULL;
        }
        if (res != FR_OK) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't umount"));
        }
    } else {
        // mount
        if (MP_STATE_PORT(fs_user_mount) != NULL) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "device already mounted"));
        }

        // create new object
        MP_STATE_PORT(fs_user_mount) = m_new_obj(fs_user_mount_t);
        MP_STATE_PORT(fs_user_mount)->str = mnt_str;
        MP_STATE_PORT(fs_user_mount)->len = mnt_len;

        // load block protocol methods
        mp_load_method(device, MP_QSTR_readblocks, MP_STATE_PORT(fs_user_mount)->readblocks);
        mp_load_method_maybe(device, MP_QSTR_writeblocks, MP_STATE_PORT(fs_user_mount)->writeblocks);
        mp_load_method_maybe(device, MP_QSTR_sync, MP_STATE_PORT(fs_user_mount)->sync);
        mp_load_method(device, MP_QSTR_count, MP_STATE_PORT(fs_user_mount)->count);

        // Read-only device indicated by writeblocks[0] == MP_OBJ_NULL.
        // User can specify read-only device by:
        //  1. readonly=True keyword argument
        //  2. nonexistent writeblocks method (then writeblocks[0] == MP_OBJ_NULL already)
        if (args[0].u_bool) {
            MP_STATE_PORT(fs_user_mount)->writeblocks[0] = MP_OBJ_NULL;
        }

        // mount the block device
        FRESULT res = f_mount(&MP_STATE_PORT(fs_user_mount)->fatfs, MP_STATE_PORT(fs_user_mount)->str, 1);

        // check the result
        if (res == FR_OK) {
        } else if (res == FR_NO_FILESYSTEM && args[1].u_bool) {
            res = f_mkfs(MP_STATE_PORT(fs_user_mount)->str, 1, 0);
            if (res != FR_OK) {
                nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't mkfs"));
            }
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't mount"));
        }

        /*
        if (MP_STATE_PORT(fs_user_mount)->writeblocks[0] == MP_OBJ_NULL) {
            printf("mounted read-only");
        } else {
            printf("mounted read-write");
        }
        DWORD nclst;
        FATFS *fatfs;
        f_getfree(MP_STATE_PORT(fs_user_mount)->str, &nclst, &fatfs);
        printf(" on %s with %u bytes free\n", MP_STATE_PORT(fs_user_mount)->str, (uint)(nclst * fatfs->csize * 512));
        */
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(pyb_mount_obj, 2, pyb_mount);
