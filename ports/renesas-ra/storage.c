/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2018 Damien P. George
 * Copyright (c) 2021 Renesas Electronics Corporation
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
#include "py/mperrno.h"
#include "extmod/vfs_fat.h"

#include "systick.h"
#include "led.h"
#include "storage.h"
#include "irq.h"

#if MICROPY_HW_ENABLE_STORAGE

#define STORAGE_SYSTICK_MASK    (0x1ff) // 512ms
#define STORAGE_IDLE_TICK(tick) (((tick) & ~(SYSTICK_DISPATCH_NUM_SLOTS - 1) & STORAGE_SYSTICK_MASK) == 0)

#if defined(MICROPY_HW_BDEV2_IOCTL)
#define FLASH_PART2_START_BLOCK (FLASH_PART1_START_BLOCK + MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_NUM_BLOCKS, 0))
#endif

static bool storage_is_initialised = false;

// ToDo: check how to implement
// static void storage_systick_callback(uint32_t ticks_ms);

void storage_init(void) {
    if (!storage_is_initialised) {
        storage_is_initialised = true;

        MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_INIT, 0);

        #if defined(MICROPY_HW_BDEV2_IOCTL)
        MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_INIT, 0);
        #endif
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

void storage_irq_handler(void) {
    MICROPY_HW_BDEV_IOCTL(BDEV_IOCTL_IRQ_HANDLER, 0);
    #if defined(MICROPY_HW_BDEV2_IOCTL)
    MICROPY_HW_BDEV2_IOCTL(BDEV_IOCTL_IRQ_HANDLER, 0);
    #endif
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
    // printf("RD %u\n", block);
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
    // printf("WR %u\n", block);
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

int storage_read_blocks(uint8_t *dest, uint32_t block_num, uint32_t num_blocks) {
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
            return -MP_EIO; // error
        }
    }
    return 0; // success
}

int storage_write_blocks(const uint8_t *src, uint32_t block_num, uint32_t num_blocks) {
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
            return -MP_EIO; // error
        }
    }
    return 0; // success
}

/******************************************************************************/
// MicroPython bindings
//
// Expose the flash as an object with the block protocol.

#if (MICROPY_VFS_LFS1 || MICROPY_VFS_LFS2) && MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE
// Board uses littlefs and internal flash, so enable extended block protocol on internal flash
#define PYB_FLASH_NATIVE_BLOCK_SIZE (FLASH_BLOCK_SIZE)
#define MICROPY_HW_BDEV_READBLOCKS_EXT(dest, bl, off, len) (flash_bdev_readblocks_ext((dest), (bl), (off), (len)))
#define MICROPY_HW_BDEV_WRITEBLOCKS_EXT(dest, bl, off, len) (flash_bdev_writeblocks_ext((dest), (bl), (off), (len)))
#endif

#ifndef PYB_FLASH_NATIVE_BLOCK_SIZE
#define PYB_FLASH_NATIVE_BLOCK_SIZE (FLASH_BLOCK_SIZE)
#endif

#if defined(MICROPY_HW_BDEV_READBLOCKS_EXT)
// Size of blocks is PYB_FLASH_NATIVE_BLOCK_SIZE
int storage_readblocks_ext(uint8_t *dest, uint32_t block, uint32_t offset, uint32_t len) {
    return MICROPY_HW_BDEV_READBLOCKS_EXT(dest, block, offset, len);
}
#endif

typedef struct _pyb_flash_obj_t {
    mp_obj_base_t base;
    uint32_t start; // in bytes
    uint32_t len; // in bytes
} pyb_flash_obj_t;

// This Flash object represents the entire available flash, with emulated partition table at start
const pyb_flash_obj_t pyb_flash_obj = {
    { &pyb_flash_type },
    -(FLASH_PART1_START_BLOCK * FLASH_BLOCK_SIZE), // to offset FLASH_PART1_START_BLOCK
    0, // actual size handled in ioctl, MP_BLOCKDEV_IOCTL_BLOCK_COUNT case
};

STATIC void pyb_flash_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    pyb_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self == &pyb_flash_obj) {
        mp_printf(print, "Flash()");
    } else {
        mp_printf(print, "Flash(start=%u, len=%u)", self->start, self->len);
    }
}

STATIC mp_obj_t pyb_flash_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Parse arguments
    enum { ARG_start, ARG_len };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_len,   MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_start].u_int == -1 && args[ARG_len].u_int == -1) {
        // Default singleton object that accesses entire flash, including virtual partition table
        return MP_OBJ_FROM_PTR(&pyb_flash_obj);
    }

    pyb_flash_obj_t *self = mp_obj_malloc(pyb_flash_obj_t, &pyb_flash_type);

    uint32_t bl_len = (storage_get_block_count() - FLASH_PART1_START_BLOCK) * FLASH_BLOCK_SIZE;

    mp_int_t start = args[ARG_start].u_int;
    if (start == -1) {
        start = 0;
    } else if (!(0 <= start && start < bl_len && start % PYB_FLASH_NATIVE_BLOCK_SIZE == 0)) {
        mp_raise_ValueError(NULL);
    }

    mp_int_t len = args[ARG_len].u_int;
    if (len == -1) {
        len = bl_len - start;
    } else if (!(0 < len && start + len <= bl_len && len % PYB_FLASH_NATIVE_BLOCK_SIZE == 0)) {
        mp_raise_ValueError(NULL);
    }

    self->start = start;
    self->len = len;

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t pyb_flash_readblocks(size_t n_args, const mp_obj_t *args) {
    pyb_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_WRITE);
    mp_uint_t ret = -MP_EIO;
    if (n_args == 3) {
        // Cast self->start to signed in case it's pyb_flash_obj with negative start
        block_num += FLASH_PART1_START_BLOCK + (int32_t)self->start / FLASH_BLOCK_SIZE;
        ret = storage_read_blocks(bufinfo.buf, block_num, bufinfo.len / FLASH_BLOCK_SIZE);
    }
    #if defined(MICROPY_HW_BDEV_READBLOCKS_EXT)
    else if (self != &pyb_flash_obj) {
        // Extended block read on a sub-section of the flash storage
        uint32_t offset = mp_obj_get_int(args[3]);
        block_num += self->start / PYB_FLASH_NATIVE_BLOCK_SIZE;
        ret = MICROPY_HW_BDEV_READBLOCKS_EXT(bufinfo.buf, block_num, offset, bufinfo.len);
    }
    #endif
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_flash_readblocks_obj, 3, 4, pyb_flash_readblocks);

