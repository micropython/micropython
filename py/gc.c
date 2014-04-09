#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "mpconfig.h"
#include "misc.h"
#include "gc.h"

#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"

#if MICROPY_ENABLE_GC

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

#define WORDS_PER_BLOCK (4)
#define BYTES_PER_BLOCK (WORDS_PER_BLOCK * BYTES_PER_WORD)
#define STACK_SIZE (64) // tunable; minimum is 1

STATIC byte *gc_alloc_table_start;
STATIC machine_uint_t gc_alloc_table_byte_len;
#if MICROPY_ENABLE_FINALISER
STATIC byte *gc_finaliser_table_start;
#endif
STATIC machine_uint_t *gc_pool_start;
STATIC machine_uint_t *gc_pool_end;

STATIC int gc_stack_overflow;
STATIC machine_uint_t gc_stack[STACK_SIZE];
STATIC machine_uint_t *gc_sp;
STATIC machine_uint_t gc_lock_depth;

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
#define ATB_GET_KIND(block) ((gc_alloc_table_start[(block) / BLOCKS_PER_ATB] >> BLOCK_SHIFT(block)) & 3)
#define ATB_ANY_TO_FREE(block) do { gc_alloc_table_start[(block) / BLOCKS_PER_ATB] &= (~(AT_MARK << BLOCK_SHIFT(block))); } while (0)
#define ATB_FREE_TO_HEAD(block) do { gc_alloc_table_start[(block) / BLOCKS_PER_ATB] |= (AT_HEAD << BLOCK_SHIFT(block)); } while (0)
#define ATB_FREE_TO_TAIL(block) do { gc_alloc_table_start[(block) / BLOCKS_PER_ATB] |= (AT_TAIL << BLOCK_SHIFT(block)); } while (0)
#define ATB_HEAD_TO_MARK(block) do { gc_alloc_table_start[(block) / BLOCKS_PER_ATB] |= (AT_MARK << BLOCK_SHIFT(block)); } while (0)
#define ATB_MARK_TO_HEAD(block) do { gc_alloc_table_start[(block) / BLOCKS_PER_ATB] &= (~(AT_TAIL << BLOCK_SHIFT(block))); } while (0)

#define BLOCK_FROM_PTR(ptr) (((ptr) - (machine_uint_t)gc_pool_start) / BYTES_PER_BLOCK)
#define PTR_FROM_BLOCK(block) (((block) * BYTES_PER_BLOCK + (machine_uint_t)gc_pool_start))
#define ATB_FROM_BLOCK(bl) ((bl) / BLOCKS_PER_ATB)

#if MICROPY_ENABLE_FINALISER
// FTB = finaliser table byte
// if set, then the corresponding block may have a finaliser

#define BLOCKS_PER_FTB (8)

#define FTB_GET(block) ((gc_finaliser_table_start[(block) / BLOCKS_PER_FTB] >> ((block) & 7)) & 1)
#define FTB_SET(block) do { gc_finaliser_table_start[(block) / BLOCKS_PER_FTB] |= (1 << ((block) & 7)); } while (0)
#define FTB_CLEAR(block) do { gc_finaliser_table_start[(block) / BLOCKS_PER_FTB] &= (~(1 << ((block) & 7))); } while (0)
#endif

