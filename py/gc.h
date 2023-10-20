/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_GC_H
#define MICROPY_INCLUDED_PY_GC_H

#include <stdbool.h>
#include <stddef.h>
#include "py/mpprint.h"

#include "py/mpconfig.h"
#include "py/mpstate.h"
#include "py/misc.h"

#if !MICROPY_GC_SPLIT_HEAP
#define HEAP_PTR(ptr) ( \
    MP_STATE_MEM(area).gc_pool_start != 0                     /* Not on the heap if it isn't inited */ \
    && ptr >= (void *)MP_STATE_MEM(area).gc_pool_start        /* must be above start of pool */ \
    && ptr < (void *)MP_STATE_MEM(area).gc_pool_end           /* must be below end of pool */ \
    )

// ptr should be of type void*
#define VERIFY_PTR(ptr) ( \
    ((uintptr_t)(ptr) & (MICROPY_BYTES_PER_GC_BLOCK - 1)) == 0          /* must be aligned on a block */ \
    && HEAP_PTR(ptr) \
    )
#endif

void gc_init(void *start, void *end);
void gc_deinit(void);

#if MICROPY_GC_SPLIT_HEAP
// Used to add additional memory areas to the heap.
void gc_add(void *start, void *end);
#endif

// These lock/unlock functions can be nested.
// They can be used to prevent the GC from allocating/freeing.
void gc_lock(void);
void gc_unlock(void);
bool gc_is_locked(void);

// A given port must implement gc_collect by using the other collect functions.
void gc_collect(void);
void gc_collect_start(void);
void gc_collect_ptr(void *ptr);
void gc_collect_root(void **ptrs, size_t len);
void gc_collect_end(void);

// Is the gc heap available?
bool gc_alloc_possible(void);

// Use this function to sweep the whole heap and run all finalisers
void gc_sweep_all(void);

enum {
    GC_ALLOC_FLAG_HAS_FINALISER = 1,
};

void *gc_alloc(size_t n_bytes, unsigned int alloc_flags);
void gc_free(void *ptr); // does not call finaliser
size_t gc_nbytes(const void *ptr);
bool gc_has_finaliser(const void *ptr);
void *gc_realloc(void *ptr, size_t n_bytes, bool allow_move);

// Prevents a pointer from ever being freed because it establishes a permanent reference to it. Use
// very sparingly because it can leak memory.
bool gc_never_free(void *ptr);

typedef struct _gc_info_t {
    size_t total;
    size_t used;
    size_t free;
    size_t max_free;
    size_t num_1block;
    size_t num_2block;
    size_t max_block;
} gc_info_t;

void gc_info(gc_info_t *info);
void gc_dump_info(const mp_print_t *print);
void gc_dump_alloc_table(const mp_print_t *print);

#endif // MICROPY_INCLUDED_PY_GC_H
