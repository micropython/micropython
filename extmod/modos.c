/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016-2022 Damien P. George
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

#include "py/mphal.h"
#include "py/objstr.h"
#include "py/runtime.h"

#if MICROPY_PY_OS

#include "extmod/misc.h"
#include "extmod/vfs.h"

#if MICROPY_VFS_FAT
#include "extmod/vfs_fat.h"
#if MICROPY_PY_OS_SYNC
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#endif
#endif

#if MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2
#include "extmod/vfs_lfs.h"
#endif

#if MICROPY_VFS_POSIX
#include "extmod/vfs_posix.h"
#endif

#if MICROPY_MBFS
#if MICROPY_VFS
#error "MICROPY_MBFS requires MICROPY_VFS to be disabled"
#endif
#include "ports/nrf/modules/os/microbitfs.h"
#endif

#if MICROPY_PY_OS_UNAME
#include "genhdr/mpversion.h"
#endif

#ifdef MICROPY_PY_OS_INCLUDEFILE
#include MICROPY_PY_OS_INCLUDEFILE
#endif

#ifdef MICROPY_BUILD_TYPE
#define MICROPY_BUILD_TYPE_PAREN " (" MICROPY_BUILD_TYPE ")"
#else
#define MICROPY_BUILD_TYPE_PAREN
#endif

#if MICROPY_PY_OS_SYNC
// sync()
// Sync all filesystems.
static mp_obj_t mp_os_sync(void) {
    #if MICROPY_VFS_FAT
    for (mp_vfs_mount_t *vfs = MP_STATE_VM(vfs_mount_table); vfs != NULL; vfs = vfs->next) {
        if (mp_obj_is_type(vfs->obj, &mp_fat_vfs_type)) {
            disk_ioctl(MP_OBJ_TO_PTR(vfs->obj), CTRL_SYNC, NULL);
        }
    }
    #endif
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mp_os_sync_obj, mp_os_sync);
#endif

#if MICROPY_PY_OS_UNAME

#if MICROPY_PY_OS_UNAME_RELEASE_DYNAMIC
#define CONST_RELEASE
#else
#define CONST_RELEASE const
#endif

