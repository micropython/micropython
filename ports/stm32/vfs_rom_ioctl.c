/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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

#include "py/obj.h"
#include "py/objarray.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "drivers/memory/spiflash.h"

#include "flash.h"
#include "qspi.h"
#include "storage.h"
#include "xspi.h"

#if MICROPY_VFS_ROM_IOCTL

#if MICROPY_HW_ROMFS_ENABLE_PART0 && defined(MICROPY_HW_ROMFS_PART0_START)
#define ROMFS0_DYNAMIC (1)
static MP_DEFINE_MEMORYVIEW_OBJ(romfs0_obj, 'B', 0, (uintptr_t)-1, (void *)-1);
#elif MICROPY_HW_ROMFS_ENABLE_PART0 && !defined(MICROPY_HW_ROMFS_PART0_START)
#define ROMFS0_DYNAMIC (0)
extern uint8_t _micropy_hw_romfs_part0_start;
extern uint8_t _micropy_hw_romfs_part0_size;
static const MP_DEFINE_MEMORYVIEW_OBJ(romfs0_obj, 'B', 0, (uintptr_t)&_micropy_hw_romfs_part0_size, (void *)&_micropy_hw_romfs_part0_start);
#endif

#if MICROPY_HW_ROMFS_ENABLE_PART1 && defined(MICROPY_HW_ROMFS_PART1_START)
#define ROMFS1_DYNAMIC (1)
static MP_DEFINE_MEMORYVIEW_OBJ(romfs1_obj, 'B', 0, (uintptr_t)-1, (void *)-1);
#elif MICROPY_HW_ROMFS_ENABLE_PART1 && !defined(MICROPY_HW_ROMFS_PART1_START)
#define ROMFS1_DYNAMIC (0)
extern uint8_t _micropy_hw_romfs_part1_start;
extern uint8_t _micropy_hw_romfs_part1_size;
static const MP_DEFINE_MEMORYVIEW_OBJ(romfs1_obj, 'B', 0, (uintptr_t)&_micropy_hw_romfs_part1_size, (void *)&_micropy_hw_romfs_part1_start);
#endif

static const mp_obj_array_t *romfs_obj_table[] = {
    #if MICROPY_HW_ROMFS_ENABLE_PART0
    &romfs0_obj,
    #endif
    #if MICROPY_HW_ROMFS_ENABLE_PART1
    &romfs1_obj,
    #endif
};

