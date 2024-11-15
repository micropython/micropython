/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Damien P. George
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

#include "py/runtime.h"

#if MICROPY_PY_VFS

#include "extmod/vfs.h"
#include "extmod/vfs_fat.h"
#include "extmod/vfs_lfs.h"
#include "extmod/vfs_posix.h"
#include "extmod/vfs_rom.h"

#if !MICROPY_VFS
#error "MICROPY_PY_VFS requires MICROPY_VFS"
#endif

#if MICROPY_VFS_ROM_IOCTL
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_vfs_rom_ioctl_obj, 1, 4, mp_vfs_rom_ioctl);
#endif

static const mp_rom_map_elem_t vfs_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_vfs) },

    { MP_ROM_QSTR(MP_QSTR_mount), MP_ROM_PTR(&mp_vfs_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umount), MP_ROM_PTR(&mp_vfs_umount_obj) },
    #if MICROPY_VFS_ROM_IOCTL
    { MP_ROM_QSTR(MP_QSTR_rom_ioctl), MP_ROM_PTR(&mp_vfs_rom_ioctl_obj) },
    #endif
    #if MICROPY_VFS_FAT
    { MP_ROM_QSTR(MP_QSTR_VfsFat), MP_ROM_PTR(&mp_fat_vfs_type) },
    #endif
    #if MICROPY_VFS_LFS1
    { MP_ROM_QSTR(MP_QSTR_VfsLfs1), MP_ROM_PTR(&mp_type_vfs_lfs1) },
    #endif
    #if MICROPY_VFS_LFS2
    { MP_ROM_QSTR(MP_QSTR_VfsLfs2), MP_ROM_PTR(&mp_type_vfs_lfs2) },
    #endif
    #if MICROPY_VFS_ROM
    { MP_ROM_QSTR(MP_QSTR_VfsRom), MP_ROM_PTR(&mp_type_vfs_rom) },
    #endif
    #if MICROPY_VFS_POSIX
    { MP_ROM_QSTR(MP_QSTR_VfsPosix), MP_ROM_PTR(&mp_type_vfs_posix) },
    #endif
};
static MP_DEFINE_CONST_DICT(vfs_module_globals, vfs_module_globals_table);

const mp_obj_module_t mp_module_vfs = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&vfs_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_vfs, mp_module_vfs);

#endif // MICROPY_PY_VFS
