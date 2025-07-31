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

#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "extmod/vfs_fat.h"

#include "fatfs/src/drivers/sflash_diskio.h"
#include "mods/pybflash.h"

/******************************************************************************/
// MicroPython bindings to expose the internal flash as an object with the
// block protocol.

// there is a singleton Flash object
static const mp_obj_base_t pyb_flash_obj = {&pyb_flash_type};

static mp_obj_t pyb_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&pyb_flash_obj;
}

static mp_obj_t pyb_flash_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    DRESULT res = sflash_disk_read(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SFLASH_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
static MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_readblocks_obj, pyb_flash_readblocks);

static mp_obj_t pyb_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    DRESULT res = sflash_disk_write(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / SFLASH_SECTOR_SIZE);
    return MP_OBJ_NEW_SMALL_INT(res != RES_OK); // return of 0 means success
}
static MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_writeblocks_obj, pyb_flash_writeblocks);

static mp_obj_t pyb_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT: return MP_OBJ_NEW_SMALL_INT(sflash_disk_init() != RES_OK);
        case MP_BLOCKDEV_IOCTL_DEINIT: sflash_disk_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_SYNC: sflash_disk_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT: return MP_OBJ_NEW_SMALL_INT(SFLASH_SECTOR_COUNT);
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE: return MP_OBJ_NEW_SMALL_INT(SFLASH_SECTOR_SIZE);
        default: return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_ioctl_obj, pyb_flash_ioctl);

static const mp_rom_map_elem_t pyb_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_flash_ioctl_obj) },
};

static MP_DEFINE_CONST_DICT(pyb_flash_locals_dict, pyb_flash_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    pyb_flash_type,
    MP_QSTR_Flash,
    MP_TYPE_FLAG_NONE,
    make_new, pyb_flash_make_new,
    locals_dict, &pyb_flash_locals_dict
    );

void pyb_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_NATIVE | MP_BLOCKDEV_FLAG_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->blockdev.readblocks[0] = (mp_obj_t)&pyb_flash_readblocks_obj;
    vfs->blockdev.readblocks[1] = (mp_obj_t)&pyb_flash_obj;
    vfs->blockdev.readblocks[2] = (mp_obj_t)sflash_disk_read; // native version
    vfs->blockdev.writeblocks[0] = (mp_obj_t)&pyb_flash_writeblocks_obj;
    vfs->blockdev.writeblocks[1] = (mp_obj_t)&pyb_flash_obj;
    vfs->blockdev.writeblocks[2] = (mp_obj_t)sflash_disk_write; // native version
    vfs->blockdev.u.ioctl[0] = (mp_obj_t)&pyb_flash_ioctl_obj;
    vfs->blockdev.u.ioctl[1] = (mp_obj_t)&pyb_flash_obj;
}
