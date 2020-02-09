/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Josef Gajdusek
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "extmod/vfs.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/os/__init__.h"
#include "shared-bindings/storage/__init__.h"
#include "supervisor/filesystem.h"
#include "supervisor/flash.h"
#include "supervisor/usb.h"

STATIC mp_obj_t mp_vfs_proxy_call(mp_vfs_mount_t *vfs, qstr meth_name, size_t n_args, const mp_obj_t *args) {
    if (vfs == MP_VFS_NONE) {
        // mount point not found
        mp_raise_OSError(MP_ENODEV);
    }
    if (vfs == MP_VFS_ROOT) {
        // can't do operation on root dir
        mp_raise_OSError(MP_EPERM);
    }
    mp_obj_t meth[n_args + 2];
    mp_load_method(vfs->obj, meth_name, meth);
    if (args != NULL) {
        memcpy(meth + 2, args, n_args * sizeof(*args));
    }
    return mp_call_method_n_kw(n_args, 0, meth);
}

void common_hal_storage_mount(mp_obj_t vfs_obj, const char* mount_path, bool readonly) {
    // create new object
    mp_vfs_mount_t *vfs = m_new_obj(mp_vfs_mount_t);
    vfs->str = mount_path;
    vfs->len = strlen(mount_path);
    vfs->obj = vfs_obj;
    vfs->next = NULL;

    mp_obj_t args[2];
    args[0] = readonly ? mp_const_true : mp_const_false;
    args[1] = mp_const_false; // Don't make the file system automatically when mounting.

    // Check that there's no file or directory with the same name as the mount point.
    // But it's ok to mount '/' in any case.
    if (strcmp(vfs->str, "/") != 0) {
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            common_hal_os_stat(mount_path);
            nlr_pop();
            // Something with the same name exists.
            mp_raise_OSError(MP_EEXIST);
        }
    }

    // check that the destination mount point is unused
    const char *path_out;
    mp_vfs_mount_t *existing_mount = mp_vfs_lookup_path(mount_path, &path_out);
    if (existing_mount != MP_VFS_NONE && existing_mount != MP_VFS_ROOT) {
        if (vfs->len != 1 && existing_mount->len == 1) {
            // if root dir is mounted, still allow to mount something within a subdir of root
        } else {
            // mount point in use
            mp_raise_OSError(MP_EPERM);
        }
    }

    // call the underlying object to do any mounting operation
    mp_vfs_proxy_call(vfs, MP_QSTR_mount, 2, (mp_obj_t*)&args);

    // Insert the vfs into the mount table by pushing it onto the front of the
    // mount table.
    mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table);
    vfs->next = *vfsp;
    *vfsp = vfs;
}

void common_hal_storage_umount_object(mp_obj_t vfs_obj) {
    // remove vfs from the mount table
    mp_vfs_mount_t *vfs = NULL;
    for (mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table); *vfsp != NULL; vfsp = &(*vfsp)->next) {
        if ((*vfsp)->obj == vfs_obj) {
            vfs = *vfsp;
            *vfsp = (*vfsp)->next;
            break;
        }
    }

    if (vfs == NULL) {
        mp_raise_OSError(MP_EINVAL);
    }

    // if we unmounted the current device then set current to root
    if (MP_STATE_VM(vfs_cur) == vfs) {
        MP_STATE_VM(vfs_cur) = MP_VFS_ROOT;
    }

    // call the underlying object to do any unmounting operation
    mp_vfs_proxy_call(vfs, MP_QSTR_umount, 0, NULL);
}

STATIC mp_obj_t storage_object_from_path(const char* mount_path) {
    for (mp_vfs_mount_t **vfsp = &MP_STATE_VM(vfs_mount_table); *vfsp != NULL; vfsp = &(*vfsp)->next) {
        if (strcmp(mount_path, (*vfsp)->str) == 0) {
            return (*vfsp)->obj;
        }
    }
    mp_raise_OSError(MP_EINVAL);
}

void common_hal_storage_umount_path(const char* mount_path) {
    common_hal_storage_umount_object(storage_object_from_path(mount_path));
}

mp_obj_t common_hal_storage_getmount(const char *mount_path) {
    return storage_object_from_path(mount_path);
}

void common_hal_storage_remount(const char *mount_path, bool readonly, bool disable_concurrent_write_protection) {
    if (strcmp(mount_path, "/") != 0) {
        mp_raise_OSError(MP_EINVAL);
    }

    #ifdef USB_AVAILABLE
    if (!usb_msc_ejected()) {
        mp_raise_RuntimeError(translate("Cannot remount '/' when USB is active."));
    }
    #endif

    filesystem_set_internal_writable_by_usb(readonly);
    filesystem_set_internal_concurrent_write_protection(!disable_concurrent_write_protection);
}

void common_hal_storage_erase_filesystem(void) {
    filesystem_init(false, true); // Force a re-format.
    common_hal_mcu_reset();
    // We won't actually get here, since we're resetting.
}
