/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2014 Paul Sokolovsky
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

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"

#if MICROPY_ENABLE_GC

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

// make this 1 to dump the heap each time it changes
#define EXTENSIVE_HEAP_PROFILING (0)

// make this 1 to zero out swept memory to more eagerly
// detect untraced object still in use
#define CLEAR_ON_SWEEP (0)

#define WORDS_PER_BLOCK ((MICROPY_BYTES_PER_GC_BLOCK) / BYTES_PER_WORD)
#define BYTES_PER_BLOCK (MICROPY_BYTES_PER_GC_BLOCK)

// ATB = allocation table byte
// 0b00 = FREE -- free block
// 0b01 = HEAD -- head of a chain of blocks
// 0b10 = TAIL -- in the tail of a chain of blocks
// 0b11 = MARK -- marked head block

#define AT_FREE (0)
#define AT_HEAD (1)
#define AT_TAIL (2)
#define AT_MARK (3)

#define BLOCKS_PER_ATB (4)
#define ATB_MASK_0 (0x03)
#define ATB_MASK_1 (0x0c)
#define ATB_MASK_2 (0x30)
#define ATB_MASK_3 (0xc0)

#define ATB_0_IS_FREE(a) (((a) & ATB_MASK_0) == 0)
#define ATB_1_IS_FREE(a) (((a) & ATB_MASK_1) == 0)
#define ATB_2_IS_FREE(a) (((a) & ATB_MASK_2) == 0)
#define ATB_3_IS_FREE(a) (((a) & ATB_MASK_3) == 0)

#define BLOCK_SHIFT(block) (2 * ((block) & (BLOCKS_PER_ATB - 1)))
#define ATB_GET_KIND(block) ((MP_STATE_MEM(gc_alloc_table_start)[(block) / BLOCKS_PER_ATB] >> BLOCK_SHIFT(block)) & 3)
#define ATB_ANY_TO_FREE(block) do { MP_STATE_MEM(gc_alloc_table_start)[(block) / BLOCKS_PER_ATB] &= (~(AT_MARK << BLOCK_SHIFT(block))); } while (0)
#define ATB_FREE_TO_HEAD(block) do { MP_STATE_MEM(gc_alloc_table_start)[(block) / BLOCKS_PER_ATB] |= (AT_HEAD << BLOCK_SHIFT(block)); } while (0)
#define ATB_FREE_TO_TAIL(block) do { MP_STATE_MEM(gc_alloc_table_start)[(block) / BLOCKS_PER_ATB] |= (AT_TAIL << BLOCK_SHIFT(block)); } while (0)
#define ATB_HEAD_TO_MARK(block) do { MP_STATE_MEM(gc_alloc_table_start)[(block) / BLOCKS_PER_ATB] |= (AT_MARK << BLOCK_SHIFT(block)); } while (0)
#define ATB_MARK_TO_HEAD(block) do { MP_STATE_MEM(gc_alloc_table_start)[(block) / BLOCKS_PER_ATB] &= (~(AT_TAIL << BLOCK_SHIFT(block))); } while (0)

#define BLOCK_FROM_PTR(ptr) (((byte *)(ptr) - MP_STATE_MEM(gc_pool_start)) / BYTES_PER_BLOCK)
#define PTR_FROM_BLOCK(block) (((block) * BYTES_PER_BLOCK + (uintptr_t)MP_STATE_MEM(gc_pool_start)))
#define ATB_FROM_BLOCK(bl) ((bl) / BLOCKS_PER_ATB)

#if MICROPY_ENABLE_FINALISER
// FTB = finaliser table byte
// if set, then the corresponding block may have a finaliser

#define BLOCKS_PER_FTB (8)

#define FTB_GET(block) ((MP_STATE_MEM(gc_finaliser_table_start)[(block) / BLOCKS_PER_FTB] >> ((block) & 7)) & 1)
#define FTB_SET(block) do { MP_STATE_MEM(gc_finaliser_table_start)[(block) / BLOCKS_PER_FTB] |= (1 << ((block) & 7)); } while (0)
#define FTB_CLEAR(block) do { MP_STATE_MEM(gc_finaliser_table_start)[(block) / BLOCKS_PER_FTB] &= (~(1 << ((block) & 7))); } while (0)
#endif

#if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
#define GC_ENTER() mp_thread_mutex_lock(&MP_STATE_MEM(gc_mutex), 1)
#define GC_EXIT() mp_thread_mutex_unlock(&MP_STATE_MEM(gc_mutex))
#else
#define GC_ENTER()
#define GC_EXIT()
#endif

