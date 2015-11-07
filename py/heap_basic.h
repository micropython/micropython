/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Garrett Berg (vitiral@gmail.com)
 * - Based on 2013, 2014 work by Damien P. George
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

#ifndef __MICROPY_INCLUDED_PY_HEAP_BASIC_H__
#define __MICROPY_INCLUDED_PY_HEAP_BASIC_H__

#include "py/mpconfig.h"
#if MICROPY_ENABLE_GC && MICROPY_GC == MICROPY_GC_BASIC

#include <stdint.h>
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
mp_uint_t           heap_first(void);
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


#endif  // GC_BASIC
#endif
