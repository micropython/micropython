/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
 * Copyright (c) 2026 Dryw Wade Clere at SparkFun Electronics
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

// Bitmap garbage collector, selected with MICROPY_GC_IMPL_BITMAP.
//
// Same algorithm and contract as the classic GC (conservative, non-moving
// mark-sweep with the same block size), but the per-block metadata is kept as
// one bit per block in each of several tables, so that allocation searches,
// object-length lookups and the sweep can work on a machine word (32 or 64
// blocks) at a time.
//
// Other differences from the classic GC:
// - The mark stack borrows the largest free run in the heap for the duration
//   of the mark phase, so deep object graphs don't overflow it and trigger a
//   rescan of the whole heap.
// - Objects with no GC pointers are marked but never pushed or scanned.
// - With a split heap, areas can be flagged GC_AREA_FLAG_EXTERNAL; see gc.h.

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "py/gc_impl.h"

#if MICROPY_ENABLE_GC && MICROPY_GC_IMPL == MICROPY_GC_IMPL_BITMAP

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

#define BYTES_PER_BLOCK (MICROPY_BYTES_PER_GC_BLOCK)

// The tables are processed a bmword_t at a time.  It's the machine word by
// default; a 64-bit build can use uint32_t to test the code paths used on
// 32-bit machines.  Only 32- and 64-bit unsigned types are supported.
#ifndef MICROPY_GC_BITMAP_WORD_TYPE
#define MICROPY_GC_BITMAP_WORD_TYPE uintptr_t
#endif
typedef MICROPY_GC_BITMAP_WORD_TYPE bmword_t;
#define BITS_PER_WORD (sizeof(bmword_t) * MP_BITS_PER_BYTE)
#define ALL_ONES (~(bmword_t)0)

// Bit tables ("planes").  HEAD and TAIL_MARK hold the same 2-bit block state
// as the classic GC's allocation table, one bit in each:
//   HEAD=0 TAIL_MARK=0: free
//   HEAD=1 TAIL_MARK=0: head of an allocation
//   HEAD=0 TAIL_MARK=1: tail of an allocation
//   HEAD=1 TAIL_MARK=1: head marked during a collection
// so a word of blocks is allocated where HEAD|TAIL_MARK, and marked where
// HEAD&TAIL_MARK.  The other planes, when enabled, are only used on heads.
enum {
    PLANE_HEAD,
    PLANE_TAIL_MARK,
    #if MICROPY_ENABLE_FINALISER
    PLANE_FINALISER,
    #endif
    #if MICROPY_PY_WEAKREF
    PLANE_WEAKREF,
    #endif
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    PLANE_HAS_PTRS, // block may contain GC pointers, so must be scanned
    #endif
    NUM_PLANES
};

#define GROUP(block) ((block) / BITS_PER_WORD)
#define BIT(block) ((bmword_t)1 << ((block) % BITS_PER_WORD))
// Table layout: the HEAD and TAIL_MARK words of each group are interleaved,
// because the mark phase reads both for every pointer it follows.  Each other
// plane is a separate array after them, so the state words stay as dense in
// the cache as the classic GC's allocation table.
#define NUM_STATE_PLANES (2)
#define BMW(area, group, plane) (((bmword_t *)(area)->gc_bitmap)[(plane) < NUM_STATE_PLANES \
    ? (group) * NUM_STATE_PLANES + (plane) \
    : N_GROUPS(area) * (plane) + (group)])
#define BM_GET(area, plane, block) ((BMW(area, GROUP(block), plane) & BIT(block)) != 0)
#define BM_SET(area, plane, block) (BMW(area, GROUP(block), plane) |= BIT(block))
#define BM_CLEAR(area, plane, block) (BMW(area, GROUP(block), plane) &= ~BIT(block))

// Per-word block states.
#define W_ALLOC(area, group) (BMW(area, group, PLANE_HEAD) | BMW(area, group, PLANE_TAIL_MARK))
#define W_MARKED(area, group) (BMW(area, group, PLANE_HEAD) & BMW(area, group, PLANE_TAIL_MARK))
#define IS_HEAD(area, block) BM_GET(area, PLANE_HEAD, block)
#define IS_ALLOC(area, block) ((W_ALLOC(area, GROUP(block)) & BIT(block)) != 0)
#define IS_MARKED_HEAD(area, block) ((W_MARKED(area, GROUP(block)) & BIT(block)) != 0)

#define N_GROUPS(area) (((area)->gc_n_blocks + BITS_PER_WORD - 1) / BITS_PER_WORD)
#define BLOCK_FROM_PTR(area, ptr) (((byte *)(ptr) - (area)->gc_pool_start) / BYTES_PER_BLOCK)
#define PTR_FROM_BLOCK(area, block) ((void *)((area)->gc_pool_start + (block) * BYTES_PER_BLOCK))

#if MICROPY_GC_SPLIT_HEAP
#define NEXT_AREA(area) ((area)->next)
#else
#define NEXT_AREA(area) (NULL)
#endif

#if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
#define HAS_PTRS(area, block) BM_GET(area, PLANE_HAS_PTRS, block)
#else
#define HAS_PTRS(area, block) (true)
#endif

// Free runs at least this long (in blocks) end the search for a run to borrow
// as the mark stack.
#define MARK_STACK_BORROW_ENOUGH (4096)

static inline unsigned int bm_ctz(bmword_t x) {
    #if defined(__GNUC__)
    if (sizeof(bmword_t) <= sizeof(unsigned long)) {
        return __builtin_ctzl((unsigned long)x);
    } else {
        return __builtin_ctzll((unsigned long long)x);
    }
    #else
    unsigned int n = 0;
    while (!(x & 1)) {
        x >>= 1;
        n += 1;
    }
    return n;
    #endif
}

static inline unsigned int bm_clz(bmword_t x) {
    #if defined(__GNUC__)
    if (sizeof(bmword_t) <= sizeof(unsigned long)) {
        return __builtin_clzl((unsigned long)x) - (sizeof(unsigned long) - sizeof(bmword_t)) * MP_BITS_PER_BYTE;
    } else {
        return __builtin_clzll((unsigned long long)x) - (sizeof(unsigned long long) - sizeof(bmword_t)) * MP_BITS_PER_BYTE;
    }
    #else
    unsigned int n = 0;
    while (!(x & ((bmword_t)1 << (BITS_PER_WORD - 1)))) {
        x <<= 1;
        n += 1;
    }
    return n;
    #endif
}

static inline unsigned int bm_popcount(bmword_t x) {
    #if defined(__GNUC__)
    if (sizeof(bmword_t) <= sizeof(unsigned long)) {
        return __builtin_popcountl((unsigned long)x);
    } else {
        return __builtin_popcountll((unsigned long long)x);
    }
    #else
    unsigned int n = 0;
    while (x) {
        x &= x - 1;
        n += 1;
    }
    return n;
    #endif
}

// Mask of bits [lo, hi), where lo < hi <= BITS_PER_WORD.
static inline bmword_t bm_range_mask(unsigned int lo, unsigned int hi) {
    bmword_t upper = hi == BITS_PER_WORD ? ALL_ONES : (((bmword_t)1 << hi) - 1);
    return upper & ~(((bmword_t)1 << lo) - 1);
}