// TODO waste less memory; currently requires that all entries in alloc_table have a corresponding block in pool
void gc_init(void *start, void *end) {
    // align end pointer on block boundary
    end = (void *)((uintptr_t)end & (~(BYTES_PER_BLOCK - 1)));
    DEBUG_printf("Initializing GC heap: %p..%p = " UINT_FMT " bytes\n", start, end, (byte *)end - (byte *)start);

    // calculate parameters for GC (T=total, A=alloc table, F=finaliser table, P=pool; all in bytes):
    // T = A + F + P
    //     F = A * BLOCKS_PER_ATB / BLOCKS_PER_FTB
    //     P = A * BLOCKS_PER_ATB * BYTES_PER_BLOCK
    // => T = A * (1 + BLOCKS_PER_ATB / BLOCKS_PER_FTB + BLOCKS_PER_ATB * BYTES_PER_BLOCK)
    size_t total_byte_len = (byte *)end - (byte *)start;
    #if MICROPY_ENABLE_FINALISER
    MP_STATE_MEM(gc_alloc_table_byte_len) = total_byte_len * BITS_PER_BYTE / (BITS_PER_BYTE + BITS_PER_BYTE * BLOCKS_PER_ATB / BLOCKS_PER_FTB + BITS_PER_BYTE * BLOCKS_PER_ATB * BYTES_PER_BLOCK);
    #else
    MP_STATE_MEM(gc_alloc_table_byte_len) = total_byte_len / (1 + BITS_PER_BYTE / 2 * BYTES_PER_BLOCK);
    #endif

    MP_STATE_MEM(gc_alloc_table_start) = (byte *)start;

    #if MICROPY_ENABLE_FINALISER
    size_t gc_finaliser_table_byte_len = (MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB + BLOCKS_PER_FTB - 1) / BLOCKS_PER_FTB;
    MP_STATE_MEM(gc_finaliser_table_start) = MP_STATE_MEM(gc_alloc_table_start) + MP_STATE_MEM(gc_alloc_table_byte_len);
    #endif

    size_t gc_pool_block_len = MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB;
    MP_STATE_MEM(gc_pool_start) = (byte *)end - gc_pool_block_len * BYTES_PER_BLOCK;
    MP_STATE_MEM(gc_pool_end) = end;

    #if MICROPY_ENABLE_FINALISER
    assert(MP_STATE_MEM(gc_pool_start) >= MP_STATE_MEM(gc_finaliser_table_start) + gc_finaliser_table_byte_len);
    #endif

    // clear ATBs
    memset(MP_STATE_MEM(gc_alloc_table_start), 0, MP_STATE_MEM(gc_alloc_table_byte_len));

    #if MICROPY_ENABLE_FINALISER
    // clear FTBs
    memset(MP_STATE_MEM(gc_finaliser_table_start), 0, gc_finaliser_table_byte_len);
    #endif

    // set last free ATB index to start of heap
    MP_STATE_MEM(gc_last_free_atb_index) = 0;

    // unlock the GC
    MP_STATE_MEM(gc_lock_depth) = 0;

    // allow auto collection
    MP_STATE_MEM(gc_auto_collect_enabled) = 1;

    #if MICROPY_GC_ALLOC_THRESHOLD
    // by default, maxuint for gc threshold, effectively turning gc-by-threshold off
    MP_STATE_MEM(gc_alloc_threshold) = (size_t)-1;
    MP_STATE_MEM(gc_alloc_amount) = 0;
    #endif

    #if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
    mp_thread_mutex_init(&MP_STATE_MEM(gc_mutex));
    #endif

    DEBUG_printf("GC layout:\n");
    DEBUG_printf("  alloc table at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", MP_STATE_MEM(gc_alloc_table_start), MP_STATE_MEM(gc_alloc_table_byte_len), MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB);
    #if MICROPY_ENABLE_FINALISER
    DEBUG_printf("  finaliser table at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", MP_STATE_MEM(gc_finaliser_table_start), gc_finaliser_table_byte_len, gc_finaliser_table_byte_len * BLOCKS_PER_FTB);
    #endif
    DEBUG_printf("  pool at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", MP_STATE_MEM(gc_pool_start), gc_pool_block_len * BYTES_PER_BLOCK, gc_pool_block_len);
}

void gc_lock(void) {
    GC_ENTER();
    MP_STATE_MEM(gc_lock_depth)++;
    GC_EXIT();
}

