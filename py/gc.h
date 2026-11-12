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

void gc_init(void *start, void *end);

#if MICROPY_GC_SPLIT_HEAP
// Attributes of a heap area, given by the port when it adds the area.  They let
// code choose areas by what they can do rather than by index.
enum {
    // Off-chip memory such as PSRAM or SDRAM.
    GC_AREA_ATTR_EXTERNAL = 0x0001,
    // Memory that the port's DMA controllers can access.
    GC_AREA_ATTR_DMA = 0x0002,
    // First of the bits that a port may define for its own attributes.
    GC_AREA_ATTR_PORT_FIRST = 0x0100,
};

// Used to add additional memory areas to the heap.
void gc_add(void *start, void *end);

#if MICROPY_GC_AREA_SELECT
// Add a memory area with the given GC_AREA_ATTR_xxx attributes.  Areas are
// indexed in the order they are added; the area given to gc_init() is index 0.
// Areas added after index GC_AREA_INDEX_MAX share that index.
void gc_add_ex(void *start, void *end, unsigned int attrs);
#define GC_AREA_INDEX_MAX (30)

typedef struct _gc_area_info_t {
    void *start; // first byte of the area's allocatable memory
    size_t total; // allocatable bytes
    size_t free; // free bytes
    size_t max_free; // largest run of free bytes
    unsigned int attrs; // GC_AREA_ATTR_xxx
} gc_area_info_t;

// Set the attributes of the area at the given index, for example of area 0,
// which gc_init() creates without attributes.
void gc_area_set_attrs(size_t index, unsigned int attrs);

// Fill in info for the area at the given index; returns false if there is no
// such area.
bool gc_area_info(size_t index, gc_area_info_t *info);

// Return the index of the area that contains ptr, or -1 if ptr isn't in the heap.
int gc_area_index(const void *ptr);

// A selection of heap areas for allocations: bit n selects the area at index n,
// and GC_AREA_SEL_STRICT makes allocations fail rather than fall back to the
// other areas when the selected ones are full.  A selection of 0 restores the
// default, which is to try the areas in order.
#define GC_AREA_SEL_STRICT (0x80000000)
#define GC_AREA_SEL_MASK (0x7fffffff)

// Return the selection bits of the areas that have all of the given attributes.
uint32_t gc_area_with_attrs(unsigned int attrs);

// Set the areas that the current thread's allocations go in, and return the
// previous selection so it can be restored.  For example, a driver that needs
// a DMA-capable buffer:
//     uint32_t prev = gc_area_select(gc_area_with_attrs(GC_AREA_ATTR_DMA) | GC_AREA_SEL_STRICT);
//     buf = m_new(uint8_t, len);
//     gc_area_select(prev);
// Selected areas are tried in index order.  Note that m_new() raises
// MemoryError, so code that must restore the selection on failure should use
// m_new_maybe() or an nlr handler.
uint32_t gc_area_select(uint32_t sel);

// Set the alignment in bytes, a power of two, of the current thread's
// allocations, and return the previous alignment.  Each allocation also uses
// a multiple of this many bytes, so an aligned buffer doesn't share a cache
// line with another object.  An alignment of 0 restores the default, which is
// MICROPY_BYTES_PER_GC_BLOCK.
size_t gc_alloc_align(size_t align);
#else
// Without area selection, areas have no attributes.
#define gc_add_ex(start, end, attrs) gc_add(start, end)
#define gc_area_set_attrs(index, attrs) ((void)0)
#endif // MICROPY_GC_AREA_SELECT

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
void gc_collect_root(void **ptrs, size_t len);
void gc_collect_end(void);

// Use this function to sweep the whole heap and run all finalisers
void gc_sweep_all(void);

// These functions are used to manage weakrefs.
void gc_weakref_mark(void *ptr);
void gc_weakref_about_to_be_freed(void *ptr);
void gc_weakref_sweep(void);

enum {
    GC_ALLOC_FLAG_HAS_FINALISER = 1,
};

void *gc_alloc(size_t n_bytes, unsigned int alloc_flags);
void gc_free(void *ptr); // does not call finaliser
uint32_t gc_meta_bits(void *ptr);
size_t gc_nbytes(const void *ptr);
void *gc_realloc(void *ptr, size_t n_bytes, bool allow_move);

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
void gc_info_fast(gc_info_t *info);
void gc_dump_info(const mp_print_t *print);
void gc_dump_alloc_table(const mp_print_t *print);

#endif // MICROPY_INCLUDED_PY_GC_H
