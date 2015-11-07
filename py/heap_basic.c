
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "py/mpstate.h"
#include "py/heap_basic.h"
#include "py/obj.h"
#include "py/runtime.h"

mem_state_mem_t mem_state_mem;

#if 0 // print debugging info
#define DEBUG_PRINT (1)
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_PRINT (0)
#define DEBUG_printf(...) (void)0
#endif

// make this 1 to dump the heap each time it changes
#define EXTENSIVE_HEAP_PROFILING (0)


#define WORDS_PER_BLOCK (4)
#define BYTES_PER_BLOCK (WORDS_PER_BLOCK * BYTES_PER_WORD)

#define AT_FREE (0)
#define AT_HEAD (1)
#define AT_TAIL (2)
#define AT_MARK (3)

#define ATB_MASK_0 (0x03)
#define ATB_MASK_1 (0x0c)
#define ATB_MASK_2 (0x30)
#define ATB_MASK_3 (0xc0)

#define ATB_0_IS_FREE(a) (((a) & ATB_MASK_0) == 0)
#define ATB_1_IS_FREE(a) (((a) & ATB_MASK_1) == 0)
#define ATB_2_IS_FREE(a) (((a) & ATB_MASK_2) == 0)
#define ATB_3_IS_FREE(a) (((a) & ATB_MASK_3) == 0)

/**
 *  /brief      ATB Access Macros
 *
 *              These macros are designed to mark "blocks" in the allocation table
 *              as used, free or marked (during garbage collection).
 *
 *              See the documentation for the Allocation Table for more information.
 */
#define BLOCK_SHIFT(block) (2 * ((block) & (MEM_BLOCKS_PER_ATB - 1)))
#define ATB_GET_KIND(block) ((MEM_STATE_MEM(gc_alloc_table_start)[(block) / MEM_BLOCKS_PER_ATB] >> BLOCK_SHIFT(block)) & 3)
#define ATB_ANY_TO_FREE(block) do { MEM_STATE_MEM(gc_alloc_table_start)[(block) / MEM_BLOCKS_PER_ATB] &= (~(AT_MARK << BLOCK_SHIFT(block))); } while (0)
#define ATB_FREE_TO_HEAD(block) do { MEM_STATE_MEM(gc_alloc_table_start)[(block) / MEM_BLOCKS_PER_ATB] |= (AT_HEAD << BLOCK_SHIFT(block)); } while (0)
#define ATB_FREE_TO_TAIL(block) do { MEM_STATE_MEM(gc_alloc_table_start)[(block) / MEM_BLOCKS_PER_ATB] |= (AT_TAIL << BLOCK_SHIFT(block)); } while (0)
#define ATB_HEAD_TO_MARK(block) do { MEM_STATE_MEM(gc_alloc_table_start)[(block) / MEM_BLOCKS_PER_ATB] |= (AT_MARK << BLOCK_SHIFT(block)); } while (0)
#define ATB_MARK_TO_HEAD(block) do { MEM_STATE_MEM(gc_alloc_table_start)[(block) / MEM_BLOCKS_PER_ATB] &= (~(AT_TAIL << BLOCK_SHIFT(block))); } while (0)

#define BLOCK_FROM_PTR(ptr) (((ptr) - (mp_uint_t)MEM_STATE_MEM(gc_pool_start)) / BYTES_PER_BLOCK)
#define PTR_FROM_BLOCK(block) (((block) * BYTES_PER_BLOCK + (mp_uint_t)MEM_STATE_MEM(gc_pool_start)))
#define ATB_FROM_BLOCK(bl) ((bl) / MEM_BLOCKS_PER_ATB)


/*----------------------------------------------------------------------------*/
/* return 1 if the pointer falls within the memory pool                       */
#define VERIFY_PTR(ptr) ( \
        (ptr & (BYTES_PER_BLOCK - 1)) == 0          /* must be aligned on a block */ \
        && ptr >= (mp_uint_t)MEM_STATE_MEM(gc_pool_start)     /* must be above start of pool */ \
        && ptr < (mp_uint_t)MEM_STATE_MEM(gc_pool_end)        /* must be below end of pool */ \
    )