void gc_unlock(void) {
    GC_ENTER();
    MP_STATE_MEM(gc_lock_depth)--;
    GC_EXIT();
}

bool gc_is_locked(void) {
    return MP_STATE_MEM(gc_lock_depth) != 0;
}

// ptr should be of type void*
#define VERIFY_PTR(ptr) ( \
    ((uintptr_t)(ptr) & (BYTES_PER_BLOCK - 1)) == 0          /* must be aligned on a block */ \
    && ptr >= (void *)MP_STATE_MEM(gc_pool_start)        /* must be above start of pool */ \
    && ptr < (void *)MP_STATE_MEM(gc_pool_end)           /* must be below end of pool */ \
    )

#ifndef TRACE_MARK
#if DEBUG_PRINT
#define TRACE_MARK(block, ptr) DEBUG_printf("gc_mark(%p)\n", ptr)
#else
#define TRACE_MARK(block, ptr)
#endif
#endif

// Take the given block as the topmost block on the stack. Check all it's
// children: mark the unmarked child blocks and put those newly marked
// blocks on the stack. When all children have been checked, pop off the
// topmost block on the stack and repeat with that one.
STATIC void gc_mark_subtree(size_t block) {
    // Start with the block passed in the argument.
    size_t sp = 0;
    for (;;) {
        // work out number of consecutive blocks in the chain starting with this one
        size_t n_blocks = 0;
        do {
            n_blocks += 1;
        } while (ATB_GET_KIND(block + n_blocks) == AT_TAIL);

        // check this block's children
        void **ptrs = (void **)PTR_FROM_BLOCK(block);
        for (size_t i = n_blocks * BYTES_PER_BLOCK / sizeof(void *); i > 0; i--, ptrs++) {
            void *ptr = *ptrs;
            if (VERIFY_PTR(ptr)) {
                // Mark and push this pointer
                size_t childblock = BLOCK_FROM_PTR(ptr);
                if (ATB_GET_KIND(childblock) == AT_HEAD) {
                    // an unmarked head, mark it, and push it on gc stack
                    TRACE_MARK(childblock, ptr);
                    ATB_HEAD_TO_MARK(childblock);
                    if (sp < MICROPY_ALLOC_GC_STACK_SIZE) {
                        MP_STATE_MEM(gc_stack)[sp++] = childblock;
                    } else {
                        MP_STATE_MEM(gc_stack_overflow) = 1;
                    }
                }
            }
        }

        // Are there any blocks on the stack?
        if (sp == 0) {
            break; // No, stack is empty, we're done.
        }

        // pop the next block off the stack
        block = MP_STATE_MEM(gc_stack)[--sp];
    }
}

STATIC void gc_deal_with_stack_overflow(void) {
    while (MP_STATE_MEM(gc_stack_overflow)) {
        MP_STATE_MEM(gc_stack_overflow) = 0;

        // scan entire memory looking for blocks which have been marked but not their children
        for (size_t block = 0; block < MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB; block++) {
            // trace (again) if mark bit set
            if (ATB_GET_KIND(block) == AT_MARK) {
                gc_mark_subtree(block);
            }
        }
    }
}

STATIC void gc_sweep(void) {
    #if MICROPY_PY_GC_COLLECT_RETVAL
    MP_STATE_MEM(gc_collected) = 0;
    #endif
    // free unmarked heads and their tails
    int free_tail = 0;
    for (size_t block = 0; block < MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB; block++) {
        switch (ATB_GET_KIND(block)) {
            case AT_HEAD:
                #if MICROPY_ENABLE_FINALISER
                if (FTB_GET(block)) {
                    mp_obj_base_t *obj = (mp_obj_base_t *)PTR_FROM_BLOCK(block);
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
                    // clear finaliser flag
                    FTB_CLEAR(block);
                }
                #endif
                free_tail = 1;
                DEBUG_printf("gc_sweep(%p)\n", PTR_FROM_BLOCK(block));
                #if MICROPY_PY_GC_COLLECT_RETVAL
                MP_STATE_MEM(gc_collected)++;
                #endif
                // fall through to free the head
                MP_FALLTHROUGH

            case AT_TAIL:
                if (free_tail) {
                    ATB_ANY_TO_FREE(block);
                    #if CLEAR_ON_SWEEP
                    memset((void *)PTR_FROM_BLOCK(block), 0, BYTES_PER_BLOCK);
                    #endif
                }
                break;

            case AT_MARK:
                ATB_MARK_TO_HEAD(block);
                free_tail = 0;
                break;
        }
    }
}

