#ifndef __MICROPY_INCLUDED_PY_MEM_H__
#define __MICROPY_INCLUDED_PY_MEM_H__

#include <stdint.h>
#include "py/mpconfig.h"
#include "py/misc.h"

#define MEM_BLOCK_ERROR     (MP_STATE_MEM(gc_alloc_table_byte_len) * BLOCKS_PER_ATB)
#define MEM_ERROR           (NULL)

mp_uint_t           mem_first();
mp_uint_t           mem_sizeof(mp_uint_t block);
void*               mem_void_p(mp_uint_t block);
mp_uint_t           mem_next(mp_uint_t block);

int8_t              mem_valid(mp_uint_t block);
int8_t              mem_ptr_valid(void *ptr);

void                mem_set_mark(mp_uint_t block);
void                mem_clear_mark(mp_uint_t block);
int8_t              mem_get_mark(mp_uint_t block);

void                mem_free(void *ptr_in);
mp_uint_t           mem_alloc(mp_uint_t n_bytes);

#endif