// Set or clear bits [block, block + n) in one plane.
static void bm_range_write(mp_state_mem_area_t *area, unsigned int plane, size_t block, size_t n, bool set) {
    while (n) {
        unsigned int lo = block % BITS_PER_WORD;
        unsigned int len = MIN(n, BITS_PER_WORD - lo);
        bmword_t mask = bm_range_mask(lo, lo + len);
        if (set) {
            BMW(area, GROUP(block), plane) |= mask;
        } else {
            BMW(area, GROUP(block), plane) &= ~mask;
        }
        block += len;
        n -= len;
    }
}

// Whether blocks [block, block + n) are all free.
static bool bm_range_is_free(mp_state_mem_area_t *area, size_t block, size_t n) {
    while (n) {
        unsigned int lo = block % BITS_PER_WORD;
        unsigned int len = MIN(n, BITS_PER_WORD - lo);
        if (W_ALLOC(area, GROUP(block)) & bm_range_mask(lo, lo + len)) {
            return false;
        }
        block += len;
        n -= len;
    }
    return true;
}

// Number of blocks in the allocation whose head is `block`: the distance to
// the next head or free block.
static size_t gc_obj_n_blocks(mp_state_mem_area_t *area, size_t block) {
    size_t group = GROUP(block);
    size_t n_groups = N_GROUPS(area);
    // Ignore this block and the ones below it.
    // (A head or free block is HEAD | ~TAIL_MARK.)
    bmword_t stop = (BMW(area, group, PLANE_HEAD) | ~BMW(area, group, PLANE_TAIL_MARK))
        & ~(((bmword_t)2 << (block % BITS_PER_WORD)) - 1);
    while (stop == 0) {
        if (++group >= n_groups) {
            return area->gc_n_blocks - block;
        }
        stop = BMW(area, group, PLANE_HEAD) | ~BMW(area, group, PLANE_TAIL_MARK);
    }
    return MIN(group * BITS_PER_WORD + bm_ctz(stop), area->gc_n_blocks) - block;
}

// Returns the area if ptr points to the start of a block within one, else NULL.
static inline MP_ALWAYSINLINE mp_state_mem_area_t *gc_block_area(const void *ptr) {
    if (((uintptr_t)ptr & (BYTES_PER_BLOCK - 1)) != 0) {
        return NULL;
    }
    #if MICROPY_GC_SPLIT_HEAP
    if ((byte *)ptr < MP_STATE_MEM(area_pool_min) || (byte *)ptr >= MP_STATE_MEM(area_pool_max)) {
        return NULL;
    }
    #endif
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        if ((byte *)ptr >= area->gc_pool_start && (byte *)ptr < area->gc_pool_end) {
            return area;
        }
    }
    return NULL;
}

#if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
// Returns the area containing ptr (at any alignment), or NULL.
static mp_state_mem_area_t *gc_any_area(const void *ptr) {
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        if ((byte *)ptr >= area->gc_pool_start && (byte *)ptr < area->gc_pool_end) {
            return area;
        }
    }
    return NULL;
}
#endif

static void gc_setup_area(mp_state_mem_area_t *area, void *start, void *end, unsigned int area_flags) {
    // Layout: [bit tables for all groups][pool].  Each group of BITS_PER_WORD
    // blocks costs NUM_PLANES words of table; the last group may be partial.
    start = (void *)(((uintptr_t)start + sizeof(bmword_t) - 1) & ~(sizeof(bmword_t) - 1));
    size_t total = (byte *)end > (byte *)start ? (size_t)((byte *)end - (byte *)start) : 0;
    size_t table_bytes_per_group = NUM_PLANES * sizeof(bmword_t);
    size_t bytes_per_group = table_bytes_per_group + BITS_PER_WORD * BYTES_PER_BLOCK;
    size_t n_groups = total / bytes_per_group;
    size_t n_blocks = n_groups * BITS_PER_WORD;
    size_t rest = total - n_groups * bytes_per_group;
    if (rest > table_bytes_per_group + BYTES_PER_BLOCK) {
        n_groups += 1;
        n_blocks += (rest - table_bytes_per_group) / BYTES_PER_BLOCK;
    }

    area->gc_bitmap = start;
    area->gc_n_blocks = n_blocks;
    area->gc_pool_end = end;
    area->gc_pool_start = (byte *)end - n_blocks * BYTES_PER_BLOCK;
    area->gc_first_free_block = 0;
    memset(area->gc_class_hint, 0, sizeof(area->gc_class_hint));
    area->gc_n_groups_used = 0;
    area->gc_area_flags = area_flags;
    assert(area->gc_pool_start >= (byte *)area->gc_bitmap + n_groups * table_bytes_per_group);
    memset(area->gc_bitmap, 0, n_groups * table_bytes_per_group);

    #if MICROPY_GC_SPLIT_HEAP
    area->next = NULL;
    MP_STATE_MEM(area_pool_min) = MIN(MP_STATE_MEM(area_pool_min), area->gc_pool_start);
    MP_STATE_MEM(area_pool_max) = MAX(MP_STATE_MEM(area_pool_max), area->gc_pool_end);
    #endif

    DEBUG_printf("GC area: tables at %p (" UINT_FMT " bytes), pool at %p (" UINT_FMT " blocks)\n",
        area->gc_bitmap, n_groups * table_bytes_per_group, area->gc_pool_start, n_blocks);
}

void gc_init(void *start, void *end) {
    // Narrower words would be promoted to int by ~ and shifts.
    MP_STATIC_ASSERT(sizeof(bmword_t) >= sizeof(unsigned int));

    // align end pointer on block boundary
    end = (void *)((uintptr_t)end & (~(BYTES_PER_BLOCK - 1)));
    DEBUG_printf("Initializing GC heap: %p..%p = " UINT_FMT " bytes\n", start, end, (byte *)end - (byte *)start);

    #if MICROPY_GC_SPLIT_HEAP
    // min/max are deliberately swapped, gc_setup_area() fixes them up
    MP_STATE_MEM(area_pool_min) = end;
    MP_STATE_MEM(area_pool_max) = start;
    #endif

    gc_setup_area(&MP_STATE_MEM(area), start, end, 0);

    MP_STATE_MEM(gc_mark_stack) = MP_STATE_MEM(gc_mark_stack_static);
    MP_STATE_MEM(gc_mark_stack_len) = MICROPY_ALLOC_GC_STACK_SIZE;

    #if MICROPY_GC_SPLIT_HEAP
    MP_STATE_MEM(gc_alloc_area)[0] = NULL;
    MP_STATE_MEM(gc_alloc_area)[1] = NULL;
    #endif

    // unlock the GC
    MP_STATE_THREAD(gc_lock_depth) = 0;

    // allow auto collection
    MP_STATE_MEM(gc_auto_collect_enabled) = 1;

    #if MICROPY_GC_ALLOC_THRESHOLD
    // by default, maxuint for gc threshold, effectively turning gc-by-threshold off
    MP_STATE_MEM(gc_alloc_threshold) = (size_t)-1;
    MP_STATE_MEM(gc_alloc_amount) = 0;
    #endif

    GC_MUTEX_INIT();
}