void gc_collect_start(void) {
    GC_ENTER();
    MP_STATE_MEM(gc_lock_depth)++;
    #if MICROPY_GC_ALLOC_THRESHOLD
    MP_STATE_MEM(gc_alloc_amount) = 0;
    #endif
    MP_STATE_MEM(gc_stack_overflow) = 0;

    // Trace root pointers.  This relies on the root pointers being organised
    // correctly in the mp_state_ctx structure.  We scan nlr_top, dict_locals,
    // dict_globals, then the root pointer section of mp_state_vm.
    void **ptrs = (void **)(void *)&mp_state_ctx;
    size_t root_start = offsetof(mp_state_ctx_t, thread.dict_locals);
    size_t root_end = offsetof(mp_state_ctx_t, vm.qstr_last_chunk);
    gc_collect_root(ptrs + root_start / sizeof(void *), (root_end - root_start) / sizeof(void *));

    #if MICROPY_ENABLE_PYSTACK
    // Trace root pointers from the Python stack.
    ptrs = (void **)(void *)MP_STATE_THREAD(pystack_start);
    gc_collect_root(ptrs, (MP_STATE_THREAD(pystack_cur) - MP_STATE_THREAD(pystack_start)) / sizeof(void *));
    #endif
}

void gc_collect_root(void **ptrs, size_t len) {
    for (size_t i = 0; i < len; i++) {
        void *ptr = ptrs[i];
        if (VERIFY_PTR(ptr)) {
            size_t block = BLOCK_FROM_PTR(ptr);
            if (ATB_GET_KIND(block) == AT_HEAD) {
                // An unmarked head: mark it, and mark all its children
                TRACE_MARK(block, ptr);
                ATB_HEAD_TO_MARK(block);
                gc_mark_subtree(block);
            }
        }
    }
}

void gc_collect_end(void) {
    gc_deal_with_stack_overflow();
    gc_sweep();
    MP_STATE_MEM(gc_last_free_atb_index) = 0;
    MP_STATE_MEM(gc_lock_depth)--;
    GC_EXIT();
}

void gc_sweep_all(void) {
    GC_ENTER();
    MP_STATE_MEM(gc_lock_depth)++;
    MP_STATE_MEM(gc_stack_overflow) = 0;
    gc_collect_end();
}

void gc_info(gc_info_t *info) {
    GC_ENTER();
    info->total = MP_STATE_MEM(gc_pool_end) - MP_STATE_MEM(gc_pool_start);
    info->used = 0;
    info->free = 0;
    info->max_free = 0;
    info->num_1block = 0;
    info->num_2block = 0;
    info->max_block = 0;
    bool finish = false;
    for (size_t block = 0, len = 0, len_free = 0; !finish;) {
        size_t kind = ATB_GET_KIND(block);
        switch (kind) {
            case AT_FREE:
                info->free += 1;
                len_free += 1;
                len = 0;
                break;

            case AT_HEAD:
                info->used += 1;
                len = 1;
                break;

            case AT_TAIL:
                info->used += 1;
                len += 1;
                break;

            case AT_MARK:
                // shouldn't happen
                break;
        }

        block++;
        finish = (block == MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB);
        // Get next block type if possible
        if (!finish) {
            kind = ATB_GET_KIND(block);
        }

        if (finish || kind == AT_FREE || kind == AT_HEAD) {
            if (len == 1) {
                info->num_1block += 1;
            } else if (len == 2) {
                info->num_2block += 1;
            }
            if (len > info->max_block) {
                info->max_block = len;
            }
            if (finish || kind == AT_HEAD) {
                if (len_free > info->max_free) {
                    info->max_free = len_free;
                }
                len_free = 0;
            }
        }
    }

    info->used *= BYTES_PER_BLOCK;
    info->free *= BYTES_PER_BLOCK;
    GC_EXIT();
}