// TODO waste less memory; currently requires that all entries in alloc_table have a corresponding block in pool
void heap_init(void *start, void *end) {
    // align end pointer on block boundary
    end = (void*)((mp_uint_t)end & (~(BYTES_PER_BLOCK - 1)));
    DEBUG_printf("Initializing GC heap: %p..%p = " UINT_FMT " bytes\n", start, end, (byte*)end - (byte*)start);

    // calculate parameters for GC (T=total, A=alloc table, F=finaliser table, P=pool; all in bytes):
    // T = A + F + P
    //     F = A * MEM_BLOCKS_PER_ATB / BLOCKS_PER_FTB
    //     P = A * MEM_BLOCKS_PER_ATB * BYTES_PER_BLOCK
    // => T = A * (1 + MEM_BLOCKS_PER_ATB / BLOCKS_PER_FTB + MEM_BLOCKS_PER_ATB * BYTES_PER_BLOCK)
    mp_uint_t total_byte_len = (byte*)end - (byte*)start;
    // TODO: this should be completely separated from the finalizer
#if MICROPY_ENABLE_FINALISER
    MEM_STATE_MEM(gc_alloc_table_byte_len) = total_byte_len * BITS_PER_BYTE / (BITS_PER_BYTE + BITS_PER_BYTE * MEM_BLOCKS_PER_ATB / BLOCKS_PER_FTB + BITS_PER_BYTE * MEM_BLOCKS_PER_ATB * BYTES_PER_BLOCK);
#else
    MEM_STATE_MEM(gc_alloc_table_byte_len) = total_byte_len / (1 + BITS_PER_BYTE / 2 * BYTES_PER_BLOCK);
#endif

    MEM_STATE_MEM(gc_alloc_table_start) = (byte*)start;


    mp_uint_t gc_pool_block_len = MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB;
    MEM_STATE_MEM(gc_pool_start) = (mp_uint_t*)((byte*)end - gc_pool_block_len * BYTES_PER_BLOCK);
    MEM_STATE_MEM(gc_pool_end) = (mp_uint_t*)end;

    // clear ATBs
    memset(MEM_STATE_MEM(gc_alloc_table_start), 0, MEM_STATE_MEM(gc_alloc_table_byte_len));

    // set last free ATB index to start of heap
    MEM_STATE_MEM(gc_last_free_atb_index) = 0;


    DEBUG_printf("GC layout:\n");
    DEBUG_printf("  alloc table at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", MEM_STATE_MEM(gc_alloc_table_start), MEM_STATE_MEM(gc_alloc_table_byte_len), MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB);
#if MICROPY_ENABLE_FINALISER
    DEBUG_printf("  finaliser table at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", MEM_STATE_MEM(gc_finaliser_table_start), gc_finaliser_table_byte_len, gc_finaliser_table_byte_len * BLOCKS_PER_FTB);
#endif
    DEBUG_printf("  pool at %p, length " UINT_FMT " bytes, " UINT_FMT " blocks\n", MEM_STATE_MEM(gc_pool_start), gc_pool_block_len * BYTES_PER_BLOCK, gc_pool_block_len);
}

mp_uint_t heap_first(){
    for (mp_uint_t block = 0; block < MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB; block++) {
        if(ATB_GET_KIND(block) == AT_MARK || ATB_GET_KIND(block) == AT_HEAD){return block;}
    }
    return MEM_BLOCK_ERROR;
}

mp_uint_t heap_sizeof(mp_uint_t block){
    mp_uint_t n_blocks = 0;
    do {
        n_blocks += 1;
    } while (ATB_GET_KIND(block + n_blocks) == AT_TAIL);
    return n_blocks * BYTES_PER_BLOCK;
}

/*----------------------------------------------------------------------------*/
/**
 *  get void pointer from block
 */