// TODO waste less memory; currently requires that all entries in alloc_table have a corresponding block in pool
void gc_init(void *start, void *end) {
    // align end pointer on block boundary
    end = (void*)((machine_uint_t)end & (~(BYTES_PER_BLOCK - 1)));
    DEBUG_printf("Initializing GC heap: %p..%p = %ld bytes\n", start, end, end - start);

    // calculate parameters for GC (T=total, A=alloc table, F=finaliser table, P=pool; all in bytes):
    // T = A + F + P
    //     F = A * BLOCKS_PER_ATB / BLOCKS_PER_FTB
    //     P = A * BLOCKS_PER_ATB * BYTES_PER_BLOCK
    // => T = A * (1 + BLOCKS_PER_ATB / BLOCKS_PER_FTB + BLOCKS_PER_ATB * BYTES_PER_BLOCK)
    machine_uint_t total_byte_len = end - start;
#if MICROPY_ENABLE_FINALISER
    gc_alloc_table_byte_len = total_byte_len * BITS_PER_BYTE / (BITS_PER_BYTE + BITS_PER_BYTE * BLOCKS_PER_ATB / BLOCKS_PER_FTB + BITS_PER_BYTE * BLOCKS_PER_ATB * BYTES_PER_BLOCK);
#else
    gc_alloc_table_byte_len = total_byte_len / (1 + BITS_PER_BYTE / 2 * BYTES_PER_BLOCK);
#endif


    gc_alloc_table_start = (byte*)start;

#if MICROPY_ENABLE_FINALISER
    machine_uint_t gc_finaliser_table_byte_len = (gc_alloc_table_byte_len * BLOCKS_PER_ATB) / BLOCKS_PER_FTB;
    gc_finaliser_table_start = gc_alloc_table_start + gc_alloc_table_byte_len;
#endif

    machine_uint_t gc_pool_block_len = gc_alloc_table_byte_len * BLOCKS_PER_ATB;
    gc_pool_start = end - gc_pool_block_len * BYTES_PER_BLOCK;
    gc_pool_end = end;

    // clear ATBs
    memset(gc_alloc_table_start, 0, gc_alloc_table_byte_len);

#if MICROPY_ENABLE_FINALISER
    // clear FTBs
    memset(gc_finaliser_table_start, 0, gc_finaliser_table_byte_len);
#endif

    // allocate first block because gc_pool_start points there and it will never
    // be freed, so allocating 1 block with null pointers will minimise memory loss
    ATB_FREE_TO_HEAD(0);
    for (int i = 0; i < WORDS_PER_BLOCK; i++) {
        gc_pool_start[i] = 0;
    }

    // unlock the GC
    gc_lock_depth = 0;

    DEBUG_printf("GC layout:\n");
    DEBUG_printf("  alloc table at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", gc_alloc_table_start, gc_alloc_table_byte_len, gc_alloc_table_byte_len * BLOCKS_PER_ATB);
#if MICROPY_ENABLE_FINALISER
    DEBUG_printf("  finaliser table at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", gc_finaliser_table_start, gc_finaliser_table_byte_len, gc_finaliser_table_byte_len * BLOCKS_PER_FTB);
#endif
    DEBUG_printf("  pool at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", gc_pool_start, gc_pool_block_len * BYTES_PER_BLOCK, gc_pool_block_len);
}

void gc_lock(void) {
    gc_lock_depth++;
}

void gc_unlock(void) {
    gc_lock_depth--;
}

#define VERIFY_PTR(ptr) ( \
        (ptr & (BYTES_PER_BLOCK - 1)) == 0          /* must be aligned on a block */ \
        && ptr >= (machine_uint_t)gc_pool_start     /* must be above start of pool */ \
        && ptr < (machine_uint_t)gc_pool_end        /* must be below end of pool */ \
    )

#define VERIFY_MARK_AND_PUSH(ptr) \
    do { \
        if (VERIFY_PTR(ptr)) { \
            machine_uint_t _block = BLOCK_FROM_PTR(ptr); \
            if (ATB_GET_KIND(_block) == AT_HEAD) { \
                /* an unmarked head, mark it, and push it on gc stack */ \
                ATB_HEAD_TO_MARK(_block); \
                if (gc_sp < &gc_stack[STACK_SIZE]) { \
                    *gc_sp++ = _block; \
                } else { \
                    gc_stack_overflow = 1; \
                } \
            } \
        } \
    } while (0)

STATIC void gc_drain_stack(void) {
    while (gc_sp > gc_stack) {
        // pop the next block off the stack
        machine_uint_t block = *--gc_sp;

        // work out number of consecutive blocks in the chain starting with this one
        machine_uint_t n_blocks = 0;
        do {
            n_blocks += 1;
        } while (ATB_GET_KIND(block + n_blocks) == AT_TAIL);

        // check this block's children
        machine_uint_t *scan = (machine_uint_t*)PTR_FROM_BLOCK(block);
        for (machine_uint_t i = n_blocks * WORDS_PER_BLOCK; i > 0; i--, scan++) {
            machine_uint_t ptr2 = *scan;
            VERIFY_MARK_AND_PUSH(ptr2);
        }
    }
}

STATIC void gc_deal_with_stack_overflow(void) {
    while (gc_stack_overflow) {
        gc_stack_overflow = 0;
        gc_sp = gc_stack;

        // scan entire memory looking for blocks which have been marked but not their children
        for (machine_uint_t block = 0; block < gc_alloc_table_byte_len * BLOCKS_PER_ATB; block++) {
            // trace (again) if mark bit set
            if (ATB_GET_KIND(block) == AT_MARK) {
                *gc_sp++ = block;
                gc_drain_stack();
            }
        }
    }
}

