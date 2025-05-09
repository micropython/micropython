/*
 * The little filesystem
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "lfs1.h"
#include "lfs1_util.h"

#include <inttypes.h>


/// Caching block device operations ///
static int lfs1_cache_read(lfs1_t *lfs1, lfs1_cache_t *rcache,
        const lfs1_cache_t *pcache, lfs1_block_t block,
        lfs1_off_t off, void *buffer, lfs1_size_t size) {
    uint8_t *data = buffer;
    LFS1_ASSERT(block < lfs1->cfg->block_count);

    while (size > 0) {
        if (pcache && block == pcache->block && off >= pcache->off &&
                off < pcache->off + lfs1->cfg->prog_size) {
            // is already in pcache?
            lfs1_size_t diff = lfs1_min(size,
                    lfs1->cfg->prog_size - (off-pcache->off));
            memcpy(data, &pcache->buffer[off-pcache->off], diff);

            data += diff;
            off += diff;
            size -= diff;
            continue;
        }

        if (block == rcache->block && off >= rcache->off &&
                off < rcache->off + lfs1->cfg->read_size) {
            // is already in rcache?
            lfs1_size_t diff = lfs1_min(size,
                    lfs1->cfg->read_size - (off-rcache->off));
            memcpy(data, &rcache->buffer[off-rcache->off], diff);

            data += diff;
            off += diff;
            size -= diff;
            continue;
        }

        if (off % lfs1->cfg->read_size == 0 && size >= lfs1->cfg->read_size) {
            // bypass cache?
            lfs1_size_t diff = size - (size % lfs1->cfg->read_size);
            int err = lfs1->cfg->read(lfs1->cfg, block, off, data, diff);
            if (err) {
                return err;
            }

            data += diff;
            off += diff;
            size -= diff;
            continue;
        }

        // load to cache, first condition can no longer fail
        rcache->block = block;
        rcache->off = off - (off % lfs1->cfg->read_size);
        int err = lfs1->cfg->read(lfs1->cfg, rcache->block,
                rcache->off, rcache->buffer, lfs1->cfg->read_size);
        if (err) {
            return err;
        }
    }

    return 0;
}

static int lfs1_cache_cmp(lfs1_t *lfs1, lfs1_cache_t *rcache,
        const lfs1_cache_t *pcache, lfs1_block_t block,
        lfs1_off_t off, const void *buffer, lfs1_size_t size) {
    const uint8_t *data = buffer;

    for (lfs1_off_t i = 0; i < size; i++) {
        uint8_t c;
        int err = lfs1_cache_read(lfs1, rcache, pcache,
                block, off+i, &c, 1);
        if (err) {
            return err;
        }

        if (c != data[i]) {
            return false;
        }
    }

    return true;
}

static int lfs1_cache_crc(lfs1_t *lfs1, lfs1_cache_t *rcache,
        const lfs1_cache_t *pcache, lfs1_block_t block,
        lfs1_off_t off, lfs1_size_t size, uint32_t *crc) {
    for (lfs1_off_t i = 0; i < size; i++) {
        uint8_t c;
        int err = lfs1_cache_read(lfs1, rcache, pcache,
                block, off+i, &c, 1);
        if (err) {
            return err;
        }

        lfs1_crc(crc, &c, 1);
    }

    return 0;
}

static inline void lfs1_cache_drop(lfs1_t *lfs1, lfs1_cache_t *rcache) {
    // do not zero, cheaper if cache is readonly or only going to be
    // written with identical data (during relocates)
    (void)lfs1;
    rcache->block = 0xffffffff;
}

static inline void lfs1_cache_zero(lfs1_t *lfs1, lfs1_cache_t *pcache) {
    // zero to avoid information leak
    memset(pcache->buffer, 0xff, lfs1->cfg->prog_size);
    pcache->block = 0xffffffff;
}

static int lfs1_cache_flush(lfs1_t *lfs1,
        lfs1_cache_t *pcache, lfs1_cache_t *rcache) {
    if (pcache->block != 0xffffffff) {
        int err = lfs1->cfg->prog(lfs1->cfg, pcache->block,
                pcache->off, pcache->buffer, lfs1->cfg->prog_size);
        if (err) {
            return err;
        }

        if (rcache) {
            int res = lfs1_cache_cmp(lfs1, rcache, NULL, pcache->block,
                    pcache->off, pcache->buffer, lfs1->cfg->prog_size);
            if (res < 0) {
                return res;
            }

            if (!res) {
                return LFS1_ERR_CORRUPT;
            }
        }

        lfs1_cache_zero(lfs1, pcache);
    }

    return 0;
}

static int lfs1_cache_prog(lfs1_t *lfs1, lfs1_cache_t *pcache,
        lfs1_cache_t *rcache, lfs1_block_t block,
        lfs1_off_t off, const void *buffer, lfs1_size_t size) {
    const uint8_t *data = buffer;
    LFS1_ASSERT(block < lfs1->cfg->block_count);

    while (size > 0) {
        if (block == pcache->block && off >= pcache->off &&
                off < pcache->off + lfs1->cfg->prog_size) {
            // is already in pcache?
            lfs1_size_t diff = lfs1_min(size,
                    lfs1->cfg->prog_size - (off-pcache->off));
            memcpy(&pcache->buffer[off-pcache->off], data, diff);

            data += diff;
            off += diff;
            size -= diff;

            if (off % lfs1->cfg->prog_size == 0) {
                // eagerly flush out pcache if we fill up
                int err = lfs1_cache_flush(lfs1, pcache, rcache);
                if (err) {
                    return err;
                }
            }

            continue;
        }

        // pcache must have been flushed, either by programming and
        // entire block or manually flushing the pcache
        LFS1_ASSERT(pcache->block == 0xffffffff);

        if (off % lfs1->cfg->prog_size == 0 &&
                size >= lfs1->cfg->prog_size) {
            // bypass pcache?
            lfs1_size_t diff = size - (size % lfs1->cfg->prog_size);
            int err = lfs1->cfg->prog(lfs1->cfg, block, off, data, diff);
            if (err) {
                return err;
            }

            if (rcache) {
                int res = lfs1_cache_cmp(lfs1, rcache, NULL,
                        block, off, data, diff);
                if (res < 0) {
                    return res;
                }

                if (!res) {
                    return LFS1_ERR_CORRUPT;
                }
            }

            data += diff;
            off += diff;
            size -= diff;
            continue;
        }

        // prepare pcache, first condition can no longer fail
        pcache->block = block;
        pcache->off = off - (off % lfs1->cfg->prog_size);
    }

    return 0;
}


/// General lfs1 block device operations ///
static int lfs1_bd_read(lfs1_t *lfs1, lfs1_block_t block,
        lfs1_off_t off, void *buffer, lfs1_size_t size) {
    // if we ever do more than writes to alternating pairs,
    // this may need to consider pcache
    return lfs1_cache_read(lfs1, &lfs1->rcache, NULL,
            block, off, buffer, size);
}

static int lfs1_bd_prog(lfs1_t *lfs1, lfs1_block_t block,
        lfs1_off_t off, const void *buffer, lfs1_size_t size) {
    return lfs1_cache_prog(lfs1, &lfs1->pcache, NULL,
            block, off, buffer, size);
}

static int lfs1_bd_cmp(lfs1_t *lfs1, lfs1_block_t block,
        lfs1_off_t off, const void *buffer, lfs1_size_t size) {
    return lfs1_cache_cmp(lfs1, &lfs1->rcache, NULL, block, off, buffer, size);
}

static int lfs1_bd_crc(lfs1_t *lfs1, lfs1_block_t block,
        lfs1_off_t off, lfs1_size_t size, uint32_t *crc) {
    return lfs1_cache_crc(lfs1, &lfs1->rcache, NULL, block, off, size, crc);
}

static int lfs1_bd_erase(lfs1_t *lfs1, lfs1_block_t block) {
    return lfs1->cfg->erase(lfs1->cfg, block);
}

static int lfs1_bd_sync(lfs1_t *lfs1) {
    lfs1_cache_drop(lfs1, &lfs1->rcache);

    int err = lfs1_cache_flush(lfs1, &lfs1->pcache, NULL);
    if (err) {
        return err;
    }

    return lfs1->cfg->sync(lfs1->cfg);
}


/// Internal operations predeclared here ///
int lfs1_traverse(lfs1_t *lfs1, int (*cb)(void*, lfs1_block_t), void *data);
static int lfs1_pred(lfs1_t *lfs1, const lfs1_block_t dir[2], lfs1_dir_t *pdir);
static int lfs1_parent(lfs1_t *lfs1, const lfs1_block_t dir[2],
        lfs1_dir_t *parent, lfs1_entry_t *entry);
static int lfs1_moved(lfs1_t *lfs1, const void *e);
static int lfs1_relocate(lfs1_t *lfs1,
        const lfs1_block_t oldpair[2], const lfs1_block_t newpair[2]);
int lfs1_deorphan(lfs1_t *lfs1);


/// Block allocator ///
static int lfs1_alloc_lookahead(void *p, lfs1_block_t block) {
    lfs1_t *lfs1 = p;

    lfs1_block_t off = ((block - lfs1->free.off)
            + lfs1->cfg->block_count) % lfs1->cfg->block_count;

    if (off < lfs1->free.size) {
        lfs1->free.buffer[off / 32] |= 1U << (off % 32);
    }

    return 0;
}

static int lfs1_alloc(lfs1_t *lfs1, lfs1_block_t *block) {
    while (true) {
        while (lfs1->free.i != lfs1->free.size) {
            lfs1_block_t off = lfs1->free.i;
            lfs1->free.i += 1;
            lfs1->free.ack -= 1;

            if (!(lfs1->free.buffer[off / 32] & (1U << (off % 32)))) {
                // found a free block
                *block = (lfs1->free.off + off) % lfs1->cfg->block_count;

                // eagerly find next off so an alloc ack can
                // discredit old lookahead blocks
                while (lfs1->free.i != lfs1->free.size &&
                        (lfs1->free.buffer[lfs1->free.i / 32]
                            & (1U << (lfs1->free.i % 32)))) {
                    lfs1->free.i += 1;
                    lfs1->free.ack -= 1;
                }

                return 0;
            }
        }

        // check if we have looked at all blocks since last ack
        if (lfs1->free.ack == 0) {
            LFS1_WARN("No more free space %" PRIu32,
                    lfs1->free.i + lfs1->free.off);
            return LFS1_ERR_NOSPC;
        }

        lfs1->free.off = (lfs1->free.off + lfs1->free.size)
                % lfs1->cfg->block_count;
        lfs1->free.size = lfs1_min(lfs1->cfg->lookahead, lfs1->free.ack);
        lfs1->free.i = 0;

        // find mask of free blocks from tree
        memset(lfs1->free.buffer, 0, lfs1->cfg->lookahead/8);
        int err = lfs1_traverse(lfs1, lfs1_alloc_lookahead, lfs1);
        if (err) {
            return err;
        }
    }
}

static void lfs1_alloc_ack(lfs1_t *lfs1) {
    lfs1->free.ack = lfs1->cfg->block_count;
}


/// Endian swapping functions ///
static void lfs1_dir_fromle32(struct lfs1_disk_dir *d) {
    d->rev     = lfs1_fromle32(d->rev);
    d->size    = lfs1_fromle32(d->size);
    d->tail[0] = lfs1_fromle32(d->tail[0]);
    d->tail[1] = lfs1_fromle32(d->tail[1]);
}

static void lfs1_dir_tole32(struct lfs1_disk_dir *d) {
    d->rev     = lfs1_tole32(d->rev);
    d->size    = lfs1_tole32(d->size);
    d->tail[0] = lfs1_tole32(d->tail[0]);
    d->tail[1] = lfs1_tole32(d->tail[1]);
}

static void lfs1_entry_fromle32(struct lfs1_disk_entry *d) {
    d->u.dir[0] = lfs1_fromle32(d->u.dir[0]);
    d->u.dir[1] = lfs1_fromle32(d->u.dir[1]);
}

static void lfs1_entry_tole32(struct lfs1_disk_entry *d) {
    d->u.dir[0] = lfs1_tole32(d->u.dir[0]);
    d->u.dir[1] = lfs1_tole32(d->u.dir[1]);
}

static void lfs1_superblock_fromle32(struct lfs1_disk_superblock *d) {
    d->root[0]     = lfs1_fromle32(d->root[0]);
    d->root[1]     = lfs1_fromle32(d->root[1]);
    d->block_size  = lfs1_fromle32(d->block_size);
    d->block_count = lfs1_fromle32(d->block_count);
    d->version     = lfs1_fromle32(d->version);
}

static void lfs1_superblock_tole32(struct lfs1_disk_superblock *d) {
    d->root[0]     = lfs1_tole32(d->root[0]);
    d->root[1]     = lfs1_tole32(d->root[1]);
    d->block_size  = lfs1_tole32(d->block_size);
    d->block_count = lfs1_tole32(d->block_count);
    d->version     = lfs1_tole32(d->version);
}


/// Metadata pair and directory operations ///
static inline void lfs1_pairswap(lfs1_block_t pair[2]) {
    lfs1_block_t t = pair[0];
    pair[0] = pair[1];
    pair[1] = t;
}

static inline bool lfs1_pairisnull(const lfs1_block_t pair[2]) {
    return pair[0] == 0xffffffff || pair[1] == 0xffffffff;
}

static inline int lfs1_paircmp(
        const lfs1_block_t paira[2],
        const lfs1_block_t pairb[2]) {
    return !(paira[0] == pairb[0] || paira[1] == pairb[1] ||
             paira[0] == pairb[1] || paira[1] == pairb[0]);
}

static inline bool lfs1_pairsync(
        const lfs1_block_t paira[2],
        const lfs1_block_t pairb[2]) {
    return (paira[0] == pairb[0] && paira[1] == pairb[1]) ||
           (paira[0] == pairb[1] && paira[1] == pairb[0]);
}

static inline lfs1_size_t lfs1_entry_size(const lfs1_entry_t *entry) {
    return 4 + entry->d.elen + entry->d.alen + entry->d.nlen;
}

static int lfs1_dir_alloc(lfs1_t *lfs1, lfs1_dir_t *dir) {
    // allocate pair of dir blocks
    for (int i = 0; i < 2; i++) {
        int err = lfs1_alloc(lfs1, &dir->pair[i]);
        if (err) {
            return err;
        }
    }

    // rather than clobbering one of the blocks we just pretend
    // the revision may be valid
    int err = lfs1_bd_read(lfs1, dir->pair[0], 0, &dir->d.rev, 4);
    if (err && err != LFS1_ERR_CORRUPT) {
        return err;
    }

    if (err != LFS1_ERR_CORRUPT) {
        dir->d.rev = lfs1_fromle32(dir->d.rev);
    }

    // set defaults
    dir->d.rev += 1;
    dir->d.size = sizeof(dir->d)+4;
    dir->d.tail[0] = 0xffffffff;
    dir->d.tail[1] = 0xffffffff;
    dir->off = sizeof(dir->d);

    // don't write out yet, let caller take care of that
    return 0;
}

static int lfs1_dir_fetch(lfs1_t *lfs1,
        lfs1_dir_t *dir, const lfs1_block_t pair[2]) {
    // copy out pair, otherwise may be aliasing dir
    const lfs1_block_t tpair[2] = {pair[0], pair[1]};
    bool valid = false;

    // check both blocks for the most recent revision
    for (int i = 0; i < 2; i++) {
        struct lfs1_disk_dir test;
        int err = lfs1_bd_read(lfs1, tpair[i], 0, &test, sizeof(test));
        lfs1_dir_fromle32(&test);
        if (err) {
            if (err == LFS1_ERR_CORRUPT) {
                continue;
            }
            return err;
        }

        if (valid && lfs1_scmp(test.rev, dir->d.rev) < 0) {
            continue;
        }

        if ((0x7fffffff & test.size) < sizeof(test)+4 ||
            (0x7fffffff & test.size) > lfs1->cfg->block_size) {
            continue;
        }

        uint32_t crc = 0xffffffff;
        lfs1_dir_tole32(&test);
        lfs1_crc(&crc, &test, sizeof(test));
        lfs1_dir_fromle32(&test);
        err = lfs1_bd_crc(lfs1, tpair[i], sizeof(test),
                (0x7fffffff & test.size) - sizeof(test), &crc);
        if (err) {
            if (err == LFS1_ERR_CORRUPT) {
                continue;
            }
            return err;
        }

        if (crc != 0) {
            continue;
        }

        valid = true;

        // setup dir in case it's valid
        dir->pair[0] = tpair[(i+0) % 2];
        dir->pair[1] = tpair[(i+1) % 2];
        dir->off = sizeof(dir->d);
        dir->d = test;
    }

    if (!valid) {
        LFS1_ERROR("Corrupted dir pair at %" PRIu32 " %" PRIu32 ,
                tpair[0], tpair[1]);
        return LFS1_ERR_CORRUPT;
    }

    return 0;
}

struct lfs1_region {
    lfs1_off_t oldoff;
    lfs1_size_t oldlen;
    const void *newdata;
    lfs1_size_t newlen;
};

static int lfs1_dir_commit(lfs1_t *lfs1, lfs1_dir_t *dir,
        const struct lfs1_region *regions, int count) {
    // increment revision count
    dir->d.rev += 1;

    // keep pairs in order such that pair[0] is most recent
    lfs1_pairswap(dir->pair);
    for (int i = 0; i < count; i++) {
        dir->d.size += regions[i].newlen - regions[i].oldlen;
    }

    const lfs1_block_t oldpair[2] = {dir->pair[0], dir->pair[1]};
    bool relocated = false;

    while (true) {
        if (true) {
            int err = lfs1_bd_erase(lfs1, dir->pair[0]);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            uint32_t crc = 0xffffffff;
            lfs1_dir_tole32(&dir->d);
            lfs1_crc(&crc, &dir->d, sizeof(dir->d));
            err = lfs1_bd_prog(lfs1, dir->pair[0], 0, &dir->d, sizeof(dir->d));
            lfs1_dir_fromle32(&dir->d);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            int i = 0;
            lfs1_off_t oldoff = sizeof(dir->d);
            lfs1_off_t newoff = sizeof(dir->d);
            while (newoff < (0x7fffffff & dir->d.size)-4) {
                if (i < count && regions[i].oldoff == oldoff) {
                    lfs1_crc(&crc, regions[i].newdata, regions[i].newlen);
                    err = lfs1_bd_prog(lfs1, dir->pair[0],
                            newoff, regions[i].newdata, regions[i].newlen);
                    if (err) {
                        if (err == LFS1_ERR_CORRUPT) {
                            goto relocate;
                        }
                        return err;
                    }

                    oldoff += regions[i].oldlen;
                    newoff += regions[i].newlen;
                    i += 1;
                } else {
                    uint8_t data;
                    err = lfs1_bd_read(lfs1, oldpair[1], oldoff, &data, 1);
                    if (err) {
                        return err;
                    }

                    lfs1_crc(&crc, &data, 1);
                    err = lfs1_bd_prog(lfs1, dir->pair[0], newoff, &data, 1);
                    if (err) {
                        if (err == LFS1_ERR_CORRUPT) {
                            goto relocate;
                        }
                        return err;
                    }

                    oldoff += 1;
                    newoff += 1;
                }
            }

            crc = lfs1_tole32(crc);
            err = lfs1_bd_prog(lfs1, dir->pair[0], newoff, &crc, 4);
            crc = lfs1_fromle32(crc);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            err = lfs1_bd_sync(lfs1);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // successful commit, check checksum to make sure
            uint32_t ncrc = 0xffffffff;
            err = lfs1_bd_crc(lfs1, dir->pair[0], 0,
                    (0x7fffffff & dir->d.size)-4, &ncrc);
            if (err) {
                return err;
            }

            if (ncrc != crc) {
                goto relocate;
            }
        }

        break;
relocate:
        //commit was corrupted
        LFS1_DEBUG("Bad block at %" PRIu32, dir->pair[0]);

        // drop caches and prepare to relocate block
        relocated = true;
        lfs1_cache_drop(lfs1, &lfs1->pcache);

        // can't relocate superblock, filesystem is now frozen
        if (lfs1_paircmp(oldpair, (const lfs1_block_t[2]){0, 1}) == 0) {
            LFS1_WARN("Superblock %" PRIu32 " has become unwritable",
                    oldpair[0]);
            return LFS1_ERR_CORRUPT;
        }

        // relocate half of pair
        int err = lfs1_alloc(lfs1, &dir->pair[0]);
        if (err) {
            return err;
        }
    }

    if (relocated) {
        // update references if we relocated
        LFS1_DEBUG("Relocating %" PRIu32 " %" PRIu32 " to %" PRIu32 " %" PRIu32,
                oldpair[0], oldpair[1], dir->pair[0], dir->pair[1]);
        int err = lfs1_relocate(lfs1, oldpair, dir->pair);
        if (err) {
            return err;
        }
    }

    // shift over any directories that are affected
    for (lfs1_dir_t *d = lfs1->dirs; d; d = d->next) {
        if (lfs1_paircmp(d->pair, dir->pair) == 0) {
            d->pair[0] = dir->pair[0];
            d->pair[1] = dir->pair[1];
        }
    }

    return 0;
}

static int lfs1_dir_update(lfs1_t *lfs1, lfs1_dir_t *dir,
        lfs1_entry_t *entry, const void *data) {
    lfs1_entry_tole32(&entry->d);
    int err = lfs1_dir_commit(lfs1, dir, (struct lfs1_region[]){
            {entry->off, sizeof(entry->d), &entry->d, sizeof(entry->d)},
            {entry->off+sizeof(entry->d), entry->d.nlen, data, entry->d.nlen}
        }, data ? 2 : 1);
    lfs1_entry_fromle32(&entry->d);
    return err;
}

static int lfs1_dir_append(lfs1_t *lfs1, lfs1_dir_t *dir,
        lfs1_entry_t *entry, const void *data) {
    // check if we fit, if top bit is set we do not and move on
    while (true) {
        if (dir->d.size + lfs1_entry_size(entry) <= lfs1->cfg->block_size) {
            entry->off = dir->d.size - 4;

            lfs1_entry_tole32(&entry->d);
            int err = lfs1_dir_commit(lfs1, dir, (struct lfs1_region[]){
                    {entry->off, 0, &entry->d, sizeof(entry->d)},
                    {entry->off, 0, data, entry->d.nlen}
                }, 2);
            lfs1_entry_fromle32(&entry->d);
            return err;
        }

        // we need to allocate a new dir block
        if (!(0x80000000 & dir->d.size)) {
            lfs1_dir_t olddir = *dir;
            int err = lfs1_dir_alloc(lfs1, dir);
            if (err) {
                return err;
            }

            dir->d.tail[0] = olddir.d.tail[0];
            dir->d.tail[1] = olddir.d.tail[1];
            entry->off = dir->d.size - 4;
            lfs1_entry_tole32(&entry->d);
            err = lfs1_dir_commit(lfs1, dir, (struct lfs1_region[]){
                    {entry->off, 0, &entry->d, sizeof(entry->d)},
                    {entry->off, 0, data, entry->d.nlen}
                }, 2);
            lfs1_entry_fromle32(&entry->d);
            if (err) {
                return err;
            }

            olddir.d.size |= 0x80000000;
            olddir.d.tail[0] = dir->pair[0];
            olddir.d.tail[1] = dir->pair[1];
            return lfs1_dir_commit(lfs1, &olddir, NULL, 0);
        }

        int err = lfs1_dir_fetch(lfs1, dir, dir->d.tail);
        if (err) {
            return err;
        }
    }
}

static int lfs1_dir_remove(lfs1_t *lfs1, lfs1_dir_t *dir, lfs1_entry_t *entry) {
    // check if we should just drop the directory block
    if ((dir->d.size & 0x7fffffff) == sizeof(dir->d)+4
            + lfs1_entry_size(entry)) {
        lfs1_dir_t pdir;
        int res = lfs1_pred(lfs1, dir->pair, &pdir);
        if (res < 0) {
            return res;
        }

        if (pdir.d.size & 0x80000000) {
            pdir.d.size &= dir->d.size | 0x7fffffff;
            pdir.d.tail[0] = dir->d.tail[0];
            pdir.d.tail[1] = dir->d.tail[1];
            return lfs1_dir_commit(lfs1, &pdir, NULL, 0);
        }
    }

    // shift out the entry
    int err = lfs1_dir_commit(lfs1, dir, (struct lfs1_region[]){
            {entry->off, lfs1_entry_size(entry), NULL, 0},
        }, 1);
    if (err) {
        return err;
    }

    // shift over any files/directories that are affected
    for (lfs1_file_t *f = lfs1->files; f; f = f->next) {
        if (lfs1_paircmp(f->pair, dir->pair) == 0) {
            if (f->poff == entry->off) {
                f->pair[0] = 0xffffffff;
                f->pair[1] = 0xffffffff;
            } else if (f->poff > entry->off) {
                f->poff -= lfs1_entry_size(entry);
            }
        }
    }

    for (lfs1_dir_t *d = lfs1->dirs; d; d = d->next) {
        if (lfs1_paircmp(d->pair, dir->pair) == 0) {
            if (d->off > entry->off) {
                d->off -= lfs1_entry_size(entry);
                d->pos -= lfs1_entry_size(entry);
            }
        }
    }

    return 0;
}

static int lfs1_dir_next(lfs1_t *lfs1, lfs1_dir_t *dir, lfs1_entry_t *entry) {
    while (dir->off + sizeof(entry->d) > (0x7fffffff & dir->d.size)-4) {
        if (!(0x80000000 & dir->d.size)) {
            entry->off = dir->off;
            return LFS1_ERR_NOENT;
        }

        int err = lfs1_dir_fetch(lfs1, dir, dir->d.tail);
        if (err) {
            return err;
        }

        dir->off = sizeof(dir->d);
        dir->pos += sizeof(dir->d) + 4;
    }

    int err = lfs1_bd_read(lfs1, dir->pair[0], dir->off,
            &entry->d, sizeof(entry->d));
    lfs1_entry_fromle32(&entry->d);
    if (err) {
        return err;
    }

    entry->off = dir->off;
    dir->off += lfs1_entry_size(entry);
    dir->pos += lfs1_entry_size(entry);
    return 0;
}

static int lfs1_dir_find(lfs1_t *lfs1, lfs1_dir_t *dir,
        lfs1_entry_t *entry, const char **path) {
    const char *pathname = *path;
    size_t pathlen;
    entry->d.type = LFS1_TYPE_DIR;
    entry->d.elen = sizeof(entry->d) - 4;
    entry->d.alen = 0;
    entry->d.nlen = 0;
    entry->d.u.dir[0] = lfs1->root[0];
    entry->d.u.dir[1] = lfs1->root[1];

    while (true) {
nextname:
        // skip slashes
        pathname += strspn(pathname, "/");
        pathlen = strcspn(pathname, "/");

        // skip '.' and root '..'
        if ((pathlen == 1 && memcmp(pathname, ".", 1) == 0) ||
            (pathlen == 2 && memcmp(pathname, "..", 2) == 0)) {
            pathname += pathlen;
            goto nextname;
        }

        // skip if matched by '..' in name
        const char *suffix = pathname + pathlen;
        size_t sufflen;
        int depth = 1;
        while (true) {
            suffix += strspn(suffix, "/");
            sufflen = strcspn(suffix, "/");
            if (sufflen == 0) {
                break;
            }

            if (sufflen == 2 && memcmp(suffix, "..", 2) == 0) {
                depth -= 1;
                if (depth == 0) {
                    pathname = suffix + sufflen;
                    goto nextname;
                }
            } else {
                depth += 1;
            }

            suffix += sufflen;
        }

        // found path
        if (pathname[0] == '\0') {
            return 0;
        }

        // update what we've found
        *path = pathname;

        // continue on if we hit a directory
        if (entry->d.type != LFS1_TYPE_DIR) {
            return LFS1_ERR_NOTDIR;
        }

        int err = lfs1_dir_fetch(lfs1, dir, entry->d.u.dir);
        if (err) {
            return err;
        }

        // find entry matching name
        while (true) {
            err = lfs1_dir_next(lfs1, dir, entry);
            if (err) {
                return err;
            }

            if (((0x7f & entry->d.type) != LFS1_TYPE_REG &&
                 (0x7f & entry->d.type) != LFS1_TYPE_DIR) ||
                entry->d.nlen != pathlen) {
                continue;
            }

            int res = lfs1_bd_cmp(lfs1, dir->pair[0],
                    entry->off + 4+entry->d.elen+entry->d.alen,
                    pathname, pathlen);
            if (res < 0) {
                return res;
            }

            // found match
            if (res) {
                break;
            }
        }

        // check that entry has not been moved
        if (!lfs1->moving && entry->d.type & 0x80) {
            int moved = lfs1_moved(lfs1, &entry->d.u);
            if (moved < 0 || moved) {
                return (moved < 0) ? moved : LFS1_ERR_NOENT;
            }

            entry->d.type &= ~0x80;
        }

        // to next name
        pathname += pathlen;
    }
}


/// Top level directory operations ///
int lfs1_mkdir(lfs1_t *lfs1, const char *path) {
    // deorphan if we haven't yet, needed at most once after poweron
    if (!lfs1->deorphaned) {
        int err = lfs1_deorphan(lfs1);
        if (err) {
            return err;
        }
    }

    // fetch parent directory
    lfs1_dir_t cwd;
    lfs1_entry_t entry;
    int err = lfs1_dir_find(lfs1, &cwd, &entry, &path);
    if (err != LFS1_ERR_NOENT || strchr(path, '/') != NULL) {
        return err ? err : LFS1_ERR_EXIST;
    }

    // build up new directory
    lfs1_alloc_ack(lfs1);

    lfs1_dir_t dir;
    err = lfs1_dir_alloc(lfs1, &dir);
    if (err) {
        return err;
    }
    dir.d.tail[0] = cwd.d.tail[0];
    dir.d.tail[1] = cwd.d.tail[1];

    err = lfs1_dir_commit(lfs1, &dir, NULL, 0);
    if (err) {
        return err;
    }

    entry.d.type = LFS1_TYPE_DIR;
    entry.d.elen = sizeof(entry.d) - 4;
    entry.d.alen = 0;
    entry.d.nlen = strlen(path);
    entry.d.u.dir[0] = dir.pair[0];
    entry.d.u.dir[1] = dir.pair[1];

    cwd.d.tail[0] = dir.pair[0];
    cwd.d.tail[1] = dir.pair[1];

    err = lfs1_dir_append(lfs1, &cwd, &entry, path);
    if (err) {
        return err;
    }

    lfs1_alloc_ack(lfs1);
    return 0;
}

int lfs1_dir_open(lfs1_t *lfs1, lfs1_dir_t *dir, const char *path) {
    dir->pair[0] = lfs1->root[0];
    dir->pair[1] = lfs1->root[1];

    lfs1_entry_t entry;
    int err = lfs1_dir_find(lfs1, dir, &entry, &path);
    if (err) {
        return err;
    } else if (entry.d.type != LFS1_TYPE_DIR) {
        return LFS1_ERR_NOTDIR;
    }

    err = lfs1_dir_fetch(lfs1, dir, entry.d.u.dir);
    if (err) {
        return err;
    }

    // setup head dir
    // special offset for '.' and '..'
    dir->head[0] = dir->pair[0];
    dir->head[1] = dir->pair[1];
    dir->pos = sizeof(dir->d) - 2;
    dir->off = sizeof(dir->d);

    // add to list of directories
    dir->next = lfs1->dirs;
    lfs1->dirs = dir;

    return 0;
}

int lfs1_dir_close(lfs1_t *lfs1, lfs1_dir_t *dir) {
    // remove from list of directories
    for (lfs1_dir_t **p = &lfs1->dirs; *p; p = &(*p)->next) {
        if (*p == dir) {
            *p = dir->next;
            break;
        }
    }

    return 0;
}

int lfs1_dir_read(lfs1_t *lfs1, lfs1_dir_t *dir, struct lfs1_info *info) {
    memset(info, 0, sizeof(*info));

    // special offset for '.' and '..'
    if (dir->pos == sizeof(dir->d) - 2) {
        info->type = LFS1_TYPE_DIR;
        strcpy(info->name, ".");
        dir->pos += 1;
        return 1;
    } else if (dir->pos == sizeof(dir->d) - 1) {
        info->type = LFS1_TYPE_DIR;
        strcpy(info->name, "..");
        dir->pos += 1;
        return 1;
    }

    lfs1_entry_t entry;
    while (true) {
        int err = lfs1_dir_next(lfs1, dir, &entry);
        if (err) {
            return (err == LFS1_ERR_NOENT) ? 0 : err;
        }

        if ((0x7f & entry.d.type) != LFS1_TYPE_REG &&
            (0x7f & entry.d.type) != LFS1_TYPE_DIR) {
            continue;
        }

        // check that entry has not been moved
        if (entry.d.type & 0x80) {
            int moved = lfs1_moved(lfs1, &entry.d.u);
            if (moved < 0) {
                return moved;
            }

            if (moved) {
                continue;
            }

            entry.d.type &= ~0x80;
        }

        break;
    }

    info->type = entry.d.type;
    if (info->type == LFS1_TYPE_REG) {
        info->size = entry.d.u.file.size;
    }

    int err = lfs1_bd_read(lfs1, dir->pair[0],
            entry.off + 4+entry.d.elen+entry.d.alen,
            info->name, entry.d.nlen);
    if (err) {
        return err;
    }

    return 1;
}

int lfs1_dir_seek(lfs1_t *lfs1, lfs1_dir_t *dir, lfs1_off_t off) {
    // simply walk from head dir
    int err = lfs1_dir_rewind(lfs1, dir);
    if (err) {
        return err;
    }
    dir->pos = off;

    while (off > (0x7fffffff & dir->d.size)) {
        off -= 0x7fffffff & dir->d.size;
        if (!(0x80000000 & dir->d.size)) {
            return LFS1_ERR_INVAL;
        }

        err = lfs1_dir_fetch(lfs1, dir, dir->d.tail);
        if (err) {
            return err;
        }
    }

    dir->off = off;
    return 0;
}

lfs1_soff_t lfs1_dir_tell(lfs1_t *lfs1, lfs1_dir_t *dir) {
    (void)lfs1;
    return dir->pos;
}

int lfs1_dir_rewind(lfs1_t *lfs1, lfs1_dir_t *dir) {
    // reload the head dir
    int err = lfs1_dir_fetch(lfs1, dir, dir->head);
    if (err) {
        return err;
    }

    dir->pair[0] = dir->head[0];
    dir->pair[1] = dir->head[1];
    dir->pos = sizeof(dir->d) - 2;
    dir->off = sizeof(dir->d);
    return 0;
}


/// File index list operations ///
static int lfs1_ctz_index(lfs1_t *lfs1, lfs1_off_t *off) {
    lfs1_off_t size = *off;
    lfs1_off_t b = lfs1->cfg->block_size - 2*4;
    lfs1_off_t i = size / b;
    if (i == 0) {
        return 0;
    }

    i = (size - 4*(lfs1_popc(i-1)+2)) / b;
    *off = size - b*i - 4*lfs1_popc(i);
    return i;
}

static int lfs1_ctz_find(lfs1_t *lfs1,
        lfs1_cache_t *rcache, const lfs1_cache_t *pcache,
        lfs1_block_t head, lfs1_size_t size,
        lfs1_size_t pos, lfs1_block_t *block, lfs1_off_t *off) {
    if (size == 0) {
        *block = 0xffffffff;
        *off = 0;
        return 0;
    }

    lfs1_off_t current = lfs1_ctz_index(lfs1, &(lfs1_off_t){size-1});
    lfs1_off_t target = lfs1_ctz_index(lfs1, &pos);

    while (current > target) {
        lfs1_size_t skip = lfs1_min(
                lfs1_npw2(current-target+1) - 1,
                lfs1_ctz(current));

        int err = lfs1_cache_read(lfs1, rcache, pcache, head, 4*skip, &head, 4);
        head = lfs1_fromle32(head);
        if (err) {
            return err;
        }

        LFS1_ASSERT(head >= 2 && head <= lfs1->cfg->block_count);
        current -= 1 << skip;
    }

    *block = head;
    *off = pos;
    return 0;
}

static int lfs1_ctz_extend(lfs1_t *lfs1,
        lfs1_cache_t *rcache, lfs1_cache_t *pcache,
        lfs1_block_t head, lfs1_size_t size,
        lfs1_block_t *block, lfs1_off_t *off) {
    while (true) {
        // go ahead and grab a block
        lfs1_block_t nblock;
        int err = lfs1_alloc(lfs1, &nblock);
        if (err) {
            return err;
        }
        LFS1_ASSERT(nblock >= 2 && nblock <= lfs1->cfg->block_count);

        if (true) {
            err = lfs1_bd_erase(lfs1, nblock);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            if (size == 0) {
                *block = nblock;
                *off = 0;
                return 0;
            }

            size -= 1;
            lfs1_off_t index = lfs1_ctz_index(lfs1, &size);
            size += 1;

            // just copy out the last block if it is incomplete
            if (size != lfs1->cfg->block_size) {
                for (lfs1_off_t i = 0; i < size; i++) {
                    uint8_t data;
                    err = lfs1_cache_read(lfs1, rcache, NULL,
                            head, i, &data, 1);
                    if (err) {
                        return err;
                    }

                    err = lfs1_cache_prog(lfs1, pcache, rcache,
                            nblock, i, &data, 1);
                    if (err) {
                        if (err == LFS1_ERR_CORRUPT) {
                            goto relocate;
                        }
                        return err;
                    }
                }

                *block = nblock;
                *off = size;
                return 0;
            }

            // append block
            index += 1;
            lfs1_size_t skips = lfs1_ctz(index) + 1;

            for (lfs1_off_t i = 0; i < skips; i++) {
                head = lfs1_tole32(head);
                err = lfs1_cache_prog(lfs1, pcache, rcache,
                        nblock, 4*i, &head, 4);
                head = lfs1_fromle32(head);
                if (err) {
                    if (err == LFS1_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }

                if (i != skips-1) {
                    err = lfs1_cache_read(lfs1, rcache, NULL,
                            head, 4*i, &head, 4);
                    head = lfs1_fromle32(head);
                    if (err) {
                        return err;
                    }
                }

                LFS1_ASSERT(head >= 2 && head <= lfs1->cfg->block_count);
            }

            *block = nblock;
            *off = 4*skips;
            return 0;
        }

relocate:
        LFS1_DEBUG("Bad block at %" PRIu32, nblock);

        // just clear cache and try a new block
        lfs1_cache_drop(lfs1, &lfs1->pcache);
    }
}

static int lfs1_ctz_traverse(lfs1_t *lfs1,
        lfs1_cache_t *rcache, const lfs1_cache_t *pcache,
        lfs1_block_t head, lfs1_size_t size,
        int (*cb)(void*, lfs1_block_t), void *data) {
    if (size == 0) {
        return 0;
    }

    lfs1_off_t index = lfs1_ctz_index(lfs1, &(lfs1_off_t){size-1});

    while (true) {
        int err = cb(data, head);
        if (err) {
            return err;
        }

        if (index == 0) {
            return 0;
        }

        lfs1_block_t heads[2];
        int count = 2 - (index & 1);
        err = lfs1_cache_read(lfs1, rcache, pcache, head, 0, &heads, count*4);
        heads[0] = lfs1_fromle32(heads[0]);
        heads[1] = lfs1_fromle32(heads[1]);
        if (err) {
            return err;
        }

        for (int i = 0; i < count-1; i++) {
            err = cb(data, heads[i]);
            if (err) {
                return err;
            }
        }

        head = heads[count-1];
        index -= count;
    }
}


/// Top level file operations ///
int lfs1_file_opencfg(lfs1_t *lfs1, lfs1_file_t *file,
        const char *path, int flags,
        const struct lfs1_file_config *cfg) {
    // deorphan if we haven't yet, needed at most once after poweron
    if ((flags & 3) != LFS1_O_RDONLY && !lfs1->deorphaned) {
        int err = lfs1_deorphan(lfs1);
        if (err) {
            return err;
        }
    }

    // allocate entry for file if it doesn't exist
    lfs1_dir_t cwd;
    lfs1_entry_t entry;
    int err = lfs1_dir_find(lfs1, &cwd, &entry, &path);
    if (err && (err != LFS1_ERR_NOENT || strchr(path, '/') != NULL)) {
        return err;
    }

    if (err == LFS1_ERR_NOENT) {
        if (!(flags & LFS1_O_CREAT)) {
            return LFS1_ERR_NOENT;
        }

        // create entry to remember name
        entry.d.type = LFS1_TYPE_REG;
        entry.d.elen = sizeof(entry.d) - 4;
        entry.d.alen = 0;
        entry.d.nlen = strlen(path);
        entry.d.u.file.head = 0xffffffff;
        entry.d.u.file.size = 0;
        err = lfs1_dir_append(lfs1, &cwd, &entry, path);
        if (err) {
            return err;
        }
    } else if (entry.d.type == LFS1_TYPE_DIR) {
        return LFS1_ERR_ISDIR;
    } else if (flags & LFS1_O_EXCL) {
        return LFS1_ERR_EXIST;
    }

    // setup file struct
    file->cfg = cfg;
    file->pair[0] = cwd.pair[0];
    file->pair[1] = cwd.pair[1];
    file->poff = entry.off;
    file->head = entry.d.u.file.head;
    file->size = entry.d.u.file.size;
    file->flags = flags;
    file->pos = 0;

    if (flags & LFS1_O_TRUNC) {
        if (file->size != 0) {
            file->flags |= LFS1_F_DIRTY;
        }
        file->head = 0xffffffff;
        file->size = 0;
    }

    // allocate buffer if needed
    file->cache.block = 0xffffffff;
    if (file->cfg && file->cfg->buffer) {
        file->cache.buffer = file->cfg->buffer;
    } else if (lfs1->cfg->file_buffer) {
        if (lfs1->files) {
            // already in use
            return LFS1_ERR_NOMEM;
        }
        file->cache.buffer = lfs1->cfg->file_buffer;
    } else if ((file->flags & 3) == LFS1_O_RDONLY) {
        file->cache.buffer = lfs1_malloc(lfs1->cfg->read_size);
        if (!file->cache.buffer) {
            return LFS1_ERR_NOMEM;
        }
    } else {
        file->cache.buffer = lfs1_malloc(lfs1->cfg->prog_size);
        if (!file->cache.buffer) {
            return LFS1_ERR_NOMEM;
        }
    }

    // zero to avoid information leak
    lfs1_cache_drop(lfs1, &file->cache);
    if ((file->flags & 3) != LFS1_O_RDONLY) {
        lfs1_cache_zero(lfs1, &file->cache);
    }

    // add to list of files
    file->next = lfs1->files;
    lfs1->files = file;

    return 0;
}

int lfs1_file_open(lfs1_t *lfs1, lfs1_file_t *file,
        const char *path, int flags) {
    return lfs1_file_opencfg(lfs1, file, path, flags, NULL);
}

int lfs1_file_close(lfs1_t *lfs1, lfs1_file_t *file) {
    int err = lfs1_file_sync(lfs1, file);

    // remove from list of files
    for (lfs1_file_t **p = &lfs1->files; *p; p = &(*p)->next) {
        if (*p == file) {
            *p = file->next;
            break;
        }
    }

    // clean up memory
    if (!(file->cfg && file->cfg->buffer) && !lfs1->cfg->file_buffer) {
        lfs1_free(file->cache.buffer);
    }

    return err;
}

static int lfs1_file_relocate(lfs1_t *lfs1, lfs1_file_t *file) {
relocate:
    LFS1_DEBUG("Bad block at %" PRIu32, file->block);

    // just relocate what exists into new block
    lfs1_block_t nblock;
    int err = lfs1_alloc(lfs1, &nblock);
    if (err) {
        return err;
    }

    err = lfs1_bd_erase(lfs1, nblock);
    if (err) {
        if (err == LFS1_ERR_CORRUPT) {
            goto relocate;
        }
        return err;
    }

    // either read from dirty cache or disk
    for (lfs1_off_t i = 0; i < file->off; i++) {
        uint8_t data;
        err = lfs1_cache_read(lfs1, &lfs1->rcache, &file->cache,
                file->block, i, &data, 1);
        if (err) {
            return err;
        }

        err = lfs1_cache_prog(lfs1, &lfs1->pcache, &lfs1->rcache,
                nblock, i, &data, 1);
        if (err) {
            if (err == LFS1_ERR_CORRUPT) {
                goto relocate;
            }
            return err;
        }
    }

    // copy over new state of file
    memcpy(file->cache.buffer, lfs1->pcache.buffer, lfs1->cfg->prog_size);
    file->cache.block = lfs1->pcache.block;
    file->cache.off = lfs1->pcache.off;
    lfs1_cache_zero(lfs1, &lfs1->pcache);

    file->block = nblock;
    return 0;
}

static int lfs1_file_flush(lfs1_t *lfs1, lfs1_file_t *file) {
    if (file->flags & LFS1_F_READING) {
        // just drop read cache
        lfs1_cache_drop(lfs1, &file->cache);
        file->flags &= ~LFS1_F_READING;
    }

    if (file->flags & LFS1_F_WRITING) {
        lfs1_off_t pos = file->pos;

        // copy over anything after current branch
        lfs1_file_t orig = {
            .head = file->head,
            .size = file->size,
            .flags = LFS1_O_RDONLY,
            .pos = file->pos,
            .cache = lfs1->rcache,
        };
        lfs1_cache_drop(lfs1, &lfs1->rcache);

        while (file->pos < file->size) {
            // copy over a byte at a time, leave it up to caching
            // to make this efficient
            uint8_t data;
            lfs1_ssize_t res = lfs1_file_read(lfs1, &orig, &data, 1);
            if (res < 0) {
                return res;
            }

            res = lfs1_file_write(lfs1, file, &data, 1);
            if (res < 0) {
                return res;
            }

            // keep our reference to the rcache in sync
            if (lfs1->rcache.block != 0xffffffff) {
                lfs1_cache_drop(lfs1, &orig.cache);
                lfs1_cache_drop(lfs1, &lfs1->rcache);
            }
        }

        // write out what we have
        while (true) {
            int err = lfs1_cache_flush(lfs1, &file->cache, &lfs1->rcache);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            break;
relocate:
            err = lfs1_file_relocate(lfs1, file);
            if (err) {
                return err;
            }
        }

        // actual file updates
        file->head = file->block;
        file->size = file->pos;
        file->flags &= ~LFS1_F_WRITING;
        file->flags |= LFS1_F_DIRTY;

        file->pos = pos;
    }

    return 0;
}

int lfs1_file_sync(lfs1_t *lfs1, lfs1_file_t *file) {
    int err = lfs1_file_flush(lfs1, file);
    if (err) {
        return err;
    }

    if ((file->flags & LFS1_F_DIRTY) &&
            !(file->flags & LFS1_F_ERRED) &&
            !lfs1_pairisnull(file->pair)) {
        // update dir entry
        lfs1_dir_t cwd;
        err = lfs1_dir_fetch(lfs1, &cwd, file->pair);
        if (err) {
            return err;
        }

        lfs1_entry_t entry = {.off = file->poff};
        err = lfs1_bd_read(lfs1, cwd.pair[0], entry.off,
                &entry.d, sizeof(entry.d));
        lfs1_entry_fromle32(&entry.d);
        if (err) {
            return err;
        }

        LFS1_ASSERT(entry.d.type == LFS1_TYPE_REG);
        entry.d.u.file.head = file->head;
        entry.d.u.file.size = file->size;

        err = lfs1_dir_update(lfs1, &cwd, &entry, NULL);
        if (err) {
            return err;
        }

        file->flags &= ~LFS1_F_DIRTY;
    }

    return 0;
}

lfs1_ssize_t lfs1_file_read(lfs1_t *lfs1, lfs1_file_t *file,
        void *buffer, lfs1_size_t size) {
    uint8_t *data = buffer;
    lfs1_size_t nsize = size;

    if ((file->flags & 3) == LFS1_O_WRONLY) {
        return LFS1_ERR_BADF;
    }

    if (file->flags & LFS1_F_WRITING) {
        // flush out any writes
        int err = lfs1_file_flush(lfs1, file);
        if (err) {
            return err;
        }
    }

    if (file->pos >= file->size) {
        // eof if past end
        return 0;
    }

    size = lfs1_min(size, file->size - file->pos);
    nsize = size;

    while (nsize > 0) {
        // check if we need a new block
        if (!(file->flags & LFS1_F_READING) ||
                file->off == lfs1->cfg->block_size) {
            int err = lfs1_ctz_find(lfs1, &file->cache, NULL,
                    file->head, file->size,
                    file->pos, &file->block, &file->off);
            if (err) {
                return err;
            }

            file->flags |= LFS1_F_READING;
        }

        // read as much as we can in current block
        lfs1_size_t diff = lfs1_min(nsize, lfs1->cfg->block_size - file->off);
        int err = lfs1_cache_read(lfs1, &file->cache, NULL,
                file->block, file->off, data, diff);
        if (err) {
            return err;
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;
    }

    return size;
}

lfs1_ssize_t lfs1_file_write(lfs1_t *lfs1, lfs1_file_t *file,
        const void *buffer, lfs1_size_t size) {
    const uint8_t *data = buffer;
    lfs1_size_t nsize = size;

    if ((file->flags & 3) == LFS1_O_RDONLY) {
        return LFS1_ERR_BADF;
    }

    if (file->flags & LFS1_F_READING) {
        // drop any reads
        int err = lfs1_file_flush(lfs1, file);
        if (err) {
            return err;
        }
    }

    if ((file->flags & LFS1_O_APPEND) && file->pos < file->size) {
        file->pos = file->size;
    }

    if (file->pos + size > LFS1_FILE_MAX) {
        // larger than file limit?
        return LFS1_ERR_FBIG;
    }

    if (!(file->flags & LFS1_F_WRITING) && file->pos > file->size) {
        // fill with zeros
        lfs1_off_t pos = file->pos;
        file->pos = file->size;

        while (file->pos < pos) {
            lfs1_ssize_t res = lfs1_file_write(lfs1, file, &(uint8_t){0}, 1);
            if (res < 0) {
                return res;
            }
        }
    }

    while (nsize > 0) {
        // check if we need a new block
        if (!(file->flags & LFS1_F_WRITING) ||
                file->off == lfs1->cfg->block_size) {
            if (!(file->flags & LFS1_F_WRITING) && file->pos > 0) {
                // find out which block we're extending from
                int err = lfs1_ctz_find(lfs1, &file->cache, NULL,
                        file->head, file->size,
                        file->pos-1, &file->block, &file->off);
                if (err) {
                    file->flags |= LFS1_F_ERRED;
                    return err;
                }

                // mark cache as dirty since we may have read data into it
                lfs1_cache_zero(lfs1, &file->cache);
            }

            // extend file with new blocks
            lfs1_alloc_ack(lfs1);
            int err = lfs1_ctz_extend(lfs1, &lfs1->rcache, &file->cache,
                    file->block, file->pos,
                    &file->block, &file->off);
            if (err) {
                file->flags |= LFS1_F_ERRED;
                return err;
            }

            file->flags |= LFS1_F_WRITING;
        }

        // program as much as we can in current block
        lfs1_size_t diff = lfs1_min(nsize, lfs1->cfg->block_size - file->off);
        while (true) {
            int err = lfs1_cache_prog(lfs1, &file->cache, &lfs1->rcache,
                    file->block, file->off, data, diff);
            if (err) {
                if (err == LFS1_ERR_CORRUPT) {
                    goto relocate;
                }
                file->flags |= LFS1_F_ERRED;
                return err;
            }

            break;
relocate:
            err = lfs1_file_relocate(lfs1, file);
            if (err) {
                file->flags |= LFS1_F_ERRED;
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;

        lfs1_alloc_ack(lfs1);
    }

    file->flags &= ~LFS1_F_ERRED;
    return size;
}

lfs1_soff_t lfs1_file_seek(lfs1_t *lfs1, lfs1_file_t *file,
        lfs1_soff_t off, int whence) {
    // write out everything beforehand, may be noop if rdonly
    int err = lfs1_file_flush(lfs1, file);
    if (err) {
        return err;
    }

    // find new pos
    lfs1_soff_t npos = file->pos;
    if (whence == LFS1_SEEK_SET) {
        npos = off;
    } else if (whence == LFS1_SEEK_CUR) {
        npos = file->pos + off;
    } else if (whence == LFS1_SEEK_END) {
        npos = file->size + off;
    }

    if (npos < 0 || npos > LFS1_FILE_MAX) {
        // file position out of range
        return LFS1_ERR_INVAL;
    }

    // update pos
    file->pos = npos;
    return npos;
}

int lfs1_file_truncate(lfs1_t *lfs1, lfs1_file_t *file, lfs1_off_t size) {
    if ((file->flags & 3) == LFS1_O_RDONLY) {
        return LFS1_ERR_BADF;
    }

    lfs1_off_t oldsize = lfs1_file_size(lfs1, file);
    if (size < oldsize) {
        // need to flush since directly changing metadata
        int err = lfs1_file_flush(lfs1, file);
        if (err) {
            return err;
        }

        // lookup new head in ctz skip list
        err = lfs1_ctz_find(lfs1, &file->cache, NULL,
                file->head, file->size,
                size, &file->head, &(lfs1_off_t){0});
        if (err) {
            return err;
        }

        file->size = size;
        file->flags |= LFS1_F_DIRTY;
    } else if (size > oldsize) {
        lfs1_off_t pos = file->pos;

        // flush+seek if not already at end
        if (file->pos != oldsize) {
            int err = lfs1_file_seek(lfs1, file, 0, LFS1_SEEK_END);
            if (err < 0) {
                return err;
            }
        }

        // fill with zeros
        while (file->pos < size) {
            lfs1_ssize_t res = lfs1_file_write(lfs1, file, &(uint8_t){0}, 1);
            if (res < 0) {
                return res;
            }
        }

        // restore pos
        int err = lfs1_file_seek(lfs1, file, pos, LFS1_SEEK_SET);
        if (err < 0) {
            return err;
        }
    }

    return 0;
}

lfs1_soff_t lfs1_file_tell(lfs1_t *lfs1, lfs1_file_t *file) {
    (void)lfs1;
    return file->pos;
}

int lfs1_file_rewind(lfs1_t *lfs1, lfs1_file_t *file) {
    lfs1_soff_t res = lfs1_file_seek(lfs1, file, 0, LFS1_SEEK_SET);
    if (res < 0) {
        return res;
    }

    return 0;
}

lfs1_soff_t lfs1_file_size(lfs1_t *lfs1, lfs1_file_t *file) {
    (void)lfs1;
    if (file->flags & LFS1_F_WRITING) {
        return lfs1_max(file->pos, file->size);
    } else {
        return file->size;
    }
}


/// General fs operations ///
int lfs1_stat(lfs1_t *lfs1, const char *path, struct lfs1_info *info) {
    lfs1_dir_t cwd;
    lfs1_entry_t entry;
    int err = lfs1_dir_find(lfs1, &cwd, &entry, &path);
    if (err) {
        return err;
    }

    memset(info, 0, sizeof(*info));
    info->type = entry.d.type;
    if (info->type == LFS1_TYPE_REG) {
        info->size = entry.d.u.file.size;
    }

    if (lfs1_paircmp(entry.d.u.dir, lfs1->root) == 0) {
        strcpy(info->name, "/");
    } else {
        err = lfs1_bd_read(lfs1, cwd.pair[0],
                entry.off + 4+entry.d.elen+entry.d.alen,
                info->name, entry.d.nlen);
        if (err) {
            return err;
        }
    }

    return 0;
}

int lfs1_remove(lfs1_t *lfs1, const char *path) {
    // deorphan if we haven't yet, needed at most once after poweron
    if (!lfs1->deorphaned) {
        int err = lfs1_deorphan(lfs1);
        if (err) {
            return err;
        }
    }

    lfs1_dir_t cwd;
    lfs1_entry_t entry;
    int err = lfs1_dir_find(lfs1, &cwd, &entry, &path);
    if (err) {
        return err;
    }

    lfs1_dir_t dir;
    if (entry.d.type == LFS1_TYPE_DIR) {
        // must be empty before removal, checking size
        // without masking top bit checks for any case where
        // dir is not empty
        err = lfs1_dir_fetch(lfs1, &dir, entry.d.u.dir);
        if (err) {
            return err;
        } else if (dir.d.size != sizeof(dir.d)+4) {
            return LFS1_ERR_NOTEMPTY;
        }
    }

    // remove the entry
    err = lfs1_dir_remove(lfs1, &cwd, &entry);
    if (err) {
        return err;
    }

    // if we were a directory, find pred, replace tail
    if (entry.d.type == LFS1_TYPE_DIR) {
        int res = lfs1_pred(lfs1, dir.pair, &cwd);
        if (res < 0) {
            return res;
        }

        LFS1_ASSERT(res); // must have pred
        cwd.d.tail[0] = dir.d.tail[0];
        cwd.d.tail[1] = dir.d.tail[1];

        err = lfs1_dir_commit(lfs1, &cwd, NULL, 0);
        if (err) {
            return err;
        }
    }

    return 0;
}

int lfs1_rename(lfs1_t *lfs1, const char *oldpath, const char *newpath) {
    // deorphan if we haven't yet, needed at most once after poweron
    if (!lfs1->deorphaned) {
        int err = lfs1_deorphan(lfs1);
        if (err) {
            return err;
        }
    }

    // find old entry
    lfs1_dir_t oldcwd;
    lfs1_entry_t oldentry;
    int err = lfs1_dir_find(lfs1, &oldcwd, &oldentry, &(const char *){oldpath});
    if (err) {
        return err;
    }

    // mark as moving
    oldentry.d.type |= 0x80;
    err = lfs1_dir_update(lfs1, &oldcwd, &oldentry, NULL);
    if (err) {
        return err;
    }

    // allocate new entry
    lfs1_dir_t newcwd;
    lfs1_entry_t preventry;
    err = lfs1_dir_find(lfs1, &newcwd, &preventry, &newpath);
    if (err && (err != LFS1_ERR_NOENT || strchr(newpath, '/') != NULL)) {
        return err;
    }

    // must have same type
    bool prevexists = (err != LFS1_ERR_NOENT);
    if (prevexists && preventry.d.type != (0x7f & oldentry.d.type)) {
        return LFS1_ERR_ISDIR;
    }

    lfs1_dir_t dir;
    if (prevexists && preventry.d.type == LFS1_TYPE_DIR) {
        // must be empty before removal, checking size
        // without masking top bit checks for any case where
        // dir is not empty
        err = lfs1_dir_fetch(lfs1, &dir, preventry.d.u.dir);
        if (err) {
            return err;
        } else if (dir.d.size != sizeof(dir.d)+4) {
            return LFS1_ERR_NOTEMPTY;
        }
    }

    // move to new location
    lfs1_entry_t newentry = preventry;
    newentry.d = oldentry.d;
    newentry.d.type &= ~0x80;
    newentry.d.nlen = strlen(newpath);

    if (prevexists) {
        err = lfs1_dir_update(lfs1, &newcwd, &newentry, newpath);
        if (err) {
            return err;
        }
    } else {
        err = lfs1_dir_append(lfs1, &newcwd, &newentry, newpath);
        if (err) {
            return err;
        }
    }

    // fetch old pair again in case dir block changed
    lfs1->moving = true;
    err = lfs1_dir_find(lfs1, &oldcwd, &oldentry, &oldpath);
    if (err) {
        return err;
    }
    lfs1->moving = false;

    // remove old entry
    err = lfs1_dir_remove(lfs1, &oldcwd, &oldentry);
    if (err) {
        return err;
    }

    // if we were a directory, find pred, replace tail
    if (prevexists && preventry.d.type == LFS1_TYPE_DIR) {
        int res = lfs1_pred(lfs1, dir.pair, &newcwd);
        if (res < 0) {
            return res;
        }

        LFS1_ASSERT(res); // must have pred
        newcwd.d.tail[0] = dir.d.tail[0];
        newcwd.d.tail[1] = dir.d.tail[1];

        err = lfs1_dir_commit(lfs1, &newcwd, NULL, 0);
        if (err) {
            return err;
        }
    }

    return 0;
}


/// Filesystem operations ///
static void lfs1_deinit(lfs1_t *lfs1) {
    // free allocated memory
    if (!lfs1->cfg->read_buffer) {
        lfs1_free(lfs1->rcache.buffer);
    }

    if (!lfs1->cfg->prog_buffer) {
        lfs1_free(lfs1->pcache.buffer);
    }

    if (!lfs1->cfg->lookahead_buffer) {
        lfs1_free(lfs1->free.buffer);
    }
}

static int lfs1_init(lfs1_t *lfs1, const struct lfs1_config *cfg) {
    lfs1->cfg = cfg;

    // setup read cache
    if (lfs1->cfg->read_buffer) {
        lfs1->rcache.buffer = lfs1->cfg->read_buffer;
    } else {
        lfs1->rcache.buffer = lfs1_malloc(lfs1->cfg->read_size);
        if (!lfs1->rcache.buffer) {
            goto cleanup;
        }
    }

    // setup program cache
    if (lfs1->cfg->prog_buffer) {
        lfs1->pcache.buffer = lfs1->cfg->prog_buffer;
    } else {
        lfs1->pcache.buffer = lfs1_malloc(lfs1->cfg->prog_size);
        if (!lfs1->pcache.buffer) {
            goto cleanup;
        }
    }

    // zero to avoid information leaks
    lfs1_cache_zero(lfs1, &lfs1->pcache);
    lfs1_cache_drop(lfs1, &lfs1->rcache);

    // setup lookahead, round down to nearest 32-bits
    LFS1_ASSERT(lfs1->cfg->lookahead % 32 == 0);
    LFS1_ASSERT(lfs1->cfg->lookahead > 0);
    if (lfs1->cfg->lookahead_buffer) {
        lfs1->free.buffer = lfs1->cfg->lookahead_buffer;
    } else {
        lfs1->free.buffer = lfs1_malloc(lfs1->cfg->lookahead/8);
        if (!lfs1->free.buffer) {
            goto cleanup;
        }
    }

    // check that program and read sizes are multiples of the block size
    LFS1_ASSERT(lfs1->cfg->prog_size % lfs1->cfg->read_size == 0);
    LFS1_ASSERT(lfs1->cfg->block_size % lfs1->cfg->prog_size == 0);

    // check that the block size is large enough to fit ctz pointers
    LFS1_ASSERT(4*lfs1_npw2(0xffffffff / (lfs1->cfg->block_size-2*4))
            <= lfs1->cfg->block_size);

    // setup default state
    lfs1->root[0] = 0xffffffff;
    lfs1->root[1] = 0xffffffff;
    lfs1->files = NULL;
    lfs1->dirs = NULL;
    lfs1->deorphaned = false;
    lfs1->moving = false;

    return 0;

cleanup:
    lfs1_deinit(lfs1);
    return LFS1_ERR_NOMEM;
}

int lfs1_format(lfs1_t *lfs1, const struct lfs1_config *cfg) {
    int err = 0;
    if (true) {
        err = lfs1_init(lfs1, cfg);
        if (err) {
            return err;
        }

        // create free lookahead
        memset(lfs1->free.buffer, 0, lfs1->cfg->lookahead/8);
        lfs1->free.off = 0;
        lfs1->free.size = lfs1_min(lfs1->cfg->lookahead, lfs1->cfg->block_count);
        lfs1->free.i = 0;
        lfs1_alloc_ack(lfs1);

        // create superblock dir
        lfs1_dir_t superdir;
        err = lfs1_dir_alloc(lfs1, &superdir);
        if (err) {
            goto cleanup;
        }

        // write root directory
        lfs1_dir_t root;
        err = lfs1_dir_alloc(lfs1, &root);
        if (err) {
            goto cleanup;
        }

        err = lfs1_dir_commit(lfs1, &root, NULL, 0);
        if (err) {
            goto cleanup;
        }

        lfs1->root[0] = root.pair[0];
        lfs1->root[1] = root.pair[1];

        // write superblocks
        lfs1_superblock_t superblock = {
            .off = sizeof(superdir.d),
            .d.type = LFS1_TYPE_SUPERBLOCK,
            .d.elen = sizeof(superblock.d) - sizeof(superblock.d.magic) - 4,
            .d.nlen = sizeof(superblock.d.magic),
            .d.version = LFS1_DISK_VERSION,
            .d.magic = {'l', 'i', 't', 't', 'l', 'e', 'f', 's'},
            .d.block_size  = lfs1->cfg->block_size,
            .d.block_count = lfs1->cfg->block_count,
            .d.root = {lfs1->root[0], lfs1->root[1]},
        };
        superdir.d.tail[0] = root.pair[0];
        superdir.d.tail[1] = root.pair[1];
        superdir.d.size = sizeof(superdir.d) + sizeof(superblock.d) + 4;

        // write both pairs to be safe
        lfs1_superblock_tole32(&superblock.d);
        bool valid = false;
        for (int i = 0; i < 2; i++) {
            err = lfs1_dir_commit(lfs1, &superdir, (struct lfs1_region[]){
                    {sizeof(superdir.d), sizeof(superblock.d),
                     &superblock.d, sizeof(superblock.d)}
                }, 1);
            if (err && err != LFS1_ERR_CORRUPT) {
                goto cleanup;
            }

            valid = valid || !err;
        }

        if (!valid) {
            err = LFS1_ERR_CORRUPT;
            goto cleanup;
        }

        // sanity check that fetch works
        err = lfs1_dir_fetch(lfs1, &superdir, (const lfs1_block_t[2]){0, 1});
        if (err) {
            goto cleanup;
        }

        lfs1_alloc_ack(lfs1);
    }

cleanup:
    lfs1_deinit(lfs1);
    return err;
}

int lfs1_mount(lfs1_t *lfs1, const struct lfs1_config *cfg) {
    int err = 0;
    if (true) {
        err = lfs1_init(lfs1, cfg);
        if (err) {
            return err;
        }

        // setup free lookahead
        lfs1->free.off = 0;
        lfs1->free.size = 0;
        lfs1->free.i = 0;
        lfs1_alloc_ack(lfs1);

        // load superblock
        lfs1_dir_t dir;
        lfs1_superblock_t superblock;
        err = lfs1_dir_fetch(lfs1, &dir, (const lfs1_block_t[2]){0, 1});
        if (err && err != LFS1_ERR_CORRUPT) {
            goto cleanup;
        }

        if (!err) {
            err = lfs1_bd_read(lfs1, dir.pair[0], sizeof(dir.d),
                    &superblock.d, sizeof(superblock.d));
            lfs1_superblock_fromle32(&superblock.d);
            if (err) {
                goto cleanup;
            }

            lfs1->root[0] = superblock.d.root[0];
            lfs1->root[1] = superblock.d.root[1];
        }

        if (err || memcmp(superblock.d.magic, "littlefs", 8) != 0) {
            LFS1_ERROR("Invalid superblock at %d %d", 0, 1);
            err = LFS1_ERR_CORRUPT;
            goto cleanup;
        }

        uint16_t major_version = (0xffff & (superblock.d.version >> 16));
        uint16_t minor_version = (0xffff & (superblock.d.version >>  0));
        if ((major_version != LFS1_DISK_VERSION_MAJOR ||
             minor_version > LFS1_DISK_VERSION_MINOR)) {
            LFS1_ERROR("Invalid version %d.%d", major_version, minor_version);
            err = LFS1_ERR_INVAL;
            goto cleanup;
        }

        return 0;
    }

cleanup:

    lfs1_deinit(lfs1);
    return err;
}

int lfs1_unmount(lfs1_t *lfs1) {
    lfs1_deinit(lfs1);
    return 0;
}


/// Littlefs specific operations ///
int lfs1_traverse(lfs1_t *lfs1, int (*cb)(void*, lfs1_block_t), void *data) {
    if (lfs1_pairisnull(lfs1->root)) {
        return 0;
    }

    // iterate over metadata pairs
    lfs1_dir_t dir;
    lfs1_entry_t entry;
    lfs1_block_t cwd[2] = {0, 1};

    while (true) {
        for (int i = 0; i < 2; i++) {
            int err = cb(data, cwd[i]);
            if (err) {
                return err;
            }
        }

        int err = lfs1_dir_fetch(lfs1, &dir, cwd);
        if (err) {
            return err;
        }

        // iterate over contents
        while (dir.off + sizeof(entry.d) <= (0x7fffffff & dir.d.size)-4) {
            err = lfs1_bd_read(lfs1, dir.pair[0], dir.off,
                    &entry.d, sizeof(entry.d));
            lfs1_entry_fromle32(&entry.d);
            if (err) {
                return err;
            }

            dir.off += lfs1_entry_size(&entry);
            if ((0x70 & entry.d.type) == (0x70 & LFS1_TYPE_REG)) {
                err = lfs1_ctz_traverse(lfs1, &lfs1->rcache, NULL,
                        entry.d.u.file.head, entry.d.u.file.size, cb, data);
                if (err) {
                    return err;
                }
            }
        }

        cwd[0] = dir.d.tail[0];
        cwd[1] = dir.d.tail[1];

        if (lfs1_pairisnull(cwd)) {
            break;
        }
    }

    // iterate over any open files
    for (lfs1_file_t *f = lfs1->files; f; f = f->next) {
        if (f->flags & LFS1_F_DIRTY) {
            int err = lfs1_ctz_traverse(lfs1, &lfs1->rcache, &f->cache,
                    f->head, f->size, cb, data);
            if (err) {
                return err;
            }
        }

        if (f->flags & LFS1_F_WRITING) {
            int err = lfs1_ctz_traverse(lfs1, &lfs1->rcache, &f->cache,
                    f->block, f->pos, cb, data);
            if (err) {
                return err;
            }
        }
    }

    return 0;
}

static int lfs1_pred(lfs1_t *lfs1, const lfs1_block_t dir[2], lfs1_dir_t *pdir) {
    if (lfs1_pairisnull(lfs1->root)) {
        return 0;
    }

    // iterate over all directory directory entries
    int err = lfs1_dir_fetch(lfs1, pdir, (const lfs1_block_t[2]){0, 1});
    if (err) {
        return err;
    }

    while (!lfs1_pairisnull(pdir->d.tail)) {
        if (lfs1_paircmp(pdir->d.tail, dir) == 0) {
            return true;
        }

        err = lfs1_dir_fetch(lfs1, pdir, pdir->d.tail);
        if (err) {
            return err;
        }
    }

    return false;
}

static int lfs1_parent(lfs1_t *lfs1, const lfs1_block_t dir[2],
        lfs1_dir_t *parent, lfs1_entry_t *entry) {
    if (lfs1_pairisnull(lfs1->root)) {
        return 0;
    }

    parent->d.tail[0] = 0;
    parent->d.tail[1] = 1;

    // iterate over all directory directory entries
    while (!lfs1_pairisnull(parent->d.tail)) {
        int err = lfs1_dir_fetch(lfs1, parent, parent->d.tail);
        if (err) {
            return err;
        }

        while (true) {
            err = lfs1_dir_next(lfs1, parent, entry);
            if (err && err != LFS1_ERR_NOENT) {
                return err;
            }

            if (err == LFS1_ERR_NOENT) {
                break;
            }

            if (((0x70 & entry->d.type) == (0x70 & LFS1_TYPE_DIR)) &&
                 lfs1_paircmp(entry->d.u.dir, dir) == 0) {
                return true;
            }
        }
    }

    return false;
}

static int lfs1_moved(lfs1_t *lfs1, const void *e) {
    if (lfs1_pairisnull(lfs1->root)) {
        return 0;
    }

    // skip superblock
    lfs1_dir_t cwd;
    int err = lfs1_dir_fetch(lfs1, &cwd, (const lfs1_block_t[2]){0, 1});
    if (err) {
        return err;
    }

    // iterate over all directory directory entries
    lfs1_entry_t entry;
    while (!lfs1_pairisnull(cwd.d.tail)) {
        err = lfs1_dir_fetch(lfs1, &cwd, cwd.d.tail);
        if (err) {
            return err;
        }

        while (true) {
            err = lfs1_dir_next(lfs1, &cwd, &entry);
            if (err && err != LFS1_ERR_NOENT) {
                return err;
            }

            if (err == LFS1_ERR_NOENT) {
                break;
            }

            if (!(0x80 & entry.d.type) &&
                 memcmp(&entry.d.u, e, sizeof(entry.d.u)) == 0) {
                return true;
            }
        }
    }

    return false;
}

static int lfs1_relocate(lfs1_t *lfs1,
        const lfs1_block_t oldpair[2], const lfs1_block_t newpair[2]) {
    // find parent
    lfs1_dir_t parent;
    lfs1_entry_t entry;
    int res = lfs1_parent(lfs1, oldpair, &parent, &entry);
    if (res < 0) {
        return res;
    }

    if (res) {
        // update disk, this creates a desync
        entry.d.u.dir[0] = newpair[0];
        entry.d.u.dir[1] = newpair[1];

        int err = lfs1_dir_update(lfs1, &parent, &entry, NULL);
        if (err) {
            return err;
        }

        // update internal root
        if (lfs1_paircmp(oldpair, lfs1->root) == 0) {
            LFS1_DEBUG("Relocating root %" PRIu32 " %" PRIu32,
                    newpair[0], newpair[1]);
            lfs1->root[0] = newpair[0];
            lfs1->root[1] = newpair[1];
        }

        // clean up bad block, which should now be a desync
        return lfs1_deorphan(lfs1);
    }

    // find pred
    res = lfs1_pred(lfs1, oldpair, &parent);
    if (res < 0) {
        return res;
    }

    if (res) {
        // just replace bad pair, no desync can occur
        parent.d.tail[0] = newpair[0];
        parent.d.tail[1] = newpair[1];

        return lfs1_dir_commit(lfs1, &parent, NULL, 0);
    }

    // couldn't find dir, must be new
    return 0;
}

int lfs1_deorphan(lfs1_t *lfs1) {
    lfs1->deorphaned = true;

    if (lfs1_pairisnull(lfs1->root)) {
        return 0;
    }

    lfs1_dir_t pdir = {.d.size = 0x80000000};
    lfs1_dir_t cwd = {.d.tail[0] = 0, .d.tail[1] = 1};

    // iterate over all directory directory entries
    for (lfs1_size_t i = 0; i < lfs1->cfg->block_count; i++) {
        if (lfs1_pairisnull(cwd.d.tail)) {
            return 0;
        }

        int err = lfs1_dir_fetch(lfs1, &cwd, cwd.d.tail);
        if (err) {
            return err;
        }

        // check head blocks for orphans
        if (!(0x80000000 & pdir.d.size)) {
            // check if we have a parent
            lfs1_dir_t parent;
            lfs1_entry_t entry;
            int res = lfs1_parent(lfs1, pdir.d.tail, &parent, &entry);
            if (res < 0) {
                return res;
            }

            if (!res) {
                // we are an orphan
                LFS1_DEBUG("Found orphan %" PRIu32 " %" PRIu32,
                        pdir.d.tail[0], pdir.d.tail[1]);

                pdir.d.tail[0] = cwd.d.tail[0];
                pdir.d.tail[1] = cwd.d.tail[1];

                err = lfs1_dir_commit(lfs1, &pdir, NULL, 0);
                if (err) {
                    return err;
                }

                return 0;
            }

            if (!lfs1_pairsync(entry.d.u.dir, pdir.d.tail)) {
                // we have desynced
                LFS1_DEBUG("Found desync %" PRIu32 " %" PRIu32,
                        entry.d.u.dir[0], entry.d.u.dir[1]);

                pdir.d.tail[0] = entry.d.u.dir[0];
                pdir.d.tail[1] = entry.d.u.dir[1];

                err = lfs1_dir_commit(lfs1, &pdir, NULL, 0);
                if (err) {
                    return err;
                }

                return 0;
            }
        }

        // check entries for moves
        lfs1_entry_t entry;
        while (true) {
            err = lfs1_dir_next(lfs1, &cwd, &entry);
            if (err && err != LFS1_ERR_NOENT) {
                return err;
            }

            if (err == LFS1_ERR_NOENT) {
                break;
            }

            // found moved entry
            if (entry.d.type & 0x80) {
                int moved = lfs1_moved(lfs1, &entry.d.u);
                if (moved < 0) {
                    return moved;
                }

                if (moved) {
                    LFS1_DEBUG("Found move %" PRIu32 " %" PRIu32,
                            entry.d.u.dir[0], entry.d.u.dir[1]);
                    err = lfs1_dir_remove(lfs1, &cwd, &entry);
                    if (err) {
                        return err;
                    }
                } else {
                    LFS1_DEBUG("Found partial move %" PRIu32 " %" PRIu32,
                            entry.d.u.dir[0], entry.d.u.dir[1]);
                    entry.d.type &= ~0x80;
                    err = lfs1_dir_update(lfs1, &cwd, &entry, NULL);
                    if (err) {
                        return err;
                    }
                }
            }
        }

        memcpy(&pdir, &cwd, sizeof(pdir));
    }

    // If we reached here, we have more directory pairs than blocks in the
    // filesystem... So something must be horribly wrong
    return LFS1_ERR_CORRUPT;
}
