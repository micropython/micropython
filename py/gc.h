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

void gc_init(void *start, void *end);
// CIRCUITPY-CHANGE
void gc_deinit(void);

#if MICROPY_GC_SPLIT_HEAP
// Used to add additional memory areas to the heap.
void gc_add(void *start, void *end);

#if MICROPY_GC_SPLIT_HEAP_AUTO
// Port must implement this function to return the maximum available block of
// RAM to allocate a new heap area into using MP_PLAT_ALLOC_HEAP.
size_t gc_get_max_new_split(void);
#endif // MICROPY_GC_SPLIT_HEAP_AUTO
#endif // MICROPY_GC_SPLIT_HEAP

// These lock/unlock functions can be nested.
// They can be used to prevent the GC from allocating/freeing.
void gc_lock(void);
void gc_unlock(void);
bool gc_is_locked(void);

// A given port must implement gc_collect by using the other collect functions.
void gc_collect(void);
void gc_collect_start(void);
// CIRCUITPY-CHANGE
void gc_collect_ptr(void *ptr);
void gc_collect_root(void **ptrs, size_t len);
void gc_collect_end(void);

// CIRCUITPY-CHANGE
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

// CIRCUITPY-CHANGE
// Prevents a pointer from ever being freed because it establishes a permanent reference to it. Use
// very sparingly because it can leak memory.
bool gc_never_free(void *ptr);

// CIRCUITPY-CHANGE
// True if the pointer is on the MP heap. Doesn't require that it is the start
// of a block.
bool gc_ptr_on_heap(void *ptr);

typedef struct _gc_info_t {
    size_t total;
    size_t used;
    size_t free;
    size_t max_free;
    size_t num_1block;
    size_t num_2block;
    size_t max_block;
    #if MICROPY_GC_SPLIT_HEAP_AUTO
    size_t max_new_split;
    #endif
} gc_info_t;

void gc_info(gc_info_t *info);
void gc_dump_info(const mp_print_t *print);
void gc_dump_alloc_table(const mp_print_t *print);

#endif // MICROPY_INCLUDED_PY_GC_H
