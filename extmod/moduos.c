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

#include "py/objstr.h"
#include "py/runtime.h"

#if MICROPY_PY_UOS

#include "extmod/misc.h"
#include "extmod/vfs.h"

#if MICROPY_VFS_FAT
#include "extmod/vfs_fat.h"
#endif

#if MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2
#include "extmod/vfs_lfs.h"
#endif

#if MICROPY_VFS_POSIX
#include "extmod/vfs_posix.h"
#endif

#if MICROPY_PY_UOS_UNAME
#include "genhdr/mpversion.h"
#endif

#ifdef MICROPY_PY_UOS_INCLUDEFILE
#include MICROPY_PY_UOS_INCLUDEFILE
#endif

#ifdef MICROPY_BUILD_TYPE
#define MICROPY_BUILD_TYPE_PAREN " (" MICROPY_BUILD_TYPE ")"
#else
#define MICROPY_BUILD_TYPE_PAREN
#endif

#if MICROPY_PY_UOS_UNAME

#if MICROPY_PY_UOS_UNAME_RELEASE_DYNAMIC
#define CONST_RELEASE
#else
#define CONST_RELEASE const
#endif

STATIC const qstr mp_uos_uname_info_fields[] = {
    MP_QSTR_sysname,
    MP_QSTR_nodename,
    MP_QSTR_release,
    MP_QSTR_version,
    MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(mp_uos_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(mp_uos_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
STATIC CONST_RELEASE MP_DEFINE_STR_OBJ(mp_uos_uname_info_release_obj, MICROPY_VERSION_STRING);
STATIC const MP_DEFINE_STR_OBJ(mp_uos_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE MICROPY_BUILD_TYPE_PAREN);
STATIC const MP_DEFINE_STR_OBJ(mp_uos_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC MP_DEFINE_ATTRTUPLE(
    mp_uos_uname_info_obj,
    mp_uos_uname_info_fields,
    5,
    MP_ROM_PTR(&mp_uos_uname_info_sysname_obj),
    MP_ROM_PTR(&mp_uos_uname_info_nodename_obj),
    MP_ROM_PTR(&mp_uos_uname_info_release_obj),
    MP_ROM_PTR(&mp_uos_uname_info_version_obj),
    MP_ROM_PTR(&mp_uos_uname_info_machine_obj)
    );

STATIC mp_obj_t mp_uos_uname(void) {
    #if MICROPY_PY_UOS_UNAME_RELEASE_DYNAMIC
    const char *release = mp_uos_uname_release();
    mp_uos_uname_info_release_obj.len = strlen(release);
    mp_uos_uname_info_release_obj.data = (const byte *)release;
    #endif
    return MP_OBJ_FROM_PTR(&mp_uos_uname_info_obj);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_uos_uname_obj, mp_uos_uname);

#endif

STATIC const mp_rom_map_elem_t os_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_uos) },

    #if MICROPY_PY_UOS_GETENV_PUTENV_UNSETENV
    { MP_ROM_QSTR(MP_QSTR_getenv), MP_ROM_PTR(&mp_uos_getenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_putenv), MP_ROM_PTR(&mp_uos_putenv_obj) },
    { MP_ROM_QSTR(MP_QSTR_unsetenv), MP_ROM_PTR(&mp_uos_unsetenv_obj) },
    #endif
    #if MICROPY_PY_UOS_SEP
    { MP_ROM_QSTR(MP_QSTR_sep), MP_ROM_QSTR(MP_QSTR__slash_) },
    #endif
    #if MICROPY_PY_UOS_SYNC
    { MP_ROM_QSTR(MP_QSTR_sync), MP_ROM_PTR(&mp_uos_sync_obj) },
    #endif
    #if MICROPY_PY_UOS_SYSTEM
    { MP_ROM_QSTR(MP_QSTR_system), MP_ROM_PTR(&mp_uos_system_obj) },
    #endif
    #if MICROPY_PY_UOS_UNAME
    { MP_ROM_QSTR(MP_QSTR_uname), MP_ROM_PTR(&mp_uos_uname_obj) },
    #endif
    #if MICROPY_PY_UOS_URANDOM
    { MP_ROM_QSTR(MP_QSTR_urandom), MP_ROM_PTR(&mp_uos_urandom_obj) },
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
    { MP_ROM_QSTR(MP_QSTR_dupterm), MP_ROM_PTR(&mp_uos_dupterm_obj) },
    #endif
    #if MICROPY_PY_UOS_DUPTERM_NOTIFY
    { MP_ROM_QSTR(MP_QSTR_dupterm_notify), MP_ROM_PTR(&mp_uos_dupterm_notify_obj) },
    #endif
    #if MICROPY_PY_UOS_ERRNO
    { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_uos_errno_obj) },
    #endif

    #if MICROPY_VFS
    { MP_ROM_QSTR(MP_QSTR_ilistdir), MP_ROM_PTR(&mp_vfs_ilistdir_obj) },
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
};
STATIC MP_DEFINE_CONST_DICT(os_module_globals, os_module_globals_table);

const mp_obj_module_t mp_module_uos = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&os_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_uos, mp_module_uos);

#endif // MICROPY_PY_UOS
