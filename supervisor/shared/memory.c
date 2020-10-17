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

#include "supervisor/memory.h"
#include "supervisor/port.h"

#include <stddef.h>

#include "supervisor/shared/display.h"

#define CIRCUITPY_SUPERVISOR_ALLOC_COUNT (12)

// Using a zero length to mark an unused allocation makes the code a bit shorter (but makes it
// impossible to support zero-length allocations).
#define FREE 0

// The lowest two bits of a valid length are always zero, so we can use them to mark an allocation
// as freed by the client but not yet reclaimed into the FREE middle.
#define HOLE 1

static supervisor_allocation allocations[CIRCUITPY_SUPERVISOR_ALLOC_COUNT];
// We use uint32_t* to ensure word (4 byte) alignment.
uint32_t* low_address;
uint32_t* high_address;

void memory_init(void) {
    low_address = port_heap_get_bottom();
    high_address = port_heap_get_top();
}

void free_memory(supervisor_allocation* allocation) {
    if (allocation == NULL) {
        return;
    }
    int32_t index = 0;
    bool found = false;
    for (index = 0; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index++) {
        found = allocation == &allocations[index];
        if (found) {
            break;
        }
    }
    if (!found) {
        // Bad!
        // TODO(tannewt): Add a way to escape into safe mode on error.
    }
    if (allocation->ptr == high_address) {
        high_address += allocation->length / 4;
        allocation->length = FREE;
        for (index++; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index++) {
            if (!(allocations[index].length & HOLE)) {
                break;
            }
            // Division automatically shifts out the HOLE bit.
            high_address += allocations[index].length / 4;
            allocations[index].length = FREE;
        }
    } else if (allocation->ptr + allocation->length / 4 == low_address) {
        low_address = allocation->ptr;
        allocation->length = FREE;
        for (index--; index >= 0; index--) {
            if (!(allocations[index].length & HOLE)) {
                break;
            }
            low_address -= allocations[index].length / 4;
            allocations[index].length = FREE;
        }
    } else {
        // Freed memory isn't in the middle so skip updating bounds. The memory will be added to the
        // middle when the memory to the inside is freed. We still need its length, but setting
        // only the lowest bit is nondestructive.
        allocation->length |= HOLE;
    }
}

supervisor_allocation* allocation_from_ptr(void *ptr) {
    for (size_t index = 0; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index++) {
        if (allocations[index].ptr == ptr) {
            return &allocations[index];
        }
    }
    return NULL;
}

supervisor_allocation* allocate_remaining_memory(void) {
    if (low_address == high_address) {
        return NULL;
    }
    return allocate_memory((high_address - low_address) * 4, false);
}

supervisor_allocation* allocate_memory(uint32_t length, bool high) {
    if (length == 0 || length % 4 != 0) {
        return NULL;
    }
    uint8_t index = 0;
    int8_t direction = 1;
    if (high) {
        index = CIRCUITPY_SUPERVISOR_ALLOC_COUNT - 1;
        direction = -1;
    }
    supervisor_allocation* alloc;
    for (; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index += direction) {
        alloc = &allocations[index];
        if (alloc->length == FREE && (high_address - low_address) * 4 >= (int32_t) length) {
            break;
        }
        // If a hole matches in length exactly, we can reuse it.
        if (alloc->length == (length | HOLE)) {
            alloc->length = length;
            return alloc;
        }
    }
    if (index >= CIRCUITPY_SUPERVISOR_ALLOC_COUNT) {
        return NULL;
    }
    if (high) {
        high_address -= length / 4;
        alloc->ptr = high_address;
    } else {
        alloc->ptr = low_address;
        low_address += length / 4;
    }
    alloc->length = length;
    return alloc;
}

void supervisor_move_memory(void) {
    #if CIRCUITPY_DISPLAYIO
    supervisor_display_move_memory();
    #endif
}
