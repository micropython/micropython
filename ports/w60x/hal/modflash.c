/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

//#include <stdint.h>
//#include <string.h>

#include "wm_include.h"
#include "wm_internal_flash.h"
#include "wm_flash_map.h"

#include "py/runtime.h"

#if MICROPY_VFS_FAT
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"
#endif
#if MICROPY_VFS_LFS2
#include "lib/littlefs/lfs2.h"
#include "extmod/vfs_lfs.h"
#include "mp_vfs_lfs.h"
#endif

#if MICROPY_USE_INTERVAL_FLS_FS

#if MICROPY_VFS_FAT
#define INTERVAL_FLS_BASE               (USER_ADDR_START - 0x8000)//80k (32+48)
#define INTERVAL_FLS_LEN                (0x8000 + USER_AREA_LEN)//gz image <=352kb
#define INTERVAL_FLS_FS_SECTOR_SIZE     (FF_MAX_SS)

STATIC DRESULT w600_flash_read (BYTE *buff, DWORD sector, UINT count) {
    int result = tls_fls_read(INTERVAL_FLS_BASE + (sector * INTERVAL_FLS_FS_SECTOR_SIZE),
                              buff, INTERVAL_FLS_FS_SECTOR_SIZE * count);
    DSTATUS stat = (TLS_FLS_STATUS_OK == result) ? RES_OK : RES_ERROR;

    return stat;
}

STATIC DRESULT w600_flash_write (const BYTE *buff, DWORD sector, UINT count) {
    int result = tls_fls_write(INTERVAL_FLS_BASE + (sector * INTERVAL_FLS_FS_SECTOR_SIZE),
                               (u8 *)buff, INTERVAL_FLS_FS_SECTOR_SIZE * count);
    DSTATUS stat = (TLS_FLS_STATUS_OK == result) ? RES_OK : RES_ERROR;

    return stat;
}

/******************************************************************************/
// MicroPython bindings to expose the internal flash as an object with the
// block protocol.

// there is a singleton Flash object
extern const mp_obj_type_t w600_flash_type;
STATIC const mp_obj_base_t w600_flash_obj = {&w600_flash_type};

STATIC mp_obj_t w600_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&w600_flash_obj;
}