inline void *heap_void_p(mp_uint_t block){
    assert(VERIFY_PTR(PTR_FROM_BLOCK(block)));
    assert(ATB_GET_KIND(block) == AT_MARK || ATB_GET_KIND(block) == AT_HEAD);
    return (void *)PTR_FROM_BLOCK(block);
}

inline mp_uint_t heap_block(const void *ptr){
    return BLOCK_FROM_PTR((mp_uint_t) ptr);
}

/*----------------------------------------------------------------------------*/
/**
 *  get the next allocated block of memory
 */
mp_uint_t heap_next(mp_uint_t block){
    block++;
    for (; block < MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB; block++) {
        switch (ATB_GET_KIND(block)) {
            case AT_HEAD:
            case AT_MARK:
                return block;
        }
    }
    return MEM_BLOCK_ERROR;
}

inline bool heap_valid(mp_uint_t block){
    if(block >= MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB){return 0;}
    if((ATB_GET_KIND(block) != AT_HEAD) && (ATB_GET_KIND(block) != AT_MARK)){return 0;}
    return 1;
}

/*----------------------------------------------------------------------------*/
/**
 *  The following three functions are for setting, clearing and getting the
 *  "mark" used during garbage collection
 */
inline void heap_set_mark(mp_uint_t block){
    assert(ATB_GET_KIND(block) == AT_HEAD);
    ATB_HEAD_TO_MARK(block);
}

inline void heap_clear_mark(mp_uint_t block){
    assert(ATB_GET_KIND(block) == AT_MARK);
    ATB_MARK_TO_HEAD(block);
}

inline int8_t heap_get_mark(mp_uint_t block){
    assert(ATB_GET_KIND(block) == AT_MARK || ATB_GET_KIND(block) == AT_HEAD);
    return ATB_GET_KIND(block) == AT_MARK;
}

void heap_free(mp_uint_t block) {
    if(heap_valid(block)){
        if (ATB_GET_KIND(block) == AT_HEAD) {
            // set the last_free pointer to this block if it's earlier in the heap
           if (block / MEM_BLOCKS_PER_ATB < MEM_STATE_MEM(gc_last_free_atb_index)) {
                MEM_STATE_MEM(gc_last_free_atb_index) = block / MEM_BLOCKS_PER_ATB;
            }
            // free head and all of its tail blocks
            do {
                ATB_ANY_TO_FREE(block);
                block += 1;
            } while (ATB_GET_KIND(block) == AT_TAIL);
        } else {
            assert(!"bad free, ptr not at head");
        }
    } else if (block != MEM_BLOCK_ERROR) {
        assert(!"bad free, ptr not valid");
    }
}

mp_uint_t heap_alloc(mp_uint_t n_bytes) {
    mp_uint_t n_blocks = ((n_bytes + BYTES_PER_BLOCK - 1) & (~(BYTES_PER_BLOCK - 1))) / BYTES_PER_BLOCK;
    DEBUG_printf("gc_alloc(" UINT_FMT " bytes -> " UINT_FMT " blocks)\n", n_bytes, n_blocks);

    // check for 0 allocation
    if (n_blocks == 0) {
        return MEM_BLOCK_ERROR;
    }

    mp_uint_t i;
    mp_uint_t end_block;
    mp_uint_t start_block;
    mp_uint_t n_free = 0;
    // look for a run of n_blocks available blocks
    for (i = MEM_STATE_MEM(gc_last_free_atb_index); i < MEM_STATE_MEM(gc_alloc_table_byte_len); i++) {
        byte a = MEM_STATE_MEM(gc_alloc_table_start)[i];
        if (ATB_0_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * MEM_BLOCKS_PER_ATB + 0; goto found; } } else { n_free = 0; }
        if (ATB_1_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * MEM_BLOCKS_PER_ATB + 1; goto found; } } else { n_free = 0; }
        if (ATB_2_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * MEM_BLOCKS_PER_ATB + 2; goto found; } } else { n_free = 0; }
        if (ATB_3_IS_FREE(a)) { if (++n_free >= n_blocks) { i = i * MEM_BLOCKS_PER_ATB + 3; goto found; } } else { n_free = 0; }
    }
    return MEM_BLOCK_ERROR; // nothing found!

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
        MEM_STATE_MEM(gc_last_free_atb_index) = (i + 1) / MEM_BLOCKS_PER_ATB;
    }

    // mark first block as used head
    ATB_FREE_TO_HEAD(start_block);

    // mark rest of blocks as used tail
    // TODO for a run of many blocks can make this more efficient
    for (mp_uint_t bl = start_block + 1; bl <= end_block; bl++) {
        ATB_FREE_TO_TAIL(bl);
    }

    return start_block;
}


