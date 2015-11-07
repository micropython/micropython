#ifndef __MICROPY_INCLUDED_PY_HEAP_BASIC_H__
#define __MICROPY_INCLUDED_PY_HEAP_BASIC_H__

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/mpstate.h"

#define HEAP_BLOCKS_PER_ATB     (4)
#define MEM_BLOCK_ERROR         (MP_STATE_MEM(gc_alloc_table_byte_len) * HEAP_BLOCKS_PER_ATB)
#define HEAP_ERROR              (NULL)

typedef struct _heap_info_t {
    mp_uint_t total;            // total memory (in bytes)
    mp_uint_t used;             // used memory  (in bytes)
    mp_uint_t free;             // free memory  (in bytesp
    mp_uint_t num_1block;       // number of 1 block allocations
    mp_uint_t num_2block;       // number of 2 block allocations
    mp_uint_t max_block;        // largest block
} heap_info_t;

void                heap_init(void* start, void* end);
mp_uint_t           heap_first();
mp_uint_t           heap_sizeof(mp_uint_t block);
extern void*        heap_void_p(mp_uint_t block);
mp_uint_t           heap_block(const void* ptr);
mp_uint_t           heap_next(mp_uint_t block);

extern bool         heap_valid(mp_uint_t block);
int8_t              heap_ptr_valid(void* ptr);

extern void         heap_set_mark(mp_uint_t block);
extern void         heap_clear_mark(mp_uint_t block);
extern int8_t       heap_get_mark(mp_uint_t block);

bool                heap_finalizer_get(const mp_uint_t block);
void                heap_finalizer_set(const mp_uint_t block);
void                heap_finalizer_clear(const mp_uint_t block);

void                heap_free(mp_uint_t block);
mp_uint_t           heap_alloc(mp_uint_t n_bytes);
mp_uint_t           heap_realloc(const mp_uint_t block, const mp_uint_t n_bytes, const bool allow_move);

void                heap_dump_alloc_table(void);
void                heap_info(heap_info_t* info);


#endif
