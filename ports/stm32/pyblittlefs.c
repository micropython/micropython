/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Damien P. George
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

#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/vfs_littlefs.h"
#include "drivers/memory/spiflash.h"
#include "storage.h"

#if MICROPY_VFS && MICROPY_VFS_LITTLEFS && MICROPY_HW_ENABLE_NATIVE_LFS

// This is the mp_spiflash_t object that is used for the storage
#define SPIFLASH &spi_bdev.spiflash

// SPI flash size
#ifdef MICROPY_HW_SPIFLASH_SIZE_BITS
#define SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS/8)
#else
#define SPIFLASH_SIZE_BYTES (1024 * 1024)
#endif

// This is the starting block within SPIFLASH to start the filesystem
#ifdef MICROPY_VFS_LITTLEFS_START_OFFSET_BYTES
#define START_BLOCK (MICROPY_VFS_LITTLEFS_START_OFFSET_BYTES / MP_SPIFLASH_ERASE_BLOCK_SIZE)
#else
#define START_BLOCK 0
#endif

// This is the number of blocks within SPIFLASH for the lfs filesystem
#define NUM_BLOCKS ((SPIFLASH_SIZE_BYTES / MP_SPIFLASH_ERASE_BLOCK_SIZE)-START_BLOCK)

STATIC int dev_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
    mp_spiflash_t *spiflash = c->context;

    block += START_BLOCK;

    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
    mp_spiflash_read(spiflash, block * MP_SPIFLASH_ERASE_BLOCK_SIZE + off, size, buffer);
    restore_irq_pri(basepri);

    return 0;
}

STATIC int dev_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
    mp_spiflash_t *spiflash = c->context;

    block += START_BLOCK;

    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
    mp_spiflash_write(spiflash, block * MP_SPIFLASH_ERASE_BLOCK_SIZE + off, size, buffer);
    restore_irq_pri(basepri);

    return 0;
}

STATIC int dev_erase(const struct lfs_config *c, lfs_block_t block) {
    mp_spiflash_t *spiflash = c->context;

    block += START_BLOCK;

    // we must disable USB irqs to prevent MSC contention with SPI flash
    uint32_t basepri = raise_irq_pri(IRQ_PRI_OTG_FS);
    mp_spiflash_erase_block(spiflash, block * MP_SPIFLASH_ERASE_BLOCK_SIZE);
    restore_irq_pri(basepri);

    return 0;
}

// Sync the state of the underlying block device. Negative error codes
// are propogated to the user.
STATIC int dev_sync(const struct lfs_config *c) {
    (void)c;
    return 0;
}

#if (LFS_VERSION >= 0x00020000)
static uint8_t __attribute__ ((aligned (64))) lookahead_buffer[128/8];
#endif

STATIC void init_config(struct lfs_config *config) {
    config->context = SPIFLASH;

    config->read = dev_read;
    config->prog = dev_prog;
    config->erase = dev_erase;
    config->sync = dev_sync;

    config->read_size = 128;
    config->prog_size = 128;
    config->block_size = MP_SPIFLASH_ERASE_BLOCK_SIZE;
    config->block_count = NUM_BLOCKS;
    config->read_buffer = m_new(uint8_t, config->read_size);
    config->prog_buffer = m_new(uint8_t, config->prog_size);
#if (LFS_VERSION >= 0x00020000)
    config->block_cycles = 100;
    config->cache_size = 128;
//   static uint8_t __attribute__ ((aligned (64))) lookahead_buffer[128/8];
//   config->lookahead_buffer = lookahead_buffer;
    config->lookahead_size = 128;
    config->lookahead_buffer = lookahead_buffer;

#else
    config->lookahead = 128;
    config->lookahead_buffer = m_new(uint8_t, config->lookahead / 8);
#endif
}

int pyb_littlefs_mount(const char * mount) {
    // create vfs object
    mp_obj_vfs_littlefs_t *lfs = m_new_obj_maybe(mp_obj_vfs_littlefs_t);
    mp_vfs_mount_t *vfs = m_new_obj_maybe(mp_vfs_mount_t);
    if (lfs == NULL || vfs == NULL) {
        return -MP_ENOMEM;
    }

    lfs->base.type = &mp_type_vfs_littlefs;
    vstr_init(&lfs->cur_dir, 16);
    init_config(&lfs->config);

    int ret = lfs_mount(&lfs->lfs, &lfs->config);
    if (ret == LFS_ERR_CORRUPT) {
        // couldn't mount, format it and try to mount again
        ret = lfs_format(&lfs->lfs, &lfs->config);
        ret = lfs_mount(&lfs->lfs, &lfs->config);
    }

    if (ret < 0) {
        // fail
        m_del_obj(mp_obj_vfs_littlefs_t, lfs);
        m_del_obj(mp_vfs_mount_t, vfs);
        return ret;
    }

    // mounted via littlefs, now mount in the VFS
    if (mount != NULL) {
        vfs->str = mount;
        vfs->len = strlen(mount);
    } else {
        vfs->str = "/lfs";
        vfs->len = 4;
    }
    
    vfs->obj = MP_OBJ_FROM_PTR(lfs);
    vfs->next = NULL;
    for (mp_vfs_mount_t **m = &MP_STATE_VM(vfs_mount_table);; m = &(*m)->next) {
        if (*m == NULL) {
            *m = vfs;
            break;
        }
    }

    return 0;
}

#endif // MICROPY_VFS && MICROPY_VFS_LITTLEFS && MICROPY_HW_ENABLE_NATIVE_LFS