void *gc_alloc(size_t n_bytes, unsigned int alloc_flags) {
    bool has_finaliser = alloc_flags & GC_ALLOC_FLAG_HAS_FINALISER;
    size_t n_blocks = ((n_bytes + BYTES_PER_BLOCK - 1) & (~(BYTES_PER_BLOCK - 1))) / BYTES_PER_BLOCK;
    DEBUG_printf("gc_alloc(" UINT_FMT " bytes -> " UINT_FMT " blocks)\n", n_bytes, n_blocks);

    // check for 0 allocation
    if (n_blocks == 0) {
        return NULL;
    }

    GC_ENTER();

    // check if GC is locked
    if (MP_STATE_MEM(gc_lock_depth) > 0) {
        GC_EXIT();
        return NULL;
    }

    size_t i;
    size_t end_block;
    size_t start_block;
    size_t n_free;
    int collected = !MP_STATE_MEM(gc_auto_collect_enabled);

    #if MICROPY_GC_ALLOC_THRESHOLD
    if (!collected && MP_STATE_MEM(gc_alloc_amount) >= MP_STATE_MEM(gc_alloc_threshold)) {
        GC_EXIT();
        gc_collect();
        collected = 1;
        GC_ENTER();
    }
    #endif

    for (;;) {

        // look for a run of n_blocks available blocks
        n_free = 0;
        for (i = MP_STATE_MEM(gc_last_free_atb_index); i < MP_STATE_MEM(gc_alloc_table_byte_len); i++) {
            byte a = MP_STATE_MEM(gc_alloc_table_start)[i];
            // *FORMAT-OFF*
            if (ATB_0_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 0; goto found; } } else { n_free = 0; }
            if (ATB_1_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 1; goto found; } } else { n_free = 0; }
            if (ATB_2_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 2; goto found; } } else { n_free = 0; }
            if (ATB_3_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 3; goto found; } } else { n_free = 0; }
            // *FORMAT-ON*
        }

        GC_EXIT();
        // nothing found!
        if (collected) {
            return NULL;
        }
        DEBUG_printf("gc_alloc(" UINT_FMT "): no free mem, triggering GC\n", n_bytes);
        gc_collect();
        collected = 1;
        GC_ENTER();
    }

    // found, ending at block i inclusive
found:
    // get starting and end blocks, both inclusive
    end_block = i;
    start_block = i - n_free + 1;

    // Set last free ATB index to block after last block we found, for start of
    // next scan.  To reduce fragmentation, we only do this if we were looking
    // for a single free block, which guarantees that there are no free blocks
    // before this one.  Also, whenever we free or shink a block we must check
    // if this index needs adjusting (see gc_realloc and gc_free).
    if (n_free == 1) {
        MP_STATE_MEM(gc_last_free_atb_index) = (i + 1) / BLOCKS_PER_ATB;
    }

    // mark first block as used head
    ATB_FREE_TO_HEAD(start_block);

    // mark rest of blocks as used tail
    // TODO for a run of many blocks can make this more efficient
    for (size_t bl = start_block + 1; bl <= end_block; bl++) {
        ATB_FREE_TO_TAIL(bl);
    }

    // get pointer to first block
    // we must create this pointer before unlocking the GC so a collection can find it
    void *ret_ptr = (void *)(MP_STATE_MEM(gc_pool_start) + start_block * BYTES_PER_BLOCK);
    DEBUG_printf("gc_alloc(%p)\n", ret_ptr);

    #if MICROPY_GC_ALLOC_THRESHOLD
    MP_STATE_MEM(gc_alloc_amount) += n_blocks;
    #endif

    GC_EXIT();

    #if MICROPY_GC_CONSERVATIVE_CLEAR
    // be conservative and zero out all the newly allocated blocks
    memset((byte *)ret_ptr, 0, (end_block - start_block + 1) * BYTES_PER_BLOCK);
    #else
    // zero out the additional bytes of the newly allocated blocks
    // This is needed because the blocks may have previously held pointers
    // to the heap and will not be set to something else if the caller
    // doesn't actually use the entire block.  As such they will continue
    // to point to the heap and may prevent other blocks from being reclaimed.
    memset((byte *)ret_ptr + n_bytes, 0, (end_block - start_block + 1) * BYTES_PER_BLOCK - n_bytes);
    #endif

    #if MICROPY_ENABLE_FINALISER
    if (has_finaliser) {
        // clear type pointer in case it is never set
        ((mp_obj_base_t *)ret_ptr)->type = NULL;
        // set mp_obj flag only if it has a finaliser
        GC_ENTER();
        FTB_SET(start_block);
        GC_EXIT();
    }
    #else
    (void)has_finaliser;
    #endif

    #if EXTENSIVE_HEAP_PROFILING
    gc_dump_alloc_table();
    #endif

    return ret_ptr;
}

