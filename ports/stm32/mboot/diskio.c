/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "mboot.h"

#if MBOOT_FSLOAD

#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE (FF_MIN_SS)
#else
#error Unsupported
#endif

DRESULT disk_read(void *pdrv, BYTE *buf, DWORD sector, UINT count) {
    fsload_bdev_t *bdev = pdrv;

    if (0 <= sector && sector < bdev->byte_len / 512) {
        do_read(bdev->base_addr + sector * SECSIZE, count * SECSIZE, buf);
        return RES_OK;
    }

    return RES_PARERR;
}

DRESULT disk_ioctl(void *pdrv, BYTE cmd, void *buf) {
    fsload_bdev_t *bdev = pdrv;

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            *((DWORD*)buf) = bdev->byte_len / SECSIZE;
            return RES_OK;

        case GET_SECTOR_SIZE:
            *((WORD*)buf) = SECSIZE;
            return RES_OK;

        case GET_BLOCK_SIZE:
            *((DWORD*)buf) = 1; // erase block size in units of sector size
            return RES_OK;

        case IOCTL_INIT:
        case IOCTL_STATUS:
            *((DSTATUS*)buf) = STA_PROTECT;
            return RES_OK;

        default:
            return RES_PARERR;
    }
}

#endif // MBOOT_FSLOAD
