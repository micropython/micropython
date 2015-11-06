#ifndef __MICROPY_INCLUDED_PY_HEAP_BASIC_H__
#define __MICROPY_INCLUDED_PY_HEAP_BASIC_H__

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/mpstate.h"

// TODO: items
#define MEM_STATE_MEM(a)    MP_STATE_MEM(a)
#define BLOCKS_PER_ATB      MEM_BLOCKS_PER_ATB

// ## Will move these to .c

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

#if MICROPY_ENABLE_FINALISER
/**
 * /brief           The "finalizer" is the method that is called when an object is
 *                  freed/deleted. In python, it is created by creating the __del__
 *                  method.
 */
// FTB = finaliser table byte
// if set, then the corresponding block may have a finaliser

#define BLOCKS_PER_FTB (8)

#define FTB_GET(block) ((MEM_STATE_MEM(gc_finaliser_table_start)[(block) / BLOCKS_PER_FTB] >> ((block) & 7)) & 1)
#define FTB_SET(block) do { MEM_STATE_MEM(gc_finaliser_table_start)[(block) / BLOCKS_PER_FTB] |= (1 << ((block) & 7)); } while (0)
#define FTB_CLEAR(block) do { MEM_STATE_MEM(gc_finaliser_table_start)[(block) / BLOCKS_PER_FTB] &= (~(1 << ((block) & 7))); } while (0)
#endif

/*----------------------------------------------------------------------------*/
/* return 1 if the pointer falls within the memory pool                       */
#define VERIFY_PTR(ptr) ( \
        (ptr & (BYTES_PER_BLOCK - 1)) == 0          /* must be aligned on a block */ \
        && ptr >= (mp_uint_t)MEM_STATE_MEM(gc_pool_start)     /* must be above start of pool */ \
        && ptr < (mp_uint_t)MEM_STATE_MEM(gc_pool_end)        /* must be below end of pool */ \
    )


// ## These will stay here
#define MEM_BLOCKS_PER_ATB  (4)
#define MEM_BLOCK_ERROR     (MEM_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB)
#define MEM_ERROR           (NULL)

typedef struct _mem_state_mem_t {
    byte *gc_alloc_table_start;
    mp_uint_t gc_alloc_table_byte_len;
    mp_uint_t *gc_pool_start;
    mp_uint_t *gc_pool_end;
    mp_uint_t gc_last_free_atb_index;
} mem_state_mem_t;

extern mem_state_mem_t mem_state_mem;

// TODO: uncomment
//#define MEM_STATE_MEM(x) (mem_state_mem.x)


typedef struct _heap_info_t {
    mp_uint_t total;            // total memory (in bytes)
    mp_uint_t used;             // used memory  (in bytes)
    mp_uint_t free;             // free memory  (in bytesp
    mp_uint_t num_1block;       // number of 1 block allocations
    mp_uint_t num_2block;       // number of 2 block allocations
    mp_uint_t max_block;        // largest block
} heap_info_t;

void                heap_init(void *start, void *end);
mp_uint_t           heap_first();
mp_uint_t           heap_sizeof(mp_uint_t block);
extern void*        heap_void_p(mp_uint_t block);
mp_uint_t           heap_next(mp_uint_t block);

extern bool         heap_valid(mp_uint_t block);
int8_t              heap_ptr_valid(void *ptr);

extern void         heap_set_mark(mp_uint_t block);
extern void         heap_clear_mark(mp_uint_t block);
extern int8_t       heap_get_mark(mp_uint_t block);

void                heap_free(mp_uint_t block);
mp_uint_t           heap_alloc(mp_uint_t n_bytes);
mp_uint_t           heap_realloc(const mp_uint_t block, const mp_uint_t n_bytes);

void                heap_dump_alloc_table(void);
void                heap_info(heap_info_t *info);


#endif