/*
void *gc_alloc(mp_uint_t n_bytes) {
    return _gc_alloc(n_bytes, false);
}

void *gc_alloc_with_finaliser(mp_uint_t n_bytes) {
    return _gc_alloc(n_bytes, true);
}
*/

// force the freeing of a piece of memory
// TODO: freeing here does not call finaliser
void gc_free(void *ptr) {
    GC_ENTER();
    if (MP_STATE_MEM(gc_lock_depth) > 0) {
        // TODO how to deal with this error?
        GC_EXIT();
        return;
    }

    DEBUG_printf("gc_free(%p)\n", ptr);

    if (ptr == NULL) {
        GC_EXIT();
    } else {
        // get the GC block number corresponding to this pointer
        assert(VERIFY_PTR(ptr));
        size_t block = BLOCK_FROM_PTR(ptr);
        assert(ATB_GET_KIND(block) == AT_HEAD);

        #if MICROPY_ENABLE_FINALISER
        FTB_CLEAR(block);
        #endif

        // set the last_free pointer to this block if it's earlier in the heap
        if (block / BLOCKS_PER_ATB < MP_STATE_MEM(gc_last_free_atb_index)) {
            MP_STATE_MEM(gc_last_free_atb_index) = block / BLOCKS_PER_ATB;
        }

        // free head and all of its tail blocks
        do {
            ATB_ANY_TO_FREE(block);
            block += 1;
        } while (ATB_GET_KIND(block) == AT_TAIL);

        GC_EXIT();

        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table();
        #endif
    }
}

size_t gc_nbytes(const void *ptr) {
    GC_ENTER();
    if (VERIFY_PTR(ptr)) {
        size_t block = BLOCK_FROM_PTR(ptr);
        if (ATB_GET_KIND(block) == AT_HEAD) {
            // work out number of consecutive blocks in the chain starting with this on
            size_t n_blocks = 0;
            do {
                n_blocks += 1;
            } while (ATB_GET_KIND(block + n_blocks) == AT_TAIL);
            GC_EXIT();
            return n_blocks * BYTES_PER_BLOCK;
        }
    }

    // invalid pointer
    GC_EXIT();
    return 0;
}

#if 0
// old, simple realloc that didn't expand memory in place
void *gc_realloc(void *ptr, mp_uint_t n_bytes) {
    mp_uint_t n_existing = gc_nbytes(ptr);
    if (n_bytes <= n_existing) {
        return ptr;
    } else {
        bool has_finaliser;
        if (ptr == NULL) {
            has_finaliser = false;
        } else {
            #if MICROPY_ENABLE_FINALISER
            has_finaliser = FTB_GET(BLOCK_FROM_PTR((mp_uint_t)ptr));
            #else
            has_finaliser = false;
            #endif
        }
        void *ptr2 = gc_alloc(n_bytes, has_finaliser);
        if (ptr2 == NULL) {
            return ptr2;
        }
        memcpy(ptr2, ptr, n_existing);
        gc_free(ptr);
        return ptr2;
    }
}

#else // Alternative gc_realloc impl

