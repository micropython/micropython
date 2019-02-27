/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
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

#include <stdint.h>
#include <string.h>

#include "py/runtime.h"
#include "extmod/vfs_fat.h"

#include "systick.h"
#include "led.h"
#include "storage.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_STORAGE

#define STORAGE_SYSTICK_MASK    (0x1ff) // 512ms
#define STORAGE_IDLE_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & STORAGE_SYSTICK_MASK) == 0)

#define FLASH_PART1_START_BLOCK (0x100)

#if defined(MICROPY_HW_BDEV2_IOCTL)
#define FLASH_PART2_START_BLOCK (FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0))
#endif

static bool storage_is_initialised = false;

static void storage_systick_callback(uint32_t ticks_ms);

void storage_init(void) {
    if (!storage_is_initialised) {
        storage_is_initialised = true;

        systick_enable_dispatch(SYSTICK_DISPATCH_STORAGE, storage_systick_callback);

        MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_INIT, 0);

        #if defined(MICROPY_HW_BDEV2_IOCTL)
        MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_INIT, 0);
        #endif

        // Enable the flash IRQ, which is used to also call our storage IRQ handler
        // It must go at the same priority as USB (see comment in irq.h).
        NVIC_SetPriority(FLASH_IRQn, IRQ_PRI_FLASH);
        HAL_NVIC_EnableIRQ(FLASH_IRQn);
    }
}

uint32_t storage_get_block_size(void) {
    return FLASH_BLOCK_SIZE;
}

uint32_t storage_get_block_count(void) {
    #if defined(MICROPY_HW_BDEV2_IOCTL)
    return FLASH_PART2_START_BLOCK + MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0);
    #else
    return FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0);
    #endif
}

static void storage_systick_callback(uint32_t ticks_ms) {
    if (STORAGE_IDLE_TICK(ticks_ms)) {
        // Trigger a FLASH IRQ to execute at a lower priority
        NVIC->STIR = FLASH_IRQn;
    }
}

void FLASH_IRQHandler(void) {
    IRQ_ENTER(FLASH_IRQn);
    MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_IRQ_HANDLER, 0);
    #if defined(MICROPY_HW_BDEV2_IOCTL)
    MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_IRQ_HANDLER, 0);
    #endif
    IRQ_EXIT(FLASH_IRQn);
}

void storage_flush(void) {
    MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_SYNC, 0);
    #if defined(MICROPY_HW_BDEV2_IOCTL)
    MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_SYNC, 0);
    #endif
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

bool storage_read_block(uint8_t *dest, uint32_t block) {
    //printf("RD %u\n", block);
    if (block == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, FLASH_PART1_START_BLOCK, MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0));
        #if defined(MICROPY_HW_BDEV2_IOCTL)
        build_partition(dest + 462, 0, 0x01 /* FAT12 */, FLASH_PART2_START_BLOCK, MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0));
        #else
        build_partition(dest + 462, 0, 0, 0, 0);
        #endif
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return true;

    #if defined(MICROPY_HW_BDEV_READBLOCK)
    } else if (FLASH_PART1_START_BLOCK <= block && block < FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0)) {
        return MICROPY_HW_BDEV_READBLOCK(dest, block - FLASH_PART1_START_BLOCK);
    #endif
    } else {
        return false;
    }
}

bool storage_write_block(const uint8_t *src, uint32_t block) {
    //printf("WR %u\n", block);
    if (block == 0) {
        // can't write MBR, but pretend we did
        return true;
    #if defined(MICROPY_HW_BDEV_WRITEBLOCK)
    } else if (FLASH_PART1_START_BLOCK <= block && block < FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0)) {
        return MICROPY_HW_BDEV_WRITEBLOCK(src, block - FLASH_PART1_START_BLOCK);
    #endif
    } else {
        return false;
    }
}