#if MICROPY_GC_SPLIT_HEAP
void gc_add_ex(void *start, void *end, unsigned int area_flags) {
    // Place the area struct at the start of the area.
    mp_state_mem_area_t *area = (mp_state_mem_area_t *)start;
    start = (void *)((uintptr_t)start + sizeof(mp_state_mem_area_t));

    end = (void *)((uintptr_t)end & (~(BYTES_PER_BLOCK - 1)));
    DEBUG_printf("Adding GC heap: %p..%p = " UINT_FMT " bytes\n", start, end, (byte *)end - (byte *)start);

    gc_setup_area(area, start, end, area_flags);

    mp_state_mem_area_t *prev_area = &MP_STATE_MEM(area);
    while (prev_area->next != NULL) {
        prev_area = prev_area->next;
    }
    prev_area->next = area;
    MP_STATE_MEM(gc_alloc_area)[0] = NULL;
    MP_STATE_MEM(gc_alloc_area)[1] = NULL;
}

void gc_add(void *start, void *end) {
    gc_add_ex(start, end, 0);
}

#if MICROPY_GC_SPLIT_HEAP_AUTO
// Try to automatically add a heap area large enough to fulfill 'failed_alloc'.
// See the classic GC for the growth policy, which this follows.
static bool gc_try_add_heap(size_t failed_alloc) {
    // Room for the allocation, its share of the tables, and some slack.
    size_t needed = failed_alloc + failed_alloc * NUM_PLANES / (MP_BITS_PER_BYTE * BYTES_PER_BLOCK)
        + MAX(2048, failed_alloc / 64) + sizeof(mp_state_mem_area_t);
    size_t avail = gc_get_max_new_split();
    if (avail < needed) {
        return false;
    }

    // Aim to double the total heap size.
    size_t total_blocks = 0;
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        total_blocks += area->gc_n_blocks;
    }
    size_t total_heap = total_blocks * BYTES_PER_BLOCK
        + (total_blocks / BITS_PER_WORD + 2) * NUM_PLANES * sizeof(bmword_t)
        + sizeof(mp_state_mem_area_t);
    total_heap = (total_heap + BYTES_PER_BLOCK - 1) & (~(BYTES_PER_BLOCK - 1));

    size_t to_alloc = MIN(avail, MAX(total_heap, needed));
    mp_state_mem_area_t *new_heap = MP_PLAT_ALLOC_HEAP(to_alloc);
    if (new_heap == NULL) {
        return false;
    }
    gc_add(new_heap, (void *)new_heap + to_alloc);
    return true;
}
#endif

#endif // MICROPY_GC_SPLIT_HEAP

/******************************************************************************/
// Mark phase

// Find the largest run of free blocks (stopping early at a big enough one) and
// use it as the mark stack until the end of the collection.  Nothing can
// allocate while the collection runs, so the run stays free.
static void gc_borrow_mark_stack(void) {
    size_t best_len = 0;
    void *best_ptr = NULL;
    // Look in internal areas first, as the stack is accessed constantly.
    for (int pass = 0; pass < 2; ++pass) {
        for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
            if (((area->gc_area_flags & GC_AREA_FLAG_EXTERNAL) != 0) != (pass == 1)) {
                continue;
            }
            size_t n_groups = N_GROUPS(area);
            size_t run_start = 0;
            size_t run_len = 0;
            for (size_t g = GROUP(area->gc_first_free_block); g < n_groups; g++) {
                bmword_t free = ~W_ALLOC(area, g);
                if (free == ALL_ONES) {
                    if (run_len == 0) {
                        run_start = g * BITS_PER_WORD;
                    }
                    run_len += BITS_PER_WORD;
                    if (run_len >= MARK_STACK_BORROW_ENOUGH && g + 1 < n_groups) {
                        best_len = run_len;
                        best_ptr = PTR_FROM_BLOCK(area, run_start);
                        goto done;
                    }
                    continue;
                }
                // Close the current run at the first used block, then start a new
                // run from the top free bits of this word.
                unsigned int lead = bm_ctz(~free);
                if (run_len == 0) {
                    run_start = g * BITS_PER_WORD;
                }
                run_len += lead;
                if (run_len > best_len) {
                    best_len = run_len;
                    best_ptr = PTR_FROM_BLOCK(area, run_start);
                    if (best_len >= MARK_STACK_BORROW_ENOUGH) {
                        goto done;
                    }
                }
                unsigned int n_top = bm_clz(~free);
                run_start = (g + 1) * BITS_PER_WORD - n_top;
                run_len = n_top;
            }
            // A run reaching the end of the area may include padding past the
            // last block.
            if (run_len) {
                run_len = MIN(run_len, area->gc_n_blocks - MIN(run_start, area->gc_n_blocks));
                if (run_len > best_len) {
                    best_len = run_len;
                    best_ptr = PTR_FROM_BLOCK(area, run_start);
                }
            }
        }
    }
done:;
    size_t n_entries = best_len * BYTES_PER_BLOCK / sizeof(void *);
    if (n_entries > MICROPY_ALLOC_GC_STACK_SIZE) {
        #ifndef NDEBUG
        mp_state_mem_area_t *best_area = gc_block_area(best_ptr);
        assert(best_area && bm_range_is_free(best_area, BLOCK_FROM_PTR(best_area, best_ptr), best_len));
        #endif
        MP_STATE_MEM(gc_mark_stack) = best_ptr;
        MP_STATE_MEM(gc_mark_stack_len) = n_entries;
    }
    MP_STATE_MEM(gc_mark_stack_max_sp) = 0;
    DEBUG_printf("gc mark stack: %p, " UINT_FMT " entries\n", MP_STATE_MEM(gc_mark_stack), MP_STATE_MEM(gc_mark_stack_len));
}

static void gc_return_mark_stack(void) {
    if (MP_STATE_MEM(gc_mark_stack) != MP_STATE_MEM(gc_mark_stack_static)) {
        // Don't leave stale heap pointers in free memory.
        memset(MP_STATE_MEM(gc_mark_stack), 0, MP_STATE_MEM(gc_mark_stack_max_sp) * sizeof(void *));
        MP_STATE_MEM(gc_mark_stack) = MP_STATE_MEM(gc_mark_stack_static);
        MP_STATE_MEM(gc_mark_stack_len) = MICROPY_ALLOC_GC_STACK_SIZE;
    }
}

void gc_impl_collect_start(bool will_mark) {
    GC_ENTER();
    assert((MP_STATE_THREAD(gc_lock_depth) & GC_COLLECT_FLAG) == 0);
    MP_STATE_THREAD(gc_lock_depth) |= GC_COLLECT_FLAG;
    MP_STATE_MEM(gc_stack_overflow) = 0;
    if (will_mark) {
        gc_borrow_mark_stack();
    }
}

#if defined(__GNUC__)
#define GC_PREFETCH(p) __builtin_prefetch(p)
#else
#define GC_PREFETCH(p)
#endif

// Objects are scanned this many words at a time.  Before a chunk is scanned, a
// continuation for the rest of the object goes on the mark stack, so the stack
// needs about this many entries per level of the object graph, however many
// pointers a single object (e.g. a big list or dict) holds.
#define MARK_CHUNK_WORDS (32)