void *gc_realloc(void *ptr_in, size_t n_bytes, bool allow_move) {
    // check for pure allocation
    if (ptr_in == NULL) {
        return gc_alloc(n_bytes, false);
    }

    // check for pure free
    if (n_bytes == 0) {
        gc_free(ptr_in);
        return NULL;
    }

    void *ptr = ptr_in;

    GC_ENTER();

    if (MP_STATE_MEM(gc_lock_depth) > 0) {
        GC_EXIT();
        return NULL;
    }

    // get the GC block number corresponding to this pointer
    assert(VERIFY_PTR(ptr));
    size_t block = BLOCK_FROM_PTR(ptr);
    assert(ATB_GET_KIND(block) == AT_HEAD);

    // compute number of new blocks that are requested
    size_t new_blocks = (n_bytes + BYTES_PER_BLOCK - 1) / BYTES_PER_BLOCK;

    // Get the total number of consecutive blocks that are already allocated to
    // this chunk of memory, and then count the number of free blocks following
    // it.  Stop if we reach the end of the heap, or if we find enough extra
    // free blocks to satisfy the realloc.  Note that we need to compute the
    // total size of the existing memory chunk so we can correctly and
    // efficiently shrink it (see below for shrinking code).
    size_t n_free = 0;
    size_t n_blocks = 1; // counting HEAD block
    size_t max_block = MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB;
    for (size_t bl = block + n_blocks; bl < max_block; bl++) {
        byte block_type = ATB_GET_KIND(bl);
        if (block_type == AT_TAIL) {
            n_blocks++;
            continue;
        }
        if (block_type == AT_FREE) {
            n_free++;
            if (n_blocks + n_free >= new_blocks) {
                // stop as soon as we find enough blocks for n_bytes
                break;
            }
            continue;
        }
        break;
    }

    // return original ptr if it already has the requested number of blocks
    if (new_blocks == n_blocks) {
        GC_EXIT();
        return ptr_in;
    }

    // check if we can shrink the allocated area
    if (new_blocks < n_blocks) {
        // free unneeded tail blocks
        for (size_t bl = block + new_blocks, count = n_blocks - new_blocks; count > 0; bl++, count--) {
            ATB_ANY_TO_FREE(bl);
        }

        // set the last_free pointer to end of this block if it's earlier in the heap
        if ((block + new_blocks) / BLOCKS_PER_ATB < MP_STATE_MEM(gc_last_free_atb_index)) {
            MP_STATE_MEM(gc_last_free_atb_index) = (block + new_blocks) / BLOCKS_PER_ATB;
        }

        GC_EXIT();

        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table();
        #endif

        return ptr_in;
    }

    // check if we can expand in place
    if (new_blocks <= n_blocks + n_free) {
        // mark few more blocks as used tail
        for (size_t bl = block + n_blocks; bl < block + new_blocks; bl++) {
            assert(ATB_GET_KIND(bl) == AT_FREE);
            ATB_FREE_TO_TAIL(bl);
        }

        GC_EXIT();

        #if MICROPY_GC_CONSERVATIVE_CLEAR
        // be conservative and zero out all the newly allocated blocks
        memset((byte *)ptr_in + n_blocks * BYTES_PER_BLOCK, 0, (new_blocks - n_blocks) * BYTES_PER_BLOCK);
        #else
        // zero out the additional bytes of the newly allocated blocks (see comment above in gc_alloc)
        memset((byte *)ptr_in + n_bytes, 0, new_blocks * BYTES_PER_BLOCK - n_bytes);
        #endif

        #if EXTENSIVE_HEAP_PROFILING
        gc_dump_alloc_table();
        #endif

        return ptr_in;
    }

    #if MICROPY_ENABLE_FINALISER
    bool ftb_state = FTB_GET(block);
    #else
    bool ftb_state = false;
    #endif

    GC_EXIT();

    if (!allow_move) {
        // not allowed to move memory block so return failure
        return NULL;
    }

    // can't resize inplace; try to find a new contiguous chain
    void *ptr_out = gc_alloc(n_bytes, ftb_state);

    // check that the alloc succeeded
    if (ptr_out == NULL) {
        return NULL;
    }

    DEBUG_printf("gc_realloc(%p -> %p)\n", ptr_in, ptr_out);
    memcpy(ptr_out, ptr_in, n_blocks * BYTES_PER_BLOCK);
    gc_free(ptr_in);
    return ptr_out;
}
#endif // Alternative gc_realloc impl

void gc_dump_info(void) {
    gc_info_t info;
    gc_info(&info);
    mp_printf(&mp_plat_print, "GC: total: %u, used: %u, free: %u\n",
        (uint)info.total, (uint)info.used, (uint)info.free);
    mp_printf(&mp_plat_print, " No. of 1-blocks: %u, 2-blocks: %u, max blk sz: %u, max free sz: %u\n",
        (uint)info.num_1block, (uint)info.num_2block, (uint)info.max_block, (uint)info.max_free);
}

