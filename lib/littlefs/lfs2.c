/*
 * The little filesystem
 *
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "lfs2.h"
#include "lfs2_util.h"

#define LFS2_BLOCK_NULL ((lfs2_block_t)-1)
#define LFS2_BLOCK_INLINE ((lfs2_block_t)-2)

/// Caching block device operations ///
static inline void lfs2_cache_drop(lfs2_t *lfs2, lfs2_cache_t *rcache) {
    // do not zero, cheaper if cache is readonly or only going to be
    // written with identical data (during relocates)
    (void)lfs2;
    rcache->block = LFS2_BLOCK_NULL;
}

static inline void lfs2_cache_zero(lfs2_t *lfs2, lfs2_cache_t *pcache) {
    // zero to avoid information leak
    memset(pcache->buffer, 0xff, lfs2->cfg->cache_size);
    pcache->block = LFS2_BLOCK_NULL;
}

static int lfs2_bd_read(lfs2_t *lfs2,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache, lfs2_size_t hint,
        lfs2_block_t block, lfs2_off_t off,
        void *buffer, lfs2_size_t size) {
    uint8_t *data = buffer;
    LFS2_ASSERT(block != LFS2_BLOCK_NULL);
    if (off+size > lfs2->cfg->block_size) {
        return LFS2_ERR_CORRUPT;
    }

    while (size > 0) {
        lfs2_size_t diff = size;

        if (pcache && block == pcache->block &&
                off < pcache->off + pcache->size) {
            if (off >= pcache->off) {
                // is already in pcache?
                diff = lfs2_min(diff, pcache->size - (off-pcache->off));
                memcpy(data, &pcache->buffer[off-pcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // pcache takes priority
            diff = lfs2_min(diff, pcache->off-off);
        }

        if (block == rcache->block &&
                off < rcache->off + rcache->size) {
            if (off >= rcache->off) {
                // is already in rcache?
                diff = lfs2_min(diff, rcache->size - (off-rcache->off));
                memcpy(data, &rcache->buffer[off-rcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // rcache takes priority
            diff = lfs2_min(diff, rcache->off-off);
        }

        // load to cache, first condition can no longer fail
        LFS2_ASSERT(block < lfs2->cfg->block_count);
        rcache->block = block;
        rcache->off = lfs2_aligndown(off, lfs2->cfg->read_size);
        rcache->size = lfs2_min(
                lfs2_min(
                    lfs2_alignup(off+hint, lfs2->cfg->read_size),
                    lfs2->cfg->block_size)
                - rcache->off,
                lfs2->cfg->cache_size);
        int err = lfs2->cfg->read(lfs2->cfg, rcache->block,
                rcache->off, rcache->buffer, rcache->size);
        LFS2_ASSERT(err <= 0);
        if (err) {
            return err;
        }
    }

    return 0;
}

enum {
    LFS2_CMP_EQ = 0,
    LFS2_CMP_LT = 1,
    LFS2_CMP_GT = 2,
};

static int lfs2_bd_cmp(lfs2_t *lfs2,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache, lfs2_size_t hint,
        lfs2_block_t block, lfs2_off_t off,
        const void *buffer, lfs2_size_t size) {
    const uint8_t *data = buffer;

    for (lfs2_off_t i = 0; i < size; i++) {
        uint8_t dat;
        int err = lfs2_bd_read(lfs2,
                pcache, rcache, hint-i,
                block, off+i, &dat, 1);
        if (err) {
            return err;
        }

        if (dat != data[i]) {
            return (dat < data[i]) ? LFS2_CMP_LT : LFS2_CMP_GT;
        }
    }

    return LFS2_CMP_EQ;
}

static int lfs2_bd_flush(lfs2_t *lfs2,
        lfs2_cache_t *pcache, lfs2_cache_t *rcache, bool validate) {
    if (pcache->block != LFS2_BLOCK_NULL && pcache->block != LFS2_BLOCK_INLINE) {
        LFS2_ASSERT(pcache->block < lfs2->cfg->block_count);
        lfs2_size_t diff = lfs2_alignup(pcache->size, lfs2->cfg->prog_size);
        int err = lfs2->cfg->prog(lfs2->cfg, pcache->block,
                pcache->off, pcache->buffer, diff);
        LFS2_ASSERT(err <= 0);
        if (err) {
            return err;
        }

        if (validate) {
            // check data on disk
            lfs2_cache_drop(lfs2, rcache);
            int res = lfs2_bd_cmp(lfs2,
                    NULL, rcache, diff,
                    pcache->block, pcache->off, pcache->buffer, diff);
            if (res < 0) {
                return res;
            }

            if (res != LFS2_CMP_EQ) {
                return LFS2_ERR_CORRUPT;
            }
        }

        lfs2_cache_zero(lfs2, pcache);
    }

    return 0;
}

static int lfs2_bd_sync(lfs2_t *lfs2,
        lfs2_cache_t *pcache, lfs2_cache_t *rcache, bool validate) {
    lfs2_cache_drop(lfs2, rcache);

    int err = lfs2_bd_flush(lfs2, pcache, rcache, validate);
    if (err) {
        return err;
    }

    err = lfs2->cfg->sync(lfs2->cfg);
    LFS2_ASSERT(err <= 0);
    return err;
}

static int lfs2_bd_prog(lfs2_t *lfs2,
        lfs2_cache_t *pcache, lfs2_cache_t *rcache, bool validate,
        lfs2_block_t block, lfs2_off_t off,
        const void *buffer, lfs2_size_t size) {
    const uint8_t *data = buffer;
    LFS2_ASSERT(block != LFS2_BLOCK_NULL);
    LFS2_ASSERT(off + size <= lfs2->cfg->block_size);

    while (size > 0) {
        if (block == pcache->block &&
                off >= pcache->off &&
                off < pcache->off + lfs2->cfg->cache_size) {
            // already fits in pcache?
            lfs2_size_t diff = lfs2_min(size,
                    lfs2->cfg->cache_size - (off-pcache->off));
            memcpy(&pcache->buffer[off-pcache->off], data, diff);

            data += diff;
            off += diff;
            size -= diff;

            pcache->size = lfs2_max(pcache->size, off - pcache->off);
            if (pcache->size == lfs2->cfg->cache_size) {
                // eagerly flush out pcache if we fill up
                int err = lfs2_bd_flush(lfs2, pcache, rcache, validate);
                if (err) {
                    return err;
                }
            }

            continue;
        }

        // pcache must have been flushed, either by programming and
        // entire block or manually flushing the pcache
        LFS2_ASSERT(pcache->block == LFS2_BLOCK_NULL);

        // prepare pcache, first condition can no longer fail
        pcache->block = block;
        pcache->off = lfs2_aligndown(off, lfs2->cfg->prog_size);
        pcache->size = 0;
    }

    return 0;
}

static int lfs2_bd_erase(lfs2_t *lfs2, lfs2_block_t block) {
    LFS2_ASSERT(block < lfs2->cfg->block_count);
    int err = lfs2->cfg->erase(lfs2->cfg, block);
    LFS2_ASSERT(err <= 0);
    return err;
}


/// Small type-level utilities ///
// operations on block pairs
static inline void lfs2_pair_swap(lfs2_block_t pair[2]) {
    lfs2_block_t t = pair[0];
    pair[0] = pair[1];
    pair[1] = t;
}

static inline bool lfs2_pair_isnull(const lfs2_block_t pair[2]) {
    return pair[0] == LFS2_BLOCK_NULL || pair[1] == LFS2_BLOCK_NULL;
}

static inline int lfs2_pair_cmp(
        const lfs2_block_t paira[2],
        const lfs2_block_t pairb[2]) {
    return !(paira[0] == pairb[0] || paira[1] == pairb[1] ||
             paira[0] == pairb[1] || paira[1] == pairb[0]);
}

static inline bool lfs2_pair_sync(
        const lfs2_block_t paira[2],
        const lfs2_block_t pairb[2]) {
    return (paira[0] == pairb[0] && paira[1] == pairb[1]) ||
           (paira[0] == pairb[1] && paira[1] == pairb[0]);
}

static inline void lfs2_pair_fromle32(lfs2_block_t pair[2]) {
    pair[0] = lfs2_fromle32(pair[0]);
    pair[1] = lfs2_fromle32(pair[1]);
}

static inline void lfs2_pair_tole32(lfs2_block_t pair[2]) {
    pair[0] = lfs2_tole32(pair[0]);
    pair[1] = lfs2_tole32(pair[1]);
}

// operations on 32-bit entry tags
typedef uint32_t lfs2_tag_t;
typedef int32_t lfs2_stag_t;

#define LFS2_MKTAG(type, id, size) \
    (((lfs2_tag_t)(type) << 20) | ((lfs2_tag_t)(id) << 10) | (lfs2_tag_t)(size))

static inline bool lfs2_tag_isvalid(lfs2_tag_t tag) {
    return !(tag & 0x80000000);
}

static inline bool lfs2_tag_isdelete(lfs2_tag_t tag) {
    return ((int32_t)(tag << 22) >> 22) == -1;
}

static inline uint16_t lfs2_tag_type1(lfs2_tag_t tag) {
    return (tag & 0x70000000) >> 20;
}

static inline uint16_t lfs2_tag_type3(lfs2_tag_t tag) {
    return (tag & 0x7ff00000) >> 20;
}

static inline uint8_t lfs2_tag_chunk(lfs2_tag_t tag) {
    return (tag & 0x0ff00000) >> 20;
}

static inline int8_t lfs2_tag_splice(lfs2_tag_t tag) {
    return (int8_t)lfs2_tag_chunk(tag);
}

static inline uint16_t lfs2_tag_id(lfs2_tag_t tag) {
    return (tag & 0x000ffc00) >> 10;
}

static inline lfs2_size_t lfs2_tag_size(lfs2_tag_t tag) {
    return tag & 0x000003ff;
}

static inline lfs2_size_t lfs2_tag_dsize(lfs2_tag_t tag) {
    return sizeof(tag) + lfs2_tag_size(tag + lfs2_tag_isdelete(tag));
}

// operations on attributes in attribute lists
struct lfs2_mattr {
    lfs2_tag_t tag;
    const void *buffer;
};

struct lfs2_diskoff {
    lfs2_block_t block;
    lfs2_off_t off;
};

#define LFS2_MKATTRS(...) \
    (struct lfs2_mattr[]){__VA_ARGS__}, \
    sizeof((struct lfs2_mattr[]){__VA_ARGS__}) / sizeof(struct lfs2_mattr)

// operations on global state
static inline void lfs2_gstate_xor(struct lfs2_gstate *a,
        const struct lfs2_gstate *b) {
    for (int i = 0; i < 3; i++) {
        ((uint32_t*)a)[i] ^= ((const uint32_t*)b)[i];
    }
}

static inline bool lfs2_gstate_iszero(const struct lfs2_gstate *a) {
    for (int i = 0; i < 3; i++) {
        if (((uint32_t*)a)[i] != 0) {
            return false;
        }
    }
    return true;
}

static inline bool lfs2_gstate_hasorphans(const struct lfs2_gstate *a) {
    return lfs2_tag_size(a->tag);
}

static inline uint8_t lfs2_gstate_getorphans(const struct lfs2_gstate *a) {
    return lfs2_tag_size(a->tag);
}

static inline bool lfs2_gstate_hasmove(const struct lfs2_gstate *a) {
    return lfs2_tag_type1(a->tag);
}

static inline bool lfs2_gstate_hasmovehere(const struct lfs2_gstate *a,
        const lfs2_block_t *pair) {
    return lfs2_tag_type1(a->tag) && lfs2_pair_cmp(a->pair, pair) == 0;
}

static inline void lfs2_gstate_xororphans(struct lfs2_gstate *a,
        const struct lfs2_gstate *b, bool orphans) {
    a->tag ^= LFS2_MKTAG(0x800, 0, 0) & (b->tag ^ ((uint32_t)orphans << 31));
}

static inline void lfs2_gstate_xormove(struct lfs2_gstate *a,
        const struct lfs2_gstate *b, uint16_t id, const lfs2_block_t pair[2]) {
    a->tag ^= LFS2_MKTAG(0x7ff, 0x3ff, 0) & (b->tag ^ (
            (id != 0x3ff) ? LFS2_MKTAG(LFS2_TYPE_DELETE, id, 0) : 0));
    a->pair[0] ^= b->pair[0] ^ ((id != 0x3ff) ? pair[0] : 0);
    a->pair[1] ^= b->pair[1] ^ ((id != 0x3ff) ? pair[1] : 0);
}

static inline void lfs2_gstate_fromle32(struct lfs2_gstate *a) {
    a->tag     = lfs2_fromle32(a->tag);
    a->pair[0] = lfs2_fromle32(a->pair[0]);
    a->pair[1] = lfs2_fromle32(a->pair[1]);
}

static inline void lfs2_gstate_tole32(struct lfs2_gstate *a) {
    a->tag     = lfs2_tole32(a->tag);
    a->pair[0] = lfs2_tole32(a->pair[0]);
    a->pair[1] = lfs2_tole32(a->pair[1]);
}

// other endianness operations
static void lfs2_ctz_fromle32(struct lfs2_ctz *ctz) {
    ctz->head = lfs2_fromle32(ctz->head);
    ctz->size = lfs2_fromle32(ctz->size);
}

static void lfs2_ctz_tole32(struct lfs2_ctz *ctz) {
    ctz->head = lfs2_tole32(ctz->head);
    ctz->size = lfs2_tole32(ctz->size);
}

static inline void lfs2_superblock_fromle32(lfs2_superblock_t *superblock) {
    superblock->version     = lfs2_fromle32(superblock->version);
    superblock->block_size  = lfs2_fromle32(superblock->block_size);
    superblock->block_count = lfs2_fromle32(superblock->block_count);
    superblock->name_max    = lfs2_fromle32(superblock->name_max);
    superblock->file_max    = lfs2_fromle32(superblock->file_max);
    superblock->attr_max    = lfs2_fromle32(superblock->attr_max);
}

static inline void lfs2_superblock_tole32(lfs2_superblock_t *superblock) {
    superblock->version     = lfs2_tole32(superblock->version);
    superblock->block_size  = lfs2_tole32(superblock->block_size);
    superblock->block_count = lfs2_tole32(superblock->block_count);
    superblock->name_max    = lfs2_tole32(superblock->name_max);
    superblock->file_max    = lfs2_tole32(superblock->file_max);
    superblock->attr_max    = lfs2_tole32(superblock->attr_max);
}


/// Internal operations predeclared here ///
static int lfs2_dir_commit(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const struct lfs2_mattr *attrs, int attrcount);
static int lfs2_dir_compact(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t begin, uint16_t end);
static int lfs2_file_outline(lfs2_t *lfs2, lfs2_file_t *file);
static int lfs2_file_flush(lfs2_t *lfs2, lfs2_file_t *file);
static void lfs2_fs_preporphans(lfs2_t *lfs2, int8_t orphans);
static void lfs2_fs_prepmove(lfs2_t *lfs2,
        uint16_t id, const lfs2_block_t pair[2]);
static int lfs2_fs_pred(lfs2_t *lfs2, const lfs2_block_t dir[2],
        lfs2_mdir_t *pdir);
static lfs2_stag_t lfs2_fs_parent(lfs2_t *lfs2, const lfs2_block_t dir[2],
        lfs2_mdir_t *parent);
static int lfs2_fs_relocate(lfs2_t *lfs2,
        const lfs2_block_t oldpair[2], lfs2_block_t newpair[2]);
static int lfs2_fs_forceconsistency(lfs2_t *lfs2);
static int lfs2_deinit(lfs2_t *lfs2);
#ifdef LFS2_MIGRATE
static int lfs21_traverse(lfs2_t *lfs2,
        int (*cb)(void*, lfs2_block_t), void *data);
#endif

/// Block allocator ///
static int lfs2_alloc_lookahead(void *p, lfs2_block_t block) {
    lfs2_t *lfs2 = (lfs2_t*)p;
    lfs2_block_t off = ((block - lfs2->free.off)
            + lfs2->cfg->block_count) % lfs2->cfg->block_count;

    if (off < lfs2->free.size) {
        lfs2->free.buffer[off / 32] |= 1U << (off % 32);
    }

    return 0;
}

static int lfs2_alloc(lfs2_t *lfs2, lfs2_block_t *block) {
    while (true) {
        while (lfs2->free.i != lfs2->free.size) {
            lfs2_block_t off = lfs2->free.i;
            lfs2->free.i += 1;
            lfs2->free.ack -= 1;

            if (!(lfs2->free.buffer[off / 32] & (1U << (off % 32)))) {
                // found a free block
                *block = (lfs2->free.off + off) % lfs2->cfg->block_count;

                // eagerly find next off so an alloc ack can
                // discredit old lookahead blocks
                while (lfs2->free.i != lfs2->free.size &&
                        (lfs2->free.buffer[lfs2->free.i / 32]
                            & (1U << (lfs2->free.i % 32)))) {
                    lfs2->free.i += 1;
                    lfs2->free.ack -= 1;
                }

                return 0;
            }
        }

        // check if we have looked at all blocks since last ack
        if (lfs2->free.ack == 0) {
            LFS2_ERROR("No more free space %"PRIu32,
                    lfs2->free.i + lfs2->free.off);
            return LFS2_ERR_NOSPC;
        }

        lfs2->free.off = (lfs2->free.off + lfs2->free.size)
                % lfs2->cfg->block_count;
        lfs2->free.size = lfs2_min(8*lfs2->cfg->lookahead_size, lfs2->free.ack);
        lfs2->free.i = 0;

        // find mask of free blocks from tree
        memset(lfs2->free.buffer, 0, lfs2->cfg->lookahead_size);
        int err = lfs2_fs_traverse(lfs2, lfs2_alloc_lookahead, lfs2);
        if (err) {
            return err;
        }
    }
}

static void lfs2_alloc_ack(lfs2_t *lfs2) {
    lfs2->free.ack = lfs2->cfg->block_count;
}


/// Metadata pair and directory operations ///
static lfs2_stag_t lfs2_dir_getslice(lfs2_t *lfs2, const lfs2_mdir_t *dir,
        lfs2_tag_t gmask, lfs2_tag_t gtag,
        lfs2_off_t goff, void *gbuffer, lfs2_size_t gsize) {
    lfs2_off_t off = dir->off;
    lfs2_tag_t ntag = dir->etag;
    lfs2_stag_t gdiff = 0;

    if (lfs2_gstate_hasmovehere(&lfs2->gstate, dir->pair) &&
            lfs2_tag_id(gtag) <= lfs2_tag_id(lfs2->gstate.tag)) {
        // synthetic moves
        gdiff -= LFS2_MKTAG(0, 1, 0);
    }

    // iterate over dir block backwards (for faster lookups)
    while (off >= sizeof(lfs2_tag_t) + lfs2_tag_dsize(ntag)) {
        off -= lfs2_tag_dsize(ntag);
        lfs2_tag_t tag = ntag;
        int err = lfs2_bd_read(lfs2,
                NULL, &lfs2->rcache, sizeof(ntag),
                dir->pair[0], off, &ntag, sizeof(ntag));
        if (err) {
            return err;
        }

        ntag = (lfs2_frombe32(ntag) ^ tag) & 0x7fffffff;

        if (lfs2_tag_id(gmask) != 0 &&
                lfs2_tag_type1(tag) == LFS2_TYPE_SPLICE &&
                lfs2_tag_id(tag) <= lfs2_tag_id(gtag - gdiff)) {
            if (tag == (LFS2_MKTAG(LFS2_TYPE_CREATE, 0, 0) |
                    (LFS2_MKTAG(0, 0x3ff, 0) & (gtag - gdiff)))) {
                // found where we were created
                return LFS2_ERR_NOENT;
            }

            // move around splices
            gdiff += LFS2_MKTAG(0, lfs2_tag_splice(tag), 0);
        }

        if ((gmask & tag) == (gmask & (gtag - gdiff))) {
            if (lfs2_tag_isdelete(tag)) {
                return LFS2_ERR_NOENT;
            }

            lfs2_size_t diff = lfs2_min(lfs2_tag_size(tag), gsize);
            err = lfs2_bd_read(lfs2,
                    NULL, &lfs2->rcache, diff,
                    dir->pair[0], off+sizeof(tag)+goff, gbuffer, diff);
            if (err) {
                return err;
            }

            memset((uint8_t*)gbuffer + diff, 0, gsize - diff);

            return tag + gdiff;
        }
    }

    return LFS2_ERR_NOENT;
}

static lfs2_stag_t lfs2_dir_get(lfs2_t *lfs2, const lfs2_mdir_t *dir,
        lfs2_tag_t gmask, lfs2_tag_t gtag, void *buffer) {
    return lfs2_dir_getslice(lfs2, dir,
            gmask, gtag,
            0, buffer, lfs2_tag_size(gtag));
}

static int lfs2_dir_getread(lfs2_t *lfs2, const lfs2_mdir_t *dir,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache, lfs2_size_t hint,
        lfs2_tag_t gmask, lfs2_tag_t gtag,
        lfs2_off_t off, void *buffer, lfs2_size_t size) {
    uint8_t *data = buffer;
    if (off+size > lfs2->cfg->block_size) {
        return LFS2_ERR_CORRUPT;
    }

    while (size > 0) {
        lfs2_size_t diff = size;

        if (pcache && pcache->block == LFS2_BLOCK_INLINE &&
                off < pcache->off + pcache->size) {
            if (off >= pcache->off) {
                // is already in pcache?
                diff = lfs2_min(diff, pcache->size - (off-pcache->off));
                memcpy(data, &pcache->buffer[off-pcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // pcache takes priority
            diff = lfs2_min(diff, pcache->off-off);
        }

        if (rcache->block == LFS2_BLOCK_INLINE &&
                off < rcache->off + rcache->size) {
            if (off >= rcache->off) {
                // is already in rcache?
                diff = lfs2_min(diff, rcache->size - (off-rcache->off));
                memcpy(data, &rcache->buffer[off-rcache->off], diff);

                data += diff;
                off += diff;
                size -= diff;
                continue;
            }

            // rcache takes priority
            diff = lfs2_min(diff, rcache->off-off);
        }

        // load to cache, first condition can no longer fail
        rcache->block = LFS2_BLOCK_INLINE;
        rcache->off = lfs2_aligndown(off, lfs2->cfg->read_size);
        rcache->size = lfs2_min(lfs2_alignup(off+hint, lfs2->cfg->read_size),
                lfs2->cfg->cache_size);
        int err = lfs2_dir_getslice(lfs2, dir, gmask, gtag,
                rcache->off, rcache->buffer, rcache->size);
        if (err < 0) {
            return err;
        }
    }

    return 0;
}

static int lfs2_dir_traverse_filter(void *p,
        lfs2_tag_t tag, const void *buffer) {
    lfs2_tag_t *filtertag = p;
    (void)buffer;

    // which mask depends on unique bit in tag structure
    uint32_t mask = (tag & LFS2_MKTAG(0x100, 0, 0))
            ? LFS2_MKTAG(0x7ff, 0x3ff, 0)
            : LFS2_MKTAG(0x700, 0x3ff, 0);

    // check for redundancy
    if ((mask & tag) == (mask & *filtertag) ||
            lfs2_tag_isdelete(*filtertag) ||
            (LFS2_MKTAG(0x7ff, 0x3ff, 0) & tag) == (
                LFS2_MKTAG(LFS2_TYPE_DELETE, 0, 0) |
                    (LFS2_MKTAG(0, 0x3ff, 0) & *filtertag))) {
        return true;
    }

    // check if we need to adjust for created/deleted tags
    if (lfs2_tag_type1(tag) == LFS2_TYPE_SPLICE &&
            lfs2_tag_id(tag) <= lfs2_tag_id(*filtertag)) {
        *filtertag += LFS2_MKTAG(0, lfs2_tag_splice(tag), 0);
    }

    return false;
}

static int lfs2_dir_traverse(lfs2_t *lfs2,
        const lfs2_mdir_t *dir, lfs2_off_t off, lfs2_tag_t ptag,
        const struct lfs2_mattr *attrs, int attrcount, bool hasseenmove,
        lfs2_tag_t tmask, lfs2_tag_t ttag,
        uint16_t begin, uint16_t end, int16_t diff,
        int (*cb)(void *data, lfs2_tag_t tag, const void *buffer), void *data) {
    // iterate over directory and attrs
    while (true) {
        lfs2_tag_t tag;
        const void *buffer;
        struct lfs2_diskoff disk;
        if (off+lfs2_tag_dsize(ptag) < dir->off) {
            off += lfs2_tag_dsize(ptag);
            int err = lfs2_bd_read(lfs2,
                    NULL, &lfs2->rcache, sizeof(tag),
                    dir->pair[0], off, &tag, sizeof(tag));
            if (err) {
                return err;
            }

            tag = (lfs2_frombe32(tag) ^ ptag) | 0x80000000;
            disk.block = dir->pair[0];
            disk.off = off+sizeof(lfs2_tag_t);
            buffer = &disk;
            ptag = tag;
        } else if (attrcount > 0) {
            tag = attrs[0].tag;
            buffer = attrs[0].buffer;
            attrs += 1;
            attrcount -= 1;
        } else if (!hasseenmove &&
                lfs2_gstate_hasmovehere(&lfs2->gpending, dir->pair)) {
            // Wait, we have pending move? Handle this here (we need to
            // or else we risk letting moves fall out of date)
            tag = lfs2->gpending.tag & LFS2_MKTAG(0x7ff, 0x3ff, 0);
            buffer = NULL;
            hasseenmove = true;
        } else {
            return 0;
        }

        lfs2_tag_t mask = LFS2_MKTAG(0x7ff, 0, 0);
        if ((mask & tmask & tag) != (mask & tmask & ttag)) {
            continue;
        }

        // do we need to filter? inlining the filtering logic here allows
        // for some minor optimizations
        if (lfs2_tag_id(tmask) != 0) {
            // scan for duplicates and update tag based on creates/deletes
            int filter = lfs2_dir_traverse(lfs2,
                    dir, off, ptag, attrs, attrcount, hasseenmove,
                    0, 0, 0, 0, 0,
                    lfs2_dir_traverse_filter, &tag);
            if (filter < 0) {
                return filter;
            }

            if (filter) {
                continue;
            }

            // in filter range?
            if (!(lfs2_tag_id(tag) >= begin && lfs2_tag_id(tag) < end)) {
                continue;
            }
        }

        // handle special cases for mcu-side operations
        if (lfs2_tag_type3(tag) == LFS2_FROM_NOOP) {
            // do nothing
        } else if (lfs2_tag_type3(tag) == LFS2_FROM_MOVE) {
            uint16_t fromid = lfs2_tag_size(tag);
            uint16_t toid = lfs2_tag_id(tag);
            int err = lfs2_dir_traverse(lfs2,
                    buffer, 0, LFS2_BLOCK_NULL, NULL, 0, true,
                    LFS2_MKTAG(0x600, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_STRUCT, 0, 0),
                    fromid, fromid+1, toid-fromid+diff,
                    cb, data);
            if (err) {
                return err;
            }
        } else if (lfs2_tag_type3(tag) == LFS2_FROM_USERATTRS) {
            for (unsigned i = 0; i < lfs2_tag_size(tag); i++) {
                const struct lfs2_attr *a = buffer;
                int err = cb(data, LFS2_MKTAG(LFS2_TYPE_USERATTR + a[i].type,
                        lfs2_tag_id(tag) + diff, a[i].size), a[i].buffer);
                if (err) {
                    return err;
                }
            }
        } else {
            int err = cb(data, tag + LFS2_MKTAG(0, diff, 0), buffer);
            if (err) {
                return err;
            }
        }
    }
}

static lfs2_stag_t lfs2_dir_fetchmatch(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const lfs2_block_t pair[2],
        lfs2_tag_t fmask, lfs2_tag_t ftag, uint16_t *id,
        int (*cb)(void *data, lfs2_tag_t tag, const void *buffer), void *data) {
    // we can find tag very efficiently during a fetch, since we're already
    // scanning the entire directory
    lfs2_stag_t besttag = -1;

    // find the block with the most recent revision
    uint32_t revs[2] = {0, 0};
    int r = 0;
    for (int i = 0; i < 2; i++) {
        int err = lfs2_bd_read(lfs2,
                NULL, &lfs2->rcache, sizeof(revs[i]),
                pair[i], 0, &revs[i], sizeof(revs[i]));
        revs[i] = lfs2_fromle32(revs[i]);
        if (err && err != LFS2_ERR_CORRUPT) {
            return err;
        }

        if (err != LFS2_ERR_CORRUPT &&
                lfs2_scmp(revs[i], revs[(i+1)%2]) > 0) {
            r = i;
        }
    }

    dir->pair[0] = pair[(r+0)%2];
    dir->pair[1] = pair[(r+1)%2];
    dir->rev = revs[(r+0)%2];
    dir->off = 0; // nonzero = found some commits

    // now scan tags to fetch the actual dir and find possible match
    for (int i = 0; i < 2; i++) {
        lfs2_off_t off = 0;
        lfs2_tag_t ptag = LFS2_BLOCK_NULL;

        uint16_t tempcount = 0;
        lfs2_block_t temptail[2] = {LFS2_BLOCK_NULL, LFS2_BLOCK_NULL};
        bool tempsplit = false;
        lfs2_stag_t tempbesttag = besttag;

        dir->rev = lfs2_tole32(dir->rev);
        uint32_t crc = lfs2_crc(LFS2_BLOCK_NULL, &dir->rev, sizeof(dir->rev));
        dir->rev = lfs2_fromle32(dir->rev);

        while (true) {
            // extract next tag
            lfs2_tag_t tag;
            off += lfs2_tag_dsize(ptag);
            int err = lfs2_bd_read(lfs2,
                    NULL, &lfs2->rcache, lfs2->cfg->block_size,
                    dir->pair[0], off, &tag, sizeof(tag));
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    // can't continue?
                    dir->erased = false;
                    break;
                }
                return err;
            }

            crc = lfs2_crc(crc, &tag, sizeof(tag));
            tag = lfs2_frombe32(tag) ^ ptag;

            // next commit not yet programmed or we're not in valid range
            if (!lfs2_tag_isvalid(tag) ||
                    off + lfs2_tag_dsize(tag) > lfs2->cfg->block_size) {
                dir->erased = (lfs2_tag_type1(ptag) == LFS2_TYPE_CRC &&
                        dir->off % lfs2->cfg->prog_size == 0);
                break;
            }

            ptag = tag;

            if (lfs2_tag_type1(tag) == LFS2_TYPE_CRC) {
                // check the crc attr
                uint32_t dcrc;
                err = lfs2_bd_read(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->block_size,
                        dir->pair[0], off+sizeof(tag), &dcrc, sizeof(dcrc));
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                    return err;
                }
                dcrc = lfs2_fromle32(dcrc);

                if (crc != dcrc) {
                    dir->erased = false;
                    break;
                }

                // reset the next bit if we need to
                ptag ^= (lfs2_tag_t)(lfs2_tag_chunk(tag) & 1U) << 31;

                // toss our crc into the filesystem seed for
                // pseudorandom numbers
                lfs2->seed ^= crc;

                // update with what's found so far
                besttag = tempbesttag;
                dir->off = off + lfs2_tag_dsize(tag);
                dir->etag = ptag;
                dir->count = tempcount;
                dir->tail[0] = temptail[0];
                dir->tail[1] = temptail[1];
                dir->split = tempsplit;

                // reset crc
                crc = LFS2_BLOCK_NULL;
                continue;
            }

            // crc the entry first, hopefully leaving it in the cache
            for (lfs2_off_t j = sizeof(tag); j < lfs2_tag_dsize(tag); j++) {
                uint8_t dat;
                err = lfs2_bd_read(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->block_size,
                        dir->pair[0], off+j, &dat, 1);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                    return err;
                }

                crc = lfs2_crc(crc, &dat, 1);
            }

            // directory modification tags?
            if (lfs2_tag_type1(tag) == LFS2_TYPE_NAME) {
                // increase count of files if necessary
                if (lfs2_tag_id(tag) >= tempcount) {
                    tempcount = lfs2_tag_id(tag) + 1;
                }
            } else if (lfs2_tag_type1(tag) == LFS2_TYPE_SPLICE) {
                tempcount += lfs2_tag_splice(tag);

                if (tag == (LFS2_MKTAG(LFS2_TYPE_DELETE, 0, 0) |
                        (LFS2_MKTAG(0, 0x3ff, 0) & tempbesttag))) {
                    tempbesttag |= 0x80000000;
                } else if (tempbesttag != -1 &&
                        lfs2_tag_id(tag) <= lfs2_tag_id(tempbesttag)) {
                    tempbesttag += LFS2_MKTAG(0, lfs2_tag_splice(tag), 0);
                }
            } else if (lfs2_tag_type1(tag) == LFS2_TYPE_TAIL) {
                tempsplit = (lfs2_tag_chunk(tag) & 1);

                err = lfs2_bd_read(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->block_size,
                        dir->pair[0], off+sizeof(tag), &temptail, 8);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                }
                lfs2_pair_fromle32(temptail);
            }

            // found a match for our fetcher?
            if ((fmask & tag) == (fmask & ftag)) {
                int res = cb(data, tag, &(struct lfs2_diskoff){
                        dir->pair[0], off+sizeof(tag)});
                if (res < 0) {
                    if (res == LFS2_ERR_CORRUPT) {
                        dir->erased = false;
                        break;
                    }
                    return res;
                }

                if (res == LFS2_CMP_EQ) {
                    // found a match
                    tempbesttag = tag;
                } else if (res == LFS2_CMP_GT &&
                        lfs2_tag_id(tag) <= lfs2_tag_id(tempbesttag)) {
                    // found a greater match, keep track to keep things sorted
                    tempbesttag = tag | 0x80000000;
                }
            }
        }

        // consider what we have good enough
        if (dir->off > 0) {
            // synthetic move
            if (lfs2_gstate_hasmovehere(&lfs2->gstate, dir->pair)) {
                if (lfs2_tag_id(lfs2->gstate.tag) == lfs2_tag_id(besttag)) {
                    besttag |= 0x80000000;
                } else if (besttag != -1 &&
                        lfs2_tag_id(lfs2->gstate.tag) < lfs2_tag_id(besttag)) {
                    besttag -= LFS2_MKTAG(0, 1, 0);
                }
            }

            // found tag? or found best id?
            if (id) {
                *id = lfs2_min(lfs2_tag_id(besttag), dir->count);
            }

            if (lfs2_tag_isvalid(besttag)) {
                return besttag;
            } else if (lfs2_tag_id(besttag) < dir->count) {
                return LFS2_ERR_NOENT;
            } else {
                return 0;
            }
        }

        // failed, try the other block?
        lfs2_pair_swap(dir->pair);
        dir->rev = revs[(r+1)%2];
    }

    LFS2_ERROR("Corrupted dir pair at %"PRIx32" %"PRIx32,
            dir->pair[0], dir->pair[1]);
    return LFS2_ERR_CORRUPT;
}

static int lfs2_dir_fetch(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const lfs2_block_t pair[2]) {
    // note, mask=-1, tag=0 can never match a tag since this
    // pattern has the invalid bit set
    return (int)lfs2_dir_fetchmatch(lfs2, dir, pair, -1, 0, NULL, NULL, NULL);
}

static int lfs2_dir_getgstate(lfs2_t *lfs2, const lfs2_mdir_t *dir,
        struct lfs2_gstate *gstate) {
    struct lfs2_gstate temp;
    lfs2_stag_t res = lfs2_dir_get(lfs2, dir, LFS2_MKTAG(0x7ff, 0, 0),
            LFS2_MKTAG(LFS2_TYPE_MOVESTATE, 0, sizeof(temp)), &temp);
    if (res < 0 && res != LFS2_ERR_NOENT) {
        return res;
    }

    if (res != LFS2_ERR_NOENT) {
        // xor together to find resulting gstate
        lfs2_gstate_fromle32(&temp);
        lfs2_gstate_xor(gstate, &temp);
    }

    return 0;
}

static int lfs2_dir_getinfo(lfs2_t *lfs2, lfs2_mdir_t *dir,
        uint16_t id, struct lfs2_info *info) {
    if (id == 0x3ff) {
        // special case for root
        strcpy(info->name, "/");
        info->type = LFS2_TYPE_DIR;
        return 0;
    }

    lfs2_stag_t tag = lfs2_dir_get(lfs2, dir, LFS2_MKTAG(0x780, 0x3ff, 0),
            LFS2_MKTAG(LFS2_TYPE_NAME, id, lfs2->name_max+1), info->name);
    if (tag < 0) {
        return (int)tag;
    }

    info->type = lfs2_tag_type3(tag);

    struct lfs2_ctz ctz;
    tag = lfs2_dir_get(lfs2, dir, LFS2_MKTAG(0x700, 0x3ff, 0),
            LFS2_MKTAG(LFS2_TYPE_STRUCT, id, sizeof(ctz)), &ctz);
    if (tag < 0) {
        return (int)tag;
    }
    lfs2_ctz_fromle32(&ctz);

    if (lfs2_tag_type3(tag) == LFS2_TYPE_CTZSTRUCT) {
        info->size = ctz.size;
    } else if (lfs2_tag_type3(tag) == LFS2_TYPE_INLINESTRUCT) {
        info->size = lfs2_tag_size(tag);
    }

    return 0;
}

struct lfs2_dir_find_match {
    lfs2_t *lfs2;
    const void *name;
    lfs2_size_t size;
};

static int lfs2_dir_find_match(void *data,
        lfs2_tag_t tag, const void *buffer) {
    struct lfs2_dir_find_match *name = data;
    lfs2_t *lfs2 = name->lfs2;
    const struct lfs2_diskoff *disk = buffer;

    // compare with disk
    lfs2_size_t diff = lfs2_min(name->size, lfs2_tag_size(tag));
    int res = lfs2_bd_cmp(lfs2,
            NULL, &lfs2->rcache, diff,
            disk->block, disk->off, name->name, diff);
    if (res != LFS2_CMP_EQ) {
        return res;
    }

    // only equal if our size is still the same
    if (name->size != lfs2_tag_size(tag)) {
        return (name->size < lfs2_tag_size(tag)) ? LFS2_CMP_LT : LFS2_CMP_GT;
    }

    // found a match!
    return LFS2_CMP_EQ;
}

static lfs2_stag_t lfs2_dir_find(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const char **path, uint16_t *id) {
    // we reduce path to a single name if we can find it
    const char *name = *path;
    if (id) {
        *id = 0x3ff;
    }

    // default to root dir
    lfs2_stag_t tag = LFS2_MKTAG(LFS2_TYPE_DIR, 0x3ff, 0);
    dir->tail[0] = lfs2->root[0];
    dir->tail[1] = lfs2->root[1];

    while (true) {
nextname:
        // skip slashes
        name += strspn(name, "/");
        lfs2_size_t namelen = strcspn(name, "/");

        // skip '.' and root '..'
        if ((namelen == 1 && memcmp(name, ".", 1) == 0) ||
            (namelen == 2 && memcmp(name, "..", 2) == 0)) {
            name += namelen;
            goto nextname;
        }

        // skip if matched by '..' in name
        const char *suffix = name + namelen;
        lfs2_size_t sufflen;
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
                    name = suffix + sufflen;
                    goto nextname;
                }
            } else {
                depth += 1;
            }

            suffix += sufflen;
        }

        // found path
        if (name[0] == '\0') {
            return tag;
        }

        // update what we've found so far
        *path = name;

        // only continue if we hit a directory
        if (lfs2_tag_type3(tag) != LFS2_TYPE_DIR) {
            return LFS2_ERR_NOTDIR;
        }

        // grab the entry data
        if (lfs2_tag_id(tag) != 0x3ff) {
            lfs2_stag_t res = lfs2_dir_get(lfs2, dir, LFS2_MKTAG(0x700, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_STRUCT, lfs2_tag_id(tag), 8), dir->tail);
            if (res < 0) {
                return res;
            }
            lfs2_pair_fromle32(dir->tail);
        }

        // find entry matching name
        while (true) {
            tag = lfs2_dir_fetchmatch(lfs2, dir, dir->tail,
                    LFS2_MKTAG(0x780, 0, 0),
                    LFS2_MKTAG(LFS2_TYPE_NAME, 0, namelen),
                     // are we last name?
                    (strchr(name, '/') == NULL) ? id : NULL,
                    lfs2_dir_find_match, &(struct lfs2_dir_find_match){
                        lfs2, name, namelen});
            if (tag < 0) {
                return tag;
            }

            if (tag) {
                break;
            }

            if (!dir->split) {
                return LFS2_ERR_NOENT;
            }
        }

        // to next name
        name += namelen;
    }
}

// commit logic
struct lfs2_commit {
    lfs2_block_t block;
    lfs2_off_t off;
    lfs2_tag_t ptag;
    uint32_t crc;

    lfs2_off_t begin;
    lfs2_off_t end;
};

static int lfs2_dir_commitprog(lfs2_t *lfs2, struct lfs2_commit *commit,
        const void *buffer, lfs2_size_t size) {
    int err = lfs2_bd_prog(lfs2,
            &lfs2->pcache, &lfs2->rcache, false,
            commit->block, commit->off ,
            (const uint8_t*)buffer, size);
    if (err) {
        return err;
    }

    commit->crc = lfs2_crc(commit->crc, buffer, size);
    commit->off += size;
    return 0;
}

static int lfs2_dir_commitattr(lfs2_t *lfs2, struct lfs2_commit *commit,
        lfs2_tag_t tag, const void *buffer) {
    // check if we fit
    lfs2_size_t dsize = lfs2_tag_dsize(tag);
    if (commit->off + dsize > commit->end) {
        return LFS2_ERR_NOSPC;
    }

    // write out tag
    lfs2_tag_t ntag = lfs2_tobe32((tag & 0x7fffffff) ^ commit->ptag);
    int err = lfs2_dir_commitprog(lfs2, commit, &ntag, sizeof(ntag));
    if (err) {
        return err;
    }

    if (!(tag & 0x80000000)) {
        // from memory
        err = lfs2_dir_commitprog(lfs2, commit, buffer, dsize-sizeof(tag));
        if (err) {
            return err;
        }
    } else {
        // from disk
        const struct lfs2_diskoff *disk = buffer;
        for (lfs2_off_t i = 0; i < dsize-sizeof(tag); i++) {
            // rely on caching to make this efficient
            uint8_t dat;
            err = lfs2_bd_read(lfs2,
                    NULL, &lfs2->rcache, dsize-sizeof(tag)-i,
                    disk->block, disk->off+i, &dat, 1);
            if (err) {
                return err;
            }

            err = lfs2_dir_commitprog(lfs2, commit, &dat, 1);
            if (err) {
                return err;
            }
        }
    }

    commit->ptag = tag & 0x7fffffff;
    return 0;
}

static int lfs2_dir_commitcrc(lfs2_t *lfs2, struct lfs2_commit *commit) {
    // align to program units
    const lfs2_off_t off1 = commit->off + sizeof(lfs2_tag_t);
    const lfs2_off_t end = lfs2_alignup(off1 + sizeof(uint32_t),
            lfs2->cfg->prog_size);

    // create crc tags to fill up remainder of commit, note that
    // padding is not crcd, which lets fetches skip padding but
    // makes committing a bit more complicated
    while (commit->off < end) {
        lfs2_off_t off = commit->off + sizeof(lfs2_tag_t);
        lfs2_off_t noff = lfs2_min(end - off, 0x3fe) + off;
        if (noff < end) {
            noff = lfs2_min(noff, end - 2*sizeof(uint32_t));
        }

        // read erased state from next program unit
        lfs2_tag_t tag = LFS2_BLOCK_NULL;
        int err = lfs2_bd_read(lfs2,
                NULL, &lfs2->rcache, sizeof(tag),
                commit->block, noff, &tag, sizeof(tag));
        if (err && err != LFS2_ERR_CORRUPT) {
            return err;
        }

        // build crc tag
        bool reset = ~lfs2_frombe32(tag) >> 31;
        tag = LFS2_MKTAG(LFS2_TYPE_CRC + reset, 0x3ff, noff - off);

        // write out crc
        uint32_t footer[2];
        footer[0] = lfs2_tobe32(tag ^ commit->ptag);
        commit->crc = lfs2_crc(commit->crc, &footer[0], sizeof(footer[0]));
        footer[1] = lfs2_tole32(commit->crc);
        err = lfs2_bd_prog(lfs2,
                &lfs2->pcache, &lfs2->rcache, false,
                commit->block, commit->off, &footer, sizeof(footer));
        if (err) {
            return err;
        }

        commit->off += sizeof(tag)+lfs2_tag_size(tag);
        commit->ptag = tag ^ ((lfs2_tag_t)reset << 31);
        commit->crc = LFS2_BLOCK_NULL; // reset crc for next "commit"
    }

    // flush buffers
    int err = lfs2_bd_sync(lfs2, &lfs2->pcache, &lfs2->rcache, false);
    if (err) {
        return err;
    }

    // successful commit, check checksums to make sure
    lfs2_off_t off = commit->begin;
    lfs2_off_t noff = off1;
    while (off < end) {
        uint32_t crc = LFS2_BLOCK_NULL;
        for (lfs2_off_t i = off; i < noff+sizeof(uint32_t); i++) {
            // leave it up to caching to make this efficient
            uint8_t dat;
            err = lfs2_bd_read(lfs2,
                    NULL, &lfs2->rcache, noff+sizeof(uint32_t)-i,
                    commit->block, i, &dat, 1);
            if (err) {
                return err;
            }

            crc = lfs2_crc(crc, &dat, 1);
        }

        // detected write error?
        if (crc != 0) {
            return LFS2_ERR_CORRUPT;
        }

        // skip padding
        off = lfs2_min(end - noff, 0x3fe) + noff;
        if (off < end) {
            off = lfs2_min(off, end - 2*sizeof(uint32_t));
        }
        noff = off + sizeof(uint32_t);
    }

    return 0;
}

static int lfs2_dir_alloc(lfs2_t *lfs2, lfs2_mdir_t *dir) {
    // allocate pair of dir blocks (backwards, so we write block 1 first)
    for (int i = 0; i < 2; i++) {
        int err = lfs2_alloc(lfs2, &dir->pair[(i+1)%2]);
        if (err) {
            return err;
        }
    }

    // rather than clobbering one of the blocks we just pretend
    // the revision may be valid
    int err = lfs2_bd_read(lfs2,
            NULL, &lfs2->rcache, sizeof(dir->rev),
            dir->pair[0], 0, &dir->rev, sizeof(dir->rev));
    dir->rev = lfs2_fromle32(dir->rev);
    if (err && err != LFS2_ERR_CORRUPT) {
        return err;
    }

    // make sure we don't immediately evict
    dir->rev += dir->rev & 1;

    // set defaults
    dir->off = sizeof(dir->rev);
    dir->etag = LFS2_BLOCK_NULL;
    dir->count = 0;
    dir->tail[0] = LFS2_BLOCK_NULL;
    dir->tail[1] = LFS2_BLOCK_NULL;
    dir->erased = false;
    dir->split = false;

    // don't write out yet, let caller take care of that
    return 0;
}

static int lfs2_dir_drop(lfs2_t *lfs2, lfs2_mdir_t *dir, lfs2_mdir_t *tail) {
    // steal state
    int err = lfs2_dir_getgstate(lfs2, tail, &lfs2->gdelta);
    if (err) {
        return err;
    }

    // steal tail
    lfs2_pair_tole32(tail->tail);
    err = lfs2_dir_commit(lfs2, dir, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_TAIL + tail->split, 0x3ff, 8), tail->tail}));
    lfs2_pair_fromle32(tail->tail);
    if (err) {
        return err;
    }

    return 0;
}

static int lfs2_dir_split(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t split, uint16_t end) {
    // create tail directory
    lfs2_mdir_t tail;
    int err = lfs2_dir_alloc(lfs2, &tail);
    if (err) {
        return err;
    }

    tail.split = dir->split;
    tail.tail[0] = dir->tail[0];
    tail.tail[1] = dir->tail[1];

    err = lfs2_dir_compact(lfs2, &tail, attrs, attrcount, source, split, end);
    if (err) {
        return err;
    }

    dir->tail[0] = tail.pair[0];
    dir->tail[1] = tail.pair[1];
    dir->split = true;

    // update root if needed
    if (lfs2_pair_cmp(dir->pair, lfs2->root) == 0 && split == 0) {
        lfs2->root[0] = tail.pair[0];
        lfs2->root[1] = tail.pair[1];
    }

    return 0;
}

static int lfs2_dir_commit_size(void *p, lfs2_tag_t tag, const void *buffer) {
    lfs2_size_t *size = p;
    (void)buffer;

    *size += lfs2_tag_dsize(tag);
    return 0;
}

struct lfs2_dir_commit_commit {
    lfs2_t *lfs2;
    struct lfs2_commit *commit;
};

static int lfs2_dir_commit_commit(void *p, lfs2_tag_t tag, const void *buffer) {
    struct lfs2_dir_commit_commit *commit = p;
    return lfs2_dir_commitattr(commit->lfs2, commit->commit, tag, buffer);
}

static int lfs2_dir_compact(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t begin, uint16_t end) {
    // save some state in case block is bad
    const lfs2_block_t oldpair[2] = {dir->pair[1], dir->pair[0]};
    bool relocated = false;
    bool exhausted = false;

    // should we split?
    while (end - begin > 1) {
        // find size
        lfs2_size_t size = 0;
        int err = lfs2_dir_traverse(lfs2,
                source, 0, LFS2_BLOCK_NULL, attrs, attrcount, false,
                LFS2_MKTAG(0x400, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_NAME, 0, 0),
                begin, end, -begin,
                lfs2_dir_commit_size, &size);
        if (err) {
            return err;
        }

        // space is complicated, we need room for tail, crc, gstate,
        // cleanup delete, and we cap at half a block to give room
        // for metadata updates.
        if (end - begin < 0xff &&
                size <= lfs2_min(lfs2->cfg->block_size - 36,
                    lfs2_alignup(lfs2->cfg->block_size/2,
                        lfs2->cfg->prog_size))) {
            break;
        }

        // can't fit, need to split, we should really be finding the
        // largest size that fits with a small binary search, but right now
        // it's not worth the code size
        uint16_t split = (end - begin) / 2;
        err = lfs2_dir_split(lfs2, dir, attrs, attrcount,
                source, begin+split, end);
        if (err) {
            // if we fail to split, we may be able to overcompact, unless
            // we're too big for even the full block, in which case our
            // only option is to error
            if (err == LFS2_ERR_NOSPC && size <= lfs2->cfg->block_size - 36) {
                break;
            }
            return err;
        }

        end = begin + split;
    }

    // increment revision count
    dir->rev += 1;
    if (lfs2->cfg->block_cycles > 0 &&
            (dir->rev % (lfs2->cfg->block_cycles+1) == 0)) {
        if (lfs2_pair_cmp(dir->pair, (const lfs2_block_t[2]){0, 1}) == 0) {
            // oh no! we're writing too much to the superblock,
            // should we expand?
            lfs2_ssize_t res = lfs2_fs_size(lfs2);
            if (res < 0) {
                return res;
            }

            // do we have extra space? littlefs can't reclaim this space
            // by itself, so expand cautiously
            if ((lfs2_size_t)res < lfs2->cfg->block_count/2) {
                LFS2_DEBUG("Expanding superblock at rev %"PRIu32, dir->rev);
                int err = lfs2_dir_split(lfs2, dir, attrs, attrcount,
                        source, begin, end);
                if (err && err != LFS2_ERR_NOSPC) {
                    return err;
                }

                // welp, we tried, if we ran out of space there's not much
                // we can do, we'll error later if we've become frozen
                if (!err) {
                    end = begin;
                }
            }
#ifdef LFS2_MIGRATE
        } else if (lfs2_pair_cmp(dir->pair, lfs2->root) == 0 && lfs2->lfs21) {
            // we can't relocate our root during migrations, as this would
            // cause the superblock to get updated, which would clobber v1
#endif
        } else {
            // we're writing too much, time to relocate
            exhausted = true;
            goto relocate;
        }
    }

    // begin loop to commit compaction to blocks until a compact sticks
    while (true) {
        {
            // There's nothing special about our global delta, so feed it into
            // our local global delta
            int err = lfs2_dir_getgstate(lfs2, dir, &lfs2->gdelta);
            if (err) {
                return err;
            }

            // setup commit state
            struct lfs2_commit commit = {
                .block = dir->pair[1],
                .off = 0,
                .ptag = LFS2_BLOCK_NULL,
                .crc = LFS2_BLOCK_NULL,

                .begin = 0,
                .end = lfs2->cfg->block_size - 8,
            };

            // erase block to write to
            err = lfs2_bd_erase(lfs2, dir->pair[1]);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // write out header
            dir->rev = lfs2_tole32(dir->rev);
            err = lfs2_dir_commitprog(lfs2, &commit,
                    &dir->rev, sizeof(dir->rev));
            dir->rev = lfs2_fromle32(dir->rev);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // traverse the directory, this time writing out all unique tags
            err = lfs2_dir_traverse(lfs2,
                    source, 0, LFS2_BLOCK_NULL, attrs, attrcount, false,
                    LFS2_MKTAG(0x400, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_NAME, 0, 0),
                    begin, end, -begin,
                    lfs2_dir_commit_commit, &(struct lfs2_dir_commit_commit){
                        lfs2, &commit});
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // commit tail, which may be new after last size check
            if (!lfs2_pair_isnull(dir->tail)) {
                lfs2_pair_tole32(dir->tail);
                err = lfs2_dir_commitattr(lfs2, &commit,
                        LFS2_MKTAG(LFS2_TYPE_TAIL + dir->split, 0x3ff, 8),
                        dir->tail);
                lfs2_pair_fromle32(dir->tail);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }
            }

            if (!relocated && !lfs2_gstate_iszero(&lfs2->gdelta)) {
                // commit any globals, unless we're relocating,
                // in which case our parent will steal our globals
                lfs2_gstate_tole32(&lfs2->gdelta);
                err = lfs2_dir_commitattr(lfs2, &commit,
                        LFS2_MKTAG(LFS2_TYPE_MOVESTATE, 0x3ff,
                            sizeof(lfs2->gdelta)), &lfs2->gdelta);
                lfs2_gstate_fromle32(&lfs2->gdelta);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }
            }

            err = lfs2_dir_commitcrc(lfs2, &commit);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }

            // successful compaction, swap dir pair to indicate most recent
            LFS2_ASSERT(commit.off % lfs2->cfg->prog_size == 0);
            lfs2_pair_swap(dir->pair);
            dir->count = end - begin;
            dir->off = commit.off;
            dir->etag = commit.ptag;
            // note we able to have already handled move here
            if (lfs2_gstate_hasmovehere(&lfs2->gpending, dir->pair)) {
                lfs2_gstate_xormove(&lfs2->gpending,
                    &lfs2->gpending, 0x3ff, NULL);
            }
        }
        break;

relocate:
        // commit was corrupted, drop caches and prepare to relocate block
        relocated = true;
        lfs2_cache_drop(lfs2, &lfs2->pcache);
        if (!exhausted) {
            LFS2_DEBUG("Bad block at %"PRIx32, dir->pair[1]);
        }

        // can't relocate superblock, filesystem is now frozen
        if (lfs2_pair_cmp(oldpair, (const lfs2_block_t[2]){0, 1}) == 0) {
            LFS2_WARN("Superblock %"PRIx32" has become unwritable", oldpair[1]);
            return LFS2_ERR_NOSPC;
        }

        // relocate half of pair
        int err = lfs2_alloc(lfs2, &dir->pair[1]);
        if (err && (err != LFS2_ERR_NOSPC && !exhausted)) {
            return err;
        }

        continue;
    }

    if (!relocated) {
        lfs2->gstate = lfs2->gpending;
        lfs2->gdelta = (struct lfs2_gstate){0};
    } else {
        // update references if we relocated
        LFS2_DEBUG("Relocating %"PRIx32" %"PRIx32" -> %"PRIx32" %"PRIx32,
                oldpair[0], oldpair[1], dir->pair[0], dir->pair[1]);
        int err = lfs2_fs_relocate(lfs2, oldpair, dir->pair);
        if (err) {
            return err;
        }
    }

    return 0;
}

static int lfs2_dir_commit(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const struct lfs2_mattr *attrs, int attrcount) {
    // check for any inline files that aren't RAM backed and
    // forcefully evict them, needed for filesystem consistency
    for (lfs2_file_t *f = (lfs2_file_t*)lfs2->mlist; f; f = f->next) {
        if (dir != &f->m && lfs2_pair_cmp(f->m.pair, dir->pair) == 0 &&
                f->type == LFS2_TYPE_REG && (f->flags & LFS2_F_INLINE) &&
                f->ctz.size > lfs2->cfg->cache_size) {
            int err = lfs2_file_outline(lfs2, f);
            if (err) {
                return err;
            }

            err = lfs2_file_flush(lfs2, f);
            if (err) {
                return err;
            }
        }
    }

    // calculate changes to the directory
    lfs2_tag_t deletetag = LFS2_BLOCK_NULL;
    lfs2_tag_t createtag = LFS2_BLOCK_NULL;
    for (int i = 0; i < attrcount; i++) {
        if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_CREATE) {
            createtag = attrs[i].tag;
            dir->count += 1;
        } else if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_DELETE) {
            deletetag = attrs[i].tag;
            LFS2_ASSERT(dir->count > 0);
            dir->count -= 1;
        } else if (lfs2_tag_type1(attrs[i].tag) == LFS2_TYPE_TAIL) {
            dir->tail[0] = ((lfs2_block_t*)attrs[i].buffer)[0];
            dir->tail[1] = ((lfs2_block_t*)attrs[i].buffer)[1];
            dir->split = (lfs2_tag_chunk(attrs[i].tag) & 1);
            lfs2_pair_fromle32(dir->tail);
        }
    }

    // do we have a pending move?
    if (lfs2_gstate_hasmovehere(&lfs2->gpending, dir->pair)) {
        deletetag = lfs2->gpending.tag & LFS2_MKTAG(0x7ff, 0x3ff, 0);
        LFS2_ASSERT(dir->count > 0);
        dir->count -= 1;

        // mark gdelta so we reflect the move we will fix
        lfs2_gstate_xormove(&lfs2->gdelta, &lfs2->gpending, 0x3ff, NULL);
    }

    // should we actually drop the directory block?
    if (lfs2_tag_isvalid(deletetag) && dir->count == 0) {
        lfs2_mdir_t pdir;
        int err = lfs2_fs_pred(lfs2, dir->pair, &pdir);
        if (err && err != LFS2_ERR_NOENT) {
            return err;
        }

        if (err != LFS2_ERR_NOENT && pdir.split) {
            return lfs2_dir_drop(lfs2, &pdir, dir);
        }
    }

    if (dir->erased || dir->count >= 0xff) {
        // try to commit
        struct lfs2_commit commit = {
            .block = dir->pair[0],
            .off = dir->off,
            .ptag = dir->etag,
            .crc = LFS2_BLOCK_NULL,

            .begin = dir->off,
            .end = lfs2->cfg->block_size - 8,
        };

        // traverse attrs that need to be written out
        lfs2_pair_tole32(dir->tail);
        int err = lfs2_dir_traverse(lfs2,
                dir, dir->off, dir->etag, attrs, attrcount, false,
                0, 0, 0, 0, 0,
                lfs2_dir_commit_commit, &(struct lfs2_dir_commit_commit){
                    lfs2, &commit});
        lfs2_pair_fromle32(dir->tail);
        if (err) {
            if (err == LFS2_ERR_NOSPC || err == LFS2_ERR_CORRUPT) {
                goto compact;
            }
            return err;
        }

        // commit any global diffs if we have any
        if (!lfs2_gstate_iszero(&lfs2->gdelta)) {
            err = lfs2_dir_getgstate(lfs2, dir, &lfs2->gdelta);
            if (err) {
                return err;
            }

            lfs2_gstate_tole32(&lfs2->gdelta);
            err = lfs2_dir_commitattr(lfs2, &commit,
                    LFS2_MKTAG(LFS2_TYPE_MOVESTATE, 0x3ff,
                        sizeof(lfs2->gdelta)), &lfs2->gdelta);
            lfs2_gstate_fromle32(&lfs2->gdelta);
            if (err) {
                if (err == LFS2_ERR_NOSPC || err == LFS2_ERR_CORRUPT) {
                    goto compact;
                }
                return err;
            }
        }

        // finalize commit with the crc
        err = lfs2_dir_commitcrc(lfs2, &commit);
        if (err) {
            if (err == LFS2_ERR_NOSPC || err == LFS2_ERR_CORRUPT) {
                goto compact;
            }
            return err;
        }

        // successful commit, update dir
        LFS2_ASSERT(commit.off % lfs2->cfg->prog_size == 0);
        dir->off = commit.off;
        dir->etag = commit.ptag;

        // note we able to have already handled move here
        if (lfs2_gstate_hasmovehere(&lfs2->gpending, dir->pair)) {
            lfs2_gstate_xormove(&lfs2->gpending, &lfs2->gpending, 0x3ff, NULL);
        }

        // update gstate
        lfs2->gstate = lfs2->gpending;
        lfs2->gdelta = (struct lfs2_gstate){0};
    } else {
compact:
        // fall back to compaction
        lfs2_cache_drop(lfs2, &lfs2->pcache);

        int err = lfs2_dir_compact(lfs2, dir, attrs, attrcount,
                dir, 0, dir->count);
        if (err) {
            return err;
        }
    }

    // update any directories that are affected
    lfs2_mdir_t copy = *dir;

    // two passes, once for things that aren't us, and one
    // for things that are
    for (struct lfs2_mlist *d = lfs2->mlist; d; d = d->next) {
        if (lfs2_pair_cmp(d->m.pair, copy.pair) == 0) {
            d->m = *dir;
            if (d->id == lfs2_tag_id(deletetag)) {
                d->m.pair[0] = LFS2_BLOCK_NULL;
                d->m.pair[1] = LFS2_BLOCK_NULL;
            } else if (d->id > lfs2_tag_id(deletetag)) {
                d->id -= 1;
                if (d->type == LFS2_TYPE_DIR) {
                    ((lfs2_dir_t*)d)->pos -= 1;
                }
            } else if (&d->m != dir && d->id >= lfs2_tag_id(createtag)) {
                d->id += 1;
                if (d->type == LFS2_TYPE_DIR) {
                    ((lfs2_dir_t*)d)->pos += 1;
                }
            }

            while (d->id >= d->m.count && d->m.split) {
                // we split and id is on tail now
                d->id -= d->m.count;
                int err = lfs2_dir_fetch(lfs2, &d->m, d->m.tail);
                if (err) {
                    return err;
                }
            }
        }
    }

    return 0;
}


/// Top level directory operations ///
int lfs2_mkdir(lfs2_t *lfs2, const char *path) {
    LFS2_TRACE("lfs2_mkdir(%p, \"%s\")", (void*)lfs2, path);
    // deorphan if we haven't yet, needed at most once after poweron
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        LFS2_TRACE("lfs2_mkdir -> %d", err);
        return err;
    }

    lfs2_mdir_t cwd;
    uint16_t id;
    err = lfs2_dir_find(lfs2, &cwd, &path, &id);
    if (!(err == LFS2_ERR_NOENT && id != 0x3ff)) {
        LFS2_TRACE("lfs2_mkdir -> %d", (err < 0) ? err : LFS2_ERR_EXIST);
        return (err < 0) ? err : LFS2_ERR_EXIST;
    }

    // check that name fits
    lfs2_size_t nlen = strlen(path);
    if (nlen > lfs2->name_max) {
        LFS2_TRACE("lfs2_mkdir -> %d", LFS2_ERR_NAMETOOLONG);
        return LFS2_ERR_NAMETOOLONG;
    }

    // build up new directory
    lfs2_alloc_ack(lfs2);
    lfs2_mdir_t dir;
    err = lfs2_dir_alloc(lfs2, &dir);
    if (err) {
        LFS2_TRACE("lfs2_mkdir -> %d", err);
        return err;
    }

    // find end of list
    lfs2_mdir_t pred = cwd;
    while (pred.split) {
        err = lfs2_dir_fetch(lfs2, &pred, pred.tail);
        if (err) {
            LFS2_TRACE("lfs2_mkdir -> %d", err);
            return err;
        }
    }

    // setup dir
    lfs2_pair_tole32(pred.tail);
    err = lfs2_dir_commit(lfs2, &dir, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8), pred.tail}));
    lfs2_pair_fromle32(pred.tail);
    if (err) {
        LFS2_TRACE("lfs2_mkdir -> %d", err);
        return err;
    }

    // current block end of list?
    if (cwd.split) {
        // update tails, this creates a desync
        lfs2_fs_preporphans(lfs2, +1);
        lfs2_pair_tole32(dir.pair);
        err = lfs2_dir_commit(lfs2, &pred, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8), dir.pair}));
        lfs2_pair_fromle32(dir.pair);
        if (err) {
            LFS2_TRACE("lfs2_mkdir -> %d", err);
            return err;
        }
        lfs2_fs_preporphans(lfs2, -1);
    }

    // now insert into our parent block
    lfs2_pair_tole32(dir.pair);
    err = lfs2_dir_commit(lfs2, &cwd, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_CREATE, id, 0), NULL},
            {LFS2_MKTAG(LFS2_TYPE_DIR, id, nlen), path},
            {LFS2_MKTAG(LFS2_TYPE_DIRSTRUCT, id, 8), dir.pair},
            {!cwd.split
                ? LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8)
                : LFS2_MKTAG(LFS2_FROM_NOOP, 0, 0), dir.pair}));
    lfs2_pair_fromle32(dir.pair);
    if (err) {
        LFS2_TRACE("lfs2_mkdir -> %d", err);
        return err;
    }

    LFS2_TRACE("lfs2_mkdir -> %d", 0);
    return 0;
}

int lfs2_dir_open(lfs2_t *lfs2, lfs2_dir_t *dir, const char *path) {
    LFS2_TRACE("lfs2_dir_open(%p, %p, \"%s\")", (void*)lfs2, (void*)dir, path);
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &dir->m, &path, NULL);
    if (tag < 0) {
        LFS2_TRACE("lfs2_dir_open -> %d", tag);
        return tag;
    }

    if (lfs2_tag_type3(tag) != LFS2_TYPE_DIR) {
        LFS2_TRACE("lfs2_dir_open -> %d", LFS2_ERR_NOTDIR);
        return LFS2_ERR_NOTDIR;
    }

    lfs2_block_t pair[2];
    if (lfs2_tag_id(tag) == 0x3ff) {
        // handle root dir separately
        pair[0] = lfs2->root[0];
        pair[1] = lfs2->root[1];
    } else {
        // get dir pair from parent
        lfs2_stag_t res = lfs2_dir_get(lfs2, &dir->m, LFS2_MKTAG(0x700, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_STRUCT, lfs2_tag_id(tag), 8), pair);
        if (res < 0) {
            LFS2_TRACE("lfs2_dir_open -> %d", res);
            return res;
        }
        lfs2_pair_fromle32(pair);
    }

    // fetch first pair
    int err = lfs2_dir_fetch(lfs2, &dir->m, pair);
    if (err) {
        LFS2_TRACE("lfs2_dir_open -> %d", err);
        return err;
    }

    // setup entry
    dir->head[0] = dir->m.pair[0];
    dir->head[1] = dir->m.pair[1];
    dir->id = 0;
    dir->pos = 0;

    // add to list of mdirs
    dir->type = LFS2_TYPE_DIR;
    dir->next = (lfs2_dir_t*)lfs2->mlist;
    lfs2->mlist = (struct lfs2_mlist*)dir;

    LFS2_TRACE("lfs2_dir_open -> %d", 0);
    return 0;
}

int lfs2_dir_close(lfs2_t *lfs2, lfs2_dir_t *dir) {
    LFS2_TRACE("lfs2_dir_close(%p, %p)", (void*)lfs2, (void*)dir);
    // remove from list of mdirs
    for (struct lfs2_mlist **p = &lfs2->mlist; *p; p = &(*p)->next) {
        if (*p == (struct lfs2_mlist*)dir) {
            *p = (*p)->next;
            break;
        }
    }

    LFS2_TRACE("lfs2_dir_close -> %d", 0);
    return 0;
}

int lfs2_dir_read(lfs2_t *lfs2, lfs2_dir_t *dir, struct lfs2_info *info) {
    LFS2_TRACE("lfs2_dir_read(%p, %p, %p)",
            (void*)lfs2, (void*)dir, (void*)info);
    memset(info, 0, sizeof(*info));

    // special offset for '.' and '..'
    if (dir->pos == 0) {
        info->type = LFS2_TYPE_DIR;
        strcpy(info->name, ".");
        dir->pos += 1;
        LFS2_TRACE("lfs2_dir_read -> %d", true);
        return true;
    } else if (dir->pos == 1) {
        info->type = LFS2_TYPE_DIR;
        strcpy(info->name, "..");
        dir->pos += 1;
        LFS2_TRACE("lfs2_dir_read -> %d", true);
        return true;
    }

    while (true) {
        if (dir->id == dir->m.count) {
            if (!dir->m.split) {
                LFS2_TRACE("lfs2_dir_read -> %d", false);
                return false;
            }

            int err = lfs2_dir_fetch(lfs2, &dir->m, dir->m.tail);
            if (err) {
                LFS2_TRACE("lfs2_dir_read -> %d", err);
                return err;
            }

            dir->id = 0;
        }

        int err = lfs2_dir_getinfo(lfs2, &dir->m, dir->id, info);
        if (err && err != LFS2_ERR_NOENT) {
            LFS2_TRACE("lfs2_dir_read -> %d", err);
            return err;
        }

        dir->id += 1;
        if (err != LFS2_ERR_NOENT) {
            break;
        }
    }

    dir->pos += 1;
    LFS2_TRACE("lfs2_dir_read -> %d", true);
    return true;
}

int lfs2_dir_seek(lfs2_t *lfs2, lfs2_dir_t *dir, lfs2_off_t off) {
    LFS2_TRACE("lfs2_dir_seek(%p, %p, %"PRIu32")",
            (void*)lfs2, (void*)dir, off);
    // simply walk from head dir
    int err = lfs2_dir_rewind(lfs2, dir);
    if (err) {
        LFS2_TRACE("lfs2_dir_seek -> %d", err);
        return err;
    }

    // first two for ./..
    dir->pos = lfs2_min(2, off);
    off -= dir->pos;

    while (off != 0) {
        dir->id = lfs2_min(dir->m.count, off);
        dir->pos += dir->id;
        off -= dir->id;

        if (dir->id == dir->m.count) {
            if (!dir->m.split) {
                LFS2_TRACE("lfs2_dir_seek -> %d", LFS2_ERR_INVAL);
                return LFS2_ERR_INVAL;
            }

            err = lfs2_dir_fetch(lfs2, &dir->m, dir->m.tail);
            if (err) {
                LFS2_TRACE("lfs2_dir_seek -> %d", err);
                return err;
            }
        }
    }

    LFS2_TRACE("lfs2_dir_seek -> %d", 0);
    return 0;
}

lfs2_soff_t lfs2_dir_tell(lfs2_t *lfs2, lfs2_dir_t *dir) {
    LFS2_TRACE("lfs2_dir_tell(%p, %p)", (void*)lfs2, (void*)dir);
    (void)lfs2;
    LFS2_TRACE("lfs2_dir_tell -> %"PRId32, dir->pos);
    return dir->pos;
}

int lfs2_dir_rewind(lfs2_t *lfs2, lfs2_dir_t *dir) {
    LFS2_TRACE("lfs2_dir_rewind(%p, %p)", (void*)lfs2, (void*)dir);
    // reload the head dir
    int err = lfs2_dir_fetch(lfs2, &dir->m, dir->head);
    if (err) {
        LFS2_TRACE("lfs2_dir_rewind -> %d", err);
        return err;
    }

    dir->m.pair[0] = dir->head[0];
    dir->m.pair[1] = dir->head[1];
    dir->id = 0;
    dir->pos = 0;
    LFS2_TRACE("lfs2_dir_rewind -> %d", 0);
    return 0;
}


/// File index list operations ///
static int lfs2_ctz_index(lfs2_t *lfs2, lfs2_off_t *off) {
    lfs2_off_t size = *off;
    lfs2_off_t b = lfs2->cfg->block_size - 2*4;
    lfs2_off_t i = size / b;
    if (i == 0) {
        return 0;
    }

    i = (size - 4*(lfs2_popc(i-1)+2)) / b;
    *off = size - b*i - 4*lfs2_popc(i);
    return i;
}

static int lfs2_ctz_find(lfs2_t *lfs2,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache,
        lfs2_block_t head, lfs2_size_t size,
        lfs2_size_t pos, lfs2_block_t *block, lfs2_off_t *off) {
    if (size == 0) {
        *block = LFS2_BLOCK_NULL;
        *off = 0;
        return 0;
    }

    lfs2_off_t current = lfs2_ctz_index(lfs2, &(lfs2_off_t){size-1});
    lfs2_off_t target = lfs2_ctz_index(lfs2, &pos);

    while (current > target) {
        lfs2_size_t skip = lfs2_min(
                lfs2_npw2(current-target+1) - 1,
                lfs2_ctz(current));

        int err = lfs2_bd_read(lfs2,
                pcache, rcache, sizeof(head),
                head, 4*skip, &head, sizeof(head));
        head = lfs2_fromle32(head);
        if (err) {
            return err;
        }

        LFS2_ASSERT(head >= 2 && head <= lfs2->cfg->block_count);
        current -= 1 << skip;
    }

    *block = head;
    *off = pos;
    return 0;
}

static int lfs2_ctz_extend(lfs2_t *lfs2,
        lfs2_cache_t *pcache, lfs2_cache_t *rcache,
        lfs2_block_t head, lfs2_size_t size,
        lfs2_block_t *block, lfs2_off_t *off) {
    while (true) {
        // go ahead and grab a block
        lfs2_block_t nblock;
        int err = lfs2_alloc(lfs2, &nblock);
        if (err) {
            return err;
        }
        LFS2_ASSERT(nblock >= 2 && nblock <= lfs2->cfg->block_count);

        {
            err = lfs2_bd_erase(lfs2, nblock);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
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
            lfs2_off_t index = lfs2_ctz_index(lfs2, &size);
            size += 1;

            // just copy out the last block if it is incomplete
            if (size != lfs2->cfg->block_size) {
                for (lfs2_off_t i = 0; i < size; i++) {
                    uint8_t data;
                    err = lfs2_bd_read(lfs2,
                            NULL, rcache, size-i,
                            head, i, &data, 1);
                    if (err) {
                        return err;
                    }

                    err = lfs2_bd_prog(lfs2,
                            pcache, rcache, true,
                            nblock, i, &data, 1);
                    if (err) {
                        if (err == LFS2_ERR_CORRUPT) {
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
            lfs2_size_t skips = lfs2_ctz(index) + 1;

            for (lfs2_off_t i = 0; i < skips; i++) {
                head = lfs2_tole32(head);
                err = lfs2_bd_prog(lfs2, pcache, rcache, true,
                        nblock, 4*i, &head, 4);
                head = lfs2_fromle32(head);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }

                if (i != skips-1) {
                    err = lfs2_bd_read(lfs2,
                            NULL, rcache, sizeof(head),
                            head, 4*i, &head, sizeof(head));
                    head = lfs2_fromle32(head);
                    if (err) {
                        return err;
                    }
                }

                LFS2_ASSERT(head >= 2 && head <= lfs2->cfg->block_count);
            }

            *block = nblock;
            *off = 4*skips;
            return 0;
        }

relocate:
        LFS2_DEBUG("Bad block at %"PRIx32, nblock);

        // just clear cache and try a new block
        lfs2_cache_drop(lfs2, pcache);
    }
}

static int lfs2_ctz_traverse(lfs2_t *lfs2,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache,
        lfs2_block_t head, lfs2_size_t size,
        int (*cb)(void*, lfs2_block_t), void *data) {
    if (size == 0) {
        return 0;
    }

    lfs2_off_t index = lfs2_ctz_index(lfs2, &(lfs2_off_t){size-1});

    while (true) {
        int err = cb(data, head);
        if (err) {
            return err;
        }

        if (index == 0) {
            return 0;
        }

        lfs2_block_t heads[2];
        int count = 2 - (index & 1);
        err = lfs2_bd_read(lfs2,
                pcache, rcache, count*sizeof(head),
                head, 0, &heads, count*sizeof(head));
        heads[0] = lfs2_fromle32(heads[0]);
        heads[1] = lfs2_fromle32(heads[1]);
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
int lfs2_file_opencfg(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags,
        const struct lfs2_file_config *cfg) {
    LFS2_TRACE("lfs2_file_opencfg(%p, %p, \"%s\", %x, %p {"
                 ".buffer=%p, .attrs=%p, .attr_count=%"PRIu32"})",
            (void*)lfs2, (void*)file, path, flags,
            (void*)cfg, cfg->buffer, (void*)cfg->attrs, cfg->attr_count);

    // deorphan if we haven't yet, needed at most once after poweron
    if ((flags & 3) != LFS2_O_RDONLY) {
        int err = lfs2_fs_forceconsistency(lfs2);
        if (err) {
            LFS2_TRACE("lfs2_file_opencfg -> %d", err);
            return err;
        }
    }

    // setup simple file details
    int err;
    file->cfg = cfg;
    file->flags = flags | LFS2_F_OPENED;
    file->pos = 0;
    file->off = 0;
    file->cache.buffer = NULL;

    // allocate entry for file if it doesn't exist
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &file->m, &path, &file->id);
    if (tag < 0 && !(tag == LFS2_ERR_NOENT && file->id != 0x3ff)) {
        err = tag;
        goto cleanup;
    }

    // get id, add to list of mdirs to catch update changes
    file->type = LFS2_TYPE_REG;
    file->next = (lfs2_file_t*)lfs2->mlist;
    lfs2->mlist = (struct lfs2_mlist*)file;

    if (tag == LFS2_ERR_NOENT) {
        if (!(flags & LFS2_O_CREAT)) {
            err = LFS2_ERR_NOENT;
            goto cleanup;
        }

        // check that name fits
        lfs2_size_t nlen = strlen(path);
        if (nlen > lfs2->name_max) {
            err = LFS2_ERR_NAMETOOLONG;
            goto cleanup;
        }

        // get next slot and create entry to remember name
        err = lfs2_dir_commit(lfs2, &file->m, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_CREATE, file->id, 0), NULL},
                {LFS2_MKTAG(LFS2_TYPE_REG, file->id, nlen), path},
                {LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, file->id, 0), NULL}));
        if (err) {
            err = LFS2_ERR_NAMETOOLONG;
            goto cleanup;
        }

        tag = LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, 0);
    } else if (flags & LFS2_O_EXCL) {
        err = LFS2_ERR_EXIST;
        goto cleanup;
    } else if (lfs2_tag_type3(tag) != LFS2_TYPE_REG) {
        err = LFS2_ERR_ISDIR;
        goto cleanup;
    } else if (flags & LFS2_O_TRUNC) {
        // truncate if requested
        tag = LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, file->id, 0);
        file->flags |= LFS2_F_DIRTY;
    } else {
        // try to load what's on disk, if it's inlined we'll fix it later
        tag = lfs2_dir_get(lfs2, &file->m, LFS2_MKTAG(0x700, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_STRUCT, file->id, 8), &file->ctz);
        if (tag < 0) {
            err = tag;
            goto cleanup;
        }
        lfs2_ctz_fromle32(&file->ctz);
    }

    // fetch attrs
    for (unsigned i = 0; i < file->cfg->attr_count; i++) {
        if ((file->flags & 3) != LFS2_O_WRONLY) {
            lfs2_stag_t res = lfs2_dir_get(lfs2, &file->m,
                    LFS2_MKTAG(0x7ff, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_USERATTR + file->cfg->attrs[i].type,
                        file->id, file->cfg->attrs[i].size),
                        file->cfg->attrs[i].buffer);
            if (res < 0 && res != LFS2_ERR_NOENT) {
                err = res;
                goto cleanup;
            }
        }

        if ((file->flags & 3) != LFS2_O_RDONLY) {
            if (file->cfg->attrs[i].size > lfs2->attr_max) {
                err = LFS2_ERR_NOSPC;
                goto cleanup;
            }

            file->flags |= LFS2_F_DIRTY;
        }
    }

    // allocate buffer if needed
    if (file->cfg->buffer) {
        file->cache.buffer = file->cfg->buffer;
    } else {
        file->cache.buffer = lfs2_malloc(lfs2->cfg->cache_size);
        if (!file->cache.buffer) {
            err = LFS2_ERR_NOMEM;
            goto cleanup;
        }
    }

    // zero to avoid information leak
    lfs2_cache_zero(lfs2, &file->cache);

    if (lfs2_tag_type3(tag) == LFS2_TYPE_INLINESTRUCT) {
        // load inline files
        file->ctz.head = LFS2_BLOCK_INLINE;
        file->ctz.size = lfs2_tag_size(tag);
        file->flags |= LFS2_F_INLINE;
        file->cache.block = file->ctz.head;
        file->cache.off = 0;
        file->cache.size = lfs2->cfg->cache_size;

        // don't always read (may be new/trunc file)
        if (file->ctz.size > 0) {
            lfs2_stag_t res = lfs2_dir_get(lfs2, &file->m,
                    LFS2_MKTAG(0x700, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_STRUCT, file->id,
                        lfs2_min(file->cache.size, 0x3fe)),
                    file->cache.buffer);
            if (res < 0) {
                err = res;
                goto cleanup;
            }
        }
    }

    LFS2_TRACE("lfs2_file_opencfg -> %d", 0);
    return 0;

cleanup:
    // clean up lingering resources
    file->flags |= LFS2_F_ERRED;
    lfs2_file_close(lfs2, file);
    LFS2_TRACE("lfs2_file_opencfg -> %d", err);
    return err;
}

int lfs2_file_open(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags) {
    LFS2_TRACE("lfs2_file_open(%p, %p, \"%s\", %x)",
            (void*)lfs2, (void*)file, path, flags);
    static const struct lfs2_file_config defaults = {0};
    int err = lfs2_file_opencfg(lfs2, file, path, flags, &defaults);
    LFS2_TRACE("lfs2_file_open -> %d", err);
    return err;
}

int lfs2_file_close(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_TRACE("lfs2_file_close(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);

    int err = lfs2_file_sync(lfs2, file);

    // remove from list of mdirs
    for (struct lfs2_mlist **p = &lfs2->mlist; *p; p = &(*p)->next) {
        if (*p == (struct lfs2_mlist*)file) {
            *p = (*p)->next;
            break;
        }
    }

    // clean up memory
    if (!file->cfg->buffer) {
        lfs2_free(file->cache.buffer);
    }

    file->flags &= ~LFS2_F_OPENED;
    LFS2_TRACE("lfs2_file_close -> %d", err);
    return err;
}

static int lfs2_file_relocate(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);

    while (true) {
        // just relocate what exists into new block
        lfs2_block_t nblock;
        int err = lfs2_alloc(lfs2, &nblock);
        if (err) {
            return err;
        }

        err = lfs2_bd_erase(lfs2, nblock);
        if (err) {
            if (err == LFS2_ERR_CORRUPT) {
                goto relocate;
            }
            return err;
        }

        // either read from dirty cache or disk
        for (lfs2_off_t i = 0; i < file->off; i++) {
            uint8_t data;
            if (file->flags & LFS2_F_INLINE) {
                err = lfs2_dir_getread(lfs2, &file->m,
                        // note we evict inline files before they can be dirty
                        NULL, &file->cache, file->off-i,
                        LFS2_MKTAG(0xfff, 0x1ff, 0),
                        LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, file->id, 0),
                        i, &data, 1);
                if (err) {
                    return err;
                }
            } else {
                err = lfs2_bd_read(lfs2,
                        &file->cache, &lfs2->rcache, file->off-i,
                        file->block, i, &data, 1);
                if (err) {
                    return err;
                }
            }

            err = lfs2_bd_prog(lfs2,
                    &lfs2->pcache, &lfs2->rcache, true,
                    nblock, i, &data, 1);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    goto relocate;
                }
                return err;
            }
        }

        // copy over new state of file
        memcpy(file->cache.buffer, lfs2->pcache.buffer, lfs2->cfg->cache_size);
        file->cache.block = lfs2->pcache.block;
        file->cache.off = lfs2->pcache.off;
        file->cache.size = lfs2->pcache.size;
        lfs2_cache_zero(lfs2, &lfs2->pcache);

        file->block = nblock;
        file->flags |= LFS2_F_WRITING;
        return 0;

relocate:
        LFS2_DEBUG("Bad block at %"PRIx32, nblock);

        // just clear cache and try a new block
        lfs2_cache_drop(lfs2, &lfs2->pcache);
    }
}

static int lfs2_file_outline(lfs2_t *lfs2, lfs2_file_t *file) {
    file->off = file->pos;
    lfs2_alloc_ack(lfs2);
    int err = lfs2_file_relocate(lfs2, file);
    if (err) {
        return err;
    }

    file->flags &= ~LFS2_F_INLINE;
    return 0;
}

static int lfs2_file_flush(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);

    if (file->flags & LFS2_F_READING) {
        if (!(file->flags & LFS2_F_INLINE)) {
            lfs2_cache_drop(lfs2, &file->cache);
        }
        file->flags &= ~LFS2_F_READING;
    }

    if (file->flags & LFS2_F_WRITING) {
        lfs2_off_t pos = file->pos;

        if (!(file->flags & LFS2_F_INLINE)) {
            // copy over anything after current branch
            lfs2_file_t orig = {
                .ctz.head = file->ctz.head,
                .ctz.size = file->ctz.size,
                .flags = LFS2_O_RDONLY | LFS2_F_OPENED,
                .pos = file->pos,
                .cache = lfs2->rcache,
            };
            lfs2_cache_drop(lfs2, &lfs2->rcache);

            while (file->pos < file->ctz.size) {
                // copy over a byte at a time, leave it up to caching
                // to make this efficient
                uint8_t data;
                lfs2_ssize_t res = lfs2_file_read(lfs2, &orig, &data, 1);
                if (res < 0) {
                    return res;
                }

                res = lfs2_file_write(lfs2, file, &data, 1);
                if (res < 0) {
                    return res;
                }

                // keep our reference to the rcache in sync
                if (lfs2->rcache.block != LFS2_BLOCK_NULL) {
                    lfs2_cache_drop(lfs2, &orig.cache);
                    lfs2_cache_drop(lfs2, &lfs2->rcache);
                }
            }

            // write out what we have
            while (true) {
                int err = lfs2_bd_flush(lfs2, &file->cache, &lfs2->rcache, true);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }

                break;

relocate:
                LFS2_DEBUG("Bad block at %"PRIx32, file->block);
                err = lfs2_file_relocate(lfs2, file);
                if (err) {
                    return err;
                }
            }
        } else {
            file->pos = lfs2_max(file->pos, file->ctz.size);
        }

        // actual file updates
        file->ctz.head = file->block;
        file->ctz.size = file->pos;
        file->flags &= ~LFS2_F_WRITING;
        file->flags |= LFS2_F_DIRTY;

        file->pos = pos;
    }

    return 0;
}

int lfs2_file_sync(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_TRACE("lfs2_file_sync(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);

    while (true) {
        int err = lfs2_file_flush(lfs2, file);
        if (err) {
            file->flags |= LFS2_F_ERRED;
            LFS2_TRACE("lfs2_file_sync -> %d", err);
            return err;
        }

        if ((file->flags & LFS2_F_DIRTY) &&
                !(file->flags & LFS2_F_ERRED) &&
                !lfs2_pair_isnull(file->m.pair)) {
            // update dir entry
            uint16_t type;
            const void *buffer;
            lfs2_size_t size;
            struct lfs2_ctz ctz;
            if (file->flags & LFS2_F_INLINE) {
                // inline the whole file
                type = LFS2_TYPE_INLINESTRUCT;
                buffer = file->cache.buffer;
                size = file->ctz.size;
            } else {
                // update the ctz reference
                type = LFS2_TYPE_CTZSTRUCT;
                // copy ctz so alloc will work during a relocate
                ctz = file->ctz;
                lfs2_ctz_tole32(&ctz);
                buffer = &ctz;
                size = sizeof(ctz);
            }

            // commit file data and attributes
            err = lfs2_dir_commit(lfs2, &file->m, LFS2_MKATTRS(
                    {LFS2_MKTAG(type, file->id, size), buffer},
                    {LFS2_MKTAG(LFS2_FROM_USERATTRS, file->id,
                        file->cfg->attr_count), file->cfg->attrs}));
            if (err) {
                if (err == LFS2_ERR_NOSPC && (file->flags & LFS2_F_INLINE)) {
                    goto relocate;
                }
                file->flags |= LFS2_F_ERRED;
                LFS2_TRACE("lfs2_file_sync -> %d", err);
                return err;
            }

            file->flags &= ~LFS2_F_DIRTY;
        }

        LFS2_TRACE("lfs2_file_sync -> %d", 0);
        return 0;

relocate:
        // inline file doesn't fit anymore
        err = lfs2_file_outline(lfs2, file);
        if (err) {
            file->flags |= LFS2_F_ERRED;
            LFS2_TRACE("lfs2_file_sync -> %d", err);
            return err;
        }
    }
}

lfs2_ssize_t lfs2_file_read(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size) {
    LFS2_TRACE("lfs2_file_read(%p, %p, %p, %"PRIu32")",
            (void*)lfs2, (void*)file, buffer, size);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);
    LFS2_ASSERT((file->flags & 3) != LFS2_O_WRONLY);

    uint8_t *data = buffer;
    lfs2_size_t nsize = size;

    if (file->flags & LFS2_F_WRITING) {
        // flush out any writes
        int err = lfs2_file_flush(lfs2, file);
        if (err) {
            LFS2_TRACE("lfs2_file_read -> %"PRId32, err);
            return err;
        }
    }

    if (file->pos >= file->ctz.size) {
        // eof if past end
        LFS2_TRACE("lfs2_file_read -> %"PRId32, 0);
        return 0;
    }

    size = lfs2_min(size, file->ctz.size - file->pos);
    nsize = size;

    while (nsize > 0) {
        // check if we need a new block
        if (!(file->flags & LFS2_F_READING) ||
                file->off == lfs2->cfg->block_size) {
            if (!(file->flags & LFS2_F_INLINE)) {
                int err = lfs2_ctz_find(lfs2, NULL, &file->cache,
                        file->ctz.head, file->ctz.size,
                        file->pos, &file->block, &file->off);
                if (err) {
                    LFS2_TRACE("lfs2_file_read -> %"PRId32, err);
                    return err;
                }
            } else {
                file->block = LFS2_BLOCK_INLINE;
                file->off = file->pos;
            }

            file->flags |= LFS2_F_READING;
        }

        // read as much as we can in current block
        lfs2_size_t diff = lfs2_min(nsize, lfs2->cfg->block_size - file->off);
        if (file->flags & LFS2_F_INLINE) {
            int err = lfs2_dir_getread(lfs2, &file->m,
                    NULL, &file->cache, lfs2->cfg->block_size,
                    LFS2_MKTAG(0xfff, 0x1ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, file->id, 0),
                    file->off, data, diff);
            if (err) {
                LFS2_TRACE("lfs2_file_read -> %"PRId32, err);
                return err;
            }
        } else {
            int err = lfs2_bd_read(lfs2,
                    NULL, &file->cache, lfs2->cfg->block_size,
                    file->block, file->off, data, diff);
            if (err) {
                LFS2_TRACE("lfs2_file_read -> %"PRId32, err);
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;
    }

    LFS2_TRACE("lfs2_file_read -> %"PRId32, size);
    return size;
}

lfs2_ssize_t lfs2_file_write(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size) {
    LFS2_TRACE("lfs2_file_write(%p, %p, %p, %"PRIu32")",
            (void*)lfs2, (void*)file, buffer, size);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);
    LFS2_ASSERT((file->flags & 3) != LFS2_O_RDONLY);

    const uint8_t *data = buffer;
    lfs2_size_t nsize = size;

    if (file->flags & LFS2_F_READING) {
        // drop any reads
        int err = lfs2_file_flush(lfs2, file);
        if (err) {
            LFS2_TRACE("lfs2_file_write -> %"PRId32, err);
            return err;
        }
    }

    if ((file->flags & LFS2_O_APPEND) && file->pos < file->ctz.size) {
        file->pos = file->ctz.size;
    }

    if (file->pos + size > lfs2->file_max) {
        // Larger than file limit?
        LFS2_TRACE("lfs2_file_write -> %"PRId32, LFS2_ERR_FBIG);
        return LFS2_ERR_FBIG;
    }

    if (!(file->flags & LFS2_F_WRITING) && file->pos > file->ctz.size) {
        // fill with zeros
        lfs2_off_t pos = file->pos;
        file->pos = file->ctz.size;

        while (file->pos < pos) {
            lfs2_ssize_t res = lfs2_file_write(lfs2, file, &(uint8_t){0}, 1);
            if (res < 0) {
                LFS2_TRACE("lfs2_file_write -> %"PRId32, res);
                return res;
            }
        }
    }

    if ((file->flags & LFS2_F_INLINE) &&
            lfs2_max(file->pos+nsize, file->ctz.size) >
            lfs2_min(0x3fe, lfs2_min(
                lfs2->cfg->cache_size, lfs2->cfg->block_size/8))) {
        // inline file doesn't fit anymore
        int err = lfs2_file_outline(lfs2, file);
        if (err) {
            file->flags |= LFS2_F_ERRED;
            LFS2_TRACE("lfs2_file_write -> %"PRId32, err);
            return err;
        }
    }

    while (nsize > 0) {
        // check if we need a new block
        if (!(file->flags & LFS2_F_WRITING) ||
                file->off == lfs2->cfg->block_size) {
            if (!(file->flags & LFS2_F_INLINE)) {
                if (!(file->flags & LFS2_F_WRITING) && file->pos > 0) {
                    // find out which block we're extending from
                    int err = lfs2_ctz_find(lfs2, NULL, &file->cache,
                            file->ctz.head, file->ctz.size,
                            file->pos-1, &file->block, &file->off);
                    if (err) {
                        file->flags |= LFS2_F_ERRED;
                        LFS2_TRACE("lfs2_file_write -> %"PRId32, err);
                        return err;
                    }

                    // mark cache as dirty since we may have read data into it
                    lfs2_cache_zero(lfs2, &file->cache);
                }

                // extend file with new blocks
                lfs2_alloc_ack(lfs2);
                int err = lfs2_ctz_extend(lfs2, &file->cache, &lfs2->rcache,
                        file->block, file->pos,
                        &file->block, &file->off);
                if (err) {
                    file->flags |= LFS2_F_ERRED;
                    LFS2_TRACE("lfs2_file_write -> %"PRId32, err);
                    return err;
                }
            } else {
                file->block = LFS2_BLOCK_INLINE;
                file->off = file->pos;
            }

            file->flags |= LFS2_F_WRITING;
        }

        // program as much as we can in current block
        lfs2_size_t diff = lfs2_min(nsize, lfs2->cfg->block_size - file->off);
        while (true) {
            int err = lfs2_bd_prog(lfs2, &file->cache, &lfs2->rcache, true,
                    file->block, file->off, data, diff);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    goto relocate;
                }
                file->flags |= LFS2_F_ERRED;
                LFS2_TRACE("lfs2_file_write -> %"PRId32, err);
                return err;
            }

            break;
relocate:
            err = lfs2_file_relocate(lfs2, file);
            if (err) {
                file->flags |= LFS2_F_ERRED;
                LFS2_TRACE("lfs2_file_write -> %"PRId32, err);
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;

        lfs2_alloc_ack(lfs2);
    }

    file->flags &= ~LFS2_F_ERRED;
    LFS2_TRACE("lfs2_file_write -> %"PRId32, size);
    return size;
}

lfs2_soff_t lfs2_file_seek(lfs2_t *lfs2, lfs2_file_t *file,
        lfs2_soff_t off, int whence) {
    LFS2_TRACE("lfs2_file_seek(%p, %p, %"PRId32", %d)",
            (void*)lfs2, (void*)file, off, whence);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);

    // write out everything beforehand, may be noop if rdonly
    int err = lfs2_file_flush(lfs2, file);
    if (err) {
        LFS2_TRACE("lfs2_file_seek -> %"PRId32, err);
        return err;
    }

    // find new pos
    lfs2_off_t npos = file->pos;
    if (whence == LFS2_SEEK_SET) {
        npos = off;
    } else if (whence == LFS2_SEEK_CUR) {
        npos = file->pos + off;
    } else if (whence == LFS2_SEEK_END) {
        npos = file->ctz.size + off;
    }

    if (npos > lfs2->file_max) {
        // file position out of range
        LFS2_TRACE("lfs2_file_seek -> %"PRId32, LFS2_ERR_INVAL);
        return LFS2_ERR_INVAL;
    }

    // update pos
    file->pos = npos;
    LFS2_TRACE("lfs2_file_seek -> %"PRId32, npos);
    return npos;
}

int lfs2_file_truncate(lfs2_t *lfs2, lfs2_file_t *file, lfs2_off_t size) {
    LFS2_TRACE("lfs2_file_truncate(%p, %p, %"PRIu32")",
            (void*)lfs2, (void*)file, size);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);
    LFS2_ASSERT((file->flags & 3) != LFS2_O_RDONLY);

    if (size > LFS2_FILE_MAX) {
        LFS2_TRACE("lfs2_file_truncate -> %d", LFS2_ERR_INVAL);
        return LFS2_ERR_INVAL;
    }

    lfs2_off_t pos = file->pos;
    lfs2_off_t oldsize = lfs2_file_size(lfs2, file);
    if (size < oldsize) {
        // need to flush since directly changing metadata
        int err = lfs2_file_flush(lfs2, file);
        if (err) {
            LFS2_TRACE("lfs2_file_truncate -> %d", err);
            return err;
        }

        // lookup new head in ctz skip list
        err = lfs2_ctz_find(lfs2, NULL, &file->cache,
                file->ctz.head, file->ctz.size,
                size, &file->block, &file->off);
        if (err) {
            LFS2_TRACE("lfs2_file_truncate -> %d", err);
            return err;
        }

        file->ctz.head = file->block;
        file->ctz.size = size;
        file->flags |= LFS2_F_DIRTY | LFS2_F_READING;
    } else if (size > oldsize) {
        // flush+seek if not already at end
        if (file->pos != oldsize) {
            lfs2_soff_t res = lfs2_file_seek(lfs2, file, 0, LFS2_SEEK_END);
            if (res < 0) {
                LFS2_TRACE("lfs2_file_truncate -> %d", res);
                return (int)res;
            }
        }

        // fill with zeros
        while (file->pos < size) {
            lfs2_ssize_t res = lfs2_file_write(lfs2, file, &(uint8_t){0}, 1);
            if (res < 0) {
                LFS2_TRACE("lfs2_file_truncate -> %d", res);
                return (int)res;
            }
        }
    }

    // restore pos
    lfs2_soff_t res = lfs2_file_seek(lfs2, file, pos, LFS2_SEEK_SET);
    if (res < 0) {
      LFS2_TRACE("lfs2_file_truncate -> %d", res);
      return (int)res;
    }

    LFS2_TRACE("lfs2_file_truncate -> %d", 0);
    return 0;
}

lfs2_soff_t lfs2_file_tell(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_TRACE("lfs2_file_tell(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);
    (void)lfs2;
    LFS2_TRACE("lfs2_file_tell -> %"PRId32, file->pos);
    return file->pos;
}

int lfs2_file_rewind(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_TRACE("lfs2_file_rewind(%p, %p)", (void*)lfs2, (void*)file);
    lfs2_soff_t res = lfs2_file_seek(lfs2, file, 0, LFS2_SEEK_SET);
    if (res < 0) {
        LFS2_TRACE("lfs2_file_rewind -> %d", res);
        return (int)res;
    }

    LFS2_TRACE("lfs2_file_rewind -> %d", 0);
    return 0;
}

lfs2_soff_t lfs2_file_size(lfs2_t *lfs2, lfs2_file_t *file) {
    LFS2_TRACE("lfs2_file_size(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(file->flags & LFS2_F_OPENED);
    (void)lfs2;
    if (file->flags & LFS2_F_WRITING) {
        LFS2_TRACE("lfs2_file_size -> %"PRId32,
                lfs2_max(file->pos, file->ctz.size));
        return lfs2_max(file->pos, file->ctz.size);
    } else {
        LFS2_TRACE("lfs2_file_size -> %"PRId32, file->ctz.size);
        return file->ctz.size;
    }
}


/// General fs operations ///
int lfs2_stat(lfs2_t *lfs2, const char *path, struct lfs2_info *info) {
    LFS2_TRACE("lfs2_stat(%p, \"%s\", %p)", (void*)lfs2, path, (void*)info);
    lfs2_mdir_t cwd;
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &cwd, &path, NULL);
    if (tag < 0) {
        LFS2_TRACE("lfs2_stat -> %d", tag);
        return (int)tag;
    }

    int err = lfs2_dir_getinfo(lfs2, &cwd, lfs2_tag_id(tag), info);
    LFS2_TRACE("lfs2_stat -> %d", err);
    return err;
}

int lfs2_remove(lfs2_t *lfs2, const char *path) {
    LFS2_TRACE("lfs2_remove(%p, \"%s\")", (void*)lfs2, path);
    // deorphan if we haven't yet, needed at most once after poweron
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        LFS2_TRACE("lfs2_remove -> %d", err);
        return err;
    }

    lfs2_mdir_t cwd;
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &cwd, &path, NULL);
    if (tag < 0 || lfs2_tag_id(tag) == 0x3ff) {
        LFS2_TRACE("lfs2_remove -> %d", (tag < 0) ? tag : LFS2_ERR_INVAL);
        return (tag < 0) ? (int)tag : LFS2_ERR_INVAL;
    }

    lfs2_mdir_t dir;
    if (lfs2_tag_type3(tag) == LFS2_TYPE_DIR) {
        // must be empty before removal
        lfs2_block_t pair[2];
        lfs2_stag_t res = lfs2_dir_get(lfs2, &cwd, LFS2_MKTAG(0x700, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_STRUCT, lfs2_tag_id(tag), 8), pair);
        if (res < 0) {
            LFS2_TRACE("lfs2_remove -> %d", res);
            return (int)res;
        }
        lfs2_pair_fromle32(pair);

        err = lfs2_dir_fetch(lfs2, &dir, pair);
        if (err) {
            LFS2_TRACE("lfs2_remove -> %d", err);
            return err;
        }

        if (dir.count > 0 || dir.split) {
            LFS2_TRACE("lfs2_remove -> %d", LFS2_ERR_NOTEMPTY);
            return LFS2_ERR_NOTEMPTY;
        }

        // mark fs as orphaned
        lfs2_fs_preporphans(lfs2, +1);
    }

    // delete the entry
    err = lfs2_dir_commit(lfs2, &cwd, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_DELETE, lfs2_tag_id(tag), 0), NULL}));
    if (err) {
        LFS2_TRACE("lfs2_remove -> %d", err);
        return err;
    }

    if (lfs2_tag_type3(tag) == LFS2_TYPE_DIR) {
        // fix orphan
        lfs2_fs_preporphans(lfs2, -1);

        err = lfs2_fs_pred(lfs2, dir.pair, &cwd);
        if (err) {
            LFS2_TRACE("lfs2_remove -> %d", err);
            return err;
        }

        err = lfs2_dir_drop(lfs2, &cwd, &dir);
        if (err) {
            LFS2_TRACE("lfs2_remove -> %d", err);
            return err;
        }
    }

    LFS2_TRACE("lfs2_remove -> %d", 0);
    return 0;
}

int lfs2_rename(lfs2_t *lfs2, const char *oldpath, const char *newpath) {
    LFS2_TRACE("lfs2_rename(%p, \"%s\", \"%s\")", (void*)lfs2, oldpath, newpath);

    // deorphan if we haven't yet, needed at most once after poweron
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        LFS2_TRACE("lfs2_rename -> %d", err);
        return err;
    }

    // find old entry
    lfs2_mdir_t oldcwd;
    lfs2_stag_t oldtag = lfs2_dir_find(lfs2, &oldcwd, &oldpath, NULL);
    if (oldtag < 0 || lfs2_tag_id(oldtag) == 0x3ff) {
        LFS2_TRACE("lfs2_rename -> %d", (oldtag < 0) ? oldtag : LFS2_ERR_INVAL);
        return (oldtag < 0) ? (int)oldtag : LFS2_ERR_INVAL;
    }

    // find new entry
    lfs2_mdir_t newcwd;
    uint16_t newid;
    lfs2_stag_t prevtag = lfs2_dir_find(lfs2, &newcwd, &newpath, &newid);
    if ((prevtag < 0 || lfs2_tag_id(prevtag) == 0x3ff) &&
            !(prevtag == LFS2_ERR_NOENT && newid != 0x3ff)) {
        LFS2_TRACE("lfs2_rename -> %d", (prevtag < 0) ? prevtag : LFS2_ERR_INVAL);
        return (prevtag < 0) ? (int)prevtag : LFS2_ERR_INVAL;
    }

    lfs2_mdir_t prevdir;
    if (prevtag == LFS2_ERR_NOENT) {
        // check that name fits
        lfs2_size_t nlen = strlen(newpath);
        if (nlen > lfs2->name_max) {
            LFS2_TRACE("lfs2_rename -> %d", LFS2_ERR_NAMETOOLONG);
            return LFS2_ERR_NAMETOOLONG;
        }
    } else if (lfs2_tag_type3(prevtag) != lfs2_tag_type3(oldtag)) {
        LFS2_TRACE("lfs2_rename -> %d", LFS2_ERR_ISDIR);
        return LFS2_ERR_ISDIR;
    } else if (lfs2_tag_type3(prevtag) == LFS2_TYPE_DIR) {
        // must be empty before removal
        lfs2_block_t prevpair[2];
        lfs2_stag_t res = lfs2_dir_get(lfs2, &newcwd, LFS2_MKTAG(0x700, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_STRUCT, newid, 8), prevpair);
        if (res < 0) {
            LFS2_TRACE("lfs2_rename -> %d", res);
            return (int)res;
        }
        lfs2_pair_fromle32(prevpair);

        // must be empty before removal
        err = lfs2_dir_fetch(lfs2, &prevdir, prevpair);
        if (err) {
            LFS2_TRACE("lfs2_rename -> %d", err);
            return err;
        }

        if (prevdir.count > 0 || prevdir.split) {
            LFS2_TRACE("lfs2_rename -> %d", LFS2_ERR_NOTEMPTY);
            return LFS2_ERR_NOTEMPTY;
        }

        // mark fs as orphaned
        lfs2_fs_preporphans(lfs2, +1);
    }

    // create move to fix later
    uint16_t newoldtagid = lfs2_tag_id(oldtag);
    if (lfs2_pair_cmp(oldcwd.pair, newcwd.pair) == 0 &&
            prevtag == LFS2_ERR_NOENT && newid <= newoldtagid) {
        // there is a small chance we are being renamed in the same directory
        // to an id less than our old id, the global update to handle this
        // is a bit messy
        newoldtagid += 1;
    }

    lfs2_fs_prepmove(lfs2, newoldtagid, oldcwd.pair);

    // move over all attributes
    err = lfs2_dir_commit(lfs2, &newcwd, LFS2_MKATTRS(
            {prevtag != LFS2_ERR_NOENT
                ? LFS2_MKTAG(LFS2_TYPE_DELETE, newid, 0)
                : LFS2_MKTAG(LFS2_FROM_NOOP, 0, 0), NULL},
            {LFS2_MKTAG(LFS2_TYPE_CREATE, newid, 0), NULL},
            {LFS2_MKTAG(lfs2_tag_type3(oldtag), newid, strlen(newpath)),
                newpath},
            {LFS2_MKTAG(LFS2_FROM_MOVE, newid, lfs2_tag_id(oldtag)), &oldcwd}));
    if (err) {
        LFS2_TRACE("lfs2_rename -> %d", err);
        return err;
    }

    // let commit clean up after move (if we're different! otherwise move
    // logic already fixed it for us)
    if (lfs2_pair_cmp(oldcwd.pair, newcwd.pair) != 0) {
        err = lfs2_dir_commit(lfs2, &oldcwd, NULL, 0);
        if (err) {
            LFS2_TRACE("lfs2_rename -> %d", err);
            return err;
        }
    }

    if (prevtag != LFS2_ERR_NOENT && lfs2_tag_type3(prevtag) == LFS2_TYPE_DIR) {
        // fix orphan
        lfs2_fs_preporphans(lfs2, -1);

        err = lfs2_fs_pred(lfs2, prevdir.pair, &newcwd);
        if (err) {
            LFS2_TRACE("lfs2_rename -> %d", err);
            return err;
        }

        err = lfs2_dir_drop(lfs2, &newcwd, &prevdir);
        if (err) {
            LFS2_TRACE("lfs2_rename -> %d", err);
            return err;
        }
    }

    LFS2_TRACE("lfs2_rename -> %d", 0);
    return 0;
}

lfs2_ssize_t lfs2_getattr(lfs2_t *lfs2, const char *path,
        uint8_t type, void *buffer, lfs2_size_t size) {
    LFS2_TRACE("lfs2_getattr(%p, \"%s\", %"PRIu8", %p, %"PRIu32")",
            (void*)lfs2, path, type, buffer, size);
    lfs2_mdir_t cwd;
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &cwd, &path, NULL);
    if (tag < 0) {
        LFS2_TRACE("lfs2_getattr -> %"PRId32, tag);
        return tag;
    }

    uint16_t id = lfs2_tag_id(tag);
    if (id == 0x3ff) {
        // special case for root
        id = 0;
        int err = lfs2_dir_fetch(lfs2, &cwd, lfs2->root);
        if (err) {
            LFS2_TRACE("lfs2_getattr -> %"PRId32, err);
            return err;
        }
    }

    tag = lfs2_dir_get(lfs2, &cwd, LFS2_MKTAG(0x7ff, 0x3ff, 0),
            LFS2_MKTAG(LFS2_TYPE_USERATTR + type,
                id, lfs2_min(size, lfs2->attr_max)),
            buffer);
    if (tag < 0) {
        if (tag == LFS2_ERR_NOENT) {
            LFS2_TRACE("lfs2_getattr -> %"PRId32, LFS2_ERR_NOATTR);
            return LFS2_ERR_NOATTR;
        }

        LFS2_TRACE("lfs2_getattr -> %"PRId32, tag);
        return tag;
    }

    size = lfs2_tag_size(tag);
    LFS2_TRACE("lfs2_getattr -> %"PRId32, size);
    return size;
}

static int lfs2_commitattr(lfs2_t *lfs2, const char *path,
        uint8_t type, const void *buffer, lfs2_size_t size) {
    lfs2_mdir_t cwd;
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &cwd, &path, NULL);
    if (tag < 0) {
        return tag;
    }

    uint16_t id = lfs2_tag_id(tag);
    if (id == 0x3ff) {
        // special case for root
        id = 0;
        int err = lfs2_dir_fetch(lfs2, &cwd, lfs2->root);
        if (err) {
            return err;
        }
    }

    return lfs2_dir_commit(lfs2, &cwd, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_USERATTR + type, id, size), buffer}));
}

int lfs2_setattr(lfs2_t *lfs2, const char *path,
        uint8_t type, const void *buffer, lfs2_size_t size) {
    LFS2_TRACE("lfs2_setattr(%p, \"%s\", %"PRIu8", %p, %"PRIu32")",
            (void*)lfs2, path, type, buffer, size);
    if (size > lfs2->attr_max) {
        LFS2_TRACE("lfs2_setattr -> %d", LFS2_ERR_NOSPC);
        return LFS2_ERR_NOSPC;
    }

    int err = lfs2_commitattr(lfs2, path, type, buffer, size);
    LFS2_TRACE("lfs2_setattr -> %d", err);
    return err;
}

int lfs2_removeattr(lfs2_t *lfs2, const char *path, uint8_t type) {
    LFS2_TRACE("lfs2_removeattr(%p, \"%s\", %"PRIu8")", (void*)lfs2, path, type);
    int err = lfs2_commitattr(lfs2, path, type, NULL, 0x3ff);
    LFS2_TRACE("lfs2_removeattr -> %d", err);
    return err;
}


/// Filesystem operations ///
static int lfs2_init(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    lfs2->cfg = cfg;
    int err = 0;

    // validate that the lfs2-cfg sizes were initiated properly before
    // performing any arithmetic logics with them
    LFS2_ASSERT(lfs2->cfg->read_size != 0);
    LFS2_ASSERT(lfs2->cfg->prog_size != 0);
    LFS2_ASSERT(lfs2->cfg->cache_size != 0);

    // check that block size is a multiple of cache size is a multiple
    // of prog and read sizes
    LFS2_ASSERT(lfs2->cfg->cache_size % lfs2->cfg->read_size == 0);
    LFS2_ASSERT(lfs2->cfg->cache_size % lfs2->cfg->prog_size == 0);
    LFS2_ASSERT(lfs2->cfg->block_size % lfs2->cfg->cache_size == 0);

    // check that the block size is large enough to fit ctz pointers
    LFS2_ASSERT(4*lfs2_npw2(LFS2_BLOCK_NULL / (lfs2->cfg->block_size-2*4))
            <= lfs2->cfg->block_size);

    // block_cycles = 0 is no longer supported.
    //
    // block_cycles is the number of erase cycles before littlefs evicts
    // metadata logs as a part of wear leveling. Suggested values are in the
    // range of 100-1000, or set block_cycles to -1 to disable block-level
    // wear-leveling.
    LFS2_ASSERT(lfs2->cfg->block_cycles != 0);


    // setup read cache
    if (lfs2->cfg->read_buffer) {
        lfs2->rcache.buffer = lfs2->cfg->read_buffer;
    } else {
        lfs2->rcache.buffer = lfs2_malloc(lfs2->cfg->cache_size);
        if (!lfs2->rcache.buffer) {
            err = LFS2_ERR_NOMEM;
            goto cleanup;
        }
    }

    // setup program cache
    if (lfs2->cfg->prog_buffer) {
        lfs2->pcache.buffer = lfs2->cfg->prog_buffer;
    } else {
        lfs2->pcache.buffer = lfs2_malloc(lfs2->cfg->cache_size);
        if (!lfs2->pcache.buffer) {
            err = LFS2_ERR_NOMEM;
            goto cleanup;
        }
    }

    // zero to avoid information leaks
    lfs2_cache_zero(lfs2, &lfs2->rcache);
    lfs2_cache_zero(lfs2, &lfs2->pcache);

    // setup lookahead, must be multiple of 64-bits, 32-bit aligned
    LFS2_ASSERT(lfs2->cfg->lookahead_size > 0);
    LFS2_ASSERT(lfs2->cfg->lookahead_size % 8 == 0 &&
            (uintptr_t)lfs2->cfg->lookahead_buffer % 4 == 0);
    if (lfs2->cfg->lookahead_buffer) {
        lfs2->free.buffer = lfs2->cfg->lookahead_buffer;
    } else {
        lfs2->free.buffer = lfs2_malloc(lfs2->cfg->lookahead_size);
        if (!lfs2->free.buffer) {
            err = LFS2_ERR_NOMEM;
            goto cleanup;
        }
    }

    // check that the size limits are sane
    LFS2_ASSERT(lfs2->cfg->name_max <= LFS2_NAME_MAX);
    lfs2->name_max = lfs2->cfg->name_max;
    if (!lfs2->name_max) {
        lfs2->name_max = LFS2_NAME_MAX;
    }

    LFS2_ASSERT(lfs2->cfg->file_max <= LFS2_FILE_MAX);
    lfs2->file_max = lfs2->cfg->file_max;
    if (!lfs2->file_max) {
        lfs2->file_max = LFS2_FILE_MAX;
    }

    LFS2_ASSERT(lfs2->cfg->attr_max <= LFS2_ATTR_MAX);
    lfs2->attr_max = lfs2->cfg->attr_max;
    if (!lfs2->attr_max) {
        lfs2->attr_max = LFS2_ATTR_MAX;
    }

    // setup default state
    lfs2->root[0] = LFS2_BLOCK_NULL;
    lfs2->root[1] = LFS2_BLOCK_NULL;
    lfs2->mlist = NULL;
    lfs2->seed = 0;
    lfs2->gstate = (struct lfs2_gstate){0};
    lfs2->gpending = (struct lfs2_gstate){0};
    lfs2->gdelta = (struct lfs2_gstate){0};
#ifdef LFS2_MIGRATE
    lfs2->lfs21 = NULL;
#endif

    return 0;

cleanup:
    lfs2_deinit(lfs2);
    return err;
}

static int lfs2_deinit(lfs2_t *lfs2) {
    // free allocated memory
    if (!lfs2->cfg->read_buffer) {
        lfs2_free(lfs2->rcache.buffer);
    }

    if (!lfs2->cfg->prog_buffer) {
        lfs2_free(lfs2->pcache.buffer);
    }

    if (!lfs2->cfg->lookahead_buffer) {
        lfs2_free(lfs2->free.buffer);
    }

    return 0;
}

int lfs2_format(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    LFS2_TRACE("lfs2_format(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .block_count=%"PRIu32", "
                ".block_cycles=%"PRIu32", .cache_size=%"PRIu32", "
                ".lookahead_size=%"PRIu32", .read_buffer=%p, "
                ".prog_buffer=%p, .lookahead_buffer=%p, "
                ".name_max=%"PRIu32", .file_max=%"PRIu32", "
                ".attr_max=%"PRIu32"})",
            (void*)lfs2, (void*)cfg, cfg->context,
            (void*)(uintptr_t)cfg->read, (void*)(uintptr_t)cfg->prog,
            (void*)(uintptr_t)cfg->erase, (void*)(uintptr_t)cfg->sync,
            cfg->read_size, cfg->prog_size, cfg->block_size, cfg->block_count,
            cfg->block_cycles, cfg->cache_size, cfg->lookahead_size,
            cfg->read_buffer, cfg->prog_buffer, cfg->lookahead_buffer,
            cfg->name_max, cfg->file_max, cfg->attr_max);
    int err = 0;
    {
        err = lfs2_init(lfs2, cfg);
        if (err) {
            LFS2_TRACE("lfs2_format -> %d", err);
            return err;
        }

        // create free lookahead
        memset(lfs2->free.buffer, 0, lfs2->cfg->lookahead_size);
        lfs2->free.off = 0;
        lfs2->free.size = lfs2_min(8*lfs2->cfg->lookahead_size,
                lfs2->cfg->block_count);
        lfs2->free.i = 0;
        lfs2_alloc_ack(lfs2);

        // create root dir
        lfs2_mdir_t root;
        err = lfs2_dir_alloc(lfs2, &root);
        if (err) {
            goto cleanup;
        }

        // write one superblock
        lfs2_superblock_t superblock = {
            .version     = LFS2_DISK_VERSION,
            .block_size  = lfs2->cfg->block_size,
            .block_count = lfs2->cfg->block_count,
            .name_max    = lfs2->name_max,
            .file_max    = lfs2->file_max,
            .attr_max    = lfs2->attr_max,
        };

        lfs2_superblock_tole32(&superblock);
        err = lfs2_dir_commit(lfs2, &root, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_CREATE, 0, 0), NULL},
                {LFS2_MKTAG(LFS2_TYPE_SUPERBLOCK, 0, 8), "littlefs"},
                {LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                    &superblock}));
        if (err) {
            goto cleanup;
        }

        // sanity check that fetch works
        err = lfs2_dir_fetch(lfs2, &root, (const lfs2_block_t[2]){0, 1});
        if (err) {
            goto cleanup;
        }

        // force compaction to prevent accidentally mounting any
        // older version of littlefs that may live on disk
        root.erased = false;
        err = lfs2_dir_commit(lfs2, &root, NULL, 0);
        if (err) {
            goto cleanup;
        }
    }

cleanup:
    lfs2_deinit(lfs2);
    LFS2_TRACE("lfs2_format -> %d", err);
    return err;
}

int lfs2_mount(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    LFS2_TRACE("lfs2_mount(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .block_count=%"PRIu32", "
                ".block_cycles=%"PRIu32", .cache_size=%"PRIu32", "
                ".lookahead_size=%"PRIu32", .read_buffer=%p, "
                ".prog_buffer=%p, .lookahead_buffer=%p, "
                ".name_max=%"PRIu32", .file_max=%"PRIu32", "
                ".attr_max=%"PRIu32"})",
            (void*)lfs2, (void*)cfg, cfg->context,
            (void*)(uintptr_t)cfg->read, (void*)(uintptr_t)cfg->prog,
            (void*)(uintptr_t)cfg->erase, (void*)(uintptr_t)cfg->sync,
            cfg->read_size, cfg->prog_size, cfg->block_size, cfg->block_count,
            cfg->block_cycles, cfg->cache_size, cfg->lookahead_size,
            cfg->read_buffer, cfg->prog_buffer, cfg->lookahead_buffer,
            cfg->name_max, cfg->file_max, cfg->attr_max);
    int err = lfs2_init(lfs2, cfg);
    if (err) {
        LFS2_TRACE("lfs2_mount -> %d", err);
        return err;
    }

    // scan directory blocks for superblock and any global updates
    lfs2_mdir_t dir = {.tail = {0, 1}};
    while (!lfs2_pair_isnull(dir.tail)) {
        // fetch next block in tail list
        lfs2_stag_t tag = lfs2_dir_fetchmatch(lfs2, &dir, dir.tail,
                LFS2_MKTAG(0x7ff, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_SUPERBLOCK, 0, 8),
                NULL,
                lfs2_dir_find_match, &(struct lfs2_dir_find_match){
                    lfs2, "littlefs", 8});
        if (tag < 0) {
            err = tag;
            goto cleanup;
        }

        // has superblock?
        if (tag && !lfs2_tag_isdelete(tag)) {
            // update root
            lfs2->root[0] = dir.pair[0];
            lfs2->root[1] = dir.pair[1];

            // grab superblock
            lfs2_superblock_t superblock;
            tag = lfs2_dir_get(lfs2, &dir, LFS2_MKTAG(0x7ff, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                    &superblock);
            if (tag < 0) {
                err = tag;
                goto cleanup;
            }
            lfs2_superblock_fromle32(&superblock);

            // check version
            uint16_t major_version = (0xffff & (superblock.version >> 16));
            uint16_t minor_version = (0xffff & (superblock.version >>  0));
            if ((major_version != LFS2_DISK_VERSION_MAJOR ||
                 minor_version > LFS2_DISK_VERSION_MINOR)) {
                LFS2_ERROR("Invalid version %"PRIu16".%"PRIu16,
                        major_version, minor_version);
                err = LFS2_ERR_INVAL;
                goto cleanup;
            }

            // check superblock configuration
            if (superblock.name_max) {
                if (superblock.name_max > lfs2->name_max) {
                    LFS2_ERROR("Unsupported name_max (%"PRIu32" > %"PRIu32")",
                            superblock.name_max, lfs2->name_max);
                    err = LFS2_ERR_INVAL;
                    goto cleanup;
                }

                lfs2->name_max = superblock.name_max;
            }

            if (superblock.file_max) {
                if (superblock.file_max > lfs2->file_max) {
                    LFS2_ERROR("Unsupported file_max (%"PRIu32" > %"PRIu32")",
                            superblock.file_max, lfs2->file_max);
                    err = LFS2_ERR_INVAL;
                    goto cleanup;
                }

                lfs2->file_max = superblock.file_max;
            }

            if (superblock.attr_max) {
                if (superblock.attr_max > lfs2->attr_max) {
                    LFS2_ERROR("Unsupported attr_max (%"PRIu32" > %"PRIu32")",
                            superblock.attr_max, lfs2->attr_max);
                    err = LFS2_ERR_INVAL;
                    goto cleanup;
                }

                lfs2->attr_max = superblock.attr_max;
            }
        }

        // has gstate?
        err = lfs2_dir_getgstate(lfs2, &dir, &lfs2->gpending);
        if (err) {
            goto cleanup;
        }
    }

    // found superblock?
    if (lfs2_pair_isnull(lfs2->root)) {
        err = LFS2_ERR_INVAL;
        goto cleanup;
    }

    // update littlefs with gstate
    lfs2->gpending.tag += !lfs2_tag_isvalid(lfs2->gpending.tag);
    lfs2->gstate = lfs2->gpending;
    if (lfs2_gstate_hasmove(&lfs2->gstate)) {
        LFS2_DEBUG("Found move %"PRIx32" %"PRIx32" %"PRIx16,
                lfs2->gstate.pair[0],
                lfs2->gstate.pair[1],
                lfs2_tag_id(lfs2->gstate.tag));
    }

    // setup free lookahead
    lfs2->free.off = lfs2->seed % lfs2->cfg->block_size;
    lfs2->free.size = 0;
    lfs2->free.i = 0;
    lfs2_alloc_ack(lfs2);

    LFS2_TRACE("lfs2_mount -> %d", 0);
    return 0;

cleanup:
    lfs2_unmount(lfs2);
    LFS2_TRACE("lfs2_mount -> %d", err);
    return err;
}

int lfs2_unmount(lfs2_t *lfs2) {
    LFS2_TRACE("lfs2_unmount(%p)", (void*)lfs2);
    int err = lfs2_deinit(lfs2);
    LFS2_TRACE("lfs2_unmount -> %d", err);
    return err;
}


/// Filesystem filesystem operations ///
int lfs2_fs_traverse(lfs2_t *lfs2,
        int (*cb)(void *data, lfs2_block_t block), void *data) {
    LFS2_TRACE("lfs2_fs_traverse(%p, %p, %p)",
            (void*)lfs2, (void*)(uintptr_t)cb, data);
    // iterate over metadata pairs
    lfs2_mdir_t dir = {.tail = {0, 1}};

#ifdef LFS2_MIGRATE
    // also consider v1 blocks during migration
    if (lfs2->lfs21) {
        int err = lfs21_traverse(lfs2, cb, data);
        if (err) {
            LFS2_TRACE("lfs2_fs_traverse -> %d", err);
            return err;
        }

        dir.tail[0] = lfs2->root[0];
        dir.tail[1] = lfs2->root[1];
    }
#endif

    while (!lfs2_pair_isnull(dir.tail)) {
        for (int i = 0; i < 2; i++) {
            int err = cb(data, dir.tail[i]);
            if (err) {
                LFS2_TRACE("lfs2_fs_traverse -> %d", err);
                return err;
            }
        }

        // iterate through ids in directory
        int err = lfs2_dir_fetch(lfs2, &dir, dir.tail);
        if (err) {
            LFS2_TRACE("lfs2_fs_traverse -> %d", err);
            return err;
        }

        for (uint16_t id = 0; id < dir.count; id++) {
            struct lfs2_ctz ctz;
            lfs2_stag_t tag = lfs2_dir_get(lfs2, &dir, LFS2_MKTAG(0x700, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_STRUCT, id, sizeof(ctz)), &ctz);
            if (tag < 0) {
                if (tag == LFS2_ERR_NOENT) {
                    continue;
                }
                LFS2_TRACE("lfs2_fs_traverse -> %d", tag);
                return tag;
            }
            lfs2_ctz_fromle32(&ctz);

            if (lfs2_tag_type3(tag) == LFS2_TYPE_CTZSTRUCT) {
                err = lfs2_ctz_traverse(lfs2, NULL, &lfs2->rcache,
                        ctz.head, ctz.size, cb, data);
                if (err) {
                    LFS2_TRACE("lfs2_fs_traverse -> %d", err);
                    return err;
                }
            }
        }
    }

    // iterate over any open files
    for (lfs2_file_t *f = (lfs2_file_t*)lfs2->mlist; f; f = f->next) {
        if (f->type != LFS2_TYPE_REG) {
            continue;
        }

        if ((f->flags & LFS2_F_DIRTY) && !(f->flags & LFS2_F_INLINE)) {
            int err = lfs2_ctz_traverse(lfs2, &f->cache, &lfs2->rcache,
                    f->ctz.head, f->ctz.size, cb, data);
            if (err) {
                LFS2_TRACE("lfs2_fs_traverse -> %d", err);
                return err;
            }
        }

        if ((f->flags & LFS2_F_WRITING) && !(f->flags & LFS2_F_INLINE)) {
            int err = lfs2_ctz_traverse(lfs2, &f->cache, &lfs2->rcache,
                    f->block, f->pos, cb, data);
            if (err) {
                LFS2_TRACE("lfs2_fs_traverse -> %d", err);
                return err;
            }
        }
    }

    LFS2_TRACE("lfs2_fs_traverse -> %d", 0);
    return 0;
}

static int lfs2_fs_pred(lfs2_t *lfs2,
        const lfs2_block_t pair[2], lfs2_mdir_t *pdir) {
    // iterate over all directory directory entries
    pdir->tail[0] = 0;
    pdir->tail[1] = 1;
    while (!lfs2_pair_isnull(pdir->tail)) {
        if (lfs2_pair_cmp(pdir->tail, pair) == 0) {
            return 0;
        }

        int err = lfs2_dir_fetch(lfs2, pdir, pdir->tail);
        if (err) {
            return err;
        }
    }

    return LFS2_ERR_NOENT;
}

struct lfs2_fs_parent_match {
    lfs2_t *lfs2;
    const lfs2_block_t pair[2];
};

static int lfs2_fs_parent_match(void *data,
        lfs2_tag_t tag, const void *buffer) {
    struct lfs2_fs_parent_match *find = data;
    lfs2_t *lfs2 = find->lfs2;
    const struct lfs2_diskoff *disk = buffer;
    (void)tag;

    lfs2_block_t child[2];
    int err = lfs2_bd_read(lfs2,
            &lfs2->pcache, &lfs2->rcache, lfs2->cfg->block_size,
            disk->block, disk->off, &child, sizeof(child));
    if (err) {
        return err;
    }

    lfs2_pair_fromle32(child);
    return (lfs2_pair_cmp(child, find->pair) == 0) ? LFS2_CMP_EQ : LFS2_CMP_LT;
}

static lfs2_stag_t lfs2_fs_parent(lfs2_t *lfs2, const lfs2_block_t pair[2],
        lfs2_mdir_t *parent) {
    // use fetchmatch with callback to find pairs
    parent->tail[0] = 0;
    parent->tail[1] = 1;
    while (!lfs2_pair_isnull(parent->tail)) {
        lfs2_stag_t tag = lfs2_dir_fetchmatch(lfs2, parent, parent->tail,
                LFS2_MKTAG(0x7ff, 0, 0x3ff),
                LFS2_MKTAG(LFS2_TYPE_DIRSTRUCT, 0, 8),
                NULL,
                lfs2_fs_parent_match, &(struct lfs2_fs_parent_match){
                    lfs2, {pair[0], pair[1]}});
        if (tag && tag != LFS2_ERR_NOENT) {
            return tag;
        }
    }

    return LFS2_ERR_NOENT;
}

static int lfs2_fs_relocate(lfs2_t *lfs2,
        const lfs2_block_t oldpair[2], lfs2_block_t newpair[2]) {
    // update internal root
    if (lfs2_pair_cmp(oldpair, lfs2->root) == 0) {
        LFS2_DEBUG("Relocating root %"PRIx32" %"PRIx32,
                newpair[0], newpair[1]);
        lfs2->root[0] = newpair[0];
        lfs2->root[1] = newpair[1];
    }

    // update internally tracked dirs
    for (struct lfs2_mlist *d = lfs2->mlist; d; d = d->next) {
        if (lfs2_pair_cmp(oldpair, d->m.pair) == 0) {
            d->m.pair[0] = newpair[0];
            d->m.pair[1] = newpair[1];
        }
    }

    // find parent
    lfs2_mdir_t parent;
    lfs2_stag_t tag = lfs2_fs_parent(lfs2, oldpair, &parent);
    if (tag < 0 && tag != LFS2_ERR_NOENT) {
        return tag;
    }

    if (tag != LFS2_ERR_NOENT) {
        // update disk, this creates a desync
        lfs2_fs_preporphans(lfs2, +1);

        lfs2_pair_tole32(newpair);
        int err = lfs2_dir_commit(lfs2, &parent, LFS2_MKATTRS({tag, newpair}));
        lfs2_pair_fromle32(newpair);
        if (err) {
            return err;
        }

        // next step, clean up orphans
        lfs2_fs_preporphans(lfs2, -1);
    }

    // find pred
    int err = lfs2_fs_pred(lfs2, oldpair, &parent);
    if (err && err != LFS2_ERR_NOENT) {
        return err;
    }

    // if we can't find dir, it must be new
    if (err != LFS2_ERR_NOENT) {
        // replace bad pair, either we clean up desync, or no desync occured
        lfs2_pair_tole32(newpair);
        err = lfs2_dir_commit(lfs2, &parent, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_TAIL + parent.split, 0x3ff, 8), newpair}));
        lfs2_pair_fromle32(newpair);
        if (err) {
            return err;
        }
    }

    return 0;
}

static void lfs2_fs_preporphans(lfs2_t *lfs2, int8_t orphans) {
    lfs2->gpending.tag += orphans;
    lfs2_gstate_xororphans(&lfs2->gdelta,   &lfs2->gpending,
            lfs2_gstate_hasorphans(&lfs2->gpending));
    lfs2_gstate_xororphans(&lfs2->gpending, &lfs2->gpending,
            lfs2_gstate_hasorphans(&lfs2->gpending));
}

static void lfs2_fs_prepmove(lfs2_t *lfs2,
        uint16_t id, const lfs2_block_t pair[2]) {
    lfs2_gstate_xormove(&lfs2->gdelta,   &lfs2->gpending, id, pair);
    lfs2_gstate_xormove(&lfs2->gpending, &lfs2->gpending, id, pair);
}


static int lfs2_fs_demove(lfs2_t *lfs2) {
    if (!lfs2_gstate_hasmove(&lfs2->gstate)) {
        return 0;
    }

    // Fix bad moves
    LFS2_DEBUG("Fixing move %"PRIx32" %"PRIx32" %"PRIx16,
            lfs2->gstate.pair[0],
            lfs2->gstate.pair[1],
            lfs2_tag_id(lfs2->gstate.tag));

    // fetch and delete the moved entry
    lfs2_mdir_t movedir;
    int err = lfs2_dir_fetch(lfs2, &movedir, lfs2->gstate.pair);
    if (err) {
        return err;
    }

    // rely on cancel logic inside commit
    err = lfs2_dir_commit(lfs2, &movedir, NULL, 0);
    if (err) {
        return err;
    }

    return 0;
}

static int lfs2_fs_deorphan(lfs2_t *lfs2) {
    if (!lfs2_gstate_hasorphans(&lfs2->gstate)) {
        return 0;
    }

    // Fix any orphans
    lfs2_mdir_t pdir = {.split = true};
    lfs2_mdir_t dir = {.tail = {0, 1}};

    // iterate over all directory directory entries
    while (!lfs2_pair_isnull(dir.tail)) {
        int err = lfs2_dir_fetch(lfs2, &dir, dir.tail);
        if (err) {
            return err;
        }

        // check head blocks for orphans
        if (!pdir.split) {
            // check if we have a parent
            lfs2_mdir_t parent;
            lfs2_stag_t tag = lfs2_fs_parent(lfs2, pdir.tail, &parent);
            if (tag < 0 && tag != LFS2_ERR_NOENT) {
                return tag;
            }

            if (tag == LFS2_ERR_NOENT) {
                // we are an orphan
                LFS2_DEBUG("Fixing orphan %"PRIx32" %"PRIx32,
                        pdir.tail[0], pdir.tail[1]);

                err = lfs2_dir_drop(lfs2, &pdir, &dir);
                if (err) {
                    return err;
                }

                break;
            }

            lfs2_block_t pair[2];
            lfs2_stag_t res = lfs2_dir_get(lfs2, &parent,
                    LFS2_MKTAG(0x7ff, 0x3ff, 0), tag, pair);
            if (res < 0) {
                return res;
            }
            lfs2_pair_fromle32(pair);

            if (!lfs2_pair_sync(pair, pdir.tail)) {
                // we have desynced
                LFS2_DEBUG("Fixing half-orphan %"PRIx32" %"PRIx32,
                        pair[0], pair[1]);

                lfs2_pair_tole32(pair);
                err = lfs2_dir_commit(lfs2, &pdir, LFS2_MKATTRS(
                        {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8), pair}));
                lfs2_pair_fromle32(pair);
                if (err) {
                    return err;
                }

                break;
            }
        }

        memcpy(&pdir, &dir, sizeof(pdir));
    }

    // mark orphans as fixed
    lfs2_fs_preporphans(lfs2, -lfs2_gstate_getorphans(&lfs2->gstate));
    lfs2->gstate = lfs2->gpending;
    return 0;
}

static int lfs2_fs_forceconsistency(lfs2_t *lfs2) {
    int err = lfs2_fs_demove(lfs2);
    if (err) {
        return err;
    }

    err = lfs2_fs_deorphan(lfs2);
    if (err) {
        return err;
    }

    return 0;
}

static int lfs2_fs_size_count(void *p, lfs2_block_t block) {
    (void)block;
    lfs2_size_t *size = p;
    *size += 1;
    return 0;
}

lfs2_ssize_t lfs2_fs_size(lfs2_t *lfs2) {
    LFS2_TRACE("lfs2_fs_size(%p)", (void*)lfs2);
    lfs2_size_t size = 0;
    int err = lfs2_fs_traverse(lfs2, lfs2_fs_size_count, &size);
    if (err) {
        LFS2_TRACE("lfs2_fs_size -> %"PRId32, err);
        return err;
    }

    LFS2_TRACE("lfs2_fs_size -> %"PRId32, err);
    return size;
}

#ifdef LFS2_MIGRATE
////// Migration from littelfs v1 below this //////

/// Version info ///

// Software library version
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS21_VERSION 0x00010007
#define LFS21_VERSION_MAJOR (0xffff & (LFS21_VERSION >> 16))
#define LFS21_VERSION_MINOR (0xffff & (LFS21_VERSION >>  0))

// Version of On-disk data structures
// Major (top-nibble), incremented on backwards incompatible changes
// Minor (bottom-nibble), incremented on feature additions
#define LFS21_DISK_VERSION 0x00010001
#define LFS21_DISK_VERSION_MAJOR (0xffff & (LFS21_DISK_VERSION >> 16))
#define LFS21_DISK_VERSION_MINOR (0xffff & (LFS21_DISK_VERSION >>  0))


/// v1 Definitions ///

// File types
enum lfs21_type {
    LFS21_TYPE_REG        = 0x11,
    LFS21_TYPE_DIR        = 0x22,
    LFS21_TYPE_SUPERBLOCK = 0x2e,
};

typedef struct lfs21 {
    lfs2_block_t root[2];
} lfs21_t;

typedef struct lfs21_entry {
    lfs2_off_t off;

    struct lfs21_disk_entry {
        uint8_t type;
        uint8_t elen;
        uint8_t alen;
        uint8_t nlen;
        union {
            struct {
                lfs2_block_t head;
                lfs2_size_t size;
            } file;
            lfs2_block_t dir[2];
        } u;
    } d;
} lfs21_entry_t;

typedef struct lfs21_dir {
    struct lfs21_dir *next;
    lfs2_block_t pair[2];
    lfs2_off_t off;

    lfs2_block_t head[2];
    lfs2_off_t pos;

    struct lfs21_disk_dir {
        uint32_t rev;
        lfs2_size_t size;
        lfs2_block_t tail[2];
    } d;
} lfs21_dir_t;

typedef struct lfs21_superblock {
    lfs2_off_t off;

    struct lfs21_disk_superblock {
        uint8_t type;
        uint8_t elen;
        uint8_t alen;
        uint8_t nlen;
        lfs2_block_t root[2];
        uint32_t block_size;
        uint32_t block_count;
        uint32_t version;
        char magic[8];
    } d;
} lfs21_superblock_t;


/// Low-level wrappers v1->v2 ///
static void lfs21_crc(uint32_t *crc, const void *buffer, size_t size) {
    *crc = lfs2_crc(*crc, buffer, size);
}

static int lfs21_bd_read(lfs2_t *lfs2, lfs2_block_t block,
        lfs2_off_t off, void *buffer, lfs2_size_t size) {
    // if we ever do more than writes to alternating pairs,
    // this may need to consider pcache
    return lfs2_bd_read(lfs2, &lfs2->pcache, &lfs2->rcache, size,
            block, off, buffer, size);
}

static int lfs21_bd_crc(lfs2_t *lfs2, lfs2_block_t block,
        lfs2_off_t off, lfs2_size_t size, uint32_t *crc) {
    for (lfs2_off_t i = 0; i < size; i++) {
        uint8_t c;
        int err = lfs21_bd_read(lfs2, block, off+i, &c, 1);
        if (err) {
            return err;
        }

        lfs21_crc(crc, &c, 1);
    }

    return 0;
}


/// Endian swapping functions ///
static void lfs21_dir_fromle32(struct lfs21_disk_dir *d) {
    d->rev     = lfs2_fromle32(d->rev);
    d->size    = lfs2_fromle32(d->size);
    d->tail[0] = lfs2_fromle32(d->tail[0]);
    d->tail[1] = lfs2_fromle32(d->tail[1]);
}

static void lfs21_dir_tole32(struct lfs21_disk_dir *d) {
    d->rev     = lfs2_tole32(d->rev);
    d->size    = lfs2_tole32(d->size);
    d->tail[0] = lfs2_tole32(d->tail[0]);
    d->tail[1] = lfs2_tole32(d->tail[1]);
}

static void lfs21_entry_fromle32(struct lfs21_disk_entry *d) {
    d->u.dir[0] = lfs2_fromle32(d->u.dir[0]);
    d->u.dir[1] = lfs2_fromle32(d->u.dir[1]);
}

static void lfs21_entry_tole32(struct lfs21_disk_entry *d) {
    d->u.dir[0] = lfs2_tole32(d->u.dir[0]);
    d->u.dir[1] = lfs2_tole32(d->u.dir[1]);
}

static void lfs21_superblock_fromle32(struct lfs21_disk_superblock *d) {
    d->root[0]     = lfs2_fromle32(d->root[0]);
    d->root[1]     = lfs2_fromle32(d->root[1]);
    d->block_size  = lfs2_fromle32(d->block_size);
    d->block_count = lfs2_fromle32(d->block_count);
    d->version     = lfs2_fromle32(d->version);
}


///// Metadata pair and directory operations ///
static inline lfs2_size_t lfs21_entry_size(const lfs21_entry_t *entry) {
    return 4 + entry->d.elen + entry->d.alen + entry->d.nlen;
}

static int lfs21_dir_fetch(lfs2_t *lfs2,
        lfs21_dir_t *dir, const lfs2_block_t pair[2]) {
    // copy out pair, otherwise may be aliasing dir
    const lfs2_block_t tpair[2] = {pair[0], pair[1]};
    bool valid = false;

    // check both blocks for the most recent revision
    for (int i = 0; i < 2; i++) {
        struct lfs21_disk_dir test;
        int err = lfs21_bd_read(lfs2, tpair[i], 0, &test, sizeof(test));
        lfs21_dir_fromle32(&test);
        if (err) {
            if (err == LFS2_ERR_CORRUPT) {
                continue;
            }
            return err;
        }

        if (valid && lfs2_scmp(test.rev, dir->d.rev) < 0) {
            continue;
        }

        if ((0x7fffffff & test.size) < sizeof(test)+4 ||
            (0x7fffffff & test.size) > lfs2->cfg->block_size) {
            continue;
        }

        uint32_t crc = LFS2_BLOCK_NULL;
        lfs21_dir_tole32(&test);
        lfs21_crc(&crc, &test, sizeof(test));
        lfs21_dir_fromle32(&test);
        err = lfs21_bd_crc(lfs2, tpair[i], sizeof(test),
                (0x7fffffff & test.size) - sizeof(test), &crc);
        if (err) {
            if (err == LFS2_ERR_CORRUPT) {
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
        LFS2_ERROR("Corrupted dir pair at %" PRIx32 " %" PRIx32 ,
                tpair[0], tpair[1]);
        return LFS2_ERR_CORRUPT;
    }

    return 0;
}

static int lfs21_dir_next(lfs2_t *lfs2, lfs21_dir_t *dir, lfs21_entry_t *entry) {
    while (dir->off + sizeof(entry->d) > (0x7fffffff & dir->d.size)-4) {
        if (!(0x80000000 & dir->d.size)) {
            entry->off = dir->off;
            return LFS2_ERR_NOENT;
        }

        int err = lfs21_dir_fetch(lfs2, dir, dir->d.tail);
        if (err) {
            return err;
        }

        dir->off = sizeof(dir->d);
        dir->pos += sizeof(dir->d) + 4;
    }

    int err = lfs21_bd_read(lfs2, dir->pair[0], dir->off,
            &entry->d, sizeof(entry->d));
    lfs21_entry_fromle32(&entry->d);
    if (err) {
        return err;
    }

    entry->off = dir->off;
    dir->off += lfs21_entry_size(entry);
    dir->pos += lfs21_entry_size(entry);
    return 0;
}

/// littlefs v1 specific operations ///
int lfs21_traverse(lfs2_t *lfs2, int (*cb)(void*, lfs2_block_t), void *data) {
    if (lfs2_pair_isnull(lfs2->lfs21->root)) {
        return 0;
    }

    // iterate over metadata pairs
    lfs21_dir_t dir;
    lfs21_entry_t entry;
    lfs2_block_t cwd[2] = {0, 1};

    while (true) {
        for (int i = 0; i < 2; i++) {
            int err = cb(data, cwd[i]);
            if (err) {
                return err;
            }
        }

        int err = lfs21_dir_fetch(lfs2, &dir, cwd);
        if (err) {
            return err;
        }

        // iterate over contents
        while (dir.off + sizeof(entry.d) <= (0x7fffffff & dir.d.size)-4) {
            err = lfs21_bd_read(lfs2, dir.pair[0], dir.off,
                    &entry.d, sizeof(entry.d));
            lfs21_entry_fromle32(&entry.d);
            if (err) {
                return err;
            }

            dir.off += lfs21_entry_size(&entry);
            if ((0x70 & entry.d.type) == (0x70 & LFS21_TYPE_REG)) {
                err = lfs2_ctz_traverse(lfs2, NULL, &lfs2->rcache,
                        entry.d.u.file.head, entry.d.u.file.size, cb, data);
                if (err) {
                    return err;
                }
            }
        }

        // we also need to check if we contain a threaded v2 directory
        lfs2_mdir_t dir2 = {.split=true, .tail={cwd[0], cwd[1]}};
        while (dir2.split) {
            err = lfs2_dir_fetch(lfs2, &dir2, dir2.tail);
            if (err) {
                break;
            }

            for (int i = 0; i < 2; i++) {
                err = cb(data, dir2.pair[i]);
                if (err) {
                    return err;
                }
            }
        }

        cwd[0] = dir.d.tail[0];
        cwd[1] = dir.d.tail[1];

        if (lfs2_pair_isnull(cwd)) {
            break;
        }
    }

    return 0;
}

static int lfs21_moved(lfs2_t *lfs2, const void *e) {
    if (lfs2_pair_isnull(lfs2->lfs21->root)) {
        return 0;
    }

    // skip superblock
    lfs21_dir_t cwd;
    int err = lfs21_dir_fetch(lfs2, &cwd, (const lfs2_block_t[2]){0, 1});
    if (err) {
        return err;
    }

    // iterate over all directory directory entries
    lfs21_entry_t entry;
    while (!lfs2_pair_isnull(cwd.d.tail)) {
        err = lfs21_dir_fetch(lfs2, &cwd, cwd.d.tail);
        if (err) {
            return err;
        }

        while (true) {
            err = lfs21_dir_next(lfs2, &cwd, &entry);
            if (err && err != LFS2_ERR_NOENT) {
                return err;
            }

            if (err == LFS2_ERR_NOENT) {
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

/// Filesystem operations ///
static int lfs21_mount(lfs2_t *lfs2, struct lfs21 *lfs21,
        const struct lfs2_config *cfg) {
    int err = 0;
    {
        err = lfs2_init(lfs2, cfg);
        if (err) {
            return err;
        }

        lfs2->lfs21 = lfs21;
        lfs2->lfs21->root[0] = LFS2_BLOCK_NULL;
        lfs2->lfs21->root[1] = LFS2_BLOCK_NULL;

        // setup free lookahead
        lfs2->free.off = 0;
        lfs2->free.size = 0;
        lfs2->free.i = 0;
        lfs2_alloc_ack(lfs2);

        // load superblock
        lfs21_dir_t dir;
        lfs21_superblock_t superblock;
        err = lfs21_dir_fetch(lfs2, &dir, (const lfs2_block_t[2]){0, 1});
        if (err && err != LFS2_ERR_CORRUPT) {
            goto cleanup;
        }

        if (!err) {
            err = lfs21_bd_read(lfs2, dir.pair[0], sizeof(dir.d),
                    &superblock.d, sizeof(superblock.d));
            lfs21_superblock_fromle32(&superblock.d);
            if (err) {
                goto cleanup;
            }

            lfs2->lfs21->root[0] = superblock.d.root[0];
            lfs2->lfs21->root[1] = superblock.d.root[1];
        }

        if (err || memcmp(superblock.d.magic, "littlefs", 8) != 0) {
            LFS2_ERROR("Invalid superblock at %d %d", 0, 1);
            err = LFS2_ERR_CORRUPT;
            goto cleanup;
        }

        uint16_t major_version = (0xffff & (superblock.d.version >> 16));
        uint16_t minor_version = (0xffff & (superblock.d.version >>  0));
        if ((major_version != LFS21_DISK_VERSION_MAJOR ||
             minor_version > LFS21_DISK_VERSION_MINOR)) {
            LFS2_ERROR("Invalid version %d.%d", major_version, minor_version);
            err = LFS2_ERR_INVAL;
            goto cleanup;
        }

        return 0;
    }

cleanup:
    lfs2_deinit(lfs2);
    return err;
}

static int lfs21_unmount(lfs2_t *lfs2) {
    return lfs2_deinit(lfs2);
}

/// v1 migration ///
int lfs2_migrate(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    LFS2_TRACE("lfs2_migrate(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .block_count=%"PRIu32", "
                ".block_cycles=%"PRIu32", .cache_size=%"PRIu32", "
                ".lookahead_size=%"PRIu32", .read_buffer=%p, "
                ".prog_buffer=%p, .lookahead_buffer=%p, "
                ".name_max=%"PRIu32", .file_max=%"PRIu32", "
                ".attr_max=%"PRIu32"})",
            (void*)lfs2, (void*)cfg, cfg->context,
            (void*)(uintptr_t)cfg->read, (void*)(uintptr_t)cfg->prog,
            (void*)(uintptr_t)cfg->erase, (void*)(uintptr_t)cfg->sync,
            cfg->read_size, cfg->prog_size, cfg->block_size, cfg->block_count,
            cfg->block_cycles, cfg->cache_size, cfg->lookahead_size,
            cfg->read_buffer, cfg->prog_buffer, cfg->lookahead_buffer,
            cfg->name_max, cfg->file_max, cfg->attr_max);
    struct lfs21 lfs21;
    int err = lfs21_mount(lfs2, &lfs21, cfg);
    if (err) {
        LFS2_TRACE("lfs2_migrate -> %d", err);
        return err;
    }

    {
        // iterate through each directory, copying over entries
        // into new directory
        lfs21_dir_t dir1;
        lfs2_mdir_t dir2;
        dir1.d.tail[0] = lfs2->lfs21->root[0];
        dir1.d.tail[1] = lfs2->lfs21->root[1];
        while (!lfs2_pair_isnull(dir1.d.tail)) {
            // iterate old dir
            err = lfs21_dir_fetch(lfs2, &dir1, dir1.d.tail);
            if (err) {
                goto cleanup;
            }

            // create new dir and bind as temporary pretend root
            err = lfs2_dir_alloc(lfs2, &dir2);
            if (err) {
                goto cleanup;
            }

            dir2.rev = dir1.d.rev;
            dir1.head[0] = dir1.pair[0];
            dir1.head[1] = dir1.pair[1];
            lfs2->root[0] = dir2.pair[0];
            lfs2->root[1] = dir2.pair[1];

            err = lfs2_dir_commit(lfs2, &dir2, NULL, 0);
            if (err) {
                goto cleanup;
            }

            while (true) {
                lfs21_entry_t entry1;
                err = lfs21_dir_next(lfs2, &dir1, &entry1);
                if (err && err != LFS2_ERR_NOENT) {
                    goto cleanup;
                }

                if (err == LFS2_ERR_NOENT) {
                    break;
                }

                // check that entry has not been moved
                if (entry1.d.type & 0x80) {
                    int moved = lfs21_moved(lfs2, &entry1.d.u);
                    if (moved < 0) {
                        err = moved;
                        goto cleanup;
                    }

                    if (moved) {
                        continue;
                    }

                    entry1.d.type &= ~0x80;
                }

                // also fetch name
                char name[LFS2_NAME_MAX+1];
                memset(name, 0, sizeof(name));
                err = lfs21_bd_read(lfs2, dir1.pair[0],
                        entry1.off + 4+entry1.d.elen+entry1.d.alen,
                        name, entry1.d.nlen);
                if (err) {
                    goto cleanup;
                }

                bool isdir = (entry1.d.type == LFS21_TYPE_DIR);

                // create entry in new dir
                err = lfs2_dir_fetch(lfs2, &dir2, lfs2->root);
                if (err) {
                    goto cleanup;
                }

                uint16_t id;
                err = lfs2_dir_find(lfs2, &dir2, &(const char*){name}, &id);
                if (!(err == LFS2_ERR_NOENT && id != 0x3ff)) {
                    err = (err < 0) ? err : LFS2_ERR_EXIST;
                    goto cleanup;
                }

                lfs21_entry_tole32(&entry1.d);
                err = lfs2_dir_commit(lfs2, &dir2, LFS2_MKATTRS(
                        {LFS2_MKTAG(LFS2_TYPE_CREATE, id, 0), NULL},
                        {LFS2_MKTAG(
                            isdir ? LFS2_TYPE_DIR : LFS2_TYPE_REG,
                            id, entry1.d.nlen), name},
                        {LFS2_MKTAG(
                            isdir ? LFS2_TYPE_DIRSTRUCT : LFS2_TYPE_CTZSTRUCT,
                            id, sizeof(&entry1.d.u)), &entry1.d.u}));
                lfs21_entry_fromle32(&entry1.d);
                if (err) {
                    goto cleanup;
                }
            }

            if (!lfs2_pair_isnull(dir1.d.tail)) {
                // find last block and update tail to thread into fs
                err = lfs2_dir_fetch(lfs2, &dir2, lfs2->root);
                if (err) {
                    goto cleanup;
                }

                while (dir2.split) {
                    err = lfs2_dir_fetch(lfs2, &dir2, dir2.tail);
                    if (err) {
                        goto cleanup;
                    }
                }

                lfs2_pair_tole32(dir2.pair);
                err = lfs2_dir_commit(lfs2, &dir2, LFS2_MKATTRS(
                        {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 0),
                            dir1.d.tail}));
                lfs2_pair_fromle32(dir2.pair);
                if (err) {
                    goto cleanup;
                }
            }

            // Copy over first block to thread into fs. Unfortunately
            // if this fails there is not much we can do.
            LFS2_DEBUG("Migrating %"PRIx32" %"PRIx32" -> %"PRIx32" %"PRIx32,
                    lfs2->root[0], lfs2->root[1], dir1.head[0], dir1.head[1]);

            err = lfs2_bd_erase(lfs2, dir1.head[1]);
            if (err) {
                goto cleanup;
            }

            err = lfs2_dir_fetch(lfs2, &dir2, lfs2->root);
            if (err) {
                goto cleanup;
            }

            for (lfs2_off_t i = 0; i < dir2.off; i++) {
                uint8_t dat;
                err = lfs2_bd_read(lfs2,
                        NULL, &lfs2->rcache, dir2.off,
                        dir2.pair[0], i, &dat, 1);
                if (err) {
                    goto cleanup;
                }

                err = lfs2_bd_prog(lfs2,
                        &lfs2->pcache, &lfs2->rcache, true,
                        dir1.head[1], i, &dat, 1);
                if (err) {
                    goto cleanup;
                }
            }

            err = lfs2_bd_flush(lfs2, &lfs2->pcache, &lfs2->rcache, true);
            if (err) {
                goto cleanup;
            }
        }

        // Create new superblock. This marks a successful migration!
        err = lfs21_dir_fetch(lfs2, &dir1, (const lfs2_block_t[2]){0, 1});
        if (err) {
            goto cleanup;
        }

        dir2.pair[0] = dir1.pair[0];
        dir2.pair[1] = dir1.pair[1];
        dir2.rev = dir1.d.rev;
        dir2.off = sizeof(dir2.rev);
        dir2.etag = LFS2_BLOCK_NULL;
        dir2.count = 0;
        dir2.tail[0] = lfs2->lfs21->root[0];
        dir2.tail[1] = lfs2->lfs21->root[1];
        dir2.erased = false;
        dir2.split = true;

        lfs2_superblock_t superblock = {
            .version     = LFS2_DISK_VERSION,
            .block_size  = lfs2->cfg->block_size,
            .block_count = lfs2->cfg->block_count,
            .name_max    = lfs2->name_max,
            .file_max    = lfs2->file_max,
            .attr_max    = lfs2->attr_max,
        };

        lfs2_superblock_tole32(&superblock);
        err = lfs2_dir_commit(lfs2, &dir2, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_CREATE, 0, 0), NULL},
                {LFS2_MKTAG(LFS2_TYPE_SUPERBLOCK, 0, 8), "littlefs"},
                {LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                    &superblock}));
        if (err) {
            goto cleanup;
        }

        // sanity check that fetch works
        err = lfs2_dir_fetch(lfs2, &dir2, (const lfs2_block_t[2]){0, 1});
        if (err) {
            goto cleanup;
        }

        // force compaction to prevent accidentally mounting v1
        dir2.erased = false;
        err = lfs2_dir_commit(lfs2, &dir2, NULL, 0);
        if (err) {
            goto cleanup;
        }
    }

cleanup:
    lfs21_unmount(lfs2);
    LFS2_TRACE("lfs2_migrate -> %d", err);
    return err;
}

#endif