// Scan the object at `block` and everything reachable from it that isn't
// already marked.  The object itself must already be marked.  Stack entries
// are heads to scan, or a continuation: (next word | 1) above the end pointer.
static void gc_mark_subtree(mp_state_mem_area_t *area, size_t block) {
    void **stack = MP_STATE_MEM(gc_mark_stack);
    size_t stack_len = MP_STATE_MEM(gc_mark_stack_len);
    size_t sp = 0;
    size_t max_sp = MP_STATE_MEM(gc_mark_stack_max_sp);
    void **ptrs = PTR_FROM_BLOCK(area, block);
    void **end = ptrs + gc_obj_n_blocks(area, block) * BYTES_PER_BLOCK / sizeof(void *);
    for (;;) {
        if (end - ptrs > MARK_CHUNK_WORDS && sp + 2 <= stack_len) {
            stack[sp++] = end;
            stack[sp++] = (void *)((uintptr_t)(ptrs + MARK_CHUNK_WORDS) | 1);
            max_sp = MAX(max_sp, sp);
            end = ptrs + MARK_CHUNK_WORDS;
        }
        for (; ptrs < end; ptrs++) {
            MICROPY_GC_HOOK_LOOP((size_t)(end - ptrs));
            void *ptr = *ptrs;
            mp_state_mem_area_t *ptr_area = gc_block_area(ptr);
            if (ptr_area == NULL) {
                continue;
            }
            size_t ptr_block = BLOCK_FROM_PTR(ptr_area, ptr);
            size_t group = GROUP(ptr_block);
            bmword_t bit = BIT(ptr_block);
            if (!(BMW(ptr_area, group, PLANE_HEAD) & bit) || (BMW(ptr_area, group, PLANE_TAIL_MARK) & bit)) {
                // Not a head, or already marked.
                continue;
            }
            BMW(ptr_area, group, PLANE_TAIL_MARK) |= bit;
            if (!HAS_PTRS(ptr_area, ptr_block)) {
                // Nothing to scan inside it.
                continue;
            }
            if (sp < stack_len) {
                stack[sp++] = ptr;
                max_sp = MAX(max_sp, sp);
            } else {
                MP_STATE_MEM(gc_stack_overflow) = 1;
            }
        }

        if (sp == 0) {
            break;
        }
        void *ptr = stack[--sp];
        if ((uintptr_t)ptr & 1) {
            // The rest of a large object.
            ptrs = (void **)((uintptr_t)ptr & ~(uintptr_t)1);
            end = stack[--sp];
        }
        if (sp) {
            // Marking is limited by memory latency when the heap is in slow
            // memory such as PSRAM, so start fetching the next object now.
            GC_PREFETCH((void *)((uintptr_t)stack[sp - 1] & ~(uintptr_t)1));
        }
        if (!((uintptr_t)ptr & 1)) {
            #if MICROPY_GC_SPLIT_HEAP
            area = gc_block_area(ptr);
            #endif
            ptrs = ptr;
            end = ptrs + gc_obj_n_blocks(area, BLOCK_FROM_PTR(area, ptr)) * BYTES_PER_BLOCK / sizeof(void *);
        }
    }
    MP_STATE_MEM(gc_mark_stack_max_sp) = max_sp;
}

void gc_collect_root(void **ptrs, size_t len) {
    for (size_t i = 0; i < len; i++) {
        MICROPY_GC_HOOK_LOOP(i);
        void *ptr = gc_get_ptr(ptrs, i);
        mp_state_mem_area_t *area = gc_block_area(ptr);
        if (area == NULL) {
            continue;
        }
        size_t block = BLOCK_FROM_PTR(area, ptr);
        if (IS_HEAD(area, block) && !BM_GET(area, PLANE_TAIL_MARK, block)) {
            BM_SET(area, PLANE_TAIL_MARK, block);
            if (HAS_PTRS(area, block)) {
                gc_mark_subtree(area, block);
            }
        }
    }
}

// The mark stack overflowed, so some marked objects may have unscanned
// children.  Rescan every marked object until no overflow happens.
static void gc_deal_with_stack_overflow(void) {
    while (MP_STATE_MEM(gc_stack_overflow)) {
        MP_STATE_MEM(gc_stack_overflow) = 0;
        for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
            for (size_t g = 0; g < area->gc_n_groups_used; g++) {
                MICROPY_GC_HOOK_LOOP(g);
                bmword_t marked = W_MARKED(area, g);
                #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
                marked &= BMW(area, g, PLANE_HAS_PTRS);
                #endif
                while (marked) {
                    gc_mark_subtree(area, g * BITS_PER_WORD + bm_ctz(marked));
                    marked &= marked - 1;
                }
            }
        }
    }
}

/******************************************************************************/
// Sweep phase

// Run finalisers and weakref callbacks of all unmarked heads.
static void gc_sweep_run_finalisers(void) {
    // All finalisers run before any weakref is cleared, because a dying
    // weakref.ref's finaliser looks up its referent to unlink itself.
    #if MICROPY_ENABLE_FINALISER
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        for (size_t g = 0; g < area->gc_n_groups_used; g++) {
            // Most words have no finalisers, so check that first.
            bmword_t fin = BMW(area, g, PLANE_FINALISER);
            if (fin) {
                fin &= BMW(area, g, PLANE_HEAD) & ~BMW(area, g, PLANE_TAIL_MARK);
            }
            while (fin) {
                MICROPY_GC_HOOK_LOOP(g);
                size_t block = g * BITS_PER_WORD + bm_ctz(fin);
                fin &= fin - 1;
                // A finaliser that ran before this one may have freed this block.
                if (!BM_GET(area, PLANE_FINALISER, block) || !IS_HEAD(area, block)) {
                    continue;
                }
                mp_obj_base_t *obj = PTR_FROM_BLOCK(area, block);
                if (obj->type != NULL) {
                    // if the object has a type then see if it has a __del__ method
                    mp_obj_t dest[2];
                    mp_load_method_maybe(MP_OBJ_FROM_PTR(obj), MP_QSTR___del__, dest);
                    if (dest[0] != MP_OBJ_NULL) {
                        // load_method returned a method, execute it in a protected environment
                        #if MICROPY_ENABLE_SCHEDULER
                        mp_sched_lock();
                        #endif
                        mp_call_function_1_protected(dest[0], dest[1]);
                        #if MICROPY_ENABLE_SCHEDULER
                        mp_sched_unlock();
                        #endif
                    }
                }
                BM_CLEAR(area, PLANE_FINALISER, block);
            }
        }
    }
    #endif
    #if MICROPY_PY_WEAKREF
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        for (size_t g = 0; g < area->gc_n_groups_used; g++) {
            bmword_t weak = BMW(area, g, PLANE_WEAKREF);
            if (weak) {
                weak &= BMW(area, g, PLANE_HEAD) & ~BMW(area, g, PLANE_TAIL_MARK);
            }
            while (weak) {
                size_t block = g * BITS_PER_WORD + bm_ctz(weak);
                weak &= weak - 1;
                gc_weakref_about_to_be_freed(PTR_FROM_BLOCK(area, block));
                BM_CLEAR(area, PLANE_WEAKREF, block);
            }
        }
    }
    #endif
}