void gc_dump_alloc_table(void) {
    GC_ENTER();
    static const size_t DUMP_BYTES_PER_LINE = 64;
    #if !EXTENSIVE_HEAP_PROFILING
    // When comparing heap output we don't want to print the starting
    // pointer of the heap because it changes from run to run.
    mp_printf(&mp_plat_print, "GC memory layout; from %p:", MP_STATE_MEM(gc_pool_start));
    #endif
    for (size_t bl = 0; bl < MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB; bl++) {
        if (bl % DUMP_BYTES_PER_LINE == 0) {
            // a new line of blocks
            {
                // check if this line contains only free blocks
                size_t bl2 = bl;
                while (bl2 < MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB && ATB_GET_KIND(bl2) == AT_FREE) {
                    bl2++;
                }
                if (bl2 - bl >= 2 * DUMP_BYTES_PER_LINE) {
                    // there are at least 2 lines containing only free blocks, so abbreviate their printing
                    mp_printf(&mp_plat_print, "\n       (%u lines all free)", (uint)(bl2 - bl) / DUMP_BYTES_PER_LINE);
                    bl = bl2 & (~(DUMP_BYTES_PER_LINE - 1));
                    if (bl >= MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB) {
                        // got to end of heap
                        break;
                    }
                }
            }
            // print header for new line of blocks
            // (the cast to uint32_t is for 16-bit ports)
            // mp_printf(&mp_plat_print, "\n%05x: ", (uint)(PTR_FROM_BLOCK(bl) & (uint32_t)0xfffff));
            mp_printf(&mp_plat_print, "\n%05x: ", (uint)((bl * BYTES_PER_BLOCK) & (uint32_t)0xfffff));
        }
        int c = ' ';
        switch (ATB_GET_KIND(bl)) {
            case AT_FREE:
                c = '.';
                break;
            /* this prints out if the object is reachable from BSS or STACK (for unix only)
            case AT_HEAD: {
                c = 'h';
                void **ptrs = (void**)(void*)&mp_state_ctx;
                mp_uint_t len = offsetof(mp_state_ctx_t, vm.stack_top) / sizeof(mp_uint_t);
                for (mp_uint_t i = 0; i < len; i++) {
                    mp_uint_t ptr = (mp_uint_t)ptrs[i];
                    if (VERIFY_PTR(ptr) && BLOCK_FROM_PTR(ptr) == bl) {
                        c = 'B';
                        break;
                    }
                }
                if (c == 'h') {
                    ptrs = (void**)&c;
                    len = ((mp_uint_t)MP_STATE_THREAD(stack_top) - (mp_uint_t)&c) / sizeof(mp_uint_t);
                    for (mp_uint_t i = 0; i < len; i++) {
                        mp_uint_t ptr = (mp_uint_t)ptrs[i];
                        if (VERIFY_PTR(ptr) && BLOCK_FROM_PTR(ptr) == bl) {
                            c = 'S';
                            break;
                        }
                    }
                }
                break;
            }
            */
            /* this prints the uPy object type of the head block */
            case AT_HEAD: {
                void **ptr = (void **)(MP_STATE_MEM(gc_pool_start) + bl * BYTES_PER_BLOCK);
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
                    #if 0
                    // This code prints "Q" for qstr-pool data, and "q" for qstr-str
                    // data.  It can be useful to see how qstrs are being allocated,
                    // but is disabled by default because it is very slow.
                    for (qstr_pool_t *pool = MP_STATE_VM(last_pool); c == 'h' && pool != NULL; pool = pool->prev) {
                        if ((qstr_pool_t *)ptr == pool) {
                            c = 'Q';
                            break;
                        }
                        for (const byte **q = pool->qstrs, **q_top = pool->qstrs + pool->len; q < q_top; q++) {
                            if ((const byte *)ptr == *q) {
                                c = 'q';
                                break;
                            }
                        }
                    }
                    #endif
                }
                break;
            }
            case AT_TAIL:
                c = '=';
                break;
            case AT_MARK:
                c = 'm';
                break;
        }
        mp_printf(&mp_plat_print, "%c", c);
    }
    mp_print_str(&mp_plat_print, "\n");
    GC_EXIT();
}

#if 0
// For testing the GC functions
void gc_test(void) {
    mp_uint_t len = 500;
    mp_uint_t *heap = malloc(len);
    gc_init(heap, heap + len / sizeof(mp_uint_t));
    void *ptrs[100];
    {
        mp_uint_t **p = gc_alloc(16, false);
        p[0] = gc_alloc(64, false);
        p[1] = gc_alloc(1, false);
        p[2] = gc_alloc(1, false);
        p[3] = gc_alloc(1, false);
        mp_uint_t ***p2 = gc_alloc(16, false);
        p2[0] = p;
        p2[1] = p;
        ptrs[0] = p2;
    }
    for (int i = 0; i < 25; i += 2) {
        mp_uint_t *p = gc_alloc(i, false);
        printf("p=%p\n", p);
        if (i & 3) {
            // ptrs[i] = p;
        }
    }

    printf("Before GC:\n");
    gc_dump_alloc_table();
    printf("Starting GC...\n");
    gc_collect_start();
    gc_collect_root(ptrs, sizeof(ptrs) / sizeof(void *));
    gc_collect_end();
    printf("After GC:\n");
    gc_dump_alloc_table();
}
#endif

#endif // MICROPY_ENABLE_GC