STATIC void gc_sweep(void) {
    // free unmarked heads and their tails
    int free_tail = 0;
    for (machine_uint_t block = 0; block < gc_alloc_table_byte_len * BLOCKS_PER_ATB; block++) {
        switch (ATB_GET_KIND(block)) {
            case AT_HEAD:
#if MICROPY_ENABLE_FINALISER
                if (FTB_GET(block)) {
                    mp_obj_t obj = (mp_obj_t)PTR_FROM_BLOCK(block);
                    if (((mp_obj_base_t*)obj)->type != MP_OBJ_NULL) {
                        // if the object has a type then see if it has a __del__ method
                        mp_obj_t dest[2];
                        mp_load_method_maybe(obj, MP_QSTR___del__, dest);
                        if (dest[0] != MP_OBJ_NULL) {
                            // load_method returned a method
                            mp_call_method_n_kw(0, 0, dest);
                        }
                    }
                    // clear finaliser flag
                    FTB_CLEAR(block);
                }
#endif
                free_tail = 1;
                // fall through to free the head

            case AT_TAIL:
                if (free_tail) {
                    ATB_ANY_TO_FREE(block);
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
    gc_lock();
    gc_stack_overflow = 0;
    gc_sp = gc_stack;
}

void gc_collect_root(void **ptrs, machine_uint_t len) {
    for (machine_uint_t i = 0; i < len; i++) {
        machine_uint_t ptr = (machine_uint_t)ptrs[i];
        VERIFY_MARK_AND_PUSH(ptr);
        gc_drain_stack();
    }
}

void gc_collect_end(void) {
    gc_deal_with_stack_overflow();
    gc_sweep();
    gc_unlock();
}

void gc_info(gc_info_t *info) {
    info->total = (gc_pool_end - gc_pool_start) * sizeof(machine_uint_t);
    info->used = 0;
    info->free = 0;
    info->num_1block = 0;
    info->num_2block = 0;
    info->max_block = 0;
    for (machine_uint_t block = 0, len = 0; block < gc_alloc_table_byte_len * BLOCKS_PER_ATB; block++) {
        machine_uint_t kind = ATB_GET_KIND(block);
        if (kind == AT_FREE || kind == AT_HEAD) {
            if (len == 1) {
                info->num_1block += 1;
            } else if (len == 2) {
                info->num_2block += 1;
            }
            if (len > info->max_block) {
                info->max_block = len;
            }
        }
        switch (kind) {
            case AT_FREE:
                info->free += 1;
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
    }

    info->used *= BYTES_PER_BLOCK;
    info->free *= BYTES_PER_BLOCK;
}

void *gc_alloc(machine_uint_t n_bytes, bool has_finaliser) {
    machine_uint_t n_blocks = ((n_bytes + BYTES_PER_BLOCK - 1) & (~(BYTES_PER_BLOCK - 1))) / BYTES_PER_BLOCK;
    DEBUG_printf("gc_alloc(" UINT_FMT " bytes -> " UINT_FMT " blocks)\n", n_bytes, n_blocks);

    // check if GC is locked
    if (gc_lock_depth > 0) {
        return NULL;
    }

    // check for 0 allocation
    if (n_blocks == 0) {
        return NULL;
    }

    machine_uint_t i;
    machine_uint_t end_block;
    machine_uint_t start_block;
    machine_uint_t n_free = 0;
    int collected = 0;
    for (;;) {

        // look for a run of n_blocks available blocks
        for (i = 0; i < gc_alloc_table_byte_len; i++) {
            byte a = gc_alloc_table_start[i];
            if (ATB_0_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 0; goto found; } } else { n_free = 0; }
            if (ATB_1_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 1; goto found; } } else { n_free = 0; }
            if (ATB_2_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 2; goto found; } } else { n_free = 0; }
            if (ATB_3_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * BLOCKS_PER_ATB + 3; goto found; } } else { n_free = 0; }
        }

        // nothing found!
        if (collected) {
            return NULL;
        }
        DEBUG_printf("gc_alloc(" UINT_FMT "): no free mem, triggering GC\n", n_bytes);
        gc_collect();
        collected = 1;
    }

    // found, ending at block i inclusive
found:
    // get starting and end blocks, both inclusive
    end_block = i;
    start_block = i - n_free + 1;

    // mark first block as used head
    ATB_FREE_TO_HEAD(start_block);

    // mark rest of blocks as used tail
    // TODO for a run of many blocks can make this more efficient
    for (machine_uint_t bl = start_block + 1; bl <= end_block; bl++) {
        ATB_FREE_TO_TAIL(bl);
    }

    // get pointer to first block
    void *ret_ptr = (void*)(gc_pool_start + start_block * WORDS_PER_BLOCK);

#if MICROPY_ENABLE_FINALISER
    if (has_finaliser) {
        // clear type pointer in case it is never set
        ((mp_obj_base_t*)ret_ptr)->type = MP_OBJ_NULL;
        // set mp_obj flag only if it has a finaliser
        FTB_SET(start_block);
    }
#endif

    return ret_ptr;
}