mp_uint_t storage_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
    #if defined(MICROPY_HW_BDEV_READBLOCKS)
    if (FLASH_PART1_START_BLOCK <= block_num && block_num + num_blocks <= FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0)) {
        return MICROPY_HW_BDEV_READBLOCKS(dest, block_num - FLASH_PART1_START_BLOCK, num_blocks);
    }
    #endif

    #if defined(MICROPY_HW_BDEV2_READBLOCKS)
    if (FLASH_PART2_START_BLOCK <= block_num && block_num + num_blocks <= FLASH_PART2_START_BLOCK + MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0)) {
        return MICROPY_HW_BDEV2_READBLOCKS(dest, block_num - FLASH_PART2_START_BLOCK, num_blocks);
    }
    #endif

    for (size_t i = 0; i < num_blocks; i++) {
        if (!storage_read_block(dest + i * FLASH_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

mp_uint_t storage_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
    #if defined(MICROPY_HW_BDEV_WRITEBLOCKS)
    if (FLASH_PART1_START_BLOCK <= block_num && block_num + num_blocks <= FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0)) {
        return MICROPY_HW_BDEV_WRITEBLOCKS(src, block_num - FLASH_PART1_START_BLOCK, num_blocks);
    }
    #endif

    #if defined(MICROPY_HW_BDEV2_WRITEBLOCKS)
    if (FLASH_PART2_START_BLOCK <= block_num && block_num + num_blocks <= FLASH_PART2_START_BLOCK + MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0)) {
        return MICROPY_HW_BDEV2_WRITEBLOCKS(src, block_num - FLASH_PART2_START_BLOCK, num_blocks);
    }
    #endif

    for (size_t i = 0; i < num_blocks; i++) {
        if (!storage_write_block(src + i * FLASH_BLOCK_SIZE, block_num + i)) {
            return 1; // error
        }
    }
    return 0; // success
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the flash as an object with the block protocol.

// there is a singleton Flash object
STATIC const mp_obj_base_t pyb_flash_obj = {&pyb_flash_type};

STATIC mp_obj_t pyb_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // return singleton object
    return MP_OBJ_FROM_PTR(&pyb_flash_obj);
}

STATIC mp_obj_t pyb_flash_readblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = storage_read_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_readblocks_obj, pyb_flash_readblocks);

STATIC mp_obj_t pyb_flash_writeblocks(mp_obj_t self, mp_obj_t block_num, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = storage_write_blocks(bufinfo.buf, mp_obj_get_int(block_num), bufinfo.len / FLASH_BLOCK_SIZE);
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_writeblocks_obj, pyb_flash_writeblocks);

STATIC mp_obj_t pyb_flash_ioctl(mp_obj_t self, mp_obj_t cmd_in, mp_obj_t arg_in) {
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case BP_IOCTL_INIT: storage_init(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_DEINIT: storage_flush(); return MP_OBJ_NEW_SMALL_INT(0); // TODO properly
        case BP_IOCTL_SYNC: storage_flush(); return MP_OBJ_NEW_SMALL_INT(0);
        case BP_IOCTL_SEC_COUNT: return MP_OBJ_NEW_SMALL_INT(storage_get_block_count());
        case BP_IOCTL_SEC_SIZE: return MP_OBJ_NEW_SMALL_INT(storage_get_block_size());
        default: return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(pyb_flash_ioctl_obj, pyb_flash_ioctl);

STATIC const mp_rom_map_elem_t pyb_flash_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readblocks), MP_ROM_PTR(&pyb_flash_readblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_writeblocks), MP_ROM_PTR(&pyb_flash_writeblocks_obj) },
    { MP_ROM_QSTR(MP_QSTR_ioctl), MP_ROM_PTR(&pyb_flash_ioctl_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_flash_locals_dict, pyb_flash_locals_dict_table);

const mp_obj_type_t pyb_flash_type = {
    { &mp_type_type },
    .name = MP_QSTR_Flash,
    .make_new = pyb_flash_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_flash_locals_dict,
};

void pyb_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->flags |= FSUSER_NATIVE | FSUSER_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    vfs->readblocks[0] = MP_OBJ_FROM_PTR(&pyb_flash_readblocks_obj);
    vfs->readblocks[1] = MP_OBJ_FROM_PTR(&pyb_flash_obj);
    vfs->readblocks[2] = MP_OBJ_FROM_PTR(storage_read_blocks); // native version
    vfs->writeblocks[0] = MP_OBJ_FROM_PTR(&pyb_flash_writeblocks_obj);
    vfs->writeblocks[1] = MP_OBJ_FROM_PTR(&pyb_flash_obj);
    vfs->writeblocks[2] = MP_OBJ_FROM_PTR(storage_write_blocks); // native version
    vfs->u.ioctl[0] = MP_OBJ_FROM_PTR(&pyb_flash_ioctl_obj);
    vfs->u.ioctl[1] = MP_OBJ_FROM_PTR(&pyb_flash_obj);
}

#endif
