#ifndef __MICROPY_INCLUDED_PY_HEAP_BASIC_H__
#define __MICROPY_INCLUDED_PY_HEAP_BASIC_H__

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/mpstate.h"

// TODO: items
#define MEM_STATE_MEM(a)    MP_STATE_MEM(a)
#define BLOCKS_PER_ATB      MEM_BLOCKS_PER_ATB


// MOVE ,,,,
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
// ## Move ^^^^


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
mp_uint_t           heap_block(const void *ptr);
mp_uint_t           heap_next(mp_uint_t block);

extern bool         heap_valid(mp_uint_t block);
int8_t              heap_ptr_valid(void *ptr);

extern void         heap_set_mark(mp_uint_t block);
extern void         heap_clear_mark(mp_uint_t block);
extern int8_t       heap_get_mark(mp_uint_t block);

void                heap_free(mp_uint_t block);
mp_uint_t           heap_alloc(mp_uint_t n_bytes);
mp_uint_t           heap_realloc(const mp_uint_t block, const mp_uint_t n_bytes, const bool allow_move);

void                heap_dump_alloc_table(void);
void                heap_info(heap_info_t *info);


#endif
