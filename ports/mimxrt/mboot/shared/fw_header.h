/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Philipp Ebensberger
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

#ifndef MICROPY_INCLUDED_MIMXRT_MBOOT_FW_HEADER_H
#define MICROPY_INCLUDED_MIMXRT_MBOOT_FW_HEADER_H

#include <stdint.h>
#include <stddef.h>

#define FW_HEADER_MAGIC "MPFW"
#define FW_HEADER_MAGIC_SIZE (4)
#define FW_HEADER_SIZE (sizeof(fw_header_t))
#define FW_HEADER_CRC_SIZE (sizeof(fw_header_crc_t))

typedef uint32_t fw_header_crc_t;

typedef struct _fw_header_t
{
    char magic[FW_HEADER_MAGIC_SIZE];  // 'MPFW'
    uint8_t reserved0[4];
    fw_header_crc_t crc;
    size_t size;
    uintptr_t entry_addr;
    uint8_t reserved1[8];
    fw_header_crc_t header_crc;
}fw_header_t;

#endif // MICROPY_INCLUDED_MIMXRT_MBOOT_FW_HEADER_H
