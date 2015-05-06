#ifndef __MICROPY_INCLUDED_PY_MEM_H__
#define __MICROPY_INCLUDED_PY_MEM_H__

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/misc.h"

#define MEM_BLOCKS_PER_ATB  (4)
#define MEM_BLOCK_ERROR     (MP_STATE_MEM(gc_alloc_table_byte_len) * MEM_BLOCKS_PER_ATB)
#define MEM_ERROR           (NULL)

void                mem_init(void *start, void *end);
mp_uint_t           mem_first();
mp_uint_t           mem_sizeof(mp_uint_t block);
extern void*        mem_void_p(mp_uint_t block);
mp_uint_t           mem_next(mp_uint_t block);

extern int8_t       mem_valid(mp_uint_t block);
int8_t              mem_ptr_valid(void *ptr);

extern void         mem_set_mark(mp_uint_t block);
extern void         mem_clear_mark(mp_uint_t block);
extern int8_t       mem_get_mark(mp_uint_t block);

void                mem_free(mp_uint_t block);
mp_uint_t           mem_alloc(mp_uint_t n_bytes);
mp_uint_t           mem_realloc(const mp_uint_t block, const mp_uint_t n_bytes);

void                mem_dump_alloc_table(void);
#include "py/gc.h"      // temporary while we figure out how we want to separate data types
void                mem_info(gc_info_t *info);

#endif
