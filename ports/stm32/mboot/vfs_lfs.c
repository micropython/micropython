/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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

#include <string.h>

#include "py/mphal.h"
#include "mboot.h"
#include "vfs.h"

#if MBOOT_FSLOAD && (MBOOT_VFS_LFS1 || MBOOT_VFS_LFS2)

#if MBOOT_VFS_LFS1
#if MBOOT_VFS_LFS2
#error Unsupported
#endif

#define LFSx_MACRO(s) LFS1##s
#define LFSx_API(x) lfs1_ ## x
#define VFS_LFSx_CONTEXT_T vfs_lfs1_context_t
#define VFS_LFSx_MOUNT vfs_lfs1_mount
#define VFS_LFSx_STREAM_METHODS vfs_lfs1_stream_methods

#define SUPERBLOCK_MAGIC_OFFSET (40)
#define SUPERBLOCK_BLOCK_SIZE_OFFSET (28)
#define SUPERBLOCK_BLOCK_COUNT_OFFSET (32)

static uint8_t lfs_read_buffer[LFS_READ_SIZE];
static uint8_t lfs_prog_buffer[LFS_PROG_SIZE];
static uint8_t lfs_lookahead_buffer[LFS_LOOKAHEAD_SIZE / 8];

#else

#define LFSx_MACRO(s) LFS2##s
#define LFSx_API(x) lfs2_ ## x
#define VFS_LFSx_CONTEXT_T vfs_lfs2_context_t
#define VFS_LFSx_MOUNT vfs_lfs2_mount
#define VFS_LFSx_STREAM_METHODS vfs_lfs2_stream_methods

#define SUPERBLOCK_MAGIC_OFFSET (8)
#define SUPERBLOCK_BLOCK_SIZE_OFFSET (24)
#define SUPERBLOCK_BLOCK_COUNT_OFFSET (28)

static uint8_t lfs_read_buffer[LFS_CACHE_SIZE];
static uint8_t lfs_prog_buffer[LFS_CACHE_SIZE];
static uint8_t lfs_lookahead_buffer[LFS_LOOKAHEAD_SIZE];

#endif

static int dev_read(const struct LFSx_API (config) * c, LFSx_API(block_t) block, LFSx_API(off_t) off, void *buffer, LFSx_API(size_t) size) {
    VFS_LFSx_CONTEXT_T *ctx = c->context;
    if (0 <= block && block < ctx->config.block_count) {
        hw_read(ctx->bdev_base_addr + block * ctx->config.block_size + off, size, buffer);
        return LFSx_MACRO(_ERR_OK);
    }
    return LFSx_MACRO(_ERR_IO);
}

static int dev_prog(const struct LFSx_API (config) * c, LFSx_API(block_t) block, LFSx_API(off_t) off, const void *buffer, LFSx_API(size_t) size) {
    return LFSx_MACRO(_ERR_IO);
}

static int dev_erase(const struct LFSx_API (config) * c, LFSx_API(block_t) block) {
    return LFSx_MACRO(_ERR_IO);
}

static int dev_sync(const struct LFSx_API (config) * c) {
    return LFSx_MACRO(_ERR_OK);
}

int VFS_LFSx_MOUNT(VFS_LFSx_CONTEXT_T *ctx, uint32_t base_addr, uint32_t byte_len) {
    // Read start of superblock.
    uint8_t buf[48];
    hw_read(base_addr, sizeof(buf), buf);

    // Verify littlefs and detect block size.
    if (memcmp(&buf[SUPERBLOCK_MAGIC_OFFSET], "littlefs", 8) != 0) {
        return -1;
    }
    uint32_t block_size = get_le32(&buf[SUPERBLOCK_BLOCK_SIZE_OFFSET]);
    uint32_t block_count = get_le32(&buf[SUPERBLOCK_BLOCK_COUNT_OFFSET]);

    // Verify size of volume.
    if (block_size * block_count != byte_len) {
        return -1;
    }

    ctx->bdev_base_addr = base_addr;

    struct LFSx_API (config) *config = &ctx->config;
    memset(config, 0, sizeof(*config));

    config->context = ctx;

    config->read = dev_read;
    config->prog = dev_prog;
    config->erase = dev_erase;
    config->sync = dev_sync;

    config->read_size = LFS_READ_SIZE;
    config->prog_size = LFS_PROG_SIZE;
    config->block_size = block_size;
    config->block_count = byte_len / block_size;

    #if MBOOT_VFS_LFS1
    config->lookahead = LFS_LOOKAHEAD_SIZE;
    config->read_buffer = lfs_read_buffer;
    config->prog_buffer = lfs_prog_buffer;
    config->lookahead_buffer = lfs_lookahead_buffer;
    #else
    config->block_cycles = 100;
    config->cache_size = LFS_CACHE_SIZE;
    config->lookahead_size = LFS_LOOKAHEAD_SIZE;
    config->read_buffer = lfs_read_buffer;
    config->prog_buffer = lfs_prog_buffer;
    config->lookahead_buffer = lfs_lookahead_buffer;
    #endif

    int ret = LFSx_API(mount)(&ctx->lfs, &ctx->config);
    if (ret < 0) {
        return -1;
    }
    return 0;
}

static int vfs_lfs_stream_open(void *stream_in, const char *fname) {
    VFS_LFSx_CONTEXT_T *ctx = stream_in;
    memset(&ctx->file, 0, sizeof(ctx->file));
    memset(&ctx->filecfg, 0, sizeof(ctx->filecfg));
    ctx->filecfg.buffer = &ctx->filebuf[0];
    LFSx_API(file_opencfg)(&ctx->lfs, &ctx->file, fname, LFSx_MACRO(_O_RDONLY), &ctx->filecfg);
    return 0;
}

static void vfs_lfs_stream_close(void *stream_in) {
    VFS_LFSx_CONTEXT_T *ctx = stream_in;
    LFSx_API(file_close)(&ctx->lfs, &ctx->file);
}

static int vfs_lfs_stream_read(void *stream_in, uint8_t *buf, size_t len) {
    VFS_LFSx_CONTEXT_T *ctx = stream_in;
    LFSx_API(ssize_t) sz = LFSx_API(file_read)(&ctx->lfs, &ctx->file, buf, len);
    if (sz < 0) {
        return -1;
    }
    return sz;
}

const stream_methods_t VFS_LFSx_STREAM_METHODS = {
    vfs_lfs_stream_open,
    vfs_lfs_stream_close,
    vfs_lfs_stream_read,
};

#endif // MBOOT_FSLOAD && (MBOOT_VFS_LFS1 || MBOOT_VFS_LFS2)
