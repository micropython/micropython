/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft
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
#include "supervisor/flash.h"

#include "extmod/vfs_fat.h"
#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "supervisor/shared/tick.h"

#define VFS_INDEX 0

#define PART1_START_BLOCK (0x1)

// there is a singleton Flash object
const mp_obj_type_t supervisor_flash_type;
STATIC const mp_obj_base_t supervisor_flash_obj = {&supervisor_flash_type};

STATIC mp_obj_t supervisor_flash_obj_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // check arguments
    mp_arg_check_num(n_args, kw_args, 0, 0, false);

    // return singleton object
    return (mp_obj_t)&supervisor_flash_obj;
}

uint32_t flash_get_block_count(void) {
    return PART1_START_BLOCK + supervisor_flash_get_block_count();
}

static void build_partition(uint8_t *buf, int boot, int type, uint32_t start_block, uint32_t num_blocks) {
    buf[0] = boot;

    if (num_blocks == 0) {
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
    } else {
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
    }

    buf[4] = type;

    if (num_blocks == 0) {
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
    } else {
        buf[5] = 0xff;
        buf[6] = 0xff;
        buf[7] = 0xff;
    }

    buf[8] = start_block;
    buf[9] = start_block >> 8;
    buf[10] = start_block >> 16;
    buf[11] = start_block >> 24;

    buf[12] = num_blocks;
    buf[13] = num_blocks >> 8;
    buf[14] = num_blocks >> 16;
    buf[15] = num_blocks >> 24;
}

mp_uint_t flash_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    if (block_num == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, PART1_START_BLOCK, supervisor_flash_get_block_count());
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;
        if (num_blocks > 1) {
            dest += 512;
            num_blocks -= 1;
            // Fall through and do a read from flash.
        } else {
            return 0; // Done and ok.
        }
    }
    return supervisor_flash_read_blocks(dest, block_num - PART1_START_BLOCK, num_blocks);
}

volatile bool filesystem_dirty = false;

mp_uint_t flash_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    if (block_num == 0) {
        if (num_blocks > 1) {
            return 1; // error
        }
        // can't write MBR, but pretend we did
        return 0;
    } else {
        if (!filesystem_dirty) {
            // Turn on ticks so that we can flush after a period of time elapses.
            supervisor_enable_tick();
            filesystem_dirty = true;
        }
        return supervisor_flash_write_blocks(src, block_num - PART1_START_BLOCK, num_blocks);
    }
}

void supervisor_flash_flush(void) {
    #if INTERNAL_FLASH_FILESYSTEM
    port_internal_flash_flush();
    #else
    supervisor_external_flash_flush();
    #endif
    // Turn off ticks now that our filesystem has been flushed.
    if (filesystem_dirty) {
        supervisor_disable_tick();
    }
    filesystem_dirty = false;
}

STATIC mp_obj_t supervisor_flash_obj_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = flash_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(supervisor_flash_obj_readblocks_obj, supervisor_flash_obj_readblocks);

STATIC mp_obj_t supervisor_flash_obj_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = flash_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FILESYSTEM_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(supervisor_flash_obj_writeblocks_obj, supervisor_flash_obj_writeblocks);

bool flash_ioctl(size_t cmd, mp_int_t *out_value) {
    *out_value = 0;
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT:
            supervisor_flash_init();
            break;
        case MP_BLOCKDEV_IOCTL_DEINIT:
            supervisor_flash_flush();
            break; // TODO properly
        case MP_BLOCKDEV_IOCTL_SYNC:
            supervisor_flash_flush();
            break;
        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT:
            *out_value = flash_get_block_count();
            break;
        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE:
            *out_value = supervisor_flash_get_block_size();
            break;
        default:
            return false;
    }
    return true;
}

STATIC mp_obj_t supervisor_flash_obj_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    mp_int_t out_value;
    if (flash_ioctl(cmd, &out_value)) {
        return MP_OBJ_NEW_SMALL_INT(out_value);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(supervisor_flash_obj_ioctl_obj, supervisor_flash_obj_ioctl);

STATIC const mp_rom_map_elem_t supervisor_flash_obj_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&supervisor_flash_obj_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&supervisor_flash_obj_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&supervisor_flash_obj_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(supervisor_flash_obj_locals_dict, supervisor_flash_obj_locals_dict_table);

const mp_obj_type_t supervisor_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = supervisor_flash_obj_make_new,
    .locals_dict = (mp_obj_t)&supervisor_flash_obj_locals_dict,
};

void supervisor_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_NATIVE | MP_BLOCKDEV_FLAG_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->blockdev.readblocks[0] = (mp_obj_t)&supervisor_flash_obj_readblocks_obj;
    vfs->blockdev.readblocks[1] = (mp_obj_t)&supervisor_flash_obj;
    vfs->blockdev.readblocks[2] = (mp_obj_t)flash_read_blocks; // native version
    vfs->blockdev.writeblocks[0] = (mp_obj_t)&supervisor_flash_obj_writeblocks_obj;
    vfs->blockdev.writeblocks[1] = (mp_obj_t)&supervisor_flash_obj;
    vfs->blockdev.writeblocks[2] = (mp_obj_t)flash_write_blocks; // native version
    vfs->blockdev.u.ioctl[0] = (mp_obj_t)&supervisor_flash_obj_ioctl_obj;
    vfs->blockdev.u.ioctl[1] = (mp_obj_t)&supervisor_flash_obj;
    vfs->blockdev.u.ioctl[2] = (mp_obj_t)flash_ioctl; // native version
}