mp_uint_t heap_realloc(const mp_uint_t block, const mp_uint_t n_bytes, const bool allow_move) {
    // check for pure allocation
    if (block == MEM_BLOCK_ERROR) {
        return heap_alloc(n_bytes);
    }

    // check for pure free
    if (n_bytes == 0) {
        heap_free(block);
        return MEM_BLOCK_ERROR;
    }

    if (!heap_valid(block)){return MEM_BLOCK_ERROR;}

    // compute number of new blocks that are requested
    mp_uint_t new_blocks = (n_bytes + BYTES_PER_BLOCK - 1) / BYTES_PER_BLOCK;

    // Get the total number of consecutive blocks that are already allocated to
    // this chunk of memory, and then count the number of free blocks following
    // it.  Stop if we reach the end of the heap, or if we find enough extra
    // free blocks to satisfy the realloc.  Note that we need to compute the
    // total size of the existing memory chunk so we can correctly and
    // efficiently shrink it (see below for shrinking code).
    mp_uint_t n_free   = 0;
    mp_uint_t n_blocks = 1; // counting HEAD block
    mp_uint_t max_block = MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB;
    for (mp_uint_t bl = block + n_blocks; bl < max_block; bl++) {
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
        return block;
    }

    // check if we can shrink the allocated area
    if (new_blocks < n_blocks) {
        // free unneeded tail blocks
        for (mp_uint_t bl = block + new_blocks, count = n_blocks - new_blocks; count > 0; bl++, count--) {
            ATB_ANY_TO_FREE(bl);
        }

        // set the last_free pointer to end of this block if it's earlier in the heap
        if ((block + new_blocks) / MEM_BLOCKS_PER_ATB < MEM_STATE_MEM(gc_last_free_atb_index)) {
            MEM_STATE_MEM(gc_last_free_atb_index) = (block + new_blocks) / MEM_BLOCKS_PER_ATB;
        }

        return block;
    }

    // check if we can expand in place
    if (new_blocks <= n_blocks + n_free) {
        // mark few more blocks as used tail
        for (mp_uint_t bl = block + n_blocks; bl < block + new_blocks; bl++) {
            assert(ATB_GET_KIND(bl) == AT_FREE);
            ATB_FREE_TO_TAIL(bl);
        }
        return block;
    }

    if(!allow_move){
        return MEM_BLOCK_ERROR;
    }

    // can't resize inplace; try to find a new contiguous chain
    //
    mp_uint_t block_out = heap_alloc(n_bytes);

    // check that the alloc succeeded
    if (block_out == MEM_BLOCK_ERROR) {return MEM_BLOCK_ERROR;}

    DEBUG_printf("gc_realloc(%p -> %p)\n", ptr_in, ptr_out);
    memcpy(heap_void_p(block_out), heap_void_p(block), n_blocks * BYTES_PER_BLOCK);
    heap_free(block);
    return block_out;
}