static const qstr mp_os_uname_info_fields[] = {
    MP_QSTR_sysname,
    MP_QSTR_nodename,
    MP_QSTR_release,
    MP_QSTR_version,
    MP_QSTR_machine
};
static const MP_DEFINE_STR_OBJ(mp_os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
static const MP_DEFINE_STR_OBJ(mp_os_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
static CONST_RELEASE MP_DEFINE_STR_OBJ(mp_os_uname_info_release_obj, MICROPY_VERSION_STRING);
static const MP_DEFINE_STR_OBJ(mp_os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE MICROPY_BUILD_TYPE_PAREN);
static const MP_DEFINE_STR_OBJ(mp_os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

static MP_DEFINE_ATTRTUPLE(
    mp_os_uname_info_obj,
    mp_os_uname_info_fields,
    5,
    MP_ROM_PTR(&mp_os_uname_info_sysname_obj),
    MP_ROM_PTR(&mp_os_uname_info_nodename_obj),
    MP_ROM_PTR(&mp_os_uname_info_release_obj),
    MP_ROM_PTR(&mp_os_uname_info_version_obj),
    MP_ROM_PTR(&mp_os_uname_info_machine_obj)
    );

static mp_obj_t mp_os_uname(void) {
    #if MICROPY_PY_OS_UNAME_RELEASE_DYNAMIC
    const char *release = mp_os_uname_release();
    mp_os_uname_info_release_obj.len = strlen(release);
    mp_os_uname_info_release_obj.data = (const byte *)release;
    #endif
    return MP_OBJ_FROM_PTR(&mp_os_uname_info_obj);
}
static MP_DEFINE_CONST_FUN_OBJ_0(mp_os_uname_obj, mp_os_uname);

#endif

#if MICROPY_PY_OS_DUPTERM_NOTIFY
static mp_obj_t mp_os_dupterm_notify(mp_obj_t obj_in) {
    (void)obj_in;
    for (;;) {
        int c = mp_os_dupterm_rx_chr();
        if (c < 0) {
            break;
        }
        ringbuf_put(&stdin_ringbuf, c);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_os_dupterm_notify_obj, mp_os_dupterm_notify);
#endif

static const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_os) },

    #if MICROPY_PY_OS_GETENV_PUTENV_UNSETENV
    { MP_ROM_QSTR(MP_QSTR_getenv), MP_ROM_PTR(&mp_os_getenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_putenv), MP_ROM_PTR(&mp_os_putenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_unsetenv), MP_ROM_PTR(&mp_os_unsetenv_obj) },
    #endif
    #if MICROPY_PY_OS_SEP
    { MP_ROM_QSTR(MP_QSTR_sep), MP_ROM_QSTR(MP_QSTR__slash_) },
    #endif
    #if MICROPY_PY_OS_SYNC
    { MP_ROM_QSTR(MP_QSTR_sync), MP_ROM_PTR(&mp_os_sync_obj) },
    #endif
    #if MICROPY_PY_OS_SYSTEM
    { MP_ROM_QSTR(MP_QSTR_system), MP_ROM_PTR(&mp_os_system_obj) },
    #endif
    #if MICROPY_PY_OS_UNAME
    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&mp_os_uname_obj) },
    #endif
    #if MICROPY_PY_OS_URANDOM
    { MP_ROM_QSTR(MP_QSTR_urandom), MP_ROM_PTR(&mp_os_urandom_obj) },
    #endif

    #if MICROPY_VFS
    { MP_ROM_QSTR(MP_QSTR_chdir), MP_ROM_PTR(&mp_vfs_chdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_getcwd), MP_ROM_PTR(&mp_vfs_getcwd_obj) },
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&mp_vfs_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_mkdir), MP_ROM_PTR(&mp_vfs_mkdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&mp_vfs_remove_obj) },
    { MP_ROM_QSTR(MP_QSTR_rename), MP_ROM_PTR(&mp_vfs_rename_obj) },
    { MP_ROM_QSTR(MP_QSTR_rmdir), MP_ROM_PTR(&mp_vfs_rmdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&mp_vfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_statvfs), MP_ROM_PTR(&mp_vfs_statvfs_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlink), MP_ROM_PTR(&mp_vfs_remove_obj) }, // unlink aliases to remove
    #endif

    // The following are MicroPython extensions.

    #if MICROPY_PY_OS_DUPTERM
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_os_dupterm_obj) },
    #endif
    #if MICROPY_PY_OS_DUPTERM_NOTIFY
    { MP_ROM_QSTR(MP_QSTR_dupterm_notify), MP_ROM_PTR(&mp_os_dupterm_notify_obj) },
    #endif
    #if MICROPY_PY_OS_ERRNO
    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_os_errno_obj) },
    #endif

    #if MICROPY_VFS
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&mp_vfs_ilistdir_obj) },
    #endif

    // The following MicroPython extensions are deprecated.  Use the `vfs` module instead.
    #if !MICROPY_PREVIEW_VERSION_2 && MICROPY_VFS
    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&mp_vfs_umount_obj) },
    #if MICROPY_VFS_FAT
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
    #endif
    #if MICROPY_VFS_LFS1
    { MP_ROM_QSTR(MP_QSTR_VfsLfs1), MP_ROM_PTR(&mp_type_vfs_lfs1) },
    #endif
    #if MICROPY_VFS_LFS2
    { MP_ROM_QSTR(MP_QSTR_VfsLfs2), MP_ROM_PTR(&mp_type_vfs_lfs2) },
    #endif
    #if MICROPY_VFS_POSIX
    { MP_ROM_QSTR(MP_QSTR_VfsPosix), MP_ROM_PTR(&mp_type_vfs_posix) },
    #endif
    #endif

    #if MICROPY_MBFS
    // For special micro:bit filesystem only.
    { MP_ROM_QSTR(MP_QSTR_listdir), MP_ROM_PTR(&os_mbfs_listdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&os_mbfs_ilistdir_obj) },
    { MP_ROM_QSTR(MP_QSTR_stat), MP_ROM_PTR(&os_mbfs_stat_obj) },
    { MP_ROM_QSTR(MP_QSTR_remove), MP_ROM_PTR(&os_mbfs_remove_obj) },
    #endif
};
static MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t mp_module_os = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&os_module_globals,
};

MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_os, mp_module_os);

#endif // MICROPY_PY_OS
