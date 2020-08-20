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

typedef struct {
    uint32_t* ptr;
    uint32_t length; // in bytes
} supervisor_allocation;



void memory_init(void);
void free_memory(supervisor_allocation* allocation);
supervisor_allocation* allocation_from_ptr(void *ptr);
supervisor_allocation* allocate_remaining_memory(void);

// Allocate a piece of a given length in bytes. If high_address is true then it should be allocated
// at a lower address from the top of the stack. Otherwise, addresses will increase starting after
// statically allocated memory.
supervisor_allocation* allocate_memory(uint32_t length, bool high_address);

static inline uint16_t align32_size(uint16_t size) {
    if (size % 4 != 0) {
        return (size & 0xfffc) + 0x4;
    }
    return size;
}

// Called after the heap is freed in case the supervisor wants to save some values.
void supervisor_move_memory(void);

#endif  // MICROPY_INCLUDED_SUPERVISOR_MEMORY_H