// Free unmarked heads and their tails, and clear all marks.
static void gc_sweep_free_blocks(void) {
    #if MICROPY_PY_GC_COLLECT_RETVAL
    MP_STATE_MEM(gc_collected) = 0;
    #endif
    #if MICROPY_GC_SPLIT_HEAP
    MP_STATE_MEM(gc_alloc_area)[0] = NULL;
    MP_STATE_MEM(gc_alloc_area)[1] = NULL;
    #endif
    #if MICROPY_GC_SPLIT_HEAP_AUTO
    mp_state_mem_area_t *prev_area = NULL;
    #endif

    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        size_t first_free = area->gc_n_blocks;
        size_t n_groups_used = 0;
        // Whether the allocation running into the next word is live, and
        // whether the last block of the previous word was allocated.
        bool live = false;
        bmword_t prev_alloc_top = 0;

        for (size_t g = 0; g < area->gc_n_groups_used; g++) {
            MICROPY_GC_HOOK_LOOP(g);
            bmword_t head = BMW(area, g, PLANE_HEAD);
            bmword_t tail_mark = BMW(area, g, PLANE_TAIL_MARK);
            bmword_t alloc = head | tail_mark;
            bmword_t mark = head & tail_mark;

            // Segments start at each head and at the first free block after an
            // allocated block.  A segment is kept if it starts at a marked head.
            bmword_t starts = head | (~alloc & ((alloc << 1) | prev_alloc_top));
            bmword_t keep = 0;
            unsigned int seg_start = 0;
            while (starts) {
                unsigned int pos = bm_ctz(starts);
                starts &= starts - 1;
                if (live && pos > seg_start) {
                    keep |= bm_range_mask(seg_start, pos);
                }
                live = (mark >> pos) & 1;
                seg_start = pos;
            }
            if (live) {
                keep |= bm_range_mask(seg_start, BITS_PER_WORD);
            }
            prev_alloc_top = alloc >> (BITS_PER_WORD - 1);

            #if MICROPY_PY_GC_COLLECT_RETVAL
            MP_STATE_MEM(gc_collected) += bm_popcount(head & ~mark);
            #endif

            // Marked heads become unmarked heads, kept tails stay tails.
            bmword_t new_alloc = alloc & keep;
            BMW(area, g, PLANE_HEAD) = mark;
            BMW(area, g, PLANE_TAIL_MARK) = tail_mark & ~head & keep;
            #if MICROPY_ENABLE_FINALISER
            BMW(area, g, PLANE_FINALISER) &= keep;
            #endif
            #if MICROPY_PY_WEAKREF
            BMW(area, g, PLANE_WEAKREF) &= keep;
            #endif
            #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
            BMW(area, g, PLANE_HAS_PTRS) &= keep;
            #endif

            if (first_free == area->gc_n_blocks && new_alloc != ALL_ONES) {
                first_free = MIN(g * BITS_PER_WORD + bm_ctz(~new_alloc), area->gc_n_blocks);
            }
            if (new_alloc) {
                n_groups_used = g + 1;
            }
        }

        if (first_free == area->gc_n_blocks) {
            first_free = MIN(area->gc_n_groups_used * BITS_PER_WORD, area->gc_n_blocks);
        }
        area->gc_first_free_block = first_free;
        area->gc_n_groups_used = n_groups_used;
        for (size_t c = 0; c < MP_GC_BITMAP_N_SIZE_CLASSES; c++) {
            area->gc_class_hint[c] = first_free;
        }

        #if MICROPY_GC_SPLIT_HEAP_AUTO
        // Free any empty area, aside from the first one
        if (n_groups_used == 0 && prev_area != NULL) {
            DEBUG_printf("gc_sweep_free_blocks free empty area %p\n", area);
            NEXT_AREA(prev_area) = NEXT_AREA(area);
            MP_PLAT_FREE_HEAP(area);
            area = prev_area;
        }
        prev_area = area;
        #endif
    }
}

void gc_collect_end(void) {
    gc_deal_with_stack_overflow();
    gc_return_mark_stack();
    gc_sweep_run_finalisers();
    gc_sweep_free_blocks();
    MP_STATE_THREAD(gc_lock_depth) &= ~GC_COLLECT_FLAG;
    GC_EXIT();
    #if MICROPY_PY_WEAKREF
    gc_weakref_sweep();
    #endif
}

/******************************************************************************/
// Allocation

// Size class of an n-block request: floor(log2(n)), capped.
static inline unsigned int gc_size_class(size_t n) {
    if (n >= (size_t)1 << (MP_GC_BITMAP_N_SIZE_CLASSES - 1)) {
        return MP_GC_BITMAP_N_SIZE_CLASSES - 1;
    }
    return BITS_PER_WORD - 1 - bm_clz((bmword_t)n);
}

// Lower the size class hints after blocks from `block` upwards were freed.
static void gc_lower_class_hints(mp_state_mem_area_t *area, size_t block) {
    // The freed blocks may have joined a free run that starts lower down.
    // Look a few words down for its start; if it's further than that, a lower
    // hint is still correct, just less useful.
    size_t group = GROUP(block);
    bmword_t used = W_ALLOC(area, group) & (BIT(block) - 1);
    for (unsigned int i = 0; used == 0 && group > 0 && i < 8; ++i) {
        --group;
        used = W_ALLOC(area, group);
    }
    size_t run_start = used ? group * BITS_PER_WORD + BITS_PER_WORD - bm_clz(used) : group * BITS_PER_WORD;
    for (size_t c = 0; c < MP_GC_BITMAP_N_SIZE_CLASSES; c++) {
        area->gc_class_hint[c] = MIN(area->gc_class_hint[c], run_start);
    }
    if (block < area->gc_first_free_block) {
        area->gc_first_free_block = block;
    }
    #if MICROPY_GC_SPLIT_HEAP
    MP_STATE_MEM(gc_alloc_area)[0] = NULL;
    MP_STATE_MEM(gc_alloc_area)[1] = NULL;
    #endif
}

// Fast path for the most common request, a single block: it's usually in the
// word holding the first-free hint.
static inline MP_ALWAYSINLINE bool gc_take_first_free_block(mp_state_mem_area_t *area, size_t *block_out) {
    size_t block = area->gc_first_free_block;
    if (block < area->gc_n_blocks) {
        bmword_t free = ~W_ALLOC(area, GROUP(block)) & ~(BIT(block) - 1);
        if (free) {
            block = GROUP(block) * BITS_PER_WORD + bm_ctz(free);
            if (block < area->gc_n_blocks) {
                area->gc_first_free_block = block + 1;
                *block_out = block;
                return true;
            }
        }
    }
    return false;
}