STATIC mp_obj_t w600_flash_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    DRESULT res = w600_flash_read(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / INTERVAL_FLS_FS_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(w600_flash_readblocks_obj, w600_flash_readblocks);

STATIC mp_obj_t w600_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    DRESULT res = w600_flash_write(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / INTERVAL_FLS_FS_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(w600_flash_writeblocks_obj, w600_flash_writeblocks);

STATIC mp_obj_t w600_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
    case MP_BLOCKDEV_IOCTL_INIT:
        return MP_OBJ_NEW_SMALL_INT(RES_OK);
    case MP_BLOCKDEV_IOCTL_DEINIT:
        return MP_OBJ_NEW_SMALL_INT(RES_OK);
    case MP_BLOCKDEV_IOCTL_SYNC:
        return MP_OBJ_NEW_SMALL_INT(RES_OK);
    case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
        return MP_OBJ_NEW_SMALL_INT(INTERVAL_FLS_LEN / INTERVAL_FLS_FS_SECTOR_SIZE);
    case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
        return MP_OBJ_NEW_SMALL_INT(INTERVAL_FLS_FS_SECTOR_SIZE);
    default:
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(w600_flash_ioctl_obj, w600_flash_ioctl);

STATIC const mp_rom_map_elem_t w600_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&w600_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&w600_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&w600_flash_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(w600_flash_locals_dict, w600_flash_locals_dict_table);

const mp_obj_type_t w600_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = w600_flash_make_new,
    .locals_dict = (mp_obj_t) &w600_flash_locals_dict,
};
#endif

#if MICROPY_VFS_LFS2
#define INTERVAL_FLS_BASE                   USER_ADDR_START
#define INTERVAL_FLS_LEN                    USER_AREA_LEN
#define INTERVAL_FLS_FS_SECTOR_SIZE         (4096)

STATIC int w600_flash_lfs_read(const struct lfs2_config *c, lfs2_block_t block,
                               lfs2_off_t off, void *buffer, lfs2_size_t size) {
    int result = tls_fls_read(INTERVAL_FLS_BASE + (block * INTERVAL_FLS_FS_SECTOR_SIZE) + off,
                              buffer, size);
    enum lfs2_error err = (TLS_FLS_STATUS_OK == result) ? LFS2_ERR_OK : LFS2_ERR_IO;
    return err;
}

STATIC int w600_flash_lfs_prog(const struct lfs2_config *c, lfs2_block_t block,
                               lfs2_off_t off, const void *buffer, lfs2_size_t size) {
    //u32 cpu_sr = tls_os_set_critical();
    int result = tls_fls_write(INTERVAL_FLS_BASE + (block * INTERVAL_FLS_FS_SECTOR_SIZE) + off,
                              (u8 *)buffer, size);
    enum lfs2_error err = (TLS_FLS_STATUS_OK == result) ? LFS2_ERR_OK : LFS2_ERR_IO;
    return err;
}

STATIC int w600_flash_lfs_erase(const struct lfs2_config *c, lfs2_block_t block) {
    int result = tls_fls_erase(INTERVAL_FLS_BASE / INTERVAL_FLS_FS_SECTOR_SIZE + block);
    enum lfs2_error err = (TLS_FLS_STATUS_OK == result) ? LFS2_ERR_OK : LFS2_ERR_IO;
    return err;
}

STATIC int w600_flash_lfs_sync(const struct lfs2_config *c) {
    return LFS2_ERR_OK;
}
#endif

void w600_flash_init_vfs(fs_user_mount_t *vfs_fs) {
#if MICROPY_VFS_FAT
    vfs_fs->base.type = &mp_fat_vfs_type;
    vfs_fs->blockdev.flags |= MP_BLOCKDEV_FLAG_NATIVE | MP_BLOCKDEV_FLAG_HAVE_IOCTL;
    vfs_fs->fatfs.drv = vfs_fs;
    vfs_fs->blockdev.readblocks[0] = (mp_obj_t)&w600_flash_readblocks_obj;
    vfs_fs->blockdev.readblocks[1] = (mp_obj_t)&w600_flash_obj;
    vfs_fs->blockdev.readblocks[2] = (mp_obj_t)w600_flash_read; // native version
    vfs_fs->blockdev.writeblocks[0] = (mp_obj_t)&w600_flash_writeblocks_obj;
    vfs_fs->blockdev.writeblocks[1] = (mp_obj_t)&w600_flash_obj;
    vfs_fs->blockdev.writeblocks[2] = (mp_obj_t)w600_flash_write; // native version
    vfs_fs->blockdev.u.ioctl[0] = (mp_obj_t)&w600_flash_ioctl_obj;
    vfs_fs->blockdev.u.ioctl[1] = (mp_obj_t)&w600_flash_obj;
#endif
#if MICROPY_VFS_LFS2
    vfs_fs->base.type = &mp_type_vfs_lfs2;

    // block device operations
    vfs_fs->config.read  = w600_flash_lfs_read;
    vfs_fs->config.prog  = w600_flash_lfs_prog;
    vfs_fs->config.erase = w600_flash_lfs_erase;
    vfs_fs->config.sync  = w600_flash_lfs_sync;

    // block device configuration
    vfs_fs->config.read_size = INTERVAL_FLS_FS_SECTOR_SIZE;
    vfs_fs->config.prog_size = INTERVAL_FLS_FS_SECTOR_SIZE;
    vfs_fs->config.block_size = INTERVAL_FLS_FS_SECTOR_SIZE;
    vfs_fs->config.block_count = INTERVAL_FLS_LEN / INTERVAL_FLS_FS_SECTOR_SIZE;
    vfs_fs->config.cache_size = INTERVAL_FLS_FS_SECTOR_SIZE;
    vfs_fs->config.lookahead_size = INTERVAL_FLS_FS_SECTOR_SIZE;
    vfs_fs->config.block_cycles = 500;
#endif
}

#endif