/*
void *gc_alloc(machine_uint_t n_bytes) {
    return _gc_alloc(n_bytes, false);
}

void *gc_alloc_with_finaliser(machine_uint_t n_bytes) {
    return _gc_alloc(n_bytes, true);
}
*/

// force the freeing of a piece of memory
void gc_free(void *ptr_in) {
    if (gc_lock_depth > 0) {
        // TODO how to deal with this error?
        return;
    }

    machine_uint_t ptr = (machine_uint_t)ptr_in;

    if (VERIFY_PTR(ptr)) {
        machine_uint_t block = BLOCK_FROM_PTR(ptr);
        if (ATB_GET_KIND(block) == AT_HEAD) {
            // free head and all of its tail blocks
            do {
                ATB_ANY_TO_FREE(block);
                block += 1;
            } while (ATB_GET_KIND(block) == AT_TAIL);
        }
    }
}

machine_uint_t gc_nbytes(void *ptr_in) {
    machine_uint_t ptr = (machine_uint_t)ptr_in;

    if (VERIFY_PTR(ptr)) {
        machine_uint_t block = BLOCK_FROM_PTR(ptr);
        if (ATB_GET_KIND(block) == AT_HEAD) {
            // work out number of consecutive blocks in the chain starting with this on
            machine_uint_t n_blocks = 0;
            do {
                n_blocks += 1;
            } while (ATB_GET_KIND(block + n_blocks) == AT_TAIL);
            return n_blocks * BYTES_PER_BLOCK;
        }
    }

    // invalid pointer
    return 0;
}

#if 0
// old, simple realloc that didn't expand memory in place
void *gc_realloc(void *ptr, machine_uint_t n_bytes) {
    machine_uint_t n_existing = gc_nbytes(ptr);
    if (n_bytes <= n_existing) {
        return ptr;
    } else {
        // TODO check if we can grow inplace
        void *ptr2 = gc_alloc(n_bytes);
        if (ptr2 == NULL) {
            return ptr2;
        }
        memcpy(ptr2, ptr, n_existing);
        gc_free(ptr);
        return ptr2;
    }
}
#endif

