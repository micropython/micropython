/*
 * The little filesystem
 *
 * Copyright (c) 2022, The littlefs authors.
 * Copyright (c) 2017, Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "lfs2.h"
#include "lfs2_util.h"


// some constants used throughout the code
#define LFS2_BLOCK_NULL ((lfs2_block_t)-1)
#define LFS2_BLOCK_INLINE ((lfs2_block_t)-2)

enum {
    LFS2_OK_RELOCATED = 1,
    LFS2_OK_DROPPED   = 2,
    LFS2_OK_ORPHANED  = 3,
};

enum {
    LFS2_CMP_EQ = 0,
    LFS2_CMP_LT = 1,
    LFS2_CMP_GT = 2,
};


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
    if (off+size > lfs2->cfg->block_size
            || (lfs2->block_count && block >= lfs2->block_count)) {
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

        if (size >= hint && off % lfs2->cfg->read_size == 0 &&
                size >= lfs2->cfg->read_size) {
            // bypass cache?
            diff = lfs2_aligndown(diff, lfs2->cfg->read_size);
            int err = lfs2->cfg->read(lfs2->cfg, block, off, data, diff);
            if (err) {
                return err;
            }

            data += diff;
            off += diff;
            size -= diff;
            continue;
        }

        // load to cache, first condition can no longer fail
        LFS2_ASSERT(!lfs2->block_count || block < lfs2->block_count);
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

static int lfs2_bd_cmp(lfs2_t *lfs2,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache, lfs2_size_t hint,
        lfs2_block_t block, lfs2_off_t off,
        const void *buffer, lfs2_size_t size) {
    const uint8_t *data = buffer;
    lfs2_size_t diff = 0;

    for (lfs2_off_t i = 0; i < size; i += diff) {
        uint8_t dat[8];

        diff = lfs2_min(size-i, sizeof(dat));
        int err = lfs2_bd_read(lfs2,
                pcache, rcache, hint-i,
                block, off+i, &dat, diff);
        if (err) {
            return err;
        }

        int res = memcmp(dat, data + i, diff);
        if (res) {
            return res < 0 ? LFS2_CMP_LT : LFS2_CMP_GT;
        }
    }

    return LFS2_CMP_EQ;
}

static int lfs2_bd_crc(lfs2_t *lfs2,
        const lfs2_cache_t *pcache, lfs2_cache_t *rcache, lfs2_size_t hint,
        lfs2_block_t block, lfs2_off_t off, lfs2_size_t size, uint32_t *crc) {
    lfs2_size_t diff = 0;

    for (lfs2_off_t i = 0; i < size; i += diff) {
        uint8_t dat[8];
        diff = lfs2_min(size-i, sizeof(dat));
        int err = lfs2_bd_read(lfs2,
                pcache, rcache, hint-i,
                block, off+i, &dat, diff);
        if (err) {
            return err;
        }

        *crc = lfs2_crc(*crc, &dat, diff);
    }

    return 0;
}

#ifndef LFS2_READONLY
static int lfs2_bd_flush(lfs2_t *lfs2,
        lfs2_cache_t *pcache, lfs2_cache_t *rcache, bool validate) {
    if (pcache->block != LFS2_BLOCK_NULL && pcache->block != LFS2_BLOCK_INLINE) {
        LFS2_ASSERT(pcache->block < lfs2->block_count);
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
#endif

#ifndef LFS2_READONLY
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
#endif

#ifndef LFS2_READONLY
static int lfs2_bd_prog(lfs2_t *lfs2,
        lfs2_cache_t *pcache, lfs2_cache_t *rcache, bool validate,
        lfs2_block_t block, lfs2_off_t off,
        const void *buffer, lfs2_size_t size) {
    const uint8_t *data = buffer;
    LFS2_ASSERT(block == LFS2_BLOCK_INLINE || block < lfs2->block_count);
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
#endif

#ifndef LFS2_READONLY
static int lfs2_bd_erase(lfs2_t *lfs2, lfs2_block_t block) {
    LFS2_ASSERT(block < lfs2->block_count);
    int err = lfs2->cfg->erase(lfs2->cfg, block);
    LFS2_ASSERT(err <= 0);
    return err;
}
#endif


/// Small type-level utilities ///

// some operations on paths
static inline lfs2_size_t lfs2_path_namelen(const char *path) {
    return strcspn(path, "/");
}

static inline bool lfs2_path_islast(const char *path) {
    lfs2_size_t namelen = lfs2_path_namelen(path);
    return path[namelen + strspn(path + namelen, "/")] == '\0';
}

static inline bool lfs2_path_isdir(const char *path) {
    return path[lfs2_path_namelen(path)] != '\0';
}

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

static inline bool lfs2_pair_issync(
        const lfs2_block_t paira[2],
        const lfs2_block_t pairb[2]) {
    return (paira[0] == pairb[0] && paira[1] == pairb[1]) ||
           (paira[0] == pairb[1] && paira[1] == pairb[0]);
}

static inline void lfs2_pair_fromle32(lfs2_block_t pair[2]) {
    pair[0] = lfs2_fromle32(pair[0]);
    pair[1] = lfs2_fromle32(pair[1]);
}

#ifndef LFS2_READONLY
static inline void lfs2_pair_tole32(lfs2_block_t pair[2]) {
    pair[0] = lfs2_tole32(pair[0]);
    pair[1] = lfs2_tole32(pair[1]);
}
#endif

// operations on 32-bit entry tags
typedef uint32_t lfs2_tag_t;
typedef int32_t lfs2_stag_t;

#define LFS2_MKTAG(type, id, size) \
    (((lfs2_tag_t)(type) << 20) | ((lfs2_tag_t)(id) << 10) | (lfs2_tag_t)(size))

#define LFS2_MKTAG_IF(cond, type, id, size) \
    ((cond) ? LFS2_MKTAG(type, id, size) : LFS2_MKTAG(LFS2_FROM_NOOP, 0, 0))

#define LFS2_MKTAG_IF_ELSE(cond, type1, id1, size1, type2, id2, size2) \
    ((cond) ? LFS2_MKTAG(type1, id1, size1) : LFS2_MKTAG(type2, id2, size2))

static inline bool lfs2_tag_isvalid(lfs2_tag_t tag) {
    return !(tag & 0x80000000);
}

static inline bool lfs2_tag_isdelete(lfs2_tag_t tag) {
    return ((int32_t)(tag << 22) >> 22) == -1;
}

static inline uint16_t lfs2_tag_type1(lfs2_tag_t tag) {
    return (tag & 0x70000000) >> 20;
}

static inline uint16_t lfs2_tag_type2(lfs2_tag_t tag) {
    return (tag & 0x78000000) >> 20;
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
static inline void lfs2_gstate_xor(lfs2_gstate_t *a, const lfs2_gstate_t *b) {
    a->tag ^= b->tag;
    a->pair[0] ^= b->pair[0];
    a->pair[1] ^= b->pair[1];
}

static inline bool lfs2_gstate_iszero(const lfs2_gstate_t *a) {
    return a->tag == 0
            && a->pair[0] == 0
            && a->pair[1] == 0;
}

#ifndef LFS2_READONLY
static inline bool lfs2_gstate_hasorphans(const lfs2_gstate_t *a) {
    return lfs2_tag_size(a->tag);
}

static inline uint8_t lfs2_gstate_getorphans(const lfs2_gstate_t *a) {
    return lfs2_tag_size(a->tag) & 0x1ff;
}

static inline bool lfs2_gstate_hasmove(const lfs2_gstate_t *a) {
    return lfs2_tag_type1(a->tag);
}
#endif

static inline bool lfs2_gstate_needssuperblock(const lfs2_gstate_t *a) {
    return lfs2_tag_size(a->tag) >> 9;
}

static inline bool lfs2_gstate_hasmovehere(const lfs2_gstate_t *a,
        const lfs2_block_t *pair) {
    return lfs2_tag_type1(a->tag) && lfs2_pair_cmp(a->pair, pair) == 0;
}

static inline void lfs2_gstate_fromle32(lfs2_gstate_t *a) {
    a->tag     = lfs2_fromle32(a->tag);
    a->pair[0] = lfs2_fromle32(a->pair[0]);
    a->pair[1] = lfs2_fromle32(a->pair[1]);
}

#ifndef LFS2_READONLY
static inline void lfs2_gstate_tole32(lfs2_gstate_t *a) {
    a->tag     = lfs2_tole32(a->tag);
    a->pair[0] = lfs2_tole32(a->pair[0]);
    a->pair[1] = lfs2_tole32(a->pair[1]);
}
#endif

// operations on forward-CRCs used to track erased state
struct lfs2_fcrc {
    lfs2_size_t size;
    uint32_t crc;
};

static void lfs2_fcrc_fromle32(struct lfs2_fcrc *fcrc) {
    fcrc->size = lfs2_fromle32(fcrc->size);
    fcrc->crc = lfs2_fromle32(fcrc->crc);
}

#ifndef LFS2_READONLY
static void lfs2_fcrc_tole32(struct lfs2_fcrc *fcrc) {
    fcrc->size = lfs2_tole32(fcrc->size);
    fcrc->crc = lfs2_tole32(fcrc->crc);
}
#endif

// other endianness operations
static void lfs2_ctz_fromle32(struct lfs2_ctz *ctz) {
    ctz->head = lfs2_fromle32(ctz->head);
    ctz->size = lfs2_fromle32(ctz->size);
}

#ifndef LFS2_READONLY
static void lfs2_ctz_tole32(struct lfs2_ctz *ctz) {
    ctz->head = lfs2_tole32(ctz->head);
    ctz->size = lfs2_tole32(ctz->size);
}
#endif

static inline void lfs2_superblock_fromle32(lfs2_superblock_t *superblock) {
    superblock->version     = lfs2_fromle32(superblock->version);
    superblock->block_size  = lfs2_fromle32(superblock->block_size);
    superblock->block_count = lfs2_fromle32(superblock->block_count);
    superblock->name_max    = lfs2_fromle32(superblock->name_max);
    superblock->file_max    = lfs2_fromle32(superblock->file_max);
    superblock->attr_max    = lfs2_fromle32(superblock->attr_max);
}

#ifndef LFS2_READONLY
static inline void lfs2_superblock_tole32(lfs2_superblock_t *superblock) {
    superblock->version     = lfs2_tole32(superblock->version);
    superblock->block_size  = lfs2_tole32(superblock->block_size);
    superblock->block_count = lfs2_tole32(superblock->block_count);
    superblock->name_max    = lfs2_tole32(superblock->name_max);
    superblock->file_max    = lfs2_tole32(superblock->file_max);
    superblock->attr_max    = lfs2_tole32(superblock->attr_max);
}
#endif

#ifndef LFS2_NO_ASSERT
static bool lfs2_mlist_isopen(struct lfs2_mlist *head,
        struct lfs2_mlist *node) {
    for (struct lfs2_mlist **p = &head; *p; p = &(*p)->next) {
        if (*p == (struct lfs2_mlist*)node) {
            return true;
        }
    }

    return false;
}
#endif

static void lfs2_mlist_remove(lfs2_t *lfs2, struct lfs2_mlist *mlist) {
    for (struct lfs2_mlist **p = &lfs2->mlist; *p; p = &(*p)->next) {
        if (*p == mlist) {
            *p = (*p)->next;
            break;
        }
    }
}

static void lfs2_mlist_append(lfs2_t *lfs2, struct lfs2_mlist *mlist) {
    mlist->next = lfs2->mlist;
    lfs2->mlist = mlist;
}

// some other filesystem operations
static uint32_t lfs2_fs_disk_version(lfs2_t *lfs2) {
    (void)lfs2;
#ifdef LFS2_MULTIVERSION
    if (lfs2->cfg->disk_version) {
        return lfs2->cfg->disk_version;
    } else
#endif
    {
        return LFS2_DISK_VERSION;
    }
}

static uint16_t lfs2_fs_disk_version_major(lfs2_t *lfs2) {
    return 0xffff & (lfs2_fs_disk_version(lfs2) >> 16);

}

static uint16_t lfs2_fs_disk_version_minor(lfs2_t *lfs2) {
    return 0xffff & (lfs2_fs_disk_version(lfs2) >> 0);
}


/// Internal operations predeclared here ///
#ifndef LFS2_READONLY
static int lfs2_dir_commit(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const struct lfs2_mattr *attrs, int attrcount);
static int lfs2_dir_compact(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t begin, uint16_t end);
static lfs2_ssize_t lfs2_file_flushedwrite(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size);
static lfs2_ssize_t lfs2_file_write_(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size);
static int lfs2_file_sync_(lfs2_t *lfs2, lfs2_file_t *file);
static int lfs2_file_outline(lfs2_t *lfs2, lfs2_file_t *file);
static int lfs2_file_flush(lfs2_t *lfs2, lfs2_file_t *file);

static int lfs2_fs_deorphan(lfs2_t *lfs2, bool powerloss);
static int lfs2_fs_preporphans(lfs2_t *lfs2, int8_t orphans);
static void lfs2_fs_prepmove(lfs2_t *lfs2,
        uint16_t id, const lfs2_block_t pair[2]);
static int lfs2_fs_pred(lfs2_t *lfs2, const lfs2_block_t dir[2],
        lfs2_mdir_t *pdir);
static lfs2_stag_t lfs2_fs_parent(lfs2_t *lfs2, const lfs2_block_t dir[2],
        lfs2_mdir_t *parent);
static int lfs2_fs_forceconsistency(lfs2_t *lfs2);
#endif

static void lfs2_fs_prepsuperblock(lfs2_t *lfs2, bool needssuperblock);

#ifdef LFS2_MIGRATE
static int lfs21_traverse(lfs2_t *lfs2,
        int (*cb)(void*, lfs2_block_t), void *data);
#endif

static int lfs2_dir_rewind_(lfs2_t *lfs2, lfs2_dir_t *dir);

static lfs2_ssize_t lfs2_file_flushedread(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size);
static lfs2_ssize_t lfs2_file_read_(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size);
static int lfs2_file_close_(lfs2_t *lfs2, lfs2_file_t *file);
static lfs2_soff_t lfs2_file_size_(lfs2_t *lfs2, lfs2_file_t *file);

static lfs2_ssize_t lfs2_fs_size_(lfs2_t *lfs2);
static int lfs2_fs_traverse_(lfs2_t *lfs2,
        int (*cb)(void *data, lfs2_block_t block), void *data,
        bool includeorphans);

static int lfs2_deinit(lfs2_t *lfs2);
static int lfs2_unmount_(lfs2_t *lfs2);


/// Block allocator ///

// allocations should call this when all allocated blocks are committed to
// the filesystem
//
// after a checkpoint, the block allocator may realloc any untracked blocks
static void lfs2_alloc_ckpoint(lfs2_t *lfs2) {
    lfs2->lookahead.ckpoint = lfs2->block_count;
}

// drop the lookahead buffer, this is done during mounting and failed
// traversals in order to avoid invalid lookahead state
static void lfs2_alloc_drop(lfs2_t *lfs2) {
    lfs2->lookahead.size = 0;
    lfs2->lookahead.next = 0;
    lfs2_alloc_ckpoint(lfs2);
}

#ifndef LFS2_READONLY
static int lfs2_alloc_lookahead(void *p, lfs2_block_t block) {
    lfs2_t *lfs2 = (lfs2_t*)p;
    lfs2_block_t off = ((block - lfs2->lookahead.start)
            + lfs2->block_count) % lfs2->block_count;

    if (off < lfs2->lookahead.size) {
        lfs2->lookahead.buffer[off / 8] |= 1U << (off % 8);
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_alloc_scan(lfs2_t *lfs2) {
    // move lookahead buffer to the first unused block
    //
    // note we limit the lookahead buffer to at most the amount of blocks
    // checkpointed, this prevents the math in lfs2_alloc from underflowing
    lfs2->lookahead.start = (lfs2->lookahead.start + lfs2->lookahead.next) 
            % lfs2->block_count;
    lfs2->lookahead.next = 0;
    lfs2->lookahead.size = lfs2_min(
            8*lfs2->cfg->lookahead_size,
            lfs2->lookahead.ckpoint);

    // find mask of free blocks from tree
    memset(lfs2->lookahead.buffer, 0, lfs2->cfg->lookahead_size);
    int err = lfs2_fs_traverse_(lfs2, lfs2_alloc_lookahead, lfs2, true);
    if (err) {
        lfs2_alloc_drop(lfs2);
        return err;
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_alloc(lfs2_t *lfs2, lfs2_block_t *block) {
    while (true) {
        // scan our lookahead buffer for free blocks
        while (lfs2->lookahead.next < lfs2->lookahead.size) {
            if (!(lfs2->lookahead.buffer[lfs2->lookahead.next / 8]
                    & (1U << (lfs2->lookahead.next % 8)))) {
                // found a free block
                *block = (lfs2->lookahead.start + lfs2->lookahead.next)
                        % lfs2->block_count;

                // eagerly find next free block to maximize how many blocks
                // lfs2_alloc_ckpoint makes available for scanning
                while (true) {
                    lfs2->lookahead.next += 1;
                    lfs2->lookahead.ckpoint -= 1;

                    if (lfs2->lookahead.next >= lfs2->lookahead.size
                            || !(lfs2->lookahead.buffer[lfs2->lookahead.next / 8]
                                & (1U << (lfs2->lookahead.next % 8)))) {
                        return 0;
                    }
                }
            }

            lfs2->lookahead.next += 1;
            lfs2->lookahead.ckpoint -= 1;
        }

        // In order to keep our block allocator from spinning forever when our
        // filesystem is full, we mark points where there are no in-flight
        // allocations with a checkpoint before starting a set of allocations.
        //
        // If we've looked at all blocks since the last checkpoint, we report
        // the filesystem as out of storage.
        //
        if (lfs2->lookahead.ckpoint <= 0) {
            LFS2_ERROR("No more free space 0x%"PRIx32,
                    (lfs2->lookahead.start + lfs2->lookahead.next)
                        % lfs2->block_count);
            return LFS2_ERR_NOSPC;
        }

        // No blocks in our lookahead buffer, we need to scan the filesystem for
        // unused blocks in the next lookahead window.
        int err = lfs2_alloc_scan(lfs2);
        if(err) {
            return err;
        }
    }
}
#endif

/// Metadata pair and directory operations ///
static lfs2_stag_t lfs2_dir_getslice(lfs2_t *lfs2, const lfs2_mdir_t *dir,
        lfs2_tag_t gmask, lfs2_tag_t gtag,
        lfs2_off_t goff, void *gbuffer, lfs2_size_t gsize) {
    lfs2_off_t off = dir->off;
    lfs2_tag_t ntag = dir->etag;
    lfs2_stag_t gdiff = 0;

    // synthetic moves
    if (lfs2_gstate_hasmovehere(&lfs2->gdisk, dir->pair) &&
            lfs2_tag_id(gmask) != 0) {
        if (lfs2_tag_id(lfs2->gdisk.tag) == lfs2_tag_id(gtag)) {
            return LFS2_ERR_NOENT;
        } else if (lfs2_tag_id(lfs2->gdisk.tag) < lfs2_tag_id(gtag)) {
            gdiff -= LFS2_MKTAG(0, 1, 0);
        }
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

#ifndef LFS2_READONLY
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
        *filtertag = LFS2_MKTAG(LFS2_FROM_NOOP, 0, 0);
        return true;
    }

    // check if we need to adjust for created/deleted tags
    if (lfs2_tag_type1(tag) == LFS2_TYPE_SPLICE &&
            lfs2_tag_id(tag) <= lfs2_tag_id(*filtertag)) {
        *filtertag += LFS2_MKTAG(0, lfs2_tag_splice(tag), 0);
    }

    return false;
}
#endif

#ifndef LFS2_READONLY
// maximum recursive depth of lfs2_dir_traverse, the deepest call:
//
// traverse with commit
// '-> traverse with move
//     '-> traverse with filter
//
#define LFS2_DIR_TRAVERSE_DEPTH 3

struct lfs2_dir_traverse {
    const lfs2_mdir_t *dir;
    lfs2_off_t off;
    lfs2_tag_t ptag;
    const struct lfs2_mattr *attrs;
    int attrcount;

    lfs2_tag_t tmask;
    lfs2_tag_t ttag;
    uint16_t begin;
    uint16_t end;
    int16_t diff;

    int (*cb)(void *data, lfs2_tag_t tag, const void *buffer);
    void *data;

    lfs2_tag_t tag;
    const void *buffer;
    struct lfs2_diskoff disk;
};

static int lfs2_dir_traverse(lfs2_t *lfs2,
        const lfs2_mdir_t *dir, lfs2_off_t off, lfs2_tag_t ptag,
        const struct lfs2_mattr *attrs, int attrcount,
        lfs2_tag_t tmask, lfs2_tag_t ttag,
        uint16_t begin, uint16_t end, int16_t diff,
        int (*cb)(void *data, lfs2_tag_t tag, const void *buffer), void *data) {
    // This function in inherently recursive, but bounded. To allow tool-based
    // analysis without unnecessary code-cost we use an explicit stack
    struct lfs2_dir_traverse stack[LFS2_DIR_TRAVERSE_DEPTH-1];
    unsigned sp = 0;
    int res;

    // iterate over directory and attrs
    lfs2_tag_t tag;
    const void *buffer;
    struct lfs2_diskoff disk = {0};
    while (true) {
        {
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
            } else {
                // finished traversal, pop from stack?
                res = 0;
                break;
            }

            // do we need to filter?
            lfs2_tag_t mask = LFS2_MKTAG(0x7ff, 0, 0);
            if ((mask & tmask & tag) != (mask & tmask & ttag)) {
                continue;
            }

            if (lfs2_tag_id(tmask) != 0) {
                LFS2_ASSERT(sp < LFS2_DIR_TRAVERSE_DEPTH);
                // recurse, scan for duplicates, and update tag based on
                // creates/deletes
                stack[sp] = (struct lfs2_dir_traverse){
                    .dir        = dir,
                    .off        = off,
                    .ptag       = ptag,
                    .attrs      = attrs,
                    .attrcount  = attrcount,
                    .tmask      = tmask,
                    .ttag       = ttag,
                    .begin      = begin,
                    .end        = end,
                    .diff       = diff,
                    .cb         = cb,
                    .data       = data,
                    .tag        = tag,
                    .buffer     = buffer,
                    .disk       = disk,
                };
                sp += 1;

                tmask = 0;
                ttag = 0;
                begin = 0;
                end = 0;
                diff = 0;
                cb = lfs2_dir_traverse_filter;
                data = &stack[sp-1].tag;
                continue;
            }
        }

popped:
        // in filter range?
        if (lfs2_tag_id(tmask) != 0 &&
                !(lfs2_tag_id(tag) >= begin && lfs2_tag_id(tag) < end)) {
            continue;
        }

        // handle special cases for mcu-side operations
        if (lfs2_tag_type3(tag) == LFS2_FROM_NOOP) {
            // do nothing
        } else if (lfs2_tag_type3(tag) == LFS2_FROM_MOVE) {
            // Without this condition, lfs2_dir_traverse can exhibit an
            // extremely expensive O(n^3) of nested loops when renaming.
            // This happens because lfs2_dir_traverse tries to filter tags by
            // the tags in the source directory, triggering a second
            // lfs2_dir_traverse with its own filter operation.
            //
            // traverse with commit
            // '-> traverse with filter
            //     '-> traverse with move
            //         '-> traverse with filter
            //
            // However we don't actually care about filtering the second set of
            // tags, since duplicate tags have no effect when filtering.
            //
            // This check skips this unnecessary recursive filtering explicitly,
            // reducing this runtime from O(n^3) to O(n^2).
            if (cb == lfs2_dir_traverse_filter) {
                continue;
            }

            // recurse into move
            stack[sp] = (struct lfs2_dir_traverse){
                .dir        = dir,
                .off        = off,
                .ptag       = ptag,
                .attrs      = attrs,
                .attrcount  = attrcount,
                .tmask      = tmask,
                .ttag       = ttag,
                .begin      = begin,
                .end        = end,
                .diff       = diff,
                .cb         = cb,
                .data       = data,
                .tag        = LFS2_MKTAG(LFS2_FROM_NOOP, 0, 0),
            };
            sp += 1;

            uint16_t fromid = lfs2_tag_size(tag);
            uint16_t toid = lfs2_tag_id(tag);
            dir = buffer;
            off = 0;
            ptag = 0xffffffff;
            attrs = NULL;
            attrcount = 0;
            tmask = LFS2_MKTAG(0x600, 0x3ff, 0);
            ttag = LFS2_MKTAG(LFS2_TYPE_STRUCT, 0, 0);
            begin = fromid;
            end = fromid+1;
            diff = toid-fromid+diff;
        } else if (lfs2_tag_type3(tag) == LFS2_FROM_USERATTRS) {
            for (unsigned i = 0; i < lfs2_tag_size(tag); i++) {
                const struct lfs2_attr *a = buffer;
                res = cb(data, LFS2_MKTAG(LFS2_TYPE_USERATTR + a[i].type,
                        lfs2_tag_id(tag) + diff, a[i].size), a[i].buffer);
                if (res < 0) {
                    return res;
                }

                if (res) {
                    break;
                }
            }
        } else {
            res = cb(data, tag + LFS2_MKTAG(0, diff, 0), buffer);
            if (res < 0) {
                return res;
            }

            if (res) {
                break;
            }
        }
    }

    if (sp > 0) {
        // pop from the stack and return, fortunately all pops share
        // a destination
        dir         = stack[sp-1].dir;
        off         = stack[sp-1].off;
        ptag        = stack[sp-1].ptag;
        attrs       = stack[sp-1].attrs;
        attrcount   = stack[sp-1].attrcount;
        tmask       = stack[sp-1].tmask;
        ttag        = stack[sp-1].ttag;
        begin       = stack[sp-1].begin;
        end         = stack[sp-1].end;
        diff        = stack[sp-1].diff;
        cb          = stack[sp-1].cb;
        data        = stack[sp-1].data;
        tag         = stack[sp-1].tag;
        buffer      = stack[sp-1].buffer;
        disk        = stack[sp-1].disk;
        sp -= 1;
        goto popped;
    } else {
        return res;
    }
}
#endif

static lfs2_stag_t lfs2_dir_fetchmatch(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const lfs2_block_t pair[2],
        lfs2_tag_t fmask, lfs2_tag_t ftag, uint16_t *id,
        int (*cb)(void *data, lfs2_tag_t tag, const void *buffer), void *data) {
    // we can find tag very efficiently during a fetch, since we're already
    // scanning the entire directory
    lfs2_stag_t besttag = -1;

    // if either block address is invalid we return LFS2_ERR_CORRUPT here,
    // otherwise later writes to the pair could fail
    if (lfs2->block_count 
            && (pair[0] >= lfs2->block_count || pair[1] >= lfs2->block_count)) {
        return LFS2_ERR_CORRUPT;
    }

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
        lfs2_tag_t ptag = 0xffffffff;

        uint16_t tempcount = 0;
        lfs2_block_t temptail[2] = {LFS2_BLOCK_NULL, LFS2_BLOCK_NULL};
        bool tempsplit = false;
        lfs2_stag_t tempbesttag = besttag;

        // assume not erased until proven otherwise
        bool maybeerased = false;
        bool hasfcrc = false;
        struct lfs2_fcrc fcrc;

        dir->rev = lfs2_tole32(dir->rev);
        uint32_t crc = lfs2_crc(0xffffffff, &dir->rev, sizeof(dir->rev));
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
                    break;
                }
                return err;
            }

            crc = lfs2_crc(crc, &tag, sizeof(tag));
            tag = lfs2_frombe32(tag) ^ ptag;

            // next commit not yet programmed?
            if (!lfs2_tag_isvalid(tag)) {
                // we only might be erased if the last tag was a crc
                maybeerased = (lfs2_tag_type2(ptag) == LFS2_TYPE_CCRC);
                break;
            // out of range?
            } else if (off + lfs2_tag_dsize(tag) > lfs2->cfg->block_size) {
                break;
            }

            ptag = tag;

            if (lfs2_tag_type2(tag) == LFS2_TYPE_CCRC) {
                // check the crc attr
                uint32_t dcrc;
                err = lfs2_bd_read(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->block_size,
                        dir->pair[0], off+sizeof(tag), &dcrc, sizeof(dcrc));
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        break;
                    }
                    return err;
                }
                dcrc = lfs2_fromle32(dcrc);

                if (crc != dcrc) {
                    break;
                }

                // reset the next bit if we need to
                ptag ^= (lfs2_tag_t)(lfs2_tag_chunk(tag) & 1U) << 31;

                // toss our crc into the filesystem seed for
                // pseudorandom numbers, note we use another crc here
                // as a collection function because it is sufficiently
                // random and convenient
                lfs2->seed = lfs2_crc(lfs2->seed, &crc, sizeof(crc));

                // update with what's found so far
                besttag = tempbesttag;
                dir->off = off + lfs2_tag_dsize(tag);
                dir->etag = ptag;
                dir->count = tempcount;
                dir->tail[0] = temptail[0];
                dir->tail[1] = temptail[1];
                dir->split = tempsplit;

                // reset crc, hasfcrc
                crc = 0xffffffff;
                continue;
            }

            // crc the entry first, hopefully leaving it in the cache
            err = lfs2_bd_crc(lfs2,
                    NULL, &lfs2->rcache, lfs2->cfg->block_size,
                    dir->pair[0], off+sizeof(tag),
                    lfs2_tag_dsize(tag)-sizeof(tag), &crc);
            if (err) {
                if (err == LFS2_ERR_CORRUPT) {
                    break;
                }
                return err;
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
                        break;
                    }
                    return err;
                }
                lfs2_pair_fromle32(temptail);
            } else if (lfs2_tag_type3(tag) == LFS2_TYPE_FCRC) {
                err = lfs2_bd_read(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->block_size,
                        dir->pair[0], off+sizeof(tag),
                        &fcrc, sizeof(fcrc));
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        break;
                    }
                }

                lfs2_fcrc_fromle32(&fcrc);
                hasfcrc = true;
            }

            // found a match for our fetcher?
            if ((fmask & tag) == (fmask & ftag)) {
                int res = cb(data, tag, &(struct lfs2_diskoff){
                        dir->pair[0], off+sizeof(tag)});
                if (res < 0) {
                    if (res == LFS2_ERR_CORRUPT) {
                        break;
                    }
                    return res;
                }

                if (res == LFS2_CMP_EQ) {
                    // found a match
                    tempbesttag = tag;
                } else if ((LFS2_MKTAG(0x7ff, 0x3ff, 0) & tag) ==
                        (LFS2_MKTAG(0x7ff, 0x3ff, 0) & tempbesttag)) {
                    // found an identical tag, but contents didn't match
                    // this must mean that our besttag has been overwritten
                    tempbesttag = -1;
                } else if (res == LFS2_CMP_GT &&
                        lfs2_tag_id(tag) <= lfs2_tag_id(tempbesttag)) {
                    // found a greater match, keep track to keep things sorted
                    tempbesttag = tag | 0x80000000;
                }
            }
        }

        // found no valid commits?
        if (dir->off == 0) {
            // try the other block?
            lfs2_pair_swap(dir->pair);
            dir->rev = revs[(r+1)%2];
            continue;
        }

        // did we end on a valid commit? we may have an erased block
        dir->erased = false;
        if (maybeerased && dir->off % lfs2->cfg->prog_size == 0) {
        #ifdef LFS2_MULTIVERSION
            // note versions < lfs22.1 did not have fcrc tags, if
            // we're < lfs22.1 treat missing fcrc as erased data
            //
            // we don't strictly need to do this, but otherwise writing
            // to lfs22.0 disks becomes very inefficient
            if (lfs2_fs_disk_version(lfs2) < 0x00020001) {
                dir->erased = true;

            } else
        #endif
            if (hasfcrc) {
                // check for an fcrc matching the next prog's erased state, if
                // this failed most likely a previous prog was interrupted, we
                // need a new erase
                uint32_t fcrc_ = 0xffffffff;
                int err = lfs2_bd_crc(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->block_size,
                        dir->pair[0], dir->off, fcrc.size, &fcrc_);
                if (err && err != LFS2_ERR_CORRUPT) {
                    return err;
                }

                // found beginning of erased part?
                dir->erased = (fcrc_ == fcrc.crc);
            }
        }

        // synthetic move
        if (lfs2_gstate_hasmovehere(&lfs2->gdisk, dir->pair)) {
            if (lfs2_tag_id(lfs2->gdisk.tag) == lfs2_tag_id(besttag)) {
                besttag |= 0x80000000;
            } else if (besttag != -1 &&
                    lfs2_tag_id(lfs2->gdisk.tag) < lfs2_tag_id(besttag)) {
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

    LFS2_ERROR("Corrupted dir pair at {0x%"PRIx32", 0x%"PRIx32"}",
            dir->pair[0], dir->pair[1]);
    return LFS2_ERR_CORRUPT;
}

static int lfs2_dir_fetch(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const lfs2_block_t pair[2]) {
    // note, mask=-1, tag=-1 can never match a tag since this
    // pattern has the invalid bit set
    return (int)lfs2_dir_fetchmatch(lfs2, dir, pair,
            (lfs2_tag_t)-1, (lfs2_tag_t)-1, NULL, NULL, NULL);
}

static int lfs2_dir_getgstate(lfs2_t *lfs2, const lfs2_mdir_t *dir,
        lfs2_gstate_t *gstate) {
    lfs2_gstate_t temp;
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

// lfs2_dir_find tries to set path and id even if file is not found
//
// returns:
// - 0                  if file is found
// - LFS2_ERR_NOENT      if file or parent is not found
// - LFS2_ERR_NOTDIR     if parent is not a dir
static lfs2_stag_t lfs2_dir_find(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const char **path, uint16_t *id) {
    // we reduce path to a single name if we can find it
    const char *name = *path;

    // default to root dir
    lfs2_stag_t tag = LFS2_MKTAG(LFS2_TYPE_DIR, 0x3ff, 0);
    dir->tail[0] = lfs2->root[0];
    dir->tail[1] = lfs2->root[1];

    // empty paths are not allowed
    if (*name == '\0') {
        return LFS2_ERR_INVAL;
    }

    while (true) {
nextname:
        // skip slashes if we're a directory
        if (lfs2_tag_type3(tag) == LFS2_TYPE_DIR) {
            name += strspn(name, "/");
        }
        lfs2_size_t namelen = strcspn(name, "/");

        // skip '.'
        if (namelen == 1 && memcmp(name, ".", 1) == 0) {
            name += namelen;
            goto nextname;
        }

        // error on unmatched '..', trying to go above root?
        if (namelen == 2 && memcmp(name, "..", 2) == 0) {
            return LFS2_ERR_INVAL;
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

            if (sufflen == 1 && memcmp(suffix, ".", 1) == 0) {
                // noop
            } else if (sufflen == 2 && memcmp(suffix, "..", 2) == 0) {
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
        if (*name == '\0') {
            return tag;
        }

        // update what we've found so far
        *path = name;

        // only continue if we're a directory
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
                    id,
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

#ifndef LFS2_READONLY
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
#endif

#ifndef LFS2_READONLY
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
#endif

#ifndef LFS2_READONLY

static int lfs2_dir_commitcrc(lfs2_t *lfs2, struct lfs2_commit *commit) {
    // align to program units
    //
    // this gets a bit complex as we have two types of crcs:
    // - 5-word crc with fcrc to check following prog (middle of block)
    // - 2-word crc with no following prog (end of block)
    const lfs2_off_t end = lfs2_alignup(
            lfs2_min(commit->off + 5*sizeof(uint32_t), lfs2->cfg->block_size),
            lfs2->cfg->prog_size);

    lfs2_off_t off1 = 0;
    uint32_t crc1 = 0;

    // create crc tags to fill up remainder of commit, note that
    // padding is not crced, which lets fetches skip padding but
    // makes committing a bit more complicated
    while (commit->off < end) {
        lfs2_off_t noff = (
                lfs2_min(end - (commit->off+sizeof(lfs2_tag_t)), 0x3fe)
                + (commit->off+sizeof(lfs2_tag_t)));
        // too large for crc tag? need padding commits
        if (noff < end) {
            noff = lfs2_min(noff, end - 5*sizeof(uint32_t));
        }

        // space for fcrc?
        uint8_t eperturb = (uint8_t)-1;
        if (noff >= end && noff <= lfs2->cfg->block_size - lfs2->cfg->prog_size) {
            // first read the leading byte, this always contains a bit
            // we can perturb to avoid writes that don't change the fcrc
            int err = lfs2_bd_read(lfs2,
                    NULL, &lfs2->rcache, lfs2->cfg->prog_size,
                    commit->block, noff, &eperturb, 1);
            if (err && err != LFS2_ERR_CORRUPT) {
                return err;
            }

        #ifdef LFS2_MULTIVERSION
            // unfortunately fcrcs break mdir fetching < lfs22.1, so only write
            // these if we're a >= lfs22.1 filesystem
            if (lfs2_fs_disk_version(lfs2) <= 0x00020000) {
                // don't write fcrc
            } else
        #endif
            {
                // find the expected fcrc, don't bother avoiding a reread
                // of the eperturb, it should still be in our cache
                struct lfs2_fcrc fcrc = {
                    .size = lfs2->cfg->prog_size,
                    .crc = 0xffffffff
                };
                err = lfs2_bd_crc(lfs2,
                        NULL, &lfs2->rcache, lfs2->cfg->prog_size,
                        commit->block, noff, fcrc.size, &fcrc.crc);
                if (err && err != LFS2_ERR_CORRUPT) {
                    return err;
                }

                lfs2_fcrc_tole32(&fcrc);
                err = lfs2_dir_commitattr(lfs2, commit,
                        LFS2_MKTAG(LFS2_TYPE_FCRC, 0x3ff, sizeof(struct lfs2_fcrc)),
                        &fcrc);
                if (err) {
                    return err;
                }
            }
        }

        // build commit crc
        struct {
            lfs2_tag_t tag;
            uint32_t crc;
        } ccrc;
        lfs2_tag_t ntag = LFS2_MKTAG(
                LFS2_TYPE_CCRC + (((uint8_t)~eperturb) >> 7), 0x3ff,
                noff - (commit->off+sizeof(lfs2_tag_t)));
        ccrc.tag = lfs2_tobe32(ntag ^ commit->ptag);
        commit->crc = lfs2_crc(commit->crc, &ccrc.tag, sizeof(lfs2_tag_t));
        ccrc.crc = lfs2_tole32(commit->crc);

        int err = lfs2_bd_prog(lfs2,
                &lfs2->pcache, &lfs2->rcache, false,
                commit->block, commit->off, &ccrc, sizeof(ccrc));
        if (err) {
            return err;
        }

        // keep track of non-padding checksum to verify
        if (off1 == 0) {
            off1 = commit->off + sizeof(lfs2_tag_t);
            crc1 = commit->crc;
        }

        commit->off = noff;
        // perturb valid bit?
        commit->ptag = ntag ^ ((0x80UL & ~eperturb) << 24);
        // reset crc for next commit
        commit->crc = 0xffffffff;

        // manually flush here since we don't prog the padding, this confuses
        // the caching layer
        if (noff >= end || noff >= lfs2->pcache.off + lfs2->cfg->cache_size) {
            // flush buffers
            int err = lfs2_bd_sync(lfs2, &lfs2->pcache, &lfs2->rcache, false);
            if (err) {
                return err;
            }
        }
    }

    // successful commit, check checksums to make sure
    //
    // note that we don't need to check padding commits, worst
    // case if they are corrupted we would have had to compact anyways
    lfs2_off_t off = commit->begin;
    uint32_t crc = 0xffffffff;
    int err = lfs2_bd_crc(lfs2,
            NULL, &lfs2->rcache, off1+sizeof(uint32_t),
            commit->block, off, off1-off, &crc);
    if (err) {
        return err;
    }

    // check non-padding commits against known crc
    if (crc != crc1) {
        return LFS2_ERR_CORRUPT;
    }

    // make sure to check crc in case we happen to pick
    // up an unrelated crc (frozen block?)
    err = lfs2_bd_crc(lfs2,
            NULL, &lfs2->rcache, sizeof(uint32_t),
            commit->block, off1, sizeof(uint32_t), &crc);
    if (err) {
        return err;
    }

    if (crc != 0) {
        return LFS2_ERR_CORRUPT;
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_alloc(lfs2_t *lfs2, lfs2_mdir_t *dir) {
    // allocate pair of dir blocks (backwards, so we write block 1 first)
    for (int i = 0; i < 2; i++) {
        int err = lfs2_alloc(lfs2, &dir->pair[(i+1)%2]);
        if (err) {
            return err;
        }
    }

    // zero for reproducibility in case initial block is unreadable
    dir->rev = 0;

    // rather than clobbering one of the blocks we just pretend
    // the revision may be valid
    int err = lfs2_bd_read(lfs2,
            NULL, &lfs2->rcache, sizeof(dir->rev),
            dir->pair[0], 0, &dir->rev, sizeof(dir->rev));
    dir->rev = lfs2_fromle32(dir->rev);
    if (err && err != LFS2_ERR_CORRUPT) {
        return err;
    }

    // to make sure we don't immediately evict, align the new revision count
    // to our block_cycles modulus, see lfs2_dir_compact for why our modulus
    // is tweaked this way
    if (lfs2->cfg->block_cycles > 0) {
        dir->rev = lfs2_alignup(dir->rev, ((lfs2->cfg->block_cycles+1)|1));
    }

    // set defaults
    dir->off = sizeof(dir->rev);
    dir->etag = 0xffffffff;
    dir->count = 0;
    dir->tail[0] = LFS2_BLOCK_NULL;
    dir->tail[1] = LFS2_BLOCK_NULL;
    dir->erased = false;
    dir->split = false;

    // don't write out yet, let caller take care of that
    return 0;
}
#endif

#ifndef LFS2_READONLY
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
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_split(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t split, uint16_t end) {
    // create tail metadata pair
    lfs2_mdir_t tail;
    int err = lfs2_dir_alloc(lfs2, &tail);
    if (err) {
        return err;
    }

    tail.split = dir->split;
    tail.tail[0] = dir->tail[0];
    tail.tail[1] = dir->tail[1];

    // note we don't care about LFS2_OK_RELOCATED
    int res = lfs2_dir_compact(lfs2, &tail, attrs, attrcount, source, split, end);
    if (res < 0) {
        return res;
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
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_commit_size(void *p, lfs2_tag_t tag, const void *buffer) {
    lfs2_size_t *size = p;
    (void)buffer;

    *size += lfs2_tag_dsize(tag);
    return 0;
}
#endif

#ifndef LFS2_READONLY
struct lfs2_dir_commit_commit {
    lfs2_t *lfs2;
    struct lfs2_commit *commit;
};
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_commit_commit(void *p, lfs2_tag_t tag, const void *buffer) {
    struct lfs2_dir_commit_commit *commit = p;
    return lfs2_dir_commitattr(commit->lfs2, commit->commit, tag, buffer);
}
#endif

#ifndef LFS2_READONLY
static bool lfs2_dir_needsrelocation(lfs2_t *lfs2, lfs2_mdir_t *dir) {
    // If our revision count == n * block_cycles, we should force a relocation,
    // this is how littlefs wear-levels at the metadata-pair level. Note that we
    // actually use (block_cycles+1)|1, this is to avoid two corner cases:
    // 1. block_cycles = 1, which would prevent relocations from terminating
    // 2. block_cycles = 2n, which, due to aliasing, would only ever relocate
    //    one metadata block in the pair, effectively making this useless
    return (lfs2->cfg->block_cycles > 0
            && ((dir->rev + 1) % ((lfs2->cfg->block_cycles+1)|1) == 0));
}
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_compact(lfs2_t *lfs2,
        lfs2_mdir_t *dir, const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t begin, uint16_t end) {
    // save some state in case block is bad
    bool relocated = false;
    bool tired = lfs2_dir_needsrelocation(lfs2, dir);

    // increment revision count
    dir->rev += 1;

    // do not proactively relocate blocks during migrations, this
    // can cause a number of failure states such: clobbering the
    // v1 superblock if we relocate root, and invalidating directory
    // pointers if we relocate the head of a directory. On top of
    // this, relocations increase the overall complexity of
    // lfs2_migration, which is already a delicate operation.
#ifdef LFS2_MIGRATE
    if (lfs2->lfs21) {
        tired = false;
    }
#endif

    if (tired && lfs2_pair_cmp(dir->pair, (const lfs2_block_t[2]){0, 1}) != 0) {
        // we're writing too much, time to relocate
        goto relocate;
    }

    // begin loop to commit compaction to blocks until a compact sticks
    while (true) {
        {
            // setup commit state
            struct lfs2_commit commit = {
                .block = dir->pair[1],
                .off = 0,
                .ptag = 0xffffffff,
                .crc = 0xffffffff,

                .begin = 0,
                .end = (lfs2->cfg->metadata_max ?
                    lfs2->cfg->metadata_max : lfs2->cfg->block_size) - 8,
            };

            // erase block to write to
            int err = lfs2_bd_erase(lfs2, dir->pair[1]);
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
                    source, 0, 0xffffffff, attrs, attrcount,
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

            // bring over gstate?
            lfs2_gstate_t delta = {0};
            if (!relocated) {
                lfs2_gstate_xor(&delta, &lfs2->gdisk);
                lfs2_gstate_xor(&delta, &lfs2->gstate);
            }
            lfs2_gstate_xor(&delta, &lfs2->gdelta);
            delta.tag &= ~LFS2_MKTAG(0, 0, 0x3ff);

            err = lfs2_dir_getgstate(lfs2, dir, &delta);
            if (err) {
                return err;
            }

            if (!lfs2_gstate_iszero(&delta)) {
                lfs2_gstate_tole32(&delta);
                err = lfs2_dir_commitattr(lfs2, &commit,
                        LFS2_MKTAG(LFS2_TYPE_MOVESTATE, 0x3ff,
                            sizeof(delta)), &delta);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }
            }

            // complete commit with crc
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
            // update gstate
            lfs2->gdelta = (lfs2_gstate_t){0};
            if (!relocated) {
                lfs2->gdisk = lfs2->gstate;
            }
        }
        break;

relocate:
        // commit was corrupted, drop caches and prepare to relocate block
        relocated = true;
        lfs2_cache_drop(lfs2, &lfs2->pcache);
        if (!tired) {
            LFS2_DEBUG("Bad block at 0x%"PRIx32, dir->pair[1]);
        }

        // can't relocate superblock, filesystem is now frozen
        if (lfs2_pair_cmp(dir->pair, (const lfs2_block_t[2]){0, 1}) == 0) {
            LFS2_WARN("Superblock 0x%"PRIx32" has become unwritable",
                    dir->pair[1]);
            return LFS2_ERR_NOSPC;
        }

        // relocate half of pair
        int err = lfs2_alloc(lfs2, &dir->pair[1]);
        if (err && (err != LFS2_ERR_NOSPC || !tired)) {
            return err;
        }

        tired = false;
        continue;
    }

    return relocated ? LFS2_OK_RELOCATED : 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_splittingcompact(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *source, uint16_t begin, uint16_t end) {
    while (true) {
        // find size of first split, we do this by halving the split until
        // the metadata is guaranteed to fit
        //
        // Note that this isn't a true binary search, we never increase the
        // split size. This may result in poorly distributed metadata but isn't
        // worth the extra code size or performance hit to fix.
        lfs2_size_t split = begin;
        while (end - split > 1) {
            lfs2_size_t size = 0;
            int err = lfs2_dir_traverse(lfs2,
                    source, 0, 0xffffffff, attrs, attrcount,
                    LFS2_MKTAG(0x400, 0x3ff, 0),
                    LFS2_MKTAG(LFS2_TYPE_NAME, 0, 0),
                    split, end, -split,
                    lfs2_dir_commit_size, &size);
            if (err) {
                return err;
            }

            // space is complicated, we need room for:
            //
            // - tail:         4+2*4 = 12 bytes
            // - gstate:       4+3*4 = 16 bytes
            // - move delete:  4     = 4 bytes
            // - crc:          4+4   = 8 bytes
            //                 total = 40 bytes
            //
            // And we cap at half a block to avoid degenerate cases with
            // nearly-full metadata blocks.
            //
            lfs2_size_t metadata_max = (lfs2->cfg->metadata_max)
                    ? lfs2->cfg->metadata_max
                    : lfs2->cfg->block_size;
            if (end - split < 0xff
                    && size <= lfs2_min(
                        metadata_max - 40,
                        lfs2_alignup(
                            metadata_max/2,
                            lfs2->cfg->prog_size))) {
                break;
            }

            split = split + ((end - split) / 2);
        }

        if (split == begin) {
            // no split needed
            break;
        }

        // split into two metadata pairs and continue
        int err = lfs2_dir_split(lfs2, dir, attrs, attrcount,
                source, split, end);
        if (err && err != LFS2_ERR_NOSPC) {
            return err;
        }

        if (err) {
            // we can't allocate a new block, try to compact with degraded
            // performance
            LFS2_WARN("Unable to split {0x%"PRIx32", 0x%"PRIx32"}",
                    dir->pair[0], dir->pair[1]);
            break;
        } else {
            end = split;
        }
    }

    if (lfs2_dir_needsrelocation(lfs2, dir)
            && lfs2_pair_cmp(dir->pair, (const lfs2_block_t[2]){0, 1}) == 0) {
        // oh no! we're writing too much to the superblock,
        // should we expand?
        lfs2_ssize_t size = lfs2_fs_size_(lfs2);
        if (size < 0) {
            return size;
        }

        // littlefs cannot reclaim expanded superblocks, so expand cautiously
        //
        // if our filesystem is more than ~88% full, don't expand, this is
        // somewhat arbitrary
        if (lfs2->block_count - size > lfs2->block_count/8) {
            LFS2_DEBUG("Expanding superblock at rev %"PRIu32, dir->rev);
            int err = lfs2_dir_split(lfs2, dir, attrs, attrcount,
                    source, begin, end);
            if (err && err != LFS2_ERR_NOSPC) {
                return err;
            }

            if (err) {
                // welp, we tried, if we ran out of space there's not much
                // we can do, we'll error later if we've become frozen
                LFS2_WARN("Unable to expand superblock");
            } else {
                // duplicate the superblock entry into the new superblock
                end = 1;
            }
        }
    }

    return lfs2_dir_compact(lfs2, dir, attrs, attrcount, source, begin, end);
}
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_relocatingcommit(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const lfs2_block_t pair[2],
        const struct lfs2_mattr *attrs, int attrcount,
        lfs2_mdir_t *pdir) {
    int state = 0;

    // calculate changes to the directory
    bool hasdelete = false;
    for (int i = 0; i < attrcount; i++) {
        if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_CREATE) {
            dir->count += 1;
        } else if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_DELETE) {
            LFS2_ASSERT(dir->count > 0);
            dir->count -= 1;
            hasdelete = true;
        } else if (lfs2_tag_type1(attrs[i].tag) == LFS2_TYPE_TAIL) {
            dir->tail[0] = ((lfs2_block_t*)attrs[i].buffer)[0];
            dir->tail[1] = ((lfs2_block_t*)attrs[i].buffer)[1];
            dir->split = (lfs2_tag_chunk(attrs[i].tag) & 1);
            lfs2_pair_fromle32(dir->tail);
        }
    }

    // should we actually drop the directory block?
    if (hasdelete && dir->count == 0) {
        LFS2_ASSERT(pdir);
        int err = lfs2_fs_pred(lfs2, dir->pair, pdir);
        if (err && err != LFS2_ERR_NOENT) {
            return err;
        }

        if (err != LFS2_ERR_NOENT && pdir->split) {
            state = LFS2_OK_DROPPED;
            goto fixmlist;
        }
    }

    if (dir->erased) {
        // try to commit
        struct lfs2_commit commit = {
            .block = dir->pair[0],
            .off = dir->off,
            .ptag = dir->etag,
            .crc = 0xffffffff,

            .begin = dir->off,
            .end = (lfs2->cfg->metadata_max ?
                lfs2->cfg->metadata_max : lfs2->cfg->block_size) - 8,
        };

        // traverse attrs that need to be written out
        lfs2_pair_tole32(dir->tail);
        int err = lfs2_dir_traverse(lfs2,
                dir, dir->off, dir->etag, attrs, attrcount,
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
        lfs2_gstate_t delta = {0};
        lfs2_gstate_xor(&delta, &lfs2->gstate);
        lfs2_gstate_xor(&delta, &lfs2->gdisk);
        lfs2_gstate_xor(&delta, &lfs2->gdelta);
        delta.tag &= ~LFS2_MKTAG(0, 0, 0x3ff);
        if (!lfs2_gstate_iszero(&delta)) {
            err = lfs2_dir_getgstate(lfs2, dir, &delta);
            if (err) {
                return err;
            }

            lfs2_gstate_tole32(&delta);
            err = lfs2_dir_commitattr(lfs2, &commit,
                    LFS2_MKTAG(LFS2_TYPE_MOVESTATE, 0x3ff,
                        sizeof(delta)), &delta);
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
        // and update gstate
        lfs2->gdisk = lfs2->gstate;
        lfs2->gdelta = (lfs2_gstate_t){0};

        goto fixmlist;
    }

compact:
    // fall back to compaction
    lfs2_cache_drop(lfs2, &lfs2->pcache);

    state = lfs2_dir_splittingcompact(lfs2, dir, attrs, attrcount,
            dir, 0, dir->count);
    if (state < 0) {
        return state;
    }

    goto fixmlist;

fixmlist:;
    // this complicated bit of logic is for fixing up any active
    // metadata-pairs that we may have affected
    //
    // note we have to make two passes since the mdir passed to
    // lfs2_dir_commit could also be in this list, and even then
    // we need to copy the pair so they don't get clobbered if we refetch
    // our mdir.
    lfs2_block_t oldpair[2] = {pair[0], pair[1]};
    for (struct lfs2_mlist *d = lfs2->mlist; d; d = d->next) {
        if (lfs2_pair_cmp(d->m.pair, oldpair) == 0) {
            d->m = *dir;
            if (d->m.pair != pair) {
                for (int i = 0; i < attrcount; i++) {
                    if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_DELETE &&
                            d->id == lfs2_tag_id(attrs[i].tag) &&
                            d->type != LFS2_TYPE_DIR) {
                        d->m.pair[0] = LFS2_BLOCK_NULL;
                        d->m.pair[1] = LFS2_BLOCK_NULL;
                    } else if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_DELETE &&
                            d->id > lfs2_tag_id(attrs[i].tag)) {
                        d->id -= 1;
                        if (d->type == LFS2_TYPE_DIR) {
                            ((lfs2_dir_t*)d)->pos -= 1;
                        }
                    } else if (lfs2_tag_type3(attrs[i].tag) == LFS2_TYPE_CREATE &&
                            d->id >= lfs2_tag_id(attrs[i].tag)) {
                        d->id += 1;
                        if (d->type == LFS2_TYPE_DIR) {
                            ((lfs2_dir_t*)d)->pos += 1;
                        }
                    }
                }
            }

            while (d->id >= d->m.count && d->m.split) {
                // we split and id is on tail now
                if (lfs2_pair_cmp(d->m.tail, lfs2->root) != 0) {
                    d->id -= d->m.count;
                }
                int err = lfs2_dir_fetch(lfs2, &d->m, d->m.tail);
                if (err) {
                    return err;
                }
            }
        }
    }

    return state;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_orphaningcommit(lfs2_t *lfs2, lfs2_mdir_t *dir,
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

    lfs2_block_t lpair[2] = {dir->pair[0], dir->pair[1]};
    lfs2_mdir_t ldir = *dir;
    lfs2_mdir_t pdir;
    int state = lfs2_dir_relocatingcommit(lfs2, &ldir, dir->pair,
            attrs, attrcount, &pdir);
    if (state < 0) {
        return state;
    }

    // update if we're not in mlist, note we may have already been
    // updated if we are in mlist
    if (lfs2_pair_cmp(dir->pair, lpair) == 0) {
        *dir = ldir;
    }

    // commit was successful, but may require other changes in the
    // filesystem, these would normally be tail recursive, but we have
    // flattened them here avoid unbounded stack usage

    // need to drop?
    if (state == LFS2_OK_DROPPED) {
        // steal state
        int err = lfs2_dir_getgstate(lfs2, dir, &lfs2->gdelta);
        if (err) {
            return err;
        }

        // steal tail, note that this can't create a recursive drop
        lpair[0] = pdir.pair[0];
        lpair[1] = pdir.pair[1];
        lfs2_pair_tole32(dir->tail);
        state = lfs2_dir_relocatingcommit(lfs2, &pdir, lpair, LFS2_MKATTRS(
                    {LFS2_MKTAG(LFS2_TYPE_TAIL + dir->split, 0x3ff, 8),
                        dir->tail}),
                NULL);
        lfs2_pair_fromle32(dir->tail);
        if (state < 0) {
            return state;
        }

        ldir = pdir;
    }

    // need to relocate?
    bool orphans = false;
    while (state == LFS2_OK_RELOCATED) {
        LFS2_DEBUG("Relocating {0x%"PRIx32", 0x%"PRIx32"} "
                    "-> {0x%"PRIx32", 0x%"PRIx32"}",
                lpair[0], lpair[1], ldir.pair[0], ldir.pair[1]);
        state = 0;

        // update internal root
        if (lfs2_pair_cmp(lpair, lfs2->root) == 0) {
            lfs2->root[0] = ldir.pair[0];
            lfs2->root[1] = ldir.pair[1];
        }

        // update internally tracked dirs
        for (struct lfs2_mlist *d = lfs2->mlist; d; d = d->next) {
            if (lfs2_pair_cmp(lpair, d->m.pair) == 0) {
                d->m.pair[0] = ldir.pair[0];
                d->m.pair[1] = ldir.pair[1];
            }

            if (d->type == LFS2_TYPE_DIR &&
                    lfs2_pair_cmp(lpair, ((lfs2_dir_t*)d)->head) == 0) {
                ((lfs2_dir_t*)d)->head[0] = ldir.pair[0];
                ((lfs2_dir_t*)d)->head[1] = ldir.pair[1];
            }
        }

        // find parent
        lfs2_stag_t tag = lfs2_fs_parent(lfs2, lpair, &pdir);
        if (tag < 0 && tag != LFS2_ERR_NOENT) {
            return tag;
        }

        bool hasparent = (tag != LFS2_ERR_NOENT);
        if (tag != LFS2_ERR_NOENT) {
            // note that if we have a parent, we must have a pred, so this will
            // always create an orphan
            int err = lfs2_fs_preporphans(lfs2, +1);
            if (err) {
                return err;
            }

            // fix pending move in this pair? this looks like an optimization but
            // is in fact _required_ since relocating may outdate the move.
            uint16_t moveid = 0x3ff;
            if (lfs2_gstate_hasmovehere(&lfs2->gstate, pdir.pair)) {
                moveid = lfs2_tag_id(lfs2->gstate.tag);
                LFS2_DEBUG("Fixing move while relocating "
                        "{0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16"\n",
                        pdir.pair[0], pdir.pair[1], moveid);
                lfs2_fs_prepmove(lfs2, 0x3ff, NULL);
                if (moveid < lfs2_tag_id(tag)) {
                    tag -= LFS2_MKTAG(0, 1, 0);
                }
            }

            lfs2_block_t ppair[2] = {pdir.pair[0], pdir.pair[1]};
            lfs2_pair_tole32(ldir.pair);
            state = lfs2_dir_relocatingcommit(lfs2, &pdir, ppair, LFS2_MKATTRS(
                        {LFS2_MKTAG_IF(moveid != 0x3ff,
                            LFS2_TYPE_DELETE, moveid, 0), NULL},
                        {tag, ldir.pair}),
                    NULL);
            lfs2_pair_fromle32(ldir.pair);
            if (state < 0) {
                return state;
            }

            if (state == LFS2_OK_RELOCATED) {
                lpair[0] = ppair[0];
                lpair[1] = ppair[1];
                ldir = pdir;
                orphans = true;
                continue;
            }
        }

        // find pred
        int err = lfs2_fs_pred(lfs2, lpair, &pdir);
        if (err && err != LFS2_ERR_NOENT) {
            return err;
        }
        LFS2_ASSERT(!(hasparent && err == LFS2_ERR_NOENT));

        // if we can't find dir, it must be new
        if (err != LFS2_ERR_NOENT) {
            if (lfs2_gstate_hasorphans(&lfs2->gstate)) {
                // next step, clean up orphans
                err = lfs2_fs_preporphans(lfs2, -(int8_t)hasparent);
                if (err) {
                    return err;
                }
            }

            // fix pending move in this pair? this looks like an optimization
            // but is in fact _required_ since relocating may outdate the move.
            uint16_t moveid = 0x3ff;
            if (lfs2_gstate_hasmovehere(&lfs2->gstate, pdir.pair)) {
                moveid = lfs2_tag_id(lfs2->gstate.tag);
                LFS2_DEBUG("Fixing move while relocating "
                        "{0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16"\n",
                        pdir.pair[0], pdir.pair[1], moveid);
                lfs2_fs_prepmove(lfs2, 0x3ff, NULL);
            }

            // replace bad pair, either we clean up desync, or no desync occured
            lpair[0] = pdir.pair[0];
            lpair[1] = pdir.pair[1];
            lfs2_pair_tole32(ldir.pair);
            state = lfs2_dir_relocatingcommit(lfs2, &pdir, lpair, LFS2_MKATTRS(
                        {LFS2_MKTAG_IF(moveid != 0x3ff,
                            LFS2_TYPE_DELETE, moveid, 0), NULL},
                        {LFS2_MKTAG(LFS2_TYPE_TAIL + pdir.split, 0x3ff, 8),
                            ldir.pair}),
                    NULL);
            lfs2_pair_fromle32(ldir.pair);
            if (state < 0) {
                return state;
            }

            ldir = pdir;
        }
    }

    return orphans ? LFS2_OK_ORPHANED : 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_dir_commit(lfs2_t *lfs2, lfs2_mdir_t *dir,
        const struct lfs2_mattr *attrs, int attrcount) {
    int orphans = lfs2_dir_orphaningcommit(lfs2, dir, attrs, attrcount);
    if (orphans < 0) {
        return orphans;
    }

    if (orphans) {
        // make sure we've removed all orphans, this is a noop if there
        // are none, but if we had nested blocks failures we may have
        // created some
        int err = lfs2_fs_deorphan(lfs2, false);
        if (err) {
            return err;
        }
    }

    return 0;
}
#endif


/// Top level directory operations ///
#ifndef LFS2_READONLY
static int lfs2_mkdir_(lfs2_t *lfs2, const char *path) {
    // deorphan if we haven't yet, needed at most once after poweron
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        return err;
    }

    struct lfs2_mlist cwd;
    cwd.next = lfs2->mlist;
    uint16_t id;
    err = lfs2_dir_find(lfs2, &cwd.m, &path, &id);
    if (!(err == LFS2_ERR_NOENT && lfs2_path_islast(path))) {
        return (err < 0) ? err : LFS2_ERR_EXIST;
    }

    // check that name fits
    lfs2_size_t nlen = lfs2_path_namelen(path);
    if (nlen > lfs2->name_max) {
        return LFS2_ERR_NAMETOOLONG;
    }

    // build up new directory
    lfs2_alloc_ckpoint(lfs2);
    lfs2_mdir_t dir;
    err = lfs2_dir_alloc(lfs2, &dir);
    if (err) {
        return err;
    }

    // find end of list
    lfs2_mdir_t pred = cwd.m;
    while (pred.split) {
        err = lfs2_dir_fetch(lfs2, &pred, pred.tail);
        if (err) {
            return err;
        }
    }

    // setup dir
    lfs2_pair_tole32(pred.tail);
    err = lfs2_dir_commit(lfs2, &dir, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8), pred.tail}));
    lfs2_pair_fromle32(pred.tail);
    if (err) {
        return err;
    }

    // current block not end of list?
    if (cwd.m.split) {
        // update tails, this creates a desync
        err = lfs2_fs_preporphans(lfs2, +1);
        if (err) {
            return err;
        }

        // it's possible our predecessor has to be relocated, and if
        // our parent is our predecessor's predecessor, this could have
        // caused our parent to go out of date, fortunately we can hook
        // ourselves into littlefs to catch this
        cwd.type = 0;
        cwd.id = 0;
        lfs2->mlist = &cwd;

        lfs2_pair_tole32(dir.pair);
        err = lfs2_dir_commit(lfs2, &pred, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8), dir.pair}));
        lfs2_pair_fromle32(dir.pair);
        if (err) {
            lfs2->mlist = cwd.next;
            return err;
        }

        lfs2->mlist = cwd.next;
        err = lfs2_fs_preporphans(lfs2, -1);
        if (err) {
            return err;
        }
    }

    // now insert into our parent block
    lfs2_pair_tole32(dir.pair);
    err = lfs2_dir_commit(lfs2, &cwd.m, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_CREATE, id, 0), NULL},
            {LFS2_MKTAG(LFS2_TYPE_DIR, id, nlen), path},
            {LFS2_MKTAG(LFS2_TYPE_DIRSTRUCT, id, 8), dir.pair},
            {LFS2_MKTAG_IF(!cwd.m.split,
                LFS2_TYPE_SOFTTAIL, 0x3ff, 8), dir.pair}));
    lfs2_pair_fromle32(dir.pair);
    if (err) {
        return err;
    }

    return 0;
}
#endif

static int lfs2_dir_open_(lfs2_t *lfs2, lfs2_dir_t *dir, const char *path) {
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &dir->m, &path, NULL);
    if (tag < 0) {
        return tag;
    }

    if (lfs2_tag_type3(tag) != LFS2_TYPE_DIR) {
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
            return res;
        }
        lfs2_pair_fromle32(pair);
    }

    // fetch first pair
    int err = lfs2_dir_fetch(lfs2, &dir->m, pair);
    if (err) {
        return err;
    }

    // setup entry
    dir->head[0] = dir->m.pair[0];
    dir->head[1] = dir->m.pair[1];
    dir->id = 0;
    dir->pos = 0;

    // add to list of mdirs
    dir->type = LFS2_TYPE_DIR;
    lfs2_mlist_append(lfs2, (struct lfs2_mlist *)dir);

    return 0;
}

static int lfs2_dir_close_(lfs2_t *lfs2, lfs2_dir_t *dir) {
    // remove from list of mdirs
    lfs2_mlist_remove(lfs2, (struct lfs2_mlist *)dir);

    return 0;
}

static int lfs2_dir_read_(lfs2_t *lfs2, lfs2_dir_t *dir, struct lfs2_info *info) {
    memset(info, 0, sizeof(*info));

    // special offset for '.' and '..'
    if (dir->pos == 0) {
        info->type = LFS2_TYPE_DIR;
        strcpy(info->name, ".");
        dir->pos += 1;
        return true;
    } else if (dir->pos == 1) {
        info->type = LFS2_TYPE_DIR;
        strcpy(info->name, "..");
        dir->pos += 1;
        return true;
    }

    while (true) {
        if (dir->id == dir->m.count) {
            if (!dir->m.split) {
                return false;
            }

            int err = lfs2_dir_fetch(lfs2, &dir->m, dir->m.tail);
            if (err) {
                return err;
            }

            dir->id = 0;
        }

        int err = lfs2_dir_getinfo(lfs2, &dir->m, dir->id, info);
        if (err && err != LFS2_ERR_NOENT) {
            return err;
        }

        dir->id += 1;
        if (err != LFS2_ERR_NOENT) {
            break;
        }
    }

    dir->pos += 1;
    return true;
}

static int lfs2_dir_seek_(lfs2_t *lfs2, lfs2_dir_t *dir, lfs2_off_t off) {
    // simply walk from head dir
    int err = lfs2_dir_rewind_(lfs2, dir);
    if (err) {
        return err;
    }

    // first two for ./..
    dir->pos = lfs2_min(2, off);
    off -= dir->pos;

    // skip superblock entry
    dir->id = (off > 0 && lfs2_pair_cmp(dir->head, lfs2->root) == 0);

    while (off > 0) {
        if (dir->id == dir->m.count) {
            if (!dir->m.split) {
                return LFS2_ERR_INVAL;
            }

            err = lfs2_dir_fetch(lfs2, &dir->m, dir->m.tail);
            if (err) {
                return err;
            }

            dir->id = 0;
        }

        int diff = lfs2_min(dir->m.count - dir->id, off);
        dir->id += diff;
        dir->pos += diff;
        off -= diff;
    }

    return 0;
}

static lfs2_soff_t lfs2_dir_tell_(lfs2_t *lfs2, lfs2_dir_t *dir) {
    (void)lfs2;
    return dir->pos;
}

static int lfs2_dir_rewind_(lfs2_t *lfs2, lfs2_dir_t *dir) {
    // reload the head dir
    int err = lfs2_dir_fetch(lfs2, &dir->m, dir->head);
    if (err) {
        return err;
    }

    dir->id = 0;
    dir->pos = 0;
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

        current -= 1 << skip;
    }

    *block = head;
    *off = pos;
    return 0;
}

#ifndef LFS2_READONLY
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

            lfs2_size_t noff = size - 1;
            lfs2_off_t index = lfs2_ctz_index(lfs2, &noff);
            noff = noff + 1;

            // just copy out the last block if it is incomplete
            if (noff != lfs2->cfg->block_size) {
                for (lfs2_off_t i = 0; i < noff; i++) {
                    uint8_t data;
                    err = lfs2_bd_read(lfs2,
                            NULL, rcache, noff-i,
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
                *off = noff;
                return 0;
            }

            // append block
            index += 1;
            lfs2_size_t skips = lfs2_ctz(index) + 1;
            lfs2_block_t nhead = head;
            for (lfs2_off_t i = 0; i < skips; i++) {
                nhead = lfs2_tole32(nhead);
                err = lfs2_bd_prog(lfs2, pcache, rcache, true,
                        nblock, 4*i, &nhead, 4);
                nhead = lfs2_fromle32(nhead);
                if (err) {
                    if (err == LFS2_ERR_CORRUPT) {
                        goto relocate;
                    }
                    return err;
                }

                if (i != skips-1) {
                    err = lfs2_bd_read(lfs2,
                            NULL, rcache, sizeof(nhead),
                            nhead, 4*i, &nhead, sizeof(nhead));
                    nhead = lfs2_fromle32(nhead);
                    if (err) {
                        return err;
                    }
                }
            }

            *block = nblock;
            *off = 4*skips;
            return 0;
        }

relocate:
        LFS2_DEBUG("Bad block at 0x%"PRIx32, nblock);

        // just clear cache and try a new block
        lfs2_cache_drop(lfs2, pcache);
    }
}
#endif

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
static int lfs2_file_opencfg_(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags,
        const struct lfs2_file_config *cfg) {
#ifndef LFS2_READONLY
    // deorphan if we haven't yet, needed at most once after poweron
    if ((flags & LFS2_O_WRONLY) == LFS2_O_WRONLY) {
        int err = lfs2_fs_forceconsistency(lfs2);
        if (err) {
            return err;
        }
    }
#else
    LFS2_ASSERT((flags & LFS2_O_RDONLY) == LFS2_O_RDONLY);
#endif

    // setup simple file details
    int err;
    file->cfg = cfg;
    file->flags = flags;
    file->pos = 0;
    file->off = 0;
    file->cache.buffer = NULL;

    // allocate entry for file if it doesn't exist
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &file->m, &path, &file->id);
    if (tag < 0 && !(tag == LFS2_ERR_NOENT && lfs2_path_islast(path))) {
        err = tag;
        goto cleanup;
    }

    // get id, add to list of mdirs to catch update changes
    file->type = LFS2_TYPE_REG;
    lfs2_mlist_append(lfs2, (struct lfs2_mlist *)file);

#ifdef LFS2_READONLY
    if (tag == LFS2_ERR_NOENT) {
        err = LFS2_ERR_NOENT;
        goto cleanup;
#else
    if (tag == LFS2_ERR_NOENT) {
        if (!(flags & LFS2_O_CREAT)) {
            err = LFS2_ERR_NOENT;
            goto cleanup;
        }

        // don't allow trailing slashes
        if (lfs2_path_isdir(path)) {
            err = LFS2_ERR_NOTDIR;
            goto cleanup;
        }

        // check that name fits
        lfs2_size_t nlen = lfs2_path_namelen(path);
        if (nlen > lfs2->name_max) {
            err = LFS2_ERR_NAMETOOLONG;
            goto cleanup;
        }

        // get next slot and create entry to remember name
        err = lfs2_dir_commit(lfs2, &file->m, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_CREATE, file->id, 0), NULL},
                {LFS2_MKTAG(LFS2_TYPE_REG, file->id, nlen), path},
                {LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, file->id, 0), NULL}));

        // it may happen that the file name doesn't fit in the metadata blocks, e.g., a 256 byte file name will
        // not fit in a 128 byte block.
        err = (err == LFS2_ERR_NOSPC) ? LFS2_ERR_NAMETOOLONG : err;
        if (err) {
            goto cleanup;
        }

        tag = LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, 0);
    } else if (flags & LFS2_O_EXCL) {
        err = LFS2_ERR_EXIST;
        goto cleanup;
#endif
    } else if (lfs2_tag_type3(tag) != LFS2_TYPE_REG) {
        err = LFS2_ERR_ISDIR;
        goto cleanup;
#ifndef LFS2_READONLY
    } else if (flags & LFS2_O_TRUNC) {
        // truncate if requested
        tag = LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, file->id, 0);
        file->flags |= LFS2_F_DIRTY;
#endif
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
        // if opened for read / read-write operations
        if ((file->flags & LFS2_O_RDONLY) == LFS2_O_RDONLY) {
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

#ifndef LFS2_READONLY
        // if opened for write / read-write operations
        if ((file->flags & LFS2_O_WRONLY) == LFS2_O_WRONLY) {
            if (file->cfg->attrs[i].size > lfs2->attr_max) {
                err = LFS2_ERR_NOSPC;
                goto cleanup;
            }

            file->flags |= LFS2_F_DIRTY;
        }
#endif
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

    return 0;

cleanup:
    // clean up lingering resources
#ifndef LFS2_READONLY
    file->flags |= LFS2_F_ERRED;
#endif
    lfs2_file_close_(lfs2, file);
    return err;
}

#ifndef LFS2_NO_MALLOC
static int lfs2_file_open_(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags) {
    static const struct lfs2_file_config defaults = {0};
    int err = lfs2_file_opencfg_(lfs2, file, path, flags, &defaults);
    return err;
}
#endif

static int lfs2_file_close_(lfs2_t *lfs2, lfs2_file_t *file) {
#ifndef LFS2_READONLY
    int err = lfs2_file_sync_(lfs2, file);
#else
    int err = 0;
#endif

    // remove from list of mdirs
    lfs2_mlist_remove(lfs2, (struct lfs2_mlist*)file);

    // clean up memory
    if (!file->cfg->buffer) {
        lfs2_free(file->cache.buffer);
    }

    return err;
}


#ifndef LFS2_READONLY
static int lfs2_file_relocate(lfs2_t *lfs2, lfs2_file_t *file) {
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
        LFS2_DEBUG("Bad block at 0x%"PRIx32, nblock);

        // just clear cache and try a new block
        lfs2_cache_drop(lfs2, &lfs2->pcache);
    }
}
#endif

#ifndef LFS2_READONLY
static int lfs2_file_outline(lfs2_t *lfs2, lfs2_file_t *file) {
    file->off = file->pos;
    lfs2_alloc_ckpoint(lfs2);
    int err = lfs2_file_relocate(lfs2, file);
    if (err) {
        return err;
    }

    file->flags &= ~LFS2_F_INLINE;
    return 0;
}
#endif

static int lfs2_file_flush(lfs2_t *lfs2, lfs2_file_t *file) {
    if (file->flags & LFS2_F_READING) {
        if (!(file->flags & LFS2_F_INLINE)) {
            lfs2_cache_drop(lfs2, &file->cache);
        }
        file->flags &= ~LFS2_F_READING;
    }

#ifndef LFS2_READONLY
    if (file->flags & LFS2_F_WRITING) {
        lfs2_off_t pos = file->pos;

        if (!(file->flags & LFS2_F_INLINE)) {
            // copy over anything after current branch
            lfs2_file_t orig = {
                .ctz.head = file->ctz.head,
                .ctz.size = file->ctz.size,
                .flags = LFS2_O_RDONLY,
                .pos = file->pos,
                .cache = lfs2->rcache,
            };
            lfs2_cache_drop(lfs2, &lfs2->rcache);

            while (file->pos < file->ctz.size) {
                // copy over a byte at a time, leave it up to caching
                // to make this efficient
                uint8_t data;
                lfs2_ssize_t res = lfs2_file_flushedread(lfs2, &orig, &data, 1);
                if (res < 0) {
                    return res;
                }

                res = lfs2_file_flushedwrite(lfs2, file, &data, 1);
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
                LFS2_DEBUG("Bad block at 0x%"PRIx32, file->block);
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
#endif

    return 0;
}

#ifndef LFS2_READONLY
static int lfs2_file_sync_(lfs2_t *lfs2, lfs2_file_t *file) {
    if (file->flags & LFS2_F_ERRED) {
        // it's not safe to do anything if our file errored
        return 0;
    }

    int err = lfs2_file_flush(lfs2, file);
    if (err) {
        file->flags |= LFS2_F_ERRED;
        return err;
    }


    if ((file->flags & LFS2_F_DIRTY) &&
            !lfs2_pair_isnull(file->m.pair)) {
        // before we commit metadata, we need sync the disk to make sure
        // data writes don't complete after metadata writes
        if (!(file->flags & LFS2_F_INLINE)) {
            err = lfs2_bd_sync(lfs2, &lfs2->pcache, &lfs2->rcache, false);
            if (err) {
                return err;
            }
        }

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
            file->flags |= LFS2_F_ERRED;
            return err;
        }

        file->flags &= ~LFS2_F_DIRTY;
    }

    return 0;
}
#endif

static lfs2_ssize_t lfs2_file_flushedread(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size) {
    uint8_t *data = buffer;
    lfs2_size_t nsize = size;

    if (file->pos >= file->ctz.size) {
        // eof if past end
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
                return err;
            }
        } else {
            int err = lfs2_bd_read(lfs2,
                    NULL, &file->cache, lfs2->cfg->block_size,
                    file->block, file->off, data, diff);
            if (err) {
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;
    }

    return size;
}

static lfs2_ssize_t lfs2_file_read_(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size) {
    LFS2_ASSERT((file->flags & LFS2_O_RDONLY) == LFS2_O_RDONLY);

#ifndef LFS2_READONLY
    if (file->flags & LFS2_F_WRITING) {
        // flush out any writes
        int err = lfs2_file_flush(lfs2, file);
        if (err) {
            return err;
        }
    }
#endif

    return lfs2_file_flushedread(lfs2, file, buffer, size);
}


#ifndef LFS2_READONLY
static lfs2_ssize_t lfs2_file_flushedwrite(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size) {
    const uint8_t *data = buffer;
    lfs2_size_t nsize = size;

    if ((file->flags & LFS2_F_INLINE) &&
            lfs2_max(file->pos+nsize, file->ctz.size) > lfs2->inline_max) {
        // inline file doesn't fit anymore
        int err = lfs2_file_outline(lfs2, file);
        if (err) {
            file->flags |= LFS2_F_ERRED;
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
                            file->pos-1, &file->block, &(lfs2_off_t){0});
                    if (err) {
                        file->flags |= LFS2_F_ERRED;
                        return err;
                    }

                    // mark cache as dirty since we may have read data into it
                    lfs2_cache_zero(lfs2, &file->cache);
                }

                // extend file with new blocks
                lfs2_alloc_ckpoint(lfs2);
                int err = lfs2_ctz_extend(lfs2, &file->cache, &lfs2->rcache,
                        file->block, file->pos,
                        &file->block, &file->off);
                if (err) {
                    file->flags |= LFS2_F_ERRED;
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
                return err;
            }

            break;
relocate:
            err = lfs2_file_relocate(lfs2, file);
            if (err) {
                file->flags |= LFS2_F_ERRED;
                return err;
            }
        }

        file->pos += diff;
        file->off += diff;
        data += diff;
        nsize -= diff;

        lfs2_alloc_ckpoint(lfs2);
    }

    return size;
}

static lfs2_ssize_t lfs2_file_write_(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size) {
    LFS2_ASSERT((file->flags & LFS2_O_WRONLY) == LFS2_O_WRONLY);

    if (file->flags & LFS2_F_READING) {
        // drop any reads
        int err = lfs2_file_flush(lfs2, file);
        if (err) {
            return err;
        }
    }

    if ((file->flags & LFS2_O_APPEND) && file->pos < file->ctz.size) {
        file->pos = file->ctz.size;
    }

    if (file->pos + size > lfs2->file_max) {
        // Larger than file limit?
        return LFS2_ERR_FBIG;
    }

    if (!(file->flags & LFS2_F_WRITING) && file->pos > file->ctz.size) {
        // fill with zeros
        lfs2_off_t pos = file->pos;
        file->pos = file->ctz.size;

        while (file->pos < pos) {
            lfs2_ssize_t res = lfs2_file_flushedwrite(lfs2, file, &(uint8_t){0}, 1);
            if (res < 0) {
                return res;
            }
        }
    }

    lfs2_ssize_t nsize = lfs2_file_flushedwrite(lfs2, file, buffer, size);
    if (nsize < 0) {
        return nsize;
    }

    file->flags &= ~LFS2_F_ERRED;
    return nsize;
}
#endif

static lfs2_soff_t lfs2_file_seek_(lfs2_t *lfs2, lfs2_file_t *file,
        lfs2_soff_t off, int whence) {
    // find new pos
    //
    // fortunately for us, littlefs is limited to 31-bit file sizes, so we
    // don't have to worry too much about integer overflow
    lfs2_off_t npos = file->pos;
    if (whence == LFS2_SEEK_SET) {
        npos = off;
    } else if (whence == LFS2_SEEK_CUR) {
        npos = file->pos + (lfs2_off_t)off;
    } else if (whence == LFS2_SEEK_END) {
        npos = (lfs2_off_t)lfs2_file_size_(lfs2, file) + (lfs2_off_t)off;
    }

    if (npos > lfs2->file_max) {
        // file position out of range
        return LFS2_ERR_INVAL;
    }

    if (file->pos == npos) {
        // noop - position has not changed
        return npos;
    }

    // if we're only reading and our new offset is still in the file's cache
    // we can avoid flushing and needing to reread the data
    if ((file->flags & LFS2_F_READING)
            && file->off != lfs2->cfg->block_size) {
        int oindex = lfs2_ctz_index(lfs2, &(lfs2_off_t){file->pos});
        lfs2_off_t noff = npos;
        int nindex = lfs2_ctz_index(lfs2, &noff);
        if (oindex == nindex
                && noff >= file->cache.off
                && noff < file->cache.off + file->cache.size) {
            file->pos = npos;
            file->off = noff;
            return npos;
        }
    }

    // write out everything beforehand, may be noop if rdonly
    int err = lfs2_file_flush(lfs2, file);
    if (err) {
        return err;
    }

    // update pos
    file->pos = npos;
    return npos;
}

#ifndef LFS2_READONLY
static int lfs2_file_truncate_(lfs2_t *lfs2, lfs2_file_t *file, lfs2_off_t size) {
    LFS2_ASSERT((file->flags & LFS2_O_WRONLY) == LFS2_O_WRONLY);

    if (size > LFS2_FILE_MAX) {
        return LFS2_ERR_INVAL;
    }

    lfs2_off_t pos = file->pos;
    lfs2_off_t oldsize = lfs2_file_size_(lfs2, file);
    if (size < oldsize) {
        // revert to inline file?
        if (size <= lfs2->inline_max) {
            // flush+seek to head
            lfs2_soff_t res = lfs2_file_seek_(lfs2, file, 0, LFS2_SEEK_SET);
            if (res < 0) {
                return (int)res;
            }

            // read our data into rcache temporarily
            lfs2_cache_drop(lfs2, &lfs2->rcache);
            res = lfs2_file_flushedread(lfs2, file,
                    lfs2->rcache.buffer, size);
            if (res < 0) {
                return (int)res;
            }

            file->ctz.head = LFS2_BLOCK_INLINE;
            file->ctz.size = size;
            file->flags |= LFS2_F_DIRTY | LFS2_F_READING | LFS2_F_INLINE;
            file->cache.block = file->ctz.head;
            file->cache.off = 0;
            file->cache.size = lfs2->cfg->cache_size;
            memcpy(file->cache.buffer, lfs2->rcache.buffer, size);

        } else {
            // need to flush since directly changing metadata
            int err = lfs2_file_flush(lfs2, file);
            if (err) {
                return err;
            }

            // lookup new head in ctz skip list
            err = lfs2_ctz_find(lfs2, NULL, &file->cache,
                    file->ctz.head, file->ctz.size,
                    size-1, &file->block, &(lfs2_off_t){0});
            if (err) {
                return err;
            }

            // need to set pos/block/off consistently so seeking back to
            // the old position does not get confused
            file->pos = size;
            file->ctz.head = file->block;
            file->ctz.size = size;
            file->flags |= LFS2_F_DIRTY | LFS2_F_READING;
        }
    } else if (size > oldsize) {
        // flush+seek if not already at end
        lfs2_soff_t res = lfs2_file_seek_(lfs2, file, 0, LFS2_SEEK_END);
        if (res < 0) {
            return (int)res;
        }

        // fill with zeros
        while (file->pos < size) {
            res = lfs2_file_write_(lfs2, file, &(uint8_t){0}, 1);
            if (res < 0) {
                return (int)res;
            }
        }
    }

    // restore pos
    lfs2_soff_t res = lfs2_file_seek_(lfs2, file, pos, LFS2_SEEK_SET);
    if (res < 0) {
      return (int)res;
    }

    return 0;
}
#endif

static lfs2_soff_t lfs2_file_tell_(lfs2_t *lfs2, lfs2_file_t *file) {
    (void)lfs2;
    return file->pos;
}

static int lfs2_file_rewind_(lfs2_t *lfs2, lfs2_file_t *file) {
    lfs2_soff_t res = lfs2_file_seek_(lfs2, file, 0, LFS2_SEEK_SET);
    if (res < 0) {
        return (int)res;
    }

    return 0;
}

static lfs2_soff_t lfs2_file_size_(lfs2_t *lfs2, lfs2_file_t *file) {
    (void)lfs2;

#ifndef LFS2_READONLY
    if (file->flags & LFS2_F_WRITING) {
        return lfs2_max(file->pos, file->ctz.size);
    }
#endif

    return file->ctz.size;
}


/// General fs operations ///
static int lfs2_stat_(lfs2_t *lfs2, const char *path, struct lfs2_info *info) {
    lfs2_mdir_t cwd;
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &cwd, &path, NULL);
    if (tag < 0) {
        return (int)tag;
    }

    // only allow trailing slashes on dirs
    if (strchr(path, '/') != NULL
            && lfs2_tag_type3(tag) != LFS2_TYPE_DIR) {
        return LFS2_ERR_NOTDIR;
    }

    return lfs2_dir_getinfo(lfs2, &cwd, lfs2_tag_id(tag), info);
}

#ifndef LFS2_READONLY
static int lfs2_remove_(lfs2_t *lfs2, const char *path) {
    // deorphan if we haven't yet, needed at most once after poweron
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        return err;
    }

    lfs2_mdir_t cwd;
    lfs2_stag_t tag = lfs2_dir_find(lfs2, &cwd, &path, NULL);
    if (tag < 0 || lfs2_tag_id(tag) == 0x3ff) {
        return (tag < 0) ? (int)tag : LFS2_ERR_INVAL;
    }

    struct lfs2_mlist dir;
    dir.next = lfs2->mlist;
    if (lfs2_tag_type3(tag) == LFS2_TYPE_DIR) {
        // must be empty before removal
        lfs2_block_t pair[2];
        lfs2_stag_t res = lfs2_dir_get(lfs2, &cwd, LFS2_MKTAG(0x700, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_STRUCT, lfs2_tag_id(tag), 8), pair);
        if (res < 0) {
            return (int)res;
        }
        lfs2_pair_fromle32(pair);

        err = lfs2_dir_fetch(lfs2, &dir.m, pair);
        if (err) {
            return err;
        }

        if (dir.m.count > 0 || dir.m.split) {
            return LFS2_ERR_NOTEMPTY;
        }

        // mark fs as orphaned
        err = lfs2_fs_preporphans(lfs2, +1);
        if (err) {
            return err;
        }

        // I know it's crazy but yes, dir can be changed by our parent's
        // commit (if predecessor is child)
        dir.type = 0;
        dir.id = 0;
        lfs2->mlist = &dir;
    }

    // delete the entry
    err = lfs2_dir_commit(lfs2, &cwd, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_DELETE, lfs2_tag_id(tag), 0), NULL}));
    if (err) {
        lfs2->mlist = dir.next;
        return err;
    }

    lfs2->mlist = dir.next;
    if (lfs2_gstate_hasorphans(&lfs2->gstate)) {
        LFS2_ASSERT(lfs2_tag_type3(tag) == LFS2_TYPE_DIR);

        // fix orphan
        err = lfs2_fs_preporphans(lfs2, -1);
        if (err) {
            return err;
        }

        err = lfs2_fs_pred(lfs2, dir.m.pair, &cwd);
        if (err) {
            return err;
        }

        err = lfs2_dir_drop(lfs2, &cwd, &dir.m);
        if (err) {
            return err;
        }
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_rename_(lfs2_t *lfs2, const char *oldpath, const char *newpath) {
    // deorphan if we haven't yet, needed at most once after poweron
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        return err;
    }

    // find old entry
    lfs2_mdir_t oldcwd;
    lfs2_stag_t oldtag = lfs2_dir_find(lfs2, &oldcwd, &oldpath, NULL);
    if (oldtag < 0 || lfs2_tag_id(oldtag) == 0x3ff) {
        return (oldtag < 0) ? (int)oldtag : LFS2_ERR_INVAL;
    }

    // find new entry
    lfs2_mdir_t newcwd;
    uint16_t newid;
    lfs2_stag_t prevtag = lfs2_dir_find(lfs2, &newcwd, &newpath, &newid);
    if ((prevtag < 0 || lfs2_tag_id(prevtag) == 0x3ff) &&
            !(prevtag == LFS2_ERR_NOENT && lfs2_path_islast(newpath))) {
        return (prevtag < 0) ? (int)prevtag : LFS2_ERR_INVAL;
    }

    // if we're in the same pair there's a few special cases...
    bool samepair = (lfs2_pair_cmp(oldcwd.pair, newcwd.pair) == 0);
    uint16_t newoldid = lfs2_tag_id(oldtag);

    struct lfs2_mlist prevdir;
    prevdir.next = lfs2->mlist;
    if (prevtag == LFS2_ERR_NOENT) {
        // if we're a file, don't allow trailing slashes
        if (lfs2_path_isdir(newpath)
                && lfs2_tag_type3(oldtag) != LFS2_TYPE_DIR) {
            return LFS2_ERR_NOTDIR;
        }

        // check that name fits
        lfs2_size_t nlen = lfs2_path_namelen(newpath);
        if (nlen > lfs2->name_max) {
            return LFS2_ERR_NAMETOOLONG;
        }

        // there is a small chance we are being renamed in the same
        // directory/ to an id less than our old id, the global update
        // to handle this is a bit messy
        if (samepair && newid <= newoldid) {
            newoldid += 1;
        }
    } else if (lfs2_tag_type3(prevtag) != lfs2_tag_type3(oldtag)) {
        return (lfs2_tag_type3(prevtag) == LFS2_TYPE_DIR)
                ? LFS2_ERR_ISDIR
                : LFS2_ERR_NOTDIR;
    } else if (samepair && newid == newoldid) {
        // we're renaming to ourselves??
        return 0;
    } else if (lfs2_tag_type3(prevtag) == LFS2_TYPE_DIR) {
        // must be empty before removal
        lfs2_block_t prevpair[2];
        lfs2_stag_t res = lfs2_dir_get(lfs2, &newcwd, LFS2_MKTAG(0x700, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_STRUCT, newid, 8), prevpair);
        if (res < 0) {
            return (int)res;
        }
        lfs2_pair_fromle32(prevpair);

        // must be empty before removal
        err = lfs2_dir_fetch(lfs2, &prevdir.m, prevpair);
        if (err) {
            return err;
        }

        if (prevdir.m.count > 0 || prevdir.m.split) {
            return LFS2_ERR_NOTEMPTY;
        }

        // mark fs as orphaned
        err = lfs2_fs_preporphans(lfs2, +1);
        if (err) {
            return err;
        }

        // I know it's crazy but yes, dir can be changed by our parent's
        // commit (if predecessor is child)
        prevdir.type = 0;
        prevdir.id = 0;
        lfs2->mlist = &prevdir;
    }

    if (!samepair) {
        lfs2_fs_prepmove(lfs2, newoldid, oldcwd.pair);
    }

    // move over all attributes
    err = lfs2_dir_commit(lfs2, &newcwd, LFS2_MKATTRS(
            {LFS2_MKTAG_IF(prevtag != LFS2_ERR_NOENT,
                LFS2_TYPE_DELETE, newid, 0), NULL},
            {LFS2_MKTAG(LFS2_TYPE_CREATE, newid, 0), NULL},
            {LFS2_MKTAG(lfs2_tag_type3(oldtag),
                newid, lfs2_path_namelen(newpath)), newpath},
            {LFS2_MKTAG(LFS2_FROM_MOVE, newid, lfs2_tag_id(oldtag)), &oldcwd},
            {LFS2_MKTAG_IF(samepair,
                LFS2_TYPE_DELETE, newoldid, 0), NULL}));
    if (err) {
        lfs2->mlist = prevdir.next;
        return err;
    }

    // let commit clean up after move (if we're different! otherwise move
    // logic already fixed it for us)
    if (!samepair && lfs2_gstate_hasmove(&lfs2->gstate)) {
        // prep gstate and delete move id
        lfs2_fs_prepmove(lfs2, 0x3ff, NULL);
        err = lfs2_dir_commit(lfs2, &oldcwd, LFS2_MKATTRS(
                {LFS2_MKTAG(LFS2_TYPE_DELETE, lfs2_tag_id(oldtag), 0), NULL}));
        if (err) {
            lfs2->mlist = prevdir.next;
            return err;
        }
    }

    lfs2->mlist = prevdir.next;
    if (lfs2_gstate_hasorphans(&lfs2->gstate)) {
        LFS2_ASSERT(prevtag != LFS2_ERR_NOENT
                && lfs2_tag_type3(prevtag) == LFS2_TYPE_DIR);

        // fix orphan
        err = lfs2_fs_preporphans(lfs2, -1);
        if (err) {
            return err;
        }

        err = lfs2_fs_pred(lfs2, prevdir.m.pair, &newcwd);
        if (err) {
            return err;
        }

        err = lfs2_dir_drop(lfs2, &newcwd, &prevdir.m);
        if (err) {
            return err;
        }
    }

    return 0;
}
#endif

static lfs2_ssize_t lfs2_getattr_(lfs2_t *lfs2, const char *path,
        uint8_t type, void *buffer, lfs2_size_t size) {
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

    tag = lfs2_dir_get(lfs2, &cwd, LFS2_MKTAG(0x7ff, 0x3ff, 0),
            LFS2_MKTAG(LFS2_TYPE_USERATTR + type,
                id, lfs2_min(size, lfs2->attr_max)),
            buffer);
    if (tag < 0) {
        if (tag == LFS2_ERR_NOENT) {
            return LFS2_ERR_NOATTR;
        }

        return tag;
    }

    return lfs2_tag_size(tag);
}

#ifndef LFS2_READONLY
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
#endif

#ifndef LFS2_READONLY
static int lfs2_setattr_(lfs2_t *lfs2, const char *path,
        uint8_t type, const void *buffer, lfs2_size_t size) {
    if (size > lfs2->attr_max) {
        return LFS2_ERR_NOSPC;
    }

    return lfs2_commitattr(lfs2, path, type, buffer, size);
}
#endif

#ifndef LFS2_READONLY
static int lfs2_removeattr_(lfs2_t *lfs2, const char *path, uint8_t type) {
    return lfs2_commitattr(lfs2, path, type, NULL, 0x3ff);
}
#endif


/// Filesystem operations ///

// compile time checks, see lfs2.h for why these limits exist
#if LFS2_NAME_MAX > 1022
#error "Invalid LFS2_NAME_MAX, must be <= 1022"
#endif

#if LFS2_FILE_MAX > 2147483647
#error "Invalid LFS2_FILE_MAX, must be <= 2147483647"
#endif

#if LFS2_ATTR_MAX > 1022
#error "Invalid LFS2_ATTR_MAX, must be <= 1022"
#endif

// common filesystem initialization
static int lfs2_init(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    lfs2->cfg = cfg;
    lfs2->block_count = cfg->block_count;  // May be 0
    int err = 0;

#ifdef LFS2_MULTIVERSION
    // this driver only supports minor version < current minor version
    LFS2_ASSERT(!lfs2->cfg->disk_version || (
            (0xffff & (lfs2->cfg->disk_version >> 16))
                    == LFS2_DISK_VERSION_MAJOR
                && (0xffff & (lfs2->cfg->disk_version >> 0))
                    <= LFS2_DISK_VERSION_MINOR));
#endif

    // check that bool is a truthy-preserving type
    //
    // note the most common reason for this failure is a before-c99 compiler,
    // which littlefs currently does not support
    LFS2_ASSERT((bool)0x80000000);

    // check that the required io functions are provided
    LFS2_ASSERT(lfs2->cfg->read != NULL);
#ifndef LFS2_READONLY
    LFS2_ASSERT(lfs2->cfg->prog != NULL);
    LFS2_ASSERT(lfs2->cfg->erase != NULL);
    LFS2_ASSERT(lfs2->cfg->sync != NULL);
#endif

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

    // check that the block size is large enough to fit all ctz pointers
    LFS2_ASSERT(lfs2->cfg->block_size >= 128);
    // this is the exact calculation for all ctz pointers, if this fails
    // and the simpler assert above does not, math must be broken
    LFS2_ASSERT(4*lfs2_npw2(0xffffffff / (lfs2->cfg->block_size-2*4))
            <= lfs2->cfg->block_size);

    // block_cycles = 0 is no longer supported.
    //
    // block_cycles is the number of erase cycles before littlefs evicts
    // metadata logs as a part of wear leveling. Suggested values are in the
    // range of 100-1000, or set block_cycles to -1 to disable block-level
    // wear-leveling.
    LFS2_ASSERT(lfs2->cfg->block_cycles != 0);

    // check that compact_thresh makes sense
    //
    // metadata can't be compacted below block_size/2, and metadata can't
    // exceed a block_size
    LFS2_ASSERT(lfs2->cfg->compact_thresh == 0
            || lfs2->cfg->compact_thresh >= lfs2->cfg->block_size/2);
    LFS2_ASSERT(lfs2->cfg->compact_thresh == (lfs2_size_t)-1
            || lfs2->cfg->compact_thresh <= lfs2->cfg->block_size);

    // check that metadata_max is a multiple of read_size and prog_size,
    // and a factor of the block_size
    LFS2_ASSERT(!lfs2->cfg->metadata_max
            || lfs2->cfg->metadata_max % lfs2->cfg->read_size == 0);
    LFS2_ASSERT(!lfs2->cfg->metadata_max
            || lfs2->cfg->metadata_max % lfs2->cfg->prog_size == 0);
    LFS2_ASSERT(!lfs2->cfg->metadata_max
            || lfs2->cfg->block_size % lfs2->cfg->metadata_max == 0);

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

    // setup lookahead buffer, note mount finishes initializing this after
    // we establish a decent pseudo-random seed
    LFS2_ASSERT(lfs2->cfg->lookahead_size > 0);
    if (lfs2->cfg->lookahead_buffer) {
        lfs2->lookahead.buffer = lfs2->cfg->lookahead_buffer;
    } else {
        lfs2->lookahead.buffer = lfs2_malloc(lfs2->cfg->lookahead_size);
        if (!lfs2->lookahead.buffer) {
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

    LFS2_ASSERT(lfs2->cfg->metadata_max <= lfs2->cfg->block_size);

    LFS2_ASSERT(lfs2->cfg->inline_max == (lfs2_size_t)-1
            || lfs2->cfg->inline_max <= lfs2->cfg->cache_size);
    LFS2_ASSERT(lfs2->cfg->inline_max == (lfs2_size_t)-1
            || lfs2->cfg->inline_max <= lfs2->attr_max);
    LFS2_ASSERT(lfs2->cfg->inline_max == (lfs2_size_t)-1
            || lfs2->cfg->inline_max <= ((lfs2->cfg->metadata_max)
                ? lfs2->cfg->metadata_max
                : lfs2->cfg->block_size)/8);
    lfs2->inline_max = lfs2->cfg->inline_max;
    if (lfs2->inline_max == (lfs2_size_t)-1) {
        lfs2->inline_max = 0;
    } else if (lfs2->inline_max == 0) {
        lfs2->inline_max = lfs2_min(
                lfs2->cfg->cache_size,
                lfs2_min(
                    lfs2->attr_max,
                    ((lfs2->cfg->metadata_max)
                        ? lfs2->cfg->metadata_max
                        : lfs2->cfg->block_size)/8));
    }

    // setup default state
    lfs2->root[0] = LFS2_BLOCK_NULL;
    lfs2->root[1] = LFS2_BLOCK_NULL;
    lfs2->mlist = NULL;
    lfs2->seed = 0;
    lfs2->gdisk = (lfs2_gstate_t){0};
    lfs2->gstate = (lfs2_gstate_t){0};
    lfs2->gdelta = (lfs2_gstate_t){0};
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
        lfs2_free(lfs2->lookahead.buffer);
    }

    return 0;
}



#ifndef LFS2_READONLY
static int lfs2_format_(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    int err = 0;
    {
        err = lfs2_init(lfs2, cfg);
        if (err) {
            return err;
        }

        LFS2_ASSERT(cfg->block_count != 0);

        // create free lookahead
        memset(lfs2->lookahead.buffer, 0, lfs2->cfg->lookahead_size);
        lfs2->lookahead.start = 0;
        lfs2->lookahead.size = lfs2_min(8*lfs2->cfg->lookahead_size,
                lfs2->block_count);
        lfs2->lookahead.next = 0;
        lfs2_alloc_ckpoint(lfs2);

        // create root dir
        lfs2_mdir_t root;
        err = lfs2_dir_alloc(lfs2, &root);
        if (err) {
            goto cleanup;
        }

        // write one superblock
        lfs2_superblock_t superblock = {
            .version     = lfs2_fs_disk_version(lfs2),
            .block_size  = lfs2->cfg->block_size,
            .block_count = lfs2->block_count,
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

        // force compaction to prevent accidentally mounting any
        // older version of littlefs that may live on disk
        root.erased = false;
        err = lfs2_dir_commit(lfs2, &root, NULL, 0);
        if (err) {
            goto cleanup;
        }

        // sanity check that fetch works
        err = lfs2_dir_fetch(lfs2, &root, (const lfs2_block_t[2]){0, 1});
        if (err) {
            goto cleanup;
        }
    }

cleanup:
    lfs2_deinit(lfs2);
    return err;

}
#endif

struct lfs2_tortoise_t {
    lfs2_block_t pair[2];
    lfs2_size_t i;
    lfs2_size_t period;
};

static int lfs2_tortoise_detectcycles(
    const lfs2_mdir_t *dir, struct lfs2_tortoise_t *tortoise) {
    // detect cycles with Brent's algorithm
    if (lfs2_pair_issync(dir->tail, tortoise->pair)) {
        LFS2_WARN("Cycle detected in tail list");
        return LFS2_ERR_CORRUPT;
    }
    if (tortoise->i == tortoise->period) {
        tortoise->pair[0] = dir->tail[0];
        tortoise->pair[1] = dir->tail[1];
        tortoise->i = 0;
        tortoise->period *= 2;
    }
    tortoise->i += 1;

    return LFS2_ERR_OK;
}

static int lfs2_mount_(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    int err = lfs2_init(lfs2, cfg);
    if (err) {
        return err;
    }

    // scan directory blocks for superblock and any global updates
    lfs2_mdir_t dir = {.tail = {0, 1}};
    struct lfs2_tortoise_t tortoise = {
        .pair = {LFS2_BLOCK_NULL, LFS2_BLOCK_NULL},
        .i = 1,
        .period = 1,
    };
    while (!lfs2_pair_isnull(dir.tail)) {
        err = lfs2_tortoise_detectcycles(&dir, &tortoise);
        if (err < 0) {
            goto cleanup;
        }

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
            if (major_version != lfs2_fs_disk_version_major(lfs2)
                    || minor_version > lfs2_fs_disk_version_minor(lfs2)) {
                LFS2_ERROR("Invalid version "
                        "v%"PRIu16".%"PRIu16" != v%"PRIu16".%"PRIu16,
                        major_version,
                        minor_version,
                        lfs2_fs_disk_version_major(lfs2),
                        lfs2_fs_disk_version_minor(lfs2));
                err = LFS2_ERR_INVAL;
                goto cleanup;
            }

            // found older minor version? set an in-device only bit in the
            // gstate so we know we need to rewrite the superblock before
            // the first write
            bool needssuperblock = false;
            if (minor_version < lfs2_fs_disk_version_minor(lfs2)) {
                LFS2_DEBUG("Found older minor version "
                        "v%"PRIu16".%"PRIu16" < v%"PRIu16".%"PRIu16,
                        major_version,
                        minor_version,
                        lfs2_fs_disk_version_major(lfs2),
                        lfs2_fs_disk_version_minor(lfs2));
                needssuperblock = true;
            }
            // note this bit is reserved on disk, so fetching more gstate
            // will not interfere here
            lfs2_fs_prepsuperblock(lfs2, needssuperblock);

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

                // we also need to update inline_max in case attr_max changed
                lfs2->inline_max = lfs2_min(lfs2->inline_max, lfs2->attr_max);
            }

            // this is where we get the block_count from disk if block_count=0
            if (lfs2->cfg->block_count
                    && superblock.block_count != lfs2->cfg->block_count) {
                LFS2_ERROR("Invalid block count (%"PRIu32" != %"PRIu32")",
                        superblock.block_count, lfs2->cfg->block_count);
                err = LFS2_ERR_INVAL;
                goto cleanup;
            }

            lfs2->block_count = superblock.block_count;

            if (superblock.block_size != lfs2->cfg->block_size) {
                LFS2_ERROR("Invalid block size (%"PRIu32" != %"PRIu32")",
                        superblock.block_size, lfs2->cfg->block_size);
                err = LFS2_ERR_INVAL;
                goto cleanup;
            }
        }

        // has gstate?
        err = lfs2_dir_getgstate(lfs2, &dir, &lfs2->gstate);
        if (err) {
            goto cleanup;
        }
    }

    // update littlefs with gstate
    if (!lfs2_gstate_iszero(&lfs2->gstate)) {
        LFS2_DEBUG("Found pending gstate 0x%08"PRIx32"%08"PRIx32"%08"PRIx32,
                lfs2->gstate.tag,
                lfs2->gstate.pair[0],
                lfs2->gstate.pair[1]);
    }
    lfs2->gstate.tag += !lfs2_tag_isvalid(lfs2->gstate.tag);
    lfs2->gdisk = lfs2->gstate;

    // setup free lookahead, to distribute allocations uniformly across
    // boots, we start the allocator at a random location
    lfs2->lookahead.start = lfs2->seed % lfs2->block_count;
    lfs2_alloc_drop(lfs2);

    return 0;

cleanup:
    lfs2_unmount_(lfs2);
    return err;
}

static int lfs2_unmount_(lfs2_t *lfs2) {
    return lfs2_deinit(lfs2);
}


/// Filesystem filesystem operations ///
static int lfs2_fs_stat_(lfs2_t *lfs2, struct lfs2_fsinfo *fsinfo) {
    // if the superblock is up-to-date, we must be on the most recent
    // minor version of littlefs
    if (!lfs2_gstate_needssuperblock(&lfs2->gstate)) {
        fsinfo->disk_version = lfs2_fs_disk_version(lfs2);

    // otherwise we need to read the minor version on disk
    } else {
        // fetch the superblock
        lfs2_mdir_t dir;
        int err = lfs2_dir_fetch(lfs2, &dir, lfs2->root);
        if (err) {
            return err;
        }

        lfs2_superblock_t superblock;
        lfs2_stag_t tag = lfs2_dir_get(lfs2, &dir, LFS2_MKTAG(0x7ff, 0x3ff, 0),
                LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                &superblock);
        if (tag < 0) {
            return tag;
        }
        lfs2_superblock_fromle32(&superblock);

        // read the on-disk version
        fsinfo->disk_version = superblock.version;
    }

    // filesystem geometry
    fsinfo->block_size = lfs2->cfg->block_size;
    fsinfo->block_count = lfs2->block_count;

    // other on-disk configuration, we cache all of these for internal use
    fsinfo->name_max = lfs2->name_max;
    fsinfo->file_max = lfs2->file_max;
    fsinfo->attr_max = lfs2->attr_max;

    return 0;
}

int lfs2_fs_traverse_(lfs2_t *lfs2,
        int (*cb)(void *data, lfs2_block_t block), void *data,
        bool includeorphans) {
    // iterate over metadata pairs
    lfs2_mdir_t dir = {.tail = {0, 1}};

#ifdef LFS2_MIGRATE
    // also consider v1 blocks during migration
    if (lfs2->lfs21) {
        int err = lfs21_traverse(lfs2, cb, data);
        if (err) {
            return err;
        }

        dir.tail[0] = lfs2->root[0];
        dir.tail[1] = lfs2->root[1];
    }
#endif

    struct lfs2_tortoise_t tortoise = {
        .pair = {LFS2_BLOCK_NULL, LFS2_BLOCK_NULL},
        .i = 1,
        .period = 1,
    };
    int err = LFS2_ERR_OK;
    while (!lfs2_pair_isnull(dir.tail)) {
        err = lfs2_tortoise_detectcycles(&dir, &tortoise);
        if (err < 0) {
            return LFS2_ERR_CORRUPT;
        }

        for (int i = 0; i < 2; i++) {
            int err = cb(data, dir.tail[i]);
            if (err) {
                return err;
            }
        }

        // iterate through ids in directory
        int err = lfs2_dir_fetch(lfs2, &dir, dir.tail);
        if (err) {
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
                return tag;
            }
            lfs2_ctz_fromle32(&ctz);

            if (lfs2_tag_type3(tag) == LFS2_TYPE_CTZSTRUCT) {
                err = lfs2_ctz_traverse(lfs2, NULL, &lfs2->rcache,
                        ctz.head, ctz.size, cb, data);
                if (err) {
                    return err;
                }
            } else if (includeorphans &&
                    lfs2_tag_type3(tag) == LFS2_TYPE_DIRSTRUCT) {
                for (int i = 0; i < 2; i++) {
                    err = cb(data, (&ctz.head)[i]);
                    if (err) {
                        return err;
                    }
                }
            }
        }
    }

#ifndef LFS2_READONLY
    // iterate over any open files
    for (lfs2_file_t *f = (lfs2_file_t*)lfs2->mlist; f; f = f->next) {
        if (f->type != LFS2_TYPE_REG) {
            continue;
        }

        if ((f->flags & LFS2_F_DIRTY) && !(f->flags & LFS2_F_INLINE)) {
            int err = lfs2_ctz_traverse(lfs2, &f->cache, &lfs2->rcache,
                    f->ctz.head, f->ctz.size, cb, data);
            if (err) {
                return err;
            }
        }

        if ((f->flags & LFS2_F_WRITING) && !(f->flags & LFS2_F_INLINE)) {
            int err = lfs2_ctz_traverse(lfs2, &f->cache, &lfs2->rcache,
                    f->block, f->pos, cb, data);
            if (err) {
                return err;
            }
        }
    }
#endif

    return 0;
}

#ifndef LFS2_READONLY
static int lfs2_fs_pred(lfs2_t *lfs2,
        const lfs2_block_t pair[2], lfs2_mdir_t *pdir) {
    // iterate over all directory directory entries
    pdir->tail[0] = 0;
    pdir->tail[1] = 1;
    struct lfs2_tortoise_t tortoise = {
        .pair = {LFS2_BLOCK_NULL, LFS2_BLOCK_NULL},
        .i = 1,
        .period = 1,
    };
    int err = LFS2_ERR_OK;
    while (!lfs2_pair_isnull(pdir->tail)) {
        err = lfs2_tortoise_detectcycles(pdir, &tortoise);
        if (err < 0) {
            return LFS2_ERR_CORRUPT;
        }

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
#endif

#ifndef LFS2_READONLY
struct lfs2_fs_parent_match {
    lfs2_t *lfs2;
    const lfs2_block_t pair[2];
};
#endif

#ifndef LFS2_READONLY
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
#endif

#ifndef LFS2_READONLY
static lfs2_stag_t lfs2_fs_parent(lfs2_t *lfs2, const lfs2_block_t pair[2],
        lfs2_mdir_t *parent) {
    // use fetchmatch with callback to find pairs
    parent->tail[0] = 0;
    parent->tail[1] = 1;
    struct lfs2_tortoise_t tortoise = {
        .pair = {LFS2_BLOCK_NULL, LFS2_BLOCK_NULL},
        .i = 1,
        .period = 1,
    };
    int err = LFS2_ERR_OK;
    while (!lfs2_pair_isnull(parent->tail)) {
        err = lfs2_tortoise_detectcycles(parent, &tortoise);
        if (err < 0) {
            return err;
        }

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
#endif

static void lfs2_fs_prepsuperblock(lfs2_t *lfs2, bool needssuperblock) {
    lfs2->gstate.tag = (lfs2->gstate.tag & ~LFS2_MKTAG(0, 0, 0x200))
            | (uint32_t)needssuperblock << 9;
}

#ifndef LFS2_READONLY
static int lfs2_fs_preporphans(lfs2_t *lfs2, int8_t orphans) {
    LFS2_ASSERT(lfs2_tag_size(lfs2->gstate.tag) > 0x000 || orphans >= 0);
    LFS2_ASSERT(lfs2_tag_size(lfs2->gstate.tag) < 0x1ff || orphans <= 0);
    lfs2->gstate.tag += orphans;
    lfs2->gstate.tag = ((lfs2->gstate.tag & ~LFS2_MKTAG(0x800, 0, 0)) |
            ((uint32_t)lfs2_gstate_hasorphans(&lfs2->gstate) << 31));

    return 0;
}
#endif

#ifndef LFS2_READONLY
static void lfs2_fs_prepmove(lfs2_t *lfs2,
        uint16_t id, const lfs2_block_t pair[2]) {
    lfs2->gstate.tag = ((lfs2->gstate.tag & ~LFS2_MKTAG(0x7ff, 0x3ff, 0)) |
            ((id != 0x3ff) ? LFS2_MKTAG(LFS2_TYPE_DELETE, id, 0) : 0));
    lfs2->gstate.pair[0] = (id != 0x3ff) ? pair[0] : 0;
    lfs2->gstate.pair[1] = (id != 0x3ff) ? pair[1] : 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_fs_desuperblock(lfs2_t *lfs2) {
    if (!lfs2_gstate_needssuperblock(&lfs2->gstate)) {
        return 0;
    }

    LFS2_DEBUG("Rewriting superblock {0x%"PRIx32", 0x%"PRIx32"}",
            lfs2->root[0],
            lfs2->root[1]);

    lfs2_mdir_t root;
    int err = lfs2_dir_fetch(lfs2, &root, lfs2->root);
    if (err) {
        return err;
    }

    // write a new superblock
    lfs2_superblock_t superblock = {
        .version     = lfs2_fs_disk_version(lfs2),
        .block_size  = lfs2->cfg->block_size,
        .block_count = lfs2->block_count,
        .name_max    = lfs2->name_max,
        .file_max    = lfs2->file_max,
        .attr_max    = lfs2->attr_max,
    };

    lfs2_superblock_tole32(&superblock);
    err = lfs2_dir_commit(lfs2, &root, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
                &superblock}));
    if (err) {
        return err;
    }

    lfs2_fs_prepsuperblock(lfs2, false);
    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_fs_demove(lfs2_t *lfs2) {
    if (!lfs2_gstate_hasmove(&lfs2->gdisk)) {
        return 0;
    }

    // Fix bad moves
    LFS2_DEBUG("Fixing move {0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16,
            lfs2->gdisk.pair[0],
            lfs2->gdisk.pair[1],
            lfs2_tag_id(lfs2->gdisk.tag));

    // no other gstate is supported at this time, so if we found something else
    // something most likely went wrong in gstate calculation
    LFS2_ASSERT(lfs2_tag_type3(lfs2->gdisk.tag) == LFS2_TYPE_DELETE);

    // fetch and delete the moved entry
    lfs2_mdir_t movedir;
    int err = lfs2_dir_fetch(lfs2, &movedir, lfs2->gdisk.pair);
    if (err) {
        return err;
    }

    // prep gstate and delete move id
    uint16_t moveid = lfs2_tag_id(lfs2->gdisk.tag);
    lfs2_fs_prepmove(lfs2, 0x3ff, NULL);
    err = lfs2_dir_commit(lfs2, &movedir, LFS2_MKATTRS(
            {LFS2_MKTAG(LFS2_TYPE_DELETE, moveid, 0), NULL}));
    if (err) {
        return err;
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_fs_deorphan(lfs2_t *lfs2, bool powerloss) {
    if (!lfs2_gstate_hasorphans(&lfs2->gstate)) {
        return 0;
    }

    // Check for orphans in two separate passes:
    // - 1 for half-orphans (relocations)
    // - 2 for full-orphans (removes/renames)
    //
    // Two separate passes are needed as half-orphans can contain outdated
    // references to full-orphans, effectively hiding them from the deorphan
    // search.
    //
    int pass = 0;
    while (pass < 2) {
        // Fix any orphans
        lfs2_mdir_t pdir = {.split = true, .tail = {0, 1}};
        lfs2_mdir_t dir;
        bool moreorphans = false;

        // iterate over all directory directory entries
        while (!lfs2_pair_isnull(pdir.tail)) {
            int err = lfs2_dir_fetch(lfs2, &dir, pdir.tail);
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

                if (pass == 0 && tag != LFS2_ERR_NOENT) {
                    lfs2_block_t pair[2];
                    lfs2_stag_t state = lfs2_dir_get(lfs2, &parent,
                            LFS2_MKTAG(0x7ff, 0x3ff, 0), tag, pair);
                    if (state < 0) {
                        return state;
                    }
                    lfs2_pair_fromle32(pair);

                    if (!lfs2_pair_issync(pair, pdir.tail)) {
                        // we have desynced
                        LFS2_DEBUG("Fixing half-orphan "
                                "{0x%"PRIx32", 0x%"PRIx32"} "
                                "-> {0x%"PRIx32", 0x%"PRIx32"}",
                                pdir.tail[0], pdir.tail[1], pair[0], pair[1]);

                        // fix pending move in this pair? this looks like an
                        // optimization but is in fact _required_ since
                        // relocating may outdate the move.
                        uint16_t moveid = 0x3ff;
                        if (lfs2_gstate_hasmovehere(&lfs2->gstate, pdir.pair)) {
                            moveid = lfs2_tag_id(lfs2->gstate.tag);
                            LFS2_DEBUG("Fixing move while fixing orphans "
                                    "{0x%"PRIx32", 0x%"PRIx32"} 0x%"PRIx16"\n",
                                    pdir.pair[0], pdir.pair[1], moveid);
                            lfs2_fs_prepmove(lfs2, 0x3ff, NULL);
                        }

                        lfs2_pair_tole32(pair);
                        state = lfs2_dir_orphaningcommit(lfs2, &pdir, LFS2_MKATTRS(
                                {LFS2_MKTAG_IF(moveid != 0x3ff,
                                    LFS2_TYPE_DELETE, moveid, 0), NULL},
                                {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8),
                                    pair}));
                        lfs2_pair_fromle32(pair);
                        if (state < 0) {
                            return state;
                        }

                        // did our commit create more orphans?
                        if (state == LFS2_OK_ORPHANED) {
                            moreorphans = true;
                        }

                        // refetch tail
                        continue;
                    }
                }

                // note we only check for full orphans if we may have had a
                // power-loss, otherwise orphans are created intentionally
                // during operations such as lfs2_mkdir
                if (pass == 1 && tag == LFS2_ERR_NOENT && powerloss) {
                    // we are an orphan
                    LFS2_DEBUG("Fixing orphan {0x%"PRIx32", 0x%"PRIx32"}",
                            pdir.tail[0], pdir.tail[1]);

                    // steal state
                    err = lfs2_dir_getgstate(lfs2, &dir, &lfs2->gdelta);
                    if (err) {
                        return err;
                    }

                    // steal tail
                    lfs2_pair_tole32(dir.tail);
                    int state = lfs2_dir_orphaningcommit(lfs2, &pdir, LFS2_MKATTRS(
                            {LFS2_MKTAG(LFS2_TYPE_TAIL + dir.split, 0x3ff, 8),
                                dir.tail}));
                    lfs2_pair_fromle32(dir.tail);
                    if (state < 0) {
                        return state;
                    }

                    // did our commit create more orphans?
                    if (state == LFS2_OK_ORPHANED) {
                        moreorphans = true;
                    }

                    // refetch tail
                    continue;
                }
            }

            pdir = dir;
        }

        pass = moreorphans ? 0 : pass+1;
    }

    // mark orphans as fixed
    return lfs2_fs_preporphans(lfs2, -lfs2_gstate_getorphans(&lfs2->gstate));
}
#endif

#ifndef LFS2_READONLY
static int lfs2_fs_forceconsistency(lfs2_t *lfs2) {
    int err = lfs2_fs_desuperblock(lfs2);
    if (err) {
        return err;
    }

    err = lfs2_fs_demove(lfs2);
    if (err) {
        return err;
    }

    err = lfs2_fs_deorphan(lfs2, true);
    if (err) {
        return err;
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
static int lfs2_fs_mkconsistent_(lfs2_t *lfs2) {
    // lfs2_fs_forceconsistency does most of the work here
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        return err;
    }

    // do we have any pending gstate?
    lfs2_gstate_t delta = {0};
    lfs2_gstate_xor(&delta, &lfs2->gdisk);
    lfs2_gstate_xor(&delta, &lfs2->gstate);
    if (!lfs2_gstate_iszero(&delta)) {
        // lfs2_dir_commit will implicitly write out any pending gstate
        lfs2_mdir_t root;
        err = lfs2_dir_fetch(lfs2, &root, lfs2->root);
        if (err) {
            return err;
        }

        err = lfs2_dir_commit(lfs2, &root, NULL, 0);
        if (err) {
            return err;
        }
    }

    return 0;
}
#endif

static int lfs2_fs_size_count(void *p, lfs2_block_t block) {
    (void)block;
    lfs2_size_t *size = p;
    *size += 1;
    return 0;
}

static lfs2_ssize_t lfs2_fs_size_(lfs2_t *lfs2) {
    lfs2_size_t size = 0;
    int err = lfs2_fs_traverse_(lfs2, lfs2_fs_size_count, &size, false);
    if (err) {
        return err;
    }

    return size;
}

// explicit garbage collection
#ifndef LFS2_READONLY
static int lfs2_fs_gc_(lfs2_t *lfs2) {
    // force consistency, even if we're not necessarily going to write,
    // because this function is supposed to take care of janitorial work
    // isn't it?
    int err = lfs2_fs_forceconsistency(lfs2);
    if (err) {
        return err;
    }

    // try to compact metadata pairs, note we can't really accomplish
    // anything if compact_thresh doesn't at least leave a prog_size
    // available
    if (lfs2->cfg->compact_thresh
            < lfs2->cfg->block_size - lfs2->cfg->prog_size) {
        // iterate over all mdirs
        lfs2_mdir_t mdir = {.tail = {0, 1}};
        while (!lfs2_pair_isnull(mdir.tail)) {
            err = lfs2_dir_fetch(lfs2, &mdir, mdir.tail);
            if (err) {
                return err;
            }

            // not erased? exceeds our compaction threshold?
            if (!mdir.erased || ((lfs2->cfg->compact_thresh == 0)
                    ? mdir.off > lfs2->cfg->block_size - lfs2->cfg->block_size/8
                    : mdir.off > lfs2->cfg->compact_thresh)) {
                // the easiest way to trigger a compaction is to mark
                // the mdir as unerased and add an empty commit
                mdir.erased = false;
                err = lfs2_dir_commit(lfs2, &mdir, NULL, 0);
                if (err) {
                    return err;
                }
            }
        }
    }

    // try to populate the lookahead buffer, unless it's already full
    if (lfs2->lookahead.size < 8*lfs2->cfg->lookahead_size) {
        err = lfs2_alloc_scan(lfs2);
        if (err) {
            return err;
        }
    }

    return 0;
}
#endif

#ifndef LFS2_READONLY
#ifdef LFS2_SHRINKNONRELOCATING
static int lfs2_shrink_checkblock(void *data, lfs2_block_t block) {
    lfs2_size_t threshold = *((lfs2_size_t*)data);
    if (block >= threshold) {
        return LFS2_ERR_NOTEMPTY;
    }
    return 0;
}
#endif

static int lfs2_fs_grow_(lfs2_t *lfs2, lfs2_size_t block_count) {
    int err;

    if (block_count == lfs2->block_count) {
        return 0;
    }

    
#ifndef LFS2_SHRINKNONRELOCATING
    // shrinking is not supported
    LFS2_ASSERT(block_count >= lfs2->block_count);
#endif
#ifdef LFS2_SHRINKNONRELOCATING
    if (block_count < lfs2->block_count) {
        err = lfs2_fs_traverse_(lfs2, lfs2_shrink_checkblock, &block_count, true);
        if (err) {
            return err;
        }
    }
#endif

    lfs2->block_count = block_count;

    // fetch the root
    lfs2_mdir_t root;
    err = lfs2_dir_fetch(lfs2, &root, lfs2->root);
    if (err) {
        return err;
    }

    // update the superblock
    lfs2_superblock_t superblock;
    lfs2_stag_t tag = lfs2_dir_get(lfs2, &root, LFS2_MKTAG(0x7ff, 0x3ff, 0),
            LFS2_MKTAG(LFS2_TYPE_INLINESTRUCT, 0, sizeof(superblock)),
            &superblock);
    if (tag < 0) {
        return tag;
    }
    lfs2_superblock_fromle32(&superblock);

    superblock.block_count = lfs2->block_count;

    lfs2_superblock_tole32(&superblock);
    err = lfs2_dir_commit(lfs2, &root, LFS2_MKATTRS(
            {tag, &superblock}));
    if (err) {
        return err;
    }
    return 0;
}
#endif

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

        uint32_t crc = 0xffffffff;
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
        LFS2_ERROR("Corrupted dir pair at {0x%"PRIx32", 0x%"PRIx32"}",
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
        lfs2->lookahead.start = 0;
        lfs2->lookahead.size = 0;
        lfs2->lookahead.next = 0;
        lfs2_alloc_ckpoint(lfs2);

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
            LFS2_ERROR("Invalid superblock at {0x%"PRIx32", 0x%"PRIx32"}",
                    0, 1);
            err = LFS2_ERR_CORRUPT;
            goto cleanup;
        }

        uint16_t major_version = (0xffff & (superblock.d.version >> 16));
        uint16_t minor_version = (0xffff & (superblock.d.version >>  0));
        if ((major_version != LFS21_DISK_VERSION_MAJOR ||
             minor_version > LFS21_DISK_VERSION_MINOR)) {
            LFS2_ERROR("Invalid version v%d.%d", major_version, minor_version);
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
static int lfs2_migrate_(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    struct lfs21 lfs21;

    // Indeterminate filesystem size not allowed for migration.
    LFS2_ASSERT(cfg->block_count != 0);

    int err = lfs21_mount(lfs2, &lfs21, cfg);
    if (err) {
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
                        {LFS2_MKTAG_IF_ELSE(isdir,
                            LFS2_TYPE_DIR, id, entry1.d.nlen,
                            LFS2_TYPE_REG, id, entry1.d.nlen),
                                name},
                        {LFS2_MKTAG_IF_ELSE(isdir,
                            LFS2_TYPE_DIRSTRUCT, id, sizeof(entry1.d.u),
                            LFS2_TYPE_CTZSTRUCT, id, sizeof(entry1.d.u)),
                                &entry1.d.u}));
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
                        {LFS2_MKTAG(LFS2_TYPE_SOFTTAIL, 0x3ff, 8), dir1.d.tail}));
                lfs2_pair_fromle32(dir2.pair);
                if (err) {
                    goto cleanup;
                }
            }

            // Copy over first block to thread into fs. Unfortunately
            // if this fails there is not much we can do.
            LFS2_DEBUG("Migrating {0x%"PRIx32", 0x%"PRIx32"} "
                        "-> {0x%"PRIx32", 0x%"PRIx32"}",
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
        dir2.etag = 0xffffffff;
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
    return err;
}

#endif


/// Public API wrappers ///

// Here we can add tracing/thread safety easily

// Thread-safe wrappers if enabled
#ifdef LFS2_THREADSAFE
#define LFS2_LOCK(cfg)   cfg->lock(cfg)
#define LFS2_UNLOCK(cfg) cfg->unlock(cfg)
#else
#define LFS2_LOCK(cfg)   ((void)cfg, 0)
#define LFS2_UNLOCK(cfg) ((void)cfg)
#endif

// Public API
#ifndef LFS2_READONLY
int lfs2_format(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    int err = LFS2_LOCK(cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_format(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .block_count=%"PRIu32", "
                ".block_cycles=%"PRId32", .cache_size=%"PRIu32", "
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

    err = lfs2_format_(lfs2, cfg);

    LFS2_TRACE("lfs2_format -> %d", err);
    LFS2_UNLOCK(cfg);
    return err;
}
#endif

int lfs2_mount(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    int err = LFS2_LOCK(cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_mount(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .block_count=%"PRIu32", "
                ".block_cycles=%"PRId32", .cache_size=%"PRIu32", "
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

    err = lfs2_mount_(lfs2, cfg);

    LFS2_TRACE("lfs2_mount -> %d", err);
    LFS2_UNLOCK(cfg);
    return err;
}

int lfs2_unmount(lfs2_t *lfs2) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_unmount(%p)", (void*)lfs2);

    err = lfs2_unmount_(lfs2);

    LFS2_TRACE("lfs2_unmount -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

#ifndef LFS2_READONLY
int lfs2_remove(lfs2_t *lfs2, const char *path) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_remove(%p, \"%s\")", (void*)lfs2, path);

    err = lfs2_remove_(lfs2, path);

    LFS2_TRACE("lfs2_remove -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

#ifndef LFS2_READONLY
int lfs2_rename(lfs2_t *lfs2, const char *oldpath, const char *newpath) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_rename(%p, \"%s\", \"%s\")", (void*)lfs2, oldpath, newpath);

    err = lfs2_rename_(lfs2, oldpath, newpath);

    LFS2_TRACE("lfs2_rename -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

int lfs2_stat(lfs2_t *lfs2, const char *path, struct lfs2_info *info) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_stat(%p, \"%s\", %p)", (void*)lfs2, path, (void*)info);

    err = lfs2_stat_(lfs2, path, info);

    LFS2_TRACE("lfs2_stat -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

lfs2_ssize_t lfs2_getattr(lfs2_t *lfs2, const char *path,
        uint8_t type, void *buffer, lfs2_size_t size) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_getattr(%p, \"%s\", %"PRIu8", %p, %"PRIu32")",
            (void*)lfs2, path, type, buffer, size);

    lfs2_ssize_t res = lfs2_getattr_(lfs2, path, type, buffer, size);

    LFS2_TRACE("lfs2_getattr -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

#ifndef LFS2_READONLY
int lfs2_setattr(lfs2_t *lfs2, const char *path,
        uint8_t type, const void *buffer, lfs2_size_t size) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_setattr(%p, \"%s\", %"PRIu8", %p, %"PRIu32")",
            (void*)lfs2, path, type, buffer, size);

    err = lfs2_setattr_(lfs2, path, type, buffer, size);

    LFS2_TRACE("lfs2_setattr -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

#ifndef LFS2_READONLY
int lfs2_removeattr(lfs2_t *lfs2, const char *path, uint8_t type) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_removeattr(%p, \"%s\", %"PRIu8")", (void*)lfs2, path, type);

    err = lfs2_removeattr_(lfs2, path, type);

    LFS2_TRACE("lfs2_removeattr -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

#ifndef LFS2_NO_MALLOC
int lfs2_file_open(lfs2_t *lfs2, lfs2_file_t *file, const char *path, int flags) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_open(%p, %p, \"%s\", %x)",
            (void*)lfs2, (void*)file, path, (unsigned)flags);
    LFS2_ASSERT(!lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    err = lfs2_file_open_(lfs2, file, path, flags);

    LFS2_TRACE("lfs2_file_open -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

int lfs2_file_opencfg(lfs2_t *lfs2, lfs2_file_t *file,
        const char *path, int flags,
        const struct lfs2_file_config *cfg) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_opencfg(%p, %p, \"%s\", %x, %p {"
                 ".buffer=%p, .attrs=%p, .attr_count=%"PRIu32"})",
            (void*)lfs2, (void*)file, path, (unsigned)flags,
            (void*)cfg, cfg->buffer, (void*)cfg->attrs, cfg->attr_count);
    LFS2_ASSERT(!lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    err = lfs2_file_opencfg_(lfs2, file, path, flags, cfg);

    LFS2_TRACE("lfs2_file_opencfg -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

int lfs2_file_close(lfs2_t *lfs2, lfs2_file_t *file) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_close(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    err = lfs2_file_close_(lfs2, file);

    LFS2_TRACE("lfs2_file_close -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

#ifndef LFS2_READONLY
int lfs2_file_sync(lfs2_t *lfs2, lfs2_file_t *file) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_sync(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    err = lfs2_file_sync_(lfs2, file);

    LFS2_TRACE("lfs2_file_sync -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

lfs2_ssize_t lfs2_file_read(lfs2_t *lfs2, lfs2_file_t *file,
        void *buffer, lfs2_size_t size) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_read(%p, %p, %p, %"PRIu32")",
            (void*)lfs2, (void*)file, buffer, size);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    lfs2_ssize_t res = lfs2_file_read_(lfs2, file, buffer, size);

    LFS2_TRACE("lfs2_file_read -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

#ifndef LFS2_READONLY
lfs2_ssize_t lfs2_file_write(lfs2_t *lfs2, lfs2_file_t *file,
        const void *buffer, lfs2_size_t size) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_write(%p, %p, %p, %"PRIu32")",
            (void*)lfs2, (void*)file, buffer, size);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    lfs2_ssize_t res = lfs2_file_write_(lfs2, file, buffer, size);

    LFS2_TRACE("lfs2_file_write -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}
#endif

lfs2_soff_t lfs2_file_seek(lfs2_t *lfs2, lfs2_file_t *file,
        lfs2_soff_t off, int whence) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_seek(%p, %p, %"PRId32", %d)",
            (void*)lfs2, (void*)file, off, whence);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    lfs2_soff_t res = lfs2_file_seek_(lfs2, file, off, whence);

    LFS2_TRACE("lfs2_file_seek -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

#ifndef LFS2_READONLY
int lfs2_file_truncate(lfs2_t *lfs2, lfs2_file_t *file, lfs2_off_t size) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_truncate(%p, %p, %"PRIu32")",
            (void*)lfs2, (void*)file, size);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    err = lfs2_file_truncate_(lfs2, file, size);

    LFS2_TRACE("lfs2_file_truncate -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

lfs2_soff_t lfs2_file_tell(lfs2_t *lfs2, lfs2_file_t *file) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_tell(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    lfs2_soff_t res = lfs2_file_tell_(lfs2, file);

    LFS2_TRACE("lfs2_file_tell -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

int lfs2_file_rewind(lfs2_t *lfs2, lfs2_file_t *file) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_rewind(%p, %p)", (void*)lfs2, (void*)file);

    err = lfs2_file_rewind_(lfs2, file);

    LFS2_TRACE("lfs2_file_rewind -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

lfs2_soff_t lfs2_file_size(lfs2_t *lfs2, lfs2_file_t *file) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_file_size(%p, %p)", (void*)lfs2, (void*)file);
    LFS2_ASSERT(lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)file));

    lfs2_soff_t res = lfs2_file_size_(lfs2, file);

    LFS2_TRACE("lfs2_file_size -> %"PRIu32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

#ifndef LFS2_READONLY
int lfs2_mkdir(lfs2_t *lfs2, const char *path) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_mkdir(%p, \"%s\")", (void*)lfs2, path);

    err = lfs2_mkdir_(lfs2, path);

    LFS2_TRACE("lfs2_mkdir -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

int lfs2_dir_open(lfs2_t *lfs2, lfs2_dir_t *dir, const char *path) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_dir_open(%p, %p, \"%s\")", (void*)lfs2, (void*)dir, path);
    LFS2_ASSERT(!lfs2_mlist_isopen(lfs2->mlist, (struct lfs2_mlist*)dir));

    err = lfs2_dir_open_(lfs2, dir, path);

    LFS2_TRACE("lfs2_dir_open -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

int lfs2_dir_close(lfs2_t *lfs2, lfs2_dir_t *dir) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_dir_close(%p, %p)", (void*)lfs2, (void*)dir);

    err = lfs2_dir_close_(lfs2, dir);

    LFS2_TRACE("lfs2_dir_close -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

int lfs2_dir_read(lfs2_t *lfs2, lfs2_dir_t *dir, struct lfs2_info *info) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_dir_read(%p, %p, %p)",
            (void*)lfs2, (void*)dir, (void*)info);

    err = lfs2_dir_read_(lfs2, dir, info);

    LFS2_TRACE("lfs2_dir_read -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

int lfs2_dir_seek(lfs2_t *lfs2, lfs2_dir_t *dir, lfs2_off_t off) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_dir_seek(%p, %p, %"PRIu32")",
            (void*)lfs2, (void*)dir, off);

    err = lfs2_dir_seek_(lfs2, dir, off);

    LFS2_TRACE("lfs2_dir_seek -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

lfs2_soff_t lfs2_dir_tell(lfs2_t *lfs2, lfs2_dir_t *dir) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_dir_tell(%p, %p)", (void*)lfs2, (void*)dir);

    lfs2_soff_t res = lfs2_dir_tell_(lfs2, dir);

    LFS2_TRACE("lfs2_dir_tell -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

int lfs2_dir_rewind(lfs2_t *lfs2, lfs2_dir_t *dir) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_dir_rewind(%p, %p)", (void*)lfs2, (void*)dir);

    err = lfs2_dir_rewind_(lfs2, dir);

    LFS2_TRACE("lfs2_dir_rewind -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

int lfs2_fs_stat(lfs2_t *lfs2, struct lfs2_fsinfo *fsinfo) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_fs_stat(%p, %p)", (void*)lfs2, (void*)fsinfo);

    err = lfs2_fs_stat_(lfs2, fsinfo);

    LFS2_TRACE("lfs2_fs_stat -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

lfs2_ssize_t lfs2_fs_size(lfs2_t *lfs2) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_fs_size(%p)", (void*)lfs2);

    lfs2_ssize_t res = lfs2_fs_size_(lfs2);

    LFS2_TRACE("lfs2_fs_size -> %"PRId32, res);
    LFS2_UNLOCK(lfs2->cfg);
    return res;
}

int lfs2_fs_traverse(lfs2_t *lfs2, int (*cb)(void *, lfs2_block_t), void *data) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_fs_traverse(%p, %p, %p)",
            (void*)lfs2, (void*)(uintptr_t)cb, data);

    err = lfs2_fs_traverse_(lfs2, cb, data, true);

    LFS2_TRACE("lfs2_fs_traverse -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}

#ifndef LFS2_READONLY
int lfs2_fs_mkconsistent(lfs2_t *lfs2) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_fs_mkconsistent(%p)", (void*)lfs2);

    err = lfs2_fs_mkconsistent_(lfs2);

    LFS2_TRACE("lfs2_fs_mkconsistent -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

#ifndef LFS2_READONLY
int lfs2_fs_gc(lfs2_t *lfs2) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_fs_gc(%p)", (void*)lfs2);

    err = lfs2_fs_gc_(lfs2);

    LFS2_TRACE("lfs2_fs_gc -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

#ifndef LFS2_READONLY
int lfs2_fs_grow(lfs2_t *lfs2, lfs2_size_t block_count) {
    int err = LFS2_LOCK(lfs2->cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_fs_grow(%p, %"PRIu32")", (void*)lfs2, block_count);

    err = lfs2_fs_grow_(lfs2, block_count);

    LFS2_TRACE("lfs2_fs_grow -> %d", err);
    LFS2_UNLOCK(lfs2->cfg);
    return err;
}
#endif

#ifdef LFS2_MIGRATE
int lfs2_migrate(lfs2_t *lfs2, const struct lfs2_config *cfg) {
    int err = LFS2_LOCK(cfg);
    if (err) {
        return err;
    }
    LFS2_TRACE("lfs2_migrate(%p, %p {.context=%p, "
                ".read=%p, .prog=%p, .erase=%p, .sync=%p, "
                ".read_size=%"PRIu32", .prog_size=%"PRIu32", "
                ".block_size=%"PRIu32", .block_count=%"PRIu32", "
                ".block_cycles=%"PRId32", .cache_size=%"PRIu32", "
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

    err = lfs2_migrate_(lfs2, cfg);

    LFS2_TRACE("lfs2_migrate -> %d", err);
    LFS2_UNLOCK(cfg);
    return err;
}
#endif