mp_obj_t mp_vfs_rom_ioctl(size_t n_args, const mp_obj_t *args) {
    mp_int_t cmd = mp_obj_get_int(args[0]);
    if (cmd == MP_VFS_ROM_IOCTL_GET_NUMBER_OF_SEGMENTS) {
        return MP_OBJ_NEW_SMALL_INT(MP_ARRAY_SIZE(romfs_obj_table));
    }

    if (n_args < 2) {
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    mp_int_t romfs_id = mp_obj_get_int(args[1]);
    if (!(0 <= romfs_id && romfs_id < MP_ARRAY_SIZE(romfs_obj_table))) {
        return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }

    #if ROMFS0_DYNAMIC
    if (romfs_id == 0) {
        romfs0_obj.items = (void *)MICROPY_HW_ROMFS_PART0_START;
        romfs0_obj.len = MICROPY_HW_ROMFS_PART0_SIZE;
    }
    #endif
    #if ROMFS1_DYNAMIC
    if (romfs_id == 1) {
        romfs1_obj.items = (void *)MICROPY_HW_ROMFS_PART1_START;
        romfs1_obj.len = MICROPY_HW_ROMFS_PART1_SIZE;
    }
    #endif

    const mp_obj_array_t *romfs_obj = romfs_obj_table[romfs_id];
    uintptr_t romfs_base = (uintptr_t)romfs_obj->items;
    uintptr_t romfs_len = romfs_obj->len;

    if (cmd == MP_VFS_ROM_IOCTL_GET_SEGMENT) {
        // Return the ROMFS memoryview object.
        return MP_OBJ_FROM_PTR(romfs_obj);
    }

    if (cmd == MP_VFS_ROM_IOCTL_WRITE_PREPARE) {
        // Erase sectors in given range.
        if (n_args < 3) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }
        uint32_t dest = romfs_base;
        uint32_t dest_max = dest + mp_obj_get_int(args[2]);
        if (dest_max > romfs_base + romfs_len) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }

        #if MICROPY_HW_ROMFS_ENABLE_INTERNAL_FLASH
        if (flash_is_valid_addr(dest)) {
            while (dest < dest_max) {
                int ret = flash_erase(dest);
                if (ret < 0) {
                    return MP_OBJ_NEW_SMALL_INT(ret);
                }
                uint32_t sector_size = 0;
                flash_get_sector_info(dest, NULL, &sector_size);
                dest += sector_size;
            }
            return MP_OBJ_NEW_SMALL_INT(16);
        }
        #endif

        #if MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI
        if (qspi_is_valid_addr(dest)) {
            dest -= QSPI_MAP_ADDR;
            dest_max -= QSPI_MAP_ADDR;
            while (dest < dest_max) {
                int ret = mp_spiflash_erase_block(MICROPY_HW_ROMFS_QSPI_SPIFLASH_OBJ, dest);
                if (ret < 0) {
                    return MP_OBJ_NEW_SMALL_INT(ret);
                }
                dest += MP_SPIFLASH_ERASE_BLOCK_SIZE;
            }
            return MP_OBJ_NEW_SMALL_INT(4);
        }
        #endif

        #if MICROPY_HW_ROMFS_ENABLE_EXTERNAL_XSPI
        if (xspi_is_valid_addr(&xspi_flash2, dest)) {
            dest -= xspi_get_xip_base(&xspi_flash2);
            dest_max -= xspi_get_xip_base(&xspi_flash2);
            int ret = spi_bdev_eraseblocks_raw(MICROPY_HW_ROMFS_XSPI_SPIBDEV_OBJ, dest / MP_SPIFLASH_ERASE_BLOCK_SIZE, dest_max - dest + MP_SPIFLASH_ERASE_BLOCK_SIZE - 1);
            if (ret < 0) {
                return MP_OBJ_NEW_SMALL_INT(ret);
            }
            return MP_OBJ_NEW_SMALL_INT(4);
        }
        #endif
    }

    if (cmd == MP_VFS_ROM_IOCTL_WRITE) {
        // Write data to flash.
        if (n_args < 4) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }
        uint32_t dest = romfs_base + mp_obj_get_int(args[2]);
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        if (dest + bufinfo.len > romfs_base + romfs_len) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }

        #if MICROPY_HW_ROMFS_ENABLE_INTERNAL_FLASH
        if (flash_is_valid_addr(dest)) {
            int ret = flash_write(dest, bufinfo.buf, bufinfo.len / 4);
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
        #endif

        #if MICROPY_HW_ROMFS_ENABLE_EXTERNAL_QSPI
        if (qspi_is_valid_addr(dest)) {
            dest -= QSPI_MAP_ADDR;
            int ret = mp_spiflash_write(MICROPY_HW_ROMFS_QSPI_SPIFLASH_OBJ, dest, bufinfo.len, bufinfo.buf);
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
        #endif

        #if MICROPY_HW_ROMFS_ENABLE_EXTERNAL_XSPI
        if (xspi_is_valid_addr(&xspi_flash2, dest)) {
            dest -= xspi_get_xip_base(&xspi_flash2);
            int ret = spi_bdev_writeblocks_raw(MICROPY_HW_ROMFS_XSPI_SPIBDEV_OBJ, bufinfo.buf, 0, dest, bufinfo.len);
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
        #endif
    }

    return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
}

#endif // MICROPY_VFS_ROM_IOCTL