// Find a run of n free blocks in the area and update the area's hints.  Each
// word of the table is handled with a few word operations however fragmented
// it is: a run is either inside one word, or made of free blocks at the top of
// one word, any number of completely free words, and free blocks at the
// bottom of a later word.  The search starts at the hint for n's size class,
// and records where the first run of that class starts, so repeated requests
// of similar size don't rescan fragmented memory.
static bool gc_find_free_run(mp_state_mem_area_t *area, size_t n, size_t *block_out) {
    size_t n_blocks = area->gc_n_blocks;
    unsigned int cls = gc_size_class(n);
    size_t class_len = (size_t)1 << cls;
    size_t start = MAX(area->gc_first_free_block, area->gc_class_hint[cls]);
    bool from_first_free = start == area->gc_first_free_block;
    if (start >= n_blocks || n > n_blocks - start) {
        return false;
    }
    size_t n_groups = N_GROUPS(area);
    bmword_t last_valid = n_blocks % BITS_PER_WORD ? bm_range_mask(0, n_blocks % BITS_PER_WORD) : ALL_ONES;
    bmword_t below_start = ((bmword_t)1 << (start % BITS_PER_WORD)) - 1;
    size_t first_seen = n_blocks;
    size_t class_seen = n_blocks;
    size_t run_start;
    // Number of free blocks running up to the start of the current word.
    size_t carry = 0;

    for (size_t g = GROUP(start); g < n_groups; g++) {
        MICROPY_GC_HOOK_LOOP(g);
        bmword_t free = ~W_ALLOC(area, g) & ~below_start;
        below_start = 0;
        if (g == n_groups - 1) {
            free &= last_valid;
        }
        if (free == 0) {
            carry = 0;
            continue;
        }
        if (first_seen == n_blocks) {
            first_seen = g * BITS_PER_WORD + bm_ctz(free);
        }
        if (free == ALL_ONES) {
            carry += BITS_PER_WORD;
            if (class_seen == n_blocks && carry >= class_len) {
                class_seen = (g + 1) * BITS_PER_WORD - carry;
            }
            if (carry >= n) {
                run_start = (g + 1) * BITS_PER_WORD - carry;
                goto found;
            }
            continue;
        }
        // A run continuing from the words below.
        size_t bottom = carry + bm_ctz(~free);
        if (class_seen == n_blocks && bottom >= class_len) {
            class_seen = g * BITS_PER_WORD - carry;
        }
        if (bottom >= n) {
            run_start = g * BITS_PER_WORD - carry;
            goto found;
        }
        // A run inside this word: y has bit i set if blocks i to i+len-1 are
        // free.  For n below the word size, n's class length is a power of
        // two below the word size, so it's one of the len values.
        if (n < BITS_PER_WORD) {
            bmword_t y = free;
            unsigned int len = 1;
            while (len * 2 <= n) {
                y &= y >> len;
                len *= 2;
            }
            if (class_seen == n_blocks && y) {
                class_seen = g * BITS_PER_WORD + bm_ctz(y);
            }
            if (len < n) {
                y &= y >> (n - len);
            }
            if (y) {
                run_start = g * BITS_PER_WORD + bm_ctz(y);
                goto found;
            }
        }
        // Free blocks at the top of this word.
        carry = bm_clz(~free);
    }
    if (from_first_free) {
        area->gc_first_free_block = first_seen;
    }
    area->gc_class_hint[cls] = class_seen;
    return false;

found:
    if (from_first_free) {
        // Nothing below first_seen is free, and if the run starts there then
        // nothing below its end is free either.
        area->gc_first_free_block = first_seen == run_start ? run_start + n : first_seen;
    }
    area->gc_class_hint[cls] = class_seen;
    *block_out = run_start;
    return true;
}

static inline MP_ALWAYSINLINE bool gc_take_free_run(mp_state_mem_area_t *area, size_t n, size_t *block_out) {
    return (n == 1 && gc_take_first_free_block(area, block_out)) || gc_find_free_run(area, n, block_out);
}

static mp_state_mem_area_t *gc_alloc_find(size_t n_blocks, unsigned int alloc_flags, size_t *block_out) {
    #if MICROPY_GC_SPLIT_HEAP
    // Large pointer-free buffers go in external areas first, everything else
    // goes in the other areas first.
    unsigned int prefer = 0;
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    if ((alloc_flags & GC_ALLOC_FLAG_CONTAINS_NO_GC_POINTERS)
        && n_blocks * BYTES_PER_BLOCK >= MICROPY_GC_EXTERNAL_ALLOC_THRESHOLD) {
        prefer = GC_AREA_FLAG_EXTERNAL;
    }
    #endif
    // Try the area that last satisfied a similar allocation first.
    mp_state_mem_area_t **last = &MP_STATE_MEM(gc_alloc_area)[prefer ? 1 : 0];
    mp_state_mem_area_t *area = *last;
    if (area != NULL && gc_take_free_run(area, n_blocks, block_out)) {
        return area;
    }
    for (int pass = 0; pass < 2; ++pass) {
        for (area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
            if (area->gc_first_free_block < area->gc_n_blocks
                && ((area->gc_area_flags & GC_AREA_FLAG_EXTERNAL) == prefer) == (pass == 0)
                && gc_take_free_run(area, n_blocks, block_out)) {
                // Remember a fallback area only for a single block: then the
                // preferred areas are full until something is freed, which
                // clears this cache.
                if (pass == 0 || n_blocks == 1) {
                    *last = area;
                }
                return area;
            }
        }
    }
    return NULL;
    #else
    (void)alloc_flags;
    mp_state_mem_area_t *area = &MP_STATE_MEM(area);
    return gc_take_free_run(area, n_blocks, block_out) ? area : NULL;
    #endif
}

void *gc_alloc(size_t n_bytes, unsigned int alloc_flags) {
    bool has_finaliser = alloc_flags & GC_ALLOC_FLAG_HAS_FINALISER;
    // an object with a finaliser starts with a type pointer, so is never pointer-free
    assert(!((alloc_flags & GC_ALLOC_FLAG_CONTAINS_NO_GC_POINTERS) && has_finaliser));
    size_t n_blocks = (n_bytes + BYTES_PER_BLOCK - 1) / BYTES_PER_BLOCK;
    DEBUG_printf("gc_alloc(" UINT_FMT " bytes -> " UINT_FMT " blocks)\n", n_bytes, n_blocks);

    if (n_blocks == 0) {
        return NULL;
    }

    if (MP_STATE_THREAD(gc_lock_depth) > 0) {
        return NULL;
    }

    GC_ENTER();

    mp_state_mem_area_t *area;
    size_t block;
    int collected = !MP_STATE_MEM(gc_auto_collect_enabled);
    #if MICROPY_GC_SPLIT_HEAP_AUTO
    bool added = false;
    #endif

    #if MICROPY_GC_ALLOC_THRESHOLD
    if (!collected && MP_STATE_MEM(gc_alloc_amount) >= MP_STATE_MEM(gc_alloc_threshold)) {
        GC_EXIT();
        gc_collect();
        collected = 1;
        GC_ENTER();
    }
    #endif

    for (;;) {
        area = gc_alloc_find(n_blocks, alloc_flags, &block);
        if (area != NULL) {
            break;
        }
        GC_EXIT();
        if (collected) {
            #if MICROPY_GC_SPLIT_HEAP_AUTO
            if (!added && gc_try_add_heap(n_bytes)) {
                added = true;
                GC_ENTER();
                continue;
            }
            #endif
            return NULL;
        }
        DEBUG_printf("gc_alloc(" UINT_FMT "): no free mem, triggering GC\n", n_bytes);
        gc_collect();
        collected = 1;
        GC_ENTER();
    }

    BM_SET(area, PLANE_HEAD, block);
    if (n_blocks > 1) {
        bm_range_write(area, PLANE_TAIL_MARK, block + 1, n_blocks - 1, true);
    }
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    if (!(alloc_flags & GC_ALLOC_FLAG_CONTAINS_NO_GC_POINTERS)) {
        BM_SET(area, PLANE_HAS_PTRS, block);
    }
    #endif
    area->gc_n_groups_used = MAX(area->gc_n_groups_used, GROUP(block + n_blocks - 1) + 1);

    // we must create this pointer before unlocking the GC so a collection can find it
    void *ret_ptr = PTR_FROM_BLOCK(area, block);
    DEBUG_printf("gc_alloc(%p)\n", ret_ptr);

    #if MICROPY_GC_ALLOC_THRESHOLD
    MP_STATE_MEM(gc_alloc_amount) += n_blocks;
    #endif

    #if MICROPY_ENABLE_FINALISER
    if (has_finaliser) {
        // clear type pointer in case it is never set
        ((mp_obj_base_t *)ret_ptr)->type = NULL;
        BM_SET(area, PLANE_FINALISER, block);
    }
    #else
    (void)has_finaliser;
    #endif

    GC_EXIT();

    #if MICROPY_GC_CONSERVATIVE_CLEAR
    // be conservative and zero out all the newly allocated blocks
    memset((byte *)ret_ptr, 0, n_blocks * BYTES_PER_BLOCK);
    #else
    // zero out the additional bytes of the newly allocated blocks, which may
    // hold stale heap pointers
    memset((byte *)ret_ptr + n_bytes, 0, n_blocks * BYTES_PER_BLOCK - n_bytes);
    #endif

    return ret_ptr;
}

