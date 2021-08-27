/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

// Basic allocations outside them for areas such as the VM heap and stack.
// supervisor/shared/memory.c has a basic implementation for a continuous chunk of memory. Add it
// to a SRC_ in a Makefile to use it.

#ifndef MICROPY_INCLUDED_SUPERVISOR_MEMORY_H
#define MICROPY_INCLUDED_SUPERVISOR_MEMORY_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t *ptr;
} supervisor_allocation;



void free_memory(supervisor_allocation *allocation);

// Find the allocation with the given ptr, NULL if not found. When called from the context of a
// supervisor_move_memory() callback, finds the allocation that had that ptr *before* the move, but
// the returned allocation already contains the ptr after the move.
// When called with NULL, may return either NULL or an unused allocation whose ptr is NULL (this is
// a feature used internally in allocate_memory to save code size). Passing the return value to
// free_memory() is a permissible no-op in either case.
supervisor_allocation *allocation_from_ptr(void *ptr);

supervisor_allocation *allocate_remaining_memory(void);

// Allocate a piece of a given length in bytes. If high_address is true then it should be allocated
// at a lower address from the top of the stack. Otherwise, addresses will increase starting after
// statically allocated memory. If movable is false, memory will be taken from outside the GC heap
// and will stay stationary until freed. While the VM is running, this will fail unless a previous
// allocation of exactly matching length has recently been freed. If movable is true, memory will be
// taken from either outside or inside the GC heap, and when the VM exits, will be moved outside.
// The ptr of the returned supervisor_allocation will change at that point. If you need to be
// notified of that, add your own callback function at the designated place near the end of
// supervisor_move_memory().
supervisor_allocation *allocate_memory(uint32_t length, bool high_address, bool movable);

static inline size_t align32_size(size_t size) {
    return (size + 3) & ~3;
}

size_t get_allocation_length(supervisor_allocation *allocation);

// Called after the GC heap is freed, transfers movable allocations from the GC heap to the
// supervisor heap and compacts the supervisor heap.
void supervisor_move_memory(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_MEMORY_H
