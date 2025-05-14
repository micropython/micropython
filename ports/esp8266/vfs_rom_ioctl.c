/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024-2025 Damien P. George
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
#include "extmod/vfs.h"
#include "ets_alt_task.h"
#include "user_interface.h"

#if MICROPY_VFS_ROM_IOCTL

#define FLASH_MEM_BASE (0x40200000)
#define FLASH_PAGE_SIZE (4096)

#define MICROPY_HW_ROMFS_BASE (uintptr_t)(&_micropy_hw_romfs_part0_start)
#define MICROPY_HW_ROMFS_BYTES (uintptr_t)(&_micropy_hw_romfs_part0_size)

#define ROMFS_SPI_FLASH_OFFSET (MICROPY_HW_ROMFS_BASE - FLASH_MEM_BASE)

extern uint8_t _micropy_hw_romfs_part0_start;
extern uint8_t _micropy_hw_romfs_part0_size;

static const MP_DEFINE_MEMORYVIEW_OBJ(romfs_obj, 'B', 0, MICROPY_HW_ROMFS_BYTES, (void *)MICROPY_HW_ROMFS_BASE);

mp_obj_t mp_vfs_rom_ioctl(size_t n_args, const mp_obj_t *args) {
    switch (mp_obj_get_int(args[0])) {
        case MP_VFS_ROM_IOCTL_GET_NUMBER_OF_SEGMENTS:
            return MP_OBJ_NEW_SMALL_INT(1);

        case MP_VFS_ROM_IOCTL_GET_SEGMENT:
            return MP_OBJ_FROM_PTR(&romfs_obj);

        case MP_VFS_ROM_IOCTL_WRITE_PREPARE: {
            uint32_t dest = ROMFS_SPI_FLASH_OFFSET;
            uint32_t dest_max = dest + mp_obj_get_int(args[2]);
            while (dest < dest_max) {
                ets_loop_iter(); // flash access takes time so run any pending tasks
                SpiFlashOpResult res = spi_flash_erase_sector(dest / FLASH_PAGE_SIZE);
                ets_loop_iter();
                if (res != SPI_FLASH_RESULT_OK) {
                    return MP_OBJ_NEW_SMALL_INT(res == SPI_FLASH_RESULT_TIMEOUT ? -MP_ETIMEDOUT : -MP_EIO);
                }
                dest += FLASH_PAGE_SIZE;
            }
            return MP_OBJ_NEW_SMALL_INT(4); // minimum write size
        }

        case MP_VFS_ROM_IOCTL_WRITE: {
            mp_int_t offset = ROMFS_SPI_FLASH_OFFSET + mp_obj_get_int(args[2]);
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
            ets_loop_iter(); // flash access takes time so run any pending tasks
            SpiFlashOpResult res = spi_flash_write(offset, bufinfo.buf, (bufinfo.len + 3) & ~3);
            ets_loop_iter();
            if (res == SPI_FLASH_RESULT_OK) {
                return MP_OBJ_NEW_SMALL_INT(0);
            }
            return MP_OBJ_NEW_SMALL_INT(res == SPI_FLASH_RESULT_TIMEOUT ? -MP_ETIMEDOUT : -MP_EIO);
        }

        default:
            return MP_OBJ_NEW_SMALL_INT(-MP_EINVAL);
    }
}

#endif // MICROPY_VFS_ROM_IOCTL