// Release the blocks of the allocation at `block`, which has n_blocks blocks.
static void gc_release(mp_state_mem_area_t *area, size_t block, size_t n_blocks) {
    BM_CLEAR(area, PLANE_HEAD, block);
    #if MICROPY_ENABLE_FINALISER
    BM_CLEAR(area, PLANE_FINALISER, block);
    #endif
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    BM_CLEAR(area, PLANE_HAS_PTRS, block);
    #endif
    // Clears the tails, and the mark if the head is marked.
    bm_range_write(area, PLANE_TAIL_MARK, block, n_blocks, false);
    gc_lower_class_hints(area, block);
}

// force the freeing of a piece of memory
// TODO: freeing here does not call finaliser
void gc_free(void *ptr) {
    // Cannot free while the GC is locked, unless we're only doing a gc sweep.
    // However free is an optimisation to reclaim the memory immediately, this
    // means it will now be left until the next collection.
    if (MP_STATE_THREAD(gc_lock_depth) & ~GC_COLLECT_FLAG) {
        return;
    }

    GC_ENTER();

    DEBUG_printf("gc_free(%p)\n", ptr);

    if (ptr == NULL) {
        GC_EXIT();
        return;
    }

    mp_state_mem_area_t *area = gc_block_area(ptr);
    assert(area);
    size_t block = BLOCK_FROM_PTR(area, ptr);
    assert(IS_HEAD(area, block));
    assert(!IS_MARKED_HEAD(area, block) || (MP_STATE_THREAD(gc_lock_depth) & GC_COLLECT_FLAG));

    #if MICROPY_PY_WEAKREF
    // Objects that have a weak reference should not be explicitly freed.
    assert(!BM_GET(area, PLANE_WEAKREF, block));
    #endif

    gc_release(area, block, gc_obj_n_blocks(area, block));

    GC_EXIT();
}

// Get the GC bits corresponding to the GC pointer, in the classic GC's format:
// bits 0-1 are FREE/HEAD/TAIL/MARK (0-3), then the FTB, WTB and STB bits.
uint32_t gc_meta_bits(void *ptr) {
    mp_state_mem_area_t *area = gc_block_area(ptr);
    assert(area);
    size_t block = BLOCK_FROM_PTR(area, ptr);
    uint32_t bits = BM_GET(area, PLANE_HEAD, block) | BM_GET(area, PLANE_TAIL_MARK, block) << 1;
    #if MICROPY_ENABLE_FINALISER
    bits |= BM_GET(area, PLANE_FINALISER, block) << 2;
    #endif
    #if MICROPY_PY_WEAKREF
    bits |= BM_GET(area, PLANE_WEAKREF, block) << 3;
    #endif
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    bits |= BM_GET(area, PLANE_HAS_PTRS, block) << 4;
    #endif
    return bits;
}

size_t gc_nbytes(const void *ptr) {
    GC_ENTER();
    size_t n = 0;
    mp_state_mem_area_t *area = gc_block_area(ptr);
    if (area != NULL) {
        size_t block = BLOCK_FROM_PTR(area, ptr);
        if (IS_HEAD(area, block)) {
            n = gc_obj_n_blocks(area, block) * BYTES_PER_BLOCK;
        }
    }
    GC_EXIT();
    return n;
}

#if MICROPY_PY_WEAKREF
// Mark the GC heap pointer as having a weakref.
void gc_weakref_mark(void *ptr) {
    mp_state_mem_area_t *area = gc_block_area(ptr);
    assert(area);
    size_t block = BLOCK_FROM_PTR(area, ptr);
    assert(IS_HEAD(area, block));
    BM_SET(area, PLANE_WEAKREF, block);
}
#endif

void gc_buf_contains_ptrs(const void *ptr) {
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    GC_ENTER();
    mp_state_mem_area_t *area = gc_any_area(ptr);
    if (area != NULL) {
        size_t block = BLOCK_FROM_PTR(area, ptr);
        if (IS_ALLOC(area, block)) {
            while (!IS_HEAD(area, block)) {
                block -= 1;
            }
            BM_SET(area, PLANE_HAS_PTRS, block);
        }
    }
    GC_EXIT();
    #else
    (void)ptr;
    #endif
}