STATIC mp_obj_t pyb_flash_writeblocks(size_t n_args, const mp_obj_t *args) {
    pyb_flash_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    uint32_t block_num = mp_obj_get_int(args[1]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[2], &bufinfo, MP_BUFFER_READ);
    mp_uint_t ret = -MP_EIO;
    if (n_args == 3) {
        // Cast self->start to signed in case it's pyb_flash_obj with negative start
        block_num += FLASH_PART1_START_BLOCK + (int32_t)self->start / FLASH_BLOCK_SIZE;
        ret = storage_write_blocks(bufinfo.buf, block_num, bufinfo.len / FLASH_BLOCK_SIZE);
    }
    #if defined(MICROPY_HW_BDEV_WRITEBLOCKS_EXT)
    else if (self != &pyb_flash_obj) {
        // Extended block write on a sub-section of the flash storage
        uint32_t offset = mp_obj_get_int(args[3]);
        block_num += self->start / PYB_FLASH_NATIVE_BLOCK_SIZE;
        ret = MICROPY_HW_BDEV_WRITEBLOCKS_EXT(bufinfo.buf, block_num, offset, bufinfo.len);
    }
    #endif
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_flash_writeblocks_obj, 3, 4, pyb_flash_writeblocks);

STATIC mp_obj_t pyb_flash_ioctl(mp_obj_t self_in, mp_obj_t cmd_in, mp_obj_t arg_in) {
    pyb_flash_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_int_t cmd = mp_obj_get_int(cmd_in);
    switch (cmd) {
        case MP_BLOCKDEV_IOCTL_INIT: {
            mp_int_t ret = 0;
            storage_init();
            if (mp_obj_get_int(arg_in) == 1) {
                // Will be using extended block protocol
                if (self == &pyb_flash_obj) {
                    ret = -1;
                }
            }
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
        case MP_BLOCKDEV_IOCTL_DEINIT:
            storage_flush();
            return MP_OBJ_NEW_SMALL_INT(0);                                             // TODO properly
        case MP_BLOCKDEV_IOCTL_SYNC:
            storage_flush();
            return MP_OBJ_NEW_SMALL_INT(0);

        case MP_BLOCKDEV_IOCTL_BLOCK_COUNT: {
            mp_int_t n;
            if (self == &pyb_flash_obj) {
                // Get true size
                n = storage_get_block_count();
            } else {
                n = self->len / FLASH_BLOCK_SIZE;
            }
            return MP_OBJ_NEW_SMALL_INT(n);
        }

        case MP_BLOCKDEV_IOCTL_BLOCK_SIZE: {
            mp_int_t n = FLASH_BLOCK_SIZE;
            return MP_OBJ_NEW_SMALL_INT(n);
        }

        case MP_BLOCKDEV_IOCTL_BLOCK_ERASE: {
            int ret = 0;
            return MP_OBJ_NEW_SMALL_INT(ret);
        }

        default:
            return mp_const_none;
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
    .print = pyb_flash_print,
    .make_new = pyb_flash_make_new,
    .locals_dict = (mp_obj_dict_t *)&pyb_flash_locals_dict,
};

void pyb_flash_init_vfs(fs_user_mount_t *vfs) {
    vfs->base.type = &mp_fat_vfs_type;
    vfs->blockdev.flags |= MP_BLOCKDEV_FLAG_NATIVE | MP_BLOCKDEV_FLAG_HAVE_IOCTL;
    vfs->fatfs.drv = vfs;
    #if MICROPY_FATFS_MULTI_PARTITION
    vfs->fatfs.part = 1; // flash filesystem lives on first partition
    #endif
    vfs->blockdev.readblocks[0] = MP_OBJ_FROM_PTR(&pyb_flash_readblocks_obj);
    vfs->blockdev.readblocks[1] = MP_OBJ_FROM_PTR(&pyb_flash_obj);
    vfs->blockdev.readblocks[2] = MP_OBJ_FROM_PTR(storage_read_blocks); // native version
    vfs->blockdev.writeblocks[0] = MP_OBJ_FROM_PTR(&pyb_flash_writeblocks_obj);
    vfs->blockdev.writeblocks[1] = MP_OBJ_FROM_PTR(&pyb_flash_obj);
    vfs->blockdev.writeblocks[2] = MP_OBJ_FROM_PTR(storage_write_blocks); // native version
    vfs->blockdev.u.ioctl[0] = MP_OBJ_FROM_PTR(&pyb_flash_ioctl_obj);
    vfs->blockdev.u.ioctl[1] = MP_OBJ_FROM_PTR(&pyb_flash_obj);
}

#endif
