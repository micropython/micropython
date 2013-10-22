#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "mpyconfig.h"
#include "gc.h"

// a machine word is big enough to hold a pointer
/*
#define BYTES_PER_WORD (8)
typedef unsigned long machine_uint_t;
*/
typedef unsigned char byte;

#define BITS_PER_BYTE (8)
#define BITS_PER_WORD (BITS_PER_BYTE * BYTES_PER_WORD)
#define WORDS_PER_BLOCK (4)
#define BYTES_PER_BLOCK (WORDS_PER_BLOCK * BYTES_PER_WORD)
#define STACK_SIZE (64) // tunable; minimum is 1

static byte *gc_alloc_table_start;
static byte *gc_alloc_table_end;
static machine_uint_t gc_alloc_table_byte_len;
static machine_uint_t *gc_pool_start;
static machine_uint_t *gc_pool_end;

static int gc_stack_overflow;
static machine_uint_t gc_stack[STACK_SIZE];
static machine_uint_t *gc_sp;

// TODO waste less memory; currently requires that all entries in alloc_table have a corresponding block in pool
void gc_init(void *start, void *end) {
    // align end pointer on block boundary
    end = (void*)((machine_uint_t)end & (~(BYTES_PER_BLOCK - 1)));

    // calculate parameters for GC
    machine_uint_t total_word_len = (machine_uint_t*)end - (machine_uint_t*)start;
    gc_alloc_table_byte_len = total_word_len * BYTES_PER_WORD / (1 + BITS_PER_BYTE / 2 * BYTES_PER_BLOCK);
    gc_alloc_table_start = (byte*)start;
    gc_alloc_table_end = gc_alloc_table_start + gc_alloc_table_byte_len;
    machine_uint_t gc_pool_block_len = gc_alloc_table_byte_len * BITS_PER_BYTE / 2;
    machine_uint_t gc_pool_word_len = gc_pool_block_len * WORDS_PER_BLOCK;
    gc_pool_start = (machine_uint_t*)end - gc_pool_word_len;
    gc_pool_end = end;

    // clear ATBs
    memset(gc_alloc_table_start, 0, gc_alloc_table_byte_len);

    /*
    printf("GC layout:\n");
    printf("  alloc table at %p, length %u bytes\n", gc_alloc_table_start, gc_alloc_table_byte_len);
    printf("  pool at %p, length %u blocks = %u words = %u bytes\n", gc_pool_start, gc_pool_block_len, gc_pool_word_len, gc_pool_word_len * BYTES_PER_WORD);
    */
}

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

static void gc_drain_stack() {
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

static void gc_deal_with_stack_overflow() {
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

static void gc_sweep() {
    // free unmarked heads and their tails
    int free_tail = 0;
    for (machine_uint_t block = 0; block < gc_alloc_table_byte_len * BLOCKS_PER_ATB; block++) {
        switch (ATB_GET_KIND(block)) {
            case AT_HEAD:
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

void gc_collect_start() {
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

void gc_collect_end() {
    gc_deal_with_stack_overflow();
    gc_sweep();
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

void *gc_alloc(machine_uint_t n_bytes) {
    machine_uint_t n_blocks = ((n_bytes + BYTES_PER_BLOCK - 1) & (~(BYTES_PER_BLOCK - 1))) / BYTES_PER_BLOCK;
    //printf("gc_alloc(%u bytes -> %u blocks)\n", n_bytes, n_blocks);

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

    // return pointer to first block
    return (void*)(gc_pool_start + start_block * WORDS_PER_BLOCK);
}

// force the freeing of a piece of memory
void gc_free(void *ptr_in) {
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

void *gc_realloc(void *ptr, machine_uint_t n_bytes) {
    machine_uint_t n_existing = gc_nbytes(ptr);
    if (n_bytes <= n_existing) {
        return ptr;
    } else {
        void *ptr2 = gc_alloc(n_bytes);
        memcpy(ptr2, ptr, n_existing);
        return ptr2;
    }
}

/*
static void gc_dump_at() {
    for (machine_uint_t bl = 0; bl < gc_alloc_table_byte_len * BLOCKS_PER_ATB; bl++) {
        printf("block % 6u ", bl);
        switch (ATB_GET_KIND(bl)) {
            case AT_FREE: printf("FREE"); break;
            case AT_HEAD: printf("HEAD"); break;
            case AT_TAIL: printf("TAIL"); break;
            default: printf("MARK"); break;
        }
        printf("\n");
    }
}

int main() {
    machine_uint_t len = 1000;
    machine_uint_t *heap = malloc(len);
    gc_init(heap, heap + len / sizeof(machine_uint_t));
    void *ptrs[100];
    {
        machine_uint_t *p = gc_alloc(16);
        p[0] = gc_alloc(64);
        p[1] = gc_alloc(1);
        p[2] = gc_alloc(1);
        p[3] = gc_alloc(1);
        machine_uint_t *p2 = gc_alloc(16);
        p2[0] = p;
        p2[1] = p;
        ptrs[0] = p2;
    }
    for (int i = 0; i < 50; i+=2) {
        machine_uint_t *p = gc_alloc(i);
        printf("p=%p\n", p);
        if (i & 3) {
            //ptrs[i] = p;
        }
    }

    gc_dump_at();
    gc_collect(ptrs, sizeof(ptrs) / sizeof(void*));
    gc_dump_at();
}
*/