void *gc_realloc(void *ptr_in, size_t n_bytes, bool allow_move) {
    // check for pure allocation
    if (ptr_in == NULL) {
        return gc_alloc(n_bytes, 0);
    }

    // check for pure free
    if (n_bytes == 0) {
        gc_free(ptr_in);
        return NULL;
    }

    if (MP_STATE_THREAD(gc_lock_depth) > 0) {
        return NULL;
    }

    GC_ENTER();

    mp_state_mem_area_t *area = gc_block_area(ptr_in);
    assert(area);
    size_t block = BLOCK_FROM_PTR(area, ptr_in);
    assert(IS_HEAD(area, block));

    size_t new_blocks = (n_bytes + BYTES_PER_BLOCK - 1) / BYTES_PER_BLOCK;
    size_t n_blocks = gc_obj_n_blocks(area, block);

    if (new_blocks == n_blocks) {
        GC_EXIT();
        return ptr_in;
    }

    if (new_blocks < n_blocks) {
        // shrink: free the unneeded tail blocks
        bm_range_write(area, PLANE_TAIL_MARK, block + new_blocks, n_blocks - new_blocks, false);
        gc_lower_class_hints(area, block + new_blocks);
        GC_EXIT();
        return ptr_in;
    }

    // grow in place if the following blocks are free
    size_t extra = new_blocks - n_blocks;
    if (block + new_blocks <= area->gc_n_blocks && bm_range_is_free(area, block + n_blocks, extra)) {
        bm_range_write(area, PLANE_TAIL_MARK, block + n_blocks, extra, true);
        area->gc_n_groups_used = MAX(area->gc_n_groups_used, GROUP(block + new_blocks - 1) + 1);
        if (area->gc_first_free_block >= block + n_blocks && area->gc_first_free_block < block + new_blocks) {
            area->gc_first_free_block = block + new_blocks;
        }
        GC_EXIT();

        #if MICROPY_GC_CONSERVATIVE_CLEAR
        memset((byte *)ptr_in + n_blocks * BYTES_PER_BLOCK, 0, extra * BYTES_PER_BLOCK);
        #else
        memset((byte *)ptr_in + n_bytes, 0, new_blocks * BYTES_PER_BLOCK - n_bytes);
        #endif
        return ptr_in;
    }

    unsigned int alloc_flags = 0;
    #if MICROPY_ENABLE_FINALISER
    if (BM_GET(area, PLANE_FINALISER, block)) {
        alloc_flags |= GC_ALLOC_FLAG_HAS_FINALISER;
    }
    #endif
    #if MICROPY_GC_ENABLE_CONTAINS_NO_GC_POINTERS
    if (!BM_GET(area, PLANE_HAS_PTRS, block)) {
        alloc_flags |= GC_ALLOC_FLAG_CONTAINS_NO_GC_POINTERS;
    }
    #endif

    GC_EXIT();

    if (!allow_move) {
        return NULL;
    }

    // can't resize in place; find a new contiguous chain
    void *ptr_out = gc_alloc(n_bytes, alloc_flags);
    if (ptr_out == NULL) {
        return NULL;
    }

    DEBUG_printf("gc_realloc(%p -> %p)\n", ptr_in, ptr_out);
    memcpy(ptr_out, ptr_in, n_blocks * BYTES_PER_BLOCK);
    gc_free(ptr_in);
    return ptr_out;
}

/******************************************************************************/
// Statistics and debugging

void gc_info(gc_info_t *info) {
    GC_ENTER();
    memset(info, 0, sizeof(*info));
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        info->total += area->gc_pool_end - area->gc_pool_start;
        size_t len_free = 0;
        for (size_t block = 0; block < area->gc_n_blocks;) {
            MICROPY_GC_HOOK_LOOP(block);
            size_t group = GROUP(block);
            bmword_t alloc = W_ALLOC(area, group);
            if (block % BITS_PER_WORD == 0 && alloc == 0) {
                // Whole word free.
                size_t n = MIN(BITS_PER_WORD, area->gc_n_blocks - block);
                info->free += n;
                len_free += n;
                block += n;
                continue;
            }
            if (!(alloc & BIT(block))) {
                info->free += 1;
                len_free += 1;
                block += 1;
                continue;
            }
            info->max_free = MAX(info->max_free, len_free);
            len_free = 0;
            // A head (a tail with no head would be a corrupt heap).
            size_t len = gc_obj_n_blocks(area, block);
            info->used += len;
            if (len == 1) {
                info->num_1block += 1;
            } else if (len == 2) {
                info->num_2block += 1;
            }
            info->max_block = MAX(info->max_block, len);
            block += len;
        }
        info->max_free = MAX(info->max_free, len_free);
    }

    info->used *= BYTES_PER_BLOCK;
    info->free *= BYTES_PER_BLOCK;

    #if MICROPY_GC_SPLIT_HEAP_AUTO
    info->max_new_split = gc_get_max_new_split();
    #endif

    GC_EXIT();
}

// Fast version of gc_info that only computes total/used/free.
void gc_info_fast(gc_info_t *info) {
    GC_ENTER();
    memset(info, 0, sizeof(*info));
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        info->total += area->gc_pool_end - area->gc_pool_start;
        size_t used_blocks = 0;
        for (size_t g = 0; g < area->gc_n_groups_used; g++) {
            used_blocks += bm_popcount(W_ALLOC(area, g));
        }
        info->used += used_blocks * BYTES_PER_BLOCK;
    }
    info->free = info->total - info->used;
    GC_EXIT();
}

void gc_dump_alloc_table(const mp_print_t *print) {
    GC_ENTER();
    static const size_t DUMP_BYTES_PER_LINE = 64;
    for (mp_state_mem_area_t *area = &MP_STATE_MEM(area); area != NULL; area = NEXT_AREA(area)) {
        mp_printf(print, "GC memory layout; from %p:", area->gc_pool_start);
        size_t n_blocks = area->gc_n_blocks;
        for (size_t bl = 0; bl < n_blocks; bl++) {
            if (bl % DUMP_BYTES_PER_LINE == 0) {
                // check if this line and the next contain only free blocks
                size_t bl2 = bl;
                while (bl2 < n_blocks && !IS_ALLOC(area, bl2)) {
                    bl2++;
                }
                if (bl2 - bl >= 2 * DUMP_BYTES_PER_LINE) {
                    mp_printf(print, "\n       (%u lines all free)", (uint)((bl2 - bl) / DUMP_BYTES_PER_LINE));
                    bl = bl2 & (~(DUMP_BYTES_PER_LINE - 1));
                    if (bl >= n_blocks) {
                        break;
                    }
                }
                // (the cast to uint32_t is for 16-bit ports)
                mp_printf(print, "\n%08x: ", (uint)(bl * BYTES_PER_BLOCK));
            }
            int c;
            if (!IS_ALLOC(area, bl)) {
                c = '.';
            } else if (!IS_HEAD(area, bl)) {
                c = '=';
            } else if (IS_MARKED_HEAD(area, bl)) {
                c = 'm';
            } else {
                // print the MicroPython object type of the head block
                void **ptr = PTR_FROM_BLOCK(area, bl);
                if (*ptr == &mp_type_tuple) {
                    c = 'T';
                } else if (*ptr == &mp_type_list) {
                    c = 'L';
                } else if (*ptr == &mp_type_dict) {
                    c = 'D';
                } else if (*ptr == &mp_type_str || *ptr == &mp_type_bytes) {
                    c = 'S';
                }
                #if MICROPY_PY_BUILTINS_BYTEARRAY
                else if (*ptr == &mp_type_bytearray) {
                    c = 'A';
                }
                #endif
                #if MICROPY_PY_ARRAY
                else if (*ptr == &mp_type_array) {
                    c = 'A';
                }
                #endif
                #if MICROPY_PY_BUILTINS_FLOAT
                else if (*ptr == &mp_type_float) {
                    c = 'F';
                }
                #endif
                else if (*ptr == &mp_type_fun_bc) {
                    c = 'B';
                } else if (*ptr == &mp_type_module) {
                    c = 'M';
                } else {
                    c = 'h';
                }
            }
            mp_printf(print, "%c", c);
        }
        mp_print_str(print, "\n");
    }
    GC_EXIT();
}

#endif // MICROPY_ENABLE_GC && MICROPY_GC_IMPL == MICROPY_GC_IMPL_BITMAP
