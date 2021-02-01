/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32_MBOOT_MBOOT_H
#define MICROPY_INCLUDED_STM32_MBOOT_MBOOT_H

#include <stdint.h>
#include <stddef.h>

// Use this to tag global static data in RAM that doesn't need to be zeroed on startup
#define SECTION_NOZERO_BSS __attribute__((section(".nozero_bss")))

#define ELEM_DATA_SIZE (1024)
#define ELEM_DATA_START (&_estack[0])
#define ELEM_DATA_MAX (&_estack[ELEM_DATA_SIZE])

#define NORETURN __attribute__((noreturn))
#define MP_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

enum {
    MBOOT_ERRNO_FLASH_ERASE_DISALLOWED = 200,
    MBOOT_ERRNO_FLASH_ERASE_FAILED,
    MBOOT_ERRNO_FLASH_WRITE_DISALLOWED,

    MBOOT_ERRNO_DFU_INVALID_HEADER = 210,
    MBOOT_ERRNO_DFU_INVALID_TARGET,
    MBOOT_ERRNO_DFU_INVALID_SIZE,
    MBOOT_ERRNO_DFU_TOO_MANY_TARGETS,
    MBOOT_ERRNO_DFU_READ_ERROR,

    MBOOT_ERRNO_FSLOAD_NO_FSLOAD = 220,
    MBOOT_ERRNO_FSLOAD_NO_MOUNT,
    MBOOT_ERRNO_FSLOAD_INVALID_MOUNT,

    MBOOT_ERRNO_PACK_INVALID_ADDR = 230,
    MBOOT_ERRNO_PACK_INVALID_CHUNK,
    MBOOT_ERRNO_PACK_INVALID_VERSION,
    MBOOT_ERRNO_PACK_DECRYPT_FAILED,
    MBOOT_ERRNO_PACK_SIGN_FAILED,

    MBOOT_ERRNO_VFS_FAT_MOUNT_FAILED = 240,
    MBOOT_ERRNO_VFS_FAT_OPEN_FAILED,
    MBOOT_ERRNO_VFS_LFS1_MOUNT_FAILED,
    MBOOT_ERRNO_VFS_LFS1_OPEN_FAILED,
    MBOOT_ERRNO_VFS_LFS2_MOUNT_FAILED,
    MBOOT_ERRNO_VFS_LFS2_OPEN_FAILED,

    MBOOT_ERRNO_GUNZIP_FAILED = 250,
};

enum {
    ELEM_TYPE_END = 1,
    ELEM_TYPE_MOUNT,
    ELEM_TYPE_FSLOAD,
    ELEM_TYPE_STATUS,
};

enum {
    ELEM_MOUNT_FAT = 1,
    ELEM_MOUNT_LFS1,
    ELEM_MOUNT_LFS2,
};

extern uint8_t _estack[ELEM_DATA_SIZE];

uint32_t get_le32(const uint8_t *b);
void led_state_all(unsigned int mask);

int hw_page_erase(uint32_t addr, uint32_t *next_addr);
void hw_read(uint32_t addr, int len, uint8_t *buf);
int hw_write(uint32_t addr, const uint8_t *src8, size_t len);

int do_page_erase(uint32_t addr, uint32_t *next_addr);
void do_read(uint32_t addr, int len, uint8_t *buf);
int do_write(uint32_t addr, const uint8_t *src8, size_t len);

const uint8_t *elem_search(const uint8_t *elem, uint8_t elem_id);
int fsload_process(void);

#endif // MICROPY_INCLUDED_STM32_MBOOT_MBOOT_H