void heap_info(heap_info_t *info) {
    info->total = (MEM_STATE_MEM(gc_pool_end) - MEM_STATE_MEM(gc_pool_start)) * sizeof(mp_uint_t);
    info->used = 0;
    info->free = 0;
    info->num_1block = 0;
    info->num_2block = 0;
    info->max_block = 0;
    for (mp_uint_t block = 0, len = 0; block < MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB; block++) {
        mp_uint_t kind = ATB_GET_KIND(block);
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

void heap_dump_alloc_table(void) {
    static const mp_uint_t DUMP_BYTES_PER_LINE = 64;
    #if !EXTENSIVE_HEAP_PROFILING
    // When comparing heap output we don't want to print the starting
    // pointer of the heap because it changes from run to run.
    mp_printf(&mp_plat_print, "GC memory layout; from %p:", MEM_STATE_MEM(gc_pool_start));
    #endif
    for (mp_uint_t bl = 0; bl < MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB; bl++) {
        if (bl % DUMP_BYTES_PER_LINE == 0) {
            // a new line of blocks
            {
                // check if this line contains only free blocks
                mp_uint_t bl2 = bl;
                while (bl2 < MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB && ATB_GET_KIND(bl2) == AT_FREE) {
                    bl2++;
                }
                if (bl2 - bl >= 2 * DUMP_BYTES_PER_LINE) {
                    // there are at least 2 lines containing only free blocks, so abbreviate their printing
                    mp_printf(&mp_plat_print, "\n       (" UINT_FMT " lines all free)", (bl2 - bl) / DUMP_BYTES_PER_LINE);
                    bl = bl2 & (~(DUMP_BYTES_PER_LINE - 1));
                    if (bl >= MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB) {
                        // got to end of heap
                        break;
                    }
                }
            }
            // print header for new line of blocks
            // (the cast to uint32_t is for 16-bit ports)
            #if EXTENSIVE_HEAP_PROFILING
            mp_printf(&mp_plat_print, "\n%05x: ", (uint)((bl * BYTES_PER_BLOCK) & (uint32_t)0xfffff));
            #else
            mp_printf(&mp_plat_print, "\n%05x: ", (uint)(PTR_FROM_BLOCK(bl) & (uint32_t)0xfffff));
            #endif
        }
        int c = ' ';
        switch (ATB_GET_KIND(bl)) {
            case AT_FREE: c = '.'; break;
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
                    len = ((mp_uint_t)MP_STATE_VM(stack_top) - (mp_uint_t)&c) / sizeof(mp_uint_t);
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
                mp_uint_t *ptr = MEM_STATE_MEM(gc_pool_start) + bl * WORDS_PER_BLOCK;
                if (*ptr == (mp_uint_t)&mp_type_tuple) { c = 'T'; }
                else if (*ptr == (mp_uint_t)&mp_type_list) { c = 'L'; }
                else if (*ptr == (mp_uint_t)&mp_type_dict) { c = 'D'; }
                #if MICROPY_PY_BUILTINS_FLOAT
                else if (*ptr == (mp_uint_t)&mp_type_float) { c = 'F'; }
                #endif
                else if (*ptr == (mp_uint_t)&mp_type_fun_bc) { c = 'B'; }
                else if (*ptr == (mp_uint_t)&mp_type_module) { c = 'M'; }
                else {
                    c = 'h';
                    #if 0
                    // This code prints "Q" for qstr-pool data, and "q" for qstr-str
                    // data.  It can be useful to see how qstrs are being allocated,
                    // but is disabled by default because it is very slow.
                    for (qstr_pool_t *pool = MP_STATE_VM(last_pool); c == 'h' && pool != NULL; pool = pool->prev) {
                        if ((qstr_pool_t*)ptr == pool) {
                            c = 'Q';
                            break;
                        }
                        for (const byte **q = pool->qstrs, **q_top = pool->qstrs + pool->len; q < q_top; q++) {
                            if ((const byte*)ptr == *q) {
                                c = 'q';
                                break;
                            }
                        }
                    }
                    #endif
                }
                break;
            }
            case AT_TAIL: c = 't'; break;
            case AT_MARK: c = 'm'; break;
        }
        mp_printf(&mp_plat_print, "%c", c);
    }
    mp_print_str(&mp_plat_print, "\n");
}
