/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 OpenMV LLC.
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

#include "py/mperrno.h"
#include "py/objarray.h"
#include "py/runtime.h"
#include "extmod/vfs.h"
#include "modalif.h"
#include "mpu.h"
#include "mram.h"
#include "ospi_flash.h"

#if MICROPY_VFS_ROM_IOCTL

#if MICROPY_HW_ROMFS_ENABLE_PART0 && !defined(MICROPY_HW_ROMFS_PART0_START)
#define MICROPY_HW_ROMFS_PART0_START (uintptr_t)(&_micropy_hw_romfs_part0_start)
#define MICROPY_HW_ROMFS_PART0_SIZE (uintptr_t)(&_micropy_hw_romfs_part0_size)
extern uint8_t _micropy_hw_romfs_part0_start;
extern uint8_t _micropy_hw_romfs_part0_size;
#endif

#if MICROPY_HW_ROMFS_ENABLE_PART1 && !defined(MICROPY_HW_ROMFS_PART1_START)
#define MICROPY_HW_ROMFS_PART1_START (uintptr_t)(&_micropy_hw_romfs_part1_start)
#define MICROPY_HW_ROMFS_PART1_SIZE (uintptr_t)(&_micropy_hw_romfs_part1_size)
extern uint8_t _micropy_hw_romfs_part1_start;
extern uint8_t _micropy_hw_romfs_part1_size;
#endif

#define ROMFS_MEMORYVIEW(base, size) {{&mp_type_memoryview}, 'B', 0, (size), (void *)(base)}

static const mp_obj_array_t romfs_obj_table[] = {
    #if MICROPY_HW_ROMFS_ENABLE_PART0
    ROMFS_MEMORYVIEW(MICROPY_HW_ROMFS_PART0_START, MICROPY_HW_ROMFS_PART0_SIZE),
    #endif
    #if MICROPY_HW_ROMFS_ENABLE_PART1
    ROMFS_MEMORYVIEW(MICROPY_HW_ROMFS_PART1_START, MICROPY_HW_ROMFS_PART1_SIZE),
    #endif
};

static inline bool mram_is_valid_addr(uintptr_t addr) {
    return MRAM_BASE <= addr && addr < MRAM_BASE + MRAM_SIZE;
}

static inline bool ospi_is_valid_addr(uintptr_t xip_base, uintptr_t addr) {
    MP_STATIC_ASSERT(OSPI0_XIP_SIZE == OSPI1_XIP_SIZE);
    return xip_base <= addr && addr < xip_base + OSPI0_XIP_SIZE;
}

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

    const mp_obj_array_t *romfs_obj = &romfs_obj_table[romfs_id];

    if (cmd == MP_VFS_ROM_IOCTL_GET_SEGMENT) {
        // Return the ROMFS memoryview object.
        return MP_OBJ_FROM_PTR(romfs_obj);
    }

    uintptr_t romfs_base = (uintptr_t)romfs_obj->items;
    uintptr_t romfs_len = romfs_obj->len;

    #if MICROPY_HW_ENABLE_OSPI
    const uintptr_t ospi_base = ospi_flash_get_xip_base();
    #endif

    if (cmd == MP_VFS_ROM_IOCTL_WRITE_PREPARE) {
        if (n_args < 3) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }
        uint32_t dest = romfs_base;
        uint32_t dest_max = dest + mp_obj_get_int(args[2]);
        if (dest_max > romfs_base + romfs_len) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }

        if (mram_is_valid_addr(dest)) {
            // No preparation needed for MRAM.
            // Return the minimum write size.
            return MP_OBJ_NEW_SMALL_INT(MRAM_SECTOR_SIZE);
        }

        #if MICROPY_HW_ENABLE_OSPI
        if (ospi_is_valid_addr(ospi_base, dest)) {
            // Erase OSPI flash.
            dest -= ospi_base;
            dest_max -= ospi_base;
            while (dest < dest_max) {
                int ret = ospi_flash_erase_sector(dest);
                mp_event_handle_nowait();
                if (ret < 0) {
                    return MP_OBJ_NEW_SMALL_INT(ret);
                }
                dest += MICROPY_HW_FLASH_BLOCK_SIZE_BYTES;
            }
            // Return the minimum write size.
            return MP_OBJ_NEW_SMALL_INT(4);
        }
        #endif
    }

    if (cmd == MP_VFS_ROM_IOCTL_WRITE) {
        if (n_args < 4) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }
        uint32_t dest = romfs_base + mp_obj_get_int(args[2]);
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
        if (dest + bufinfo.len > romfs_base + romfs_len) {
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
        }

        if (mram_is_valid_addr(dest)) {
            // Write data to MRAM.
            mpu_config_mram(false);
            const uint8_t *src = bufinfo.buf;
            const uint8_t *max = src + bufinfo.len;
            while (src < max) {
                mram_write_128bit((uint8_t *)dest, src);
                dest += MRAM_SECTOR_SIZE;
                src += MRAM_SECTOR_SIZE;
            }
            mpu_config_mram(true);
            return MP_OBJ_NEW_SMALL_INT(0); // success
        }

        #if MICROPY_HW_ENABLE_OSPI
        if (ospi_is_valid_addr(ospi_base, dest)) {
            // Write data to OSPI flash.
            dest -= ospi_base;
            int ret = ospi_flash_write(dest, bufinfo.len, bufinfo.buf);
            mp_event_handle_nowait();
            return MP_OBJ_NEW_SMALL_INT(ret);
        }
        #endif
    }

    return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
}

#endif // MICROPY_VFS_ROM_IOCTL
