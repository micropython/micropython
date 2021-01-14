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

#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "lib/oofatfs/diskio.h"
#include "mboot.h"
#include "vfs.h"

#if MBOOT_FSLOAD && MBOOT_VFS_FAT

#if FF_MAX_SS == FF_MIN_SS
#define SECSIZE (FF_MIN_SS)
#else
#error Unsupported
#endif

DRESULT disk_read(void *pdrv, BYTE *buf, DWORD sector, UINT count) {
    vfs_fat_context_t *ctx = pdrv;

    if (0 <= sector && sector < ctx->bdev_byte_len / 512) {
        hw_read(ctx->bdev_base_addr + sector * SECSIZE, count * SECSIZE, buf);
        return RES_OK;
    }

    return RES_PARERR;
}

DRESULT disk_ioctl(void *pdrv, BYTE cmd, void *buf) {
    vfs_fat_context_t *ctx = pdrv;

    switch (cmd) {
        case CTRL_SYNC:
            return RES_OK;

        case GET_SECTOR_COUNT:
            *((DWORD*)buf) = ctx->bdev_byte_len / SECSIZE;
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

int vfs_fat_mount(vfs_fat_context_t *ctx, uint32_t base_addr, uint32_t byte_len) {
    ctx->bdev_base_addr = base_addr;
    ctx->bdev_byte_len = byte_len;
    ctx->fatfs.drv = ctx;
    FRESULT res = f_mount(&ctx->fatfs);
    if (res != FR_OK) {
        return -1;
    }
    return 0;
}

static int vfs_fat_stream_open(void *stream_in, const char *fname) {
    vfs_fat_context_t *stream = stream_in;
    FRESULT res = f_open(&stream->fatfs, &stream->fp, fname, FA_READ);
    if (res != FR_OK) {
        return -1;
    }
    return 0;
}

static void vfs_fat_stream_close(void *stream_in) {
    vfs_fat_context_t *stream = stream_in;
    f_close(&stream->fp);
}

static int vfs_fat_stream_read(void *stream_in, uint8_t *buf, size_t len) {
    vfs_fat_context_t *stream = stream_in;
    UINT n;
    FRESULT res = f_read(&stream->fp, buf, len, &n);
    if (res != FR_OK) {
        return -1;
    }
    return n;
}

const stream_methods_t vfs_fat_stream_methods = {
    vfs_fat_stream_open,
    vfs_fat_stream_close,
    vfs_fat_stream_read,
};

#endif // MBOOT_FSLOAD && MBOOT_VFS_FAT
