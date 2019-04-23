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
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"

#if MICROPY_USE_INTERVAL_FLS_FS

#define INTERVAL_FLS_BASE               (USER_ADDR_START - 0x8000)//80k (32+48)
#define INTERVAL_FLS_LEN                (0x8000 + USER_AREA_LEN)//gz image <=352kb
#define INTERVAL_LFS_SECTOR_SIZE        (FF_MAX_SS)

STATIC DRESULT w600_flash_read (BYTE *buff, DWORD sector, UINT count) {
    int result = tls_fls_read(INTERVAL_FLS_BASE + (sector * INTERVAL_LFS_SECTOR_SIZE),
                              buff, INTERVAL_LFS_SECTOR_SIZE * count);
    DSTATUS stat = (TLS_FLS_STATUS_OK == result) ? RES_OK : RES_ERROR;

    return stat;
}

STATIC DRESULT w600_flash_write (const BYTE *buff, DWORD sector, UINT count) {
    int result = tls_fls_write(INTERVAL_FLS_BASE + (sector * INTERVAL_LFS_SECTOR_SIZE),
                               (u8 *)buff, INTERVAL_LFS_SECTOR_SIZE * count);
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
    DRESULT res = w600_flash_read(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / INTERVAL_LFS_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(w600_flash_readblocks_obj, w600_flash_readblocks);

STATIC mp_obj_t w600_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    DRESULT res = w600_flash_write(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / INTERVAL_LFS_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(w600_flash_writeblocks_obj, w600_flash_writeblocks);

STATIC mp_obj_t w600_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
    case BP_IOCTL_INIT:
        return MP_OBJ_NEW_SMALL_INT(RES_OK);
    case BP_IOCTL_DEINIT:
        return MP_OBJ_NEW_SMALL_INT(RES_OK);
    case BP_IOCTL_SYNC:
        return MP_OBJ_NEW_SMALL_INT(RES_OK);
    case BP_IOCTL_SEC_COUNT:
        return MP_OBJ_NEW_SMALL_INT(INTERVAL_FLS_LEN / INTERVAL_LFS_SECTOR_SIZE);
    case BP_IOCTL_SEC_SIZE:
        return MP_OBJ_NEW_SMALL_INT(INTERVAL_LFS_SECTOR_SIZE);
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

void w600_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->readblocks[0] = (mp_obj_t)&w600_flash_readblocks_obj;
    vfs->readblocks[1] = (mp_obj_t)&w600_flash_obj;
    vfs->readblocks[2] = (mp_obj_t)w600_flash_read; // native version
    vfs->writeblocks[0] = (mp_obj_t)&w600_flash_writeblocks_obj;
    vfs->writeblocks[1] = (mp_obj_t)&w600_flash_obj;
    vfs->writeblocks[2] = (mp_obj_t)w600_flash_write; // native version
    vfs->u.ioctl[0] = (mp_obj_t)&w600_flash_ioctl_obj;
    vfs->u.ioctl[1] = (mp_obj_t)&w600_flash_obj;
}

#endif