void *gc_realloc(void *ptr_in, machine_uint_t n_bytes) {
    if (gc_lock_depth > 0) {
        return NULL;
    }

    void *ptr_out = NULL;
    machine_uint_t block = 0;
    machine_uint_t ptr = (machine_uint_t)ptr_in;

    if (ptr_in == NULL) {
        return gc_alloc(n_bytes, false);
    }

    if (VERIFY_PTR(ptr)                         // verify pointer
        && (block = BLOCK_FROM_PTR(ptr))        // get first block
        && ATB_GET_KIND(block) == AT_HEAD) {    // make sure it's a HEAD block

        byte block_type;
        machine_uint_t n_free   = 0;
        machine_uint_t n_blocks = 1; // counting HEAD block
        machine_uint_t max_block = gc_alloc_table_byte_len * BLOCKS_PER_ATB;

        // get the number of consecutive tail blocks and
        // the number of free blocks after last tail block
        // stop if we reach (or are at) end of heap
        while ((block + n_blocks + n_free) < max_block
                // stop as soon as we find enough blocks for n_bytes
                && (n_bytes > ((n_blocks+n_free) * BYTES_PER_BLOCK))
                // stop if block is HEAD
                && (block_type = ATB_GET_KIND(block + n_blocks + n_free)) != AT_HEAD) {
            switch (block_type) {
                case AT_FREE: n_free++; break;
                case AT_TAIL: n_blocks++; break;
                default: break;
            }
        }
        // number of allocated bytes
        machine_uint_t n_existing = n_blocks * BYTES_PER_BLOCK;

        // check if realloc'ing to a smaller size
        if (n_bytes <= n_existing) {
            ptr_out = ptr_in;
            // free unneeded tail blocks
            for (machine_uint_t bl = block + n_blocks; ATB_GET_KIND(bl) == AT_TAIL; bl++) {
                ATB_ANY_TO_FREE(bl);
            }

        // check if we can expand in place
        } else if (n_bytes <= (n_existing + (n_free * BYTES_PER_BLOCK))) {
            // number of blocks needed to expand +1 if there's a remainder
            machine_uint_t n_diff = ( n_bytes - n_existing)/BYTES_PER_BLOCK+
                                    ((n_bytes - n_existing)%BYTES_PER_BLOCK!=0);

            DEBUG_printf("gc_realloc: expanding " UINT_FMT " blocks (" UINT_FMT " bytes) to " UINT_FMT " blocks (" UINT_FMT " bytes)\n",
                    n_existing/BYTES_PER_BLOCK, n_existing, n_existing/BYTES_PER_BLOCK+n_diff, n_existing + n_diff*BYTES_PER_BLOCK);

            // mark rest of blocks as used tail
            for (machine_uint_t bl = block + n_blocks; bl < (block + n_blocks + n_diff); bl++) {
                ATB_FREE_TO_TAIL(bl);
            }
            ptr_out = ptr_in;

        // try to find a new contiguous chain
        } else if ((ptr_out = gc_alloc(n_bytes,
#if MICROPY_ENABLE_FINALISER
                        FTB_GET(block)
#else
                        false
#endif
                        )) != NULL) {
            DEBUG_printf("gc_realloc: allocating new block\n");
            memcpy(ptr_out, ptr_in, n_existing);
            gc_free(ptr_in);
        }
    }

    return ptr_out;
}

void gc_dump_info() {
    gc_info_t info;
    gc_info(&info);
    printf("GC: total: " UINT_FMT ", used: " UINT_FMT ", free: " UINT_FMT "\n", info.total, info.used, info.free);
    printf(" No. of 1-blocks: " UINT_FMT ", 2-blocks: " UINT_FMT ", max blk sz: " UINT_FMT "\n",
           info.num_1block, info.num_2block, info.max_block);
}

void gc_dump_alloc_table(void) {
    printf("GC memory layout:");
    for (machine_uint_t bl = 0; bl < gc_alloc_table_byte_len * BLOCKS_PER_ATB; bl++) {
        if (bl % 64 == 0) {
            printf("\n%04x: ", (uint)bl);
        }
        int c = ' ';
        switch (ATB_GET_KIND(bl)) {
            case AT_FREE: c = '.'; break;
            case AT_HEAD: c = 'h'; break;
            case AT_TAIL: c = 't'; break;
            case AT_MARK: c = 'm'; break;
        }
        printf("%c", c);
    }
    printf("\n");
}

#if DEBUG_PRINT
void gc_test(void) {
    machine_uint_t len = 500;
    machine_uint_t *heap = malloc(len);
    gc_init(heap, heap + len / sizeof(machine_uint_t));
    void *ptrs[100];
    {
        machine_uint_t **p = gc_alloc(16, false);
        p[0] = gc_alloc(64, false);
        p[1] = gc_alloc(1, false);
        p[2] = gc_alloc(1, false);
        p[3] = gc_alloc(1, false);
        machine_uint_t ***p2 = gc_alloc(16, false);
        p2[0] = p;
        p2[1] = p;
        ptrs[0] = p2;
    }
    for (int i = 0; i < 25; i+=2) {
        machine_uint_t *p = gc_alloc(i, false);
        printf("p=%p\n", p);
        if (i & 3) {
            //ptrs[i] = p;
        }
    }

    printf("Before GC:\n");
    gc_dump_alloc_table();
    printf("Starting GC...\n");
    gc_collect_start();
    gc_collect_root(ptrs, sizeof(ptrs) / sizeof(void*));
    gc_collect_end();
    printf("After GC:\n");
    gc_dump_alloc_table();
}
#endif

#endif // MICROPY_ENABLE_GC
