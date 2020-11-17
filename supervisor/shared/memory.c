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
        for (index++; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index++) {
            if (allocations[index].ptr != NULL) {
                break;
            }
            high_address += allocations[index].length / 4;
        }
    } else if (allocation->ptr + allocation->length / 4 == low_address) {
        low_address = allocation->ptr;
        for (index--; index >= 0; index--) {
            if (allocations[index].ptr != NULL) {
                break;
            }
            low_address -= allocations[index].length / 4;
        }
    } else {
        // Freed memory isn't in the middle so skip updating bounds. The memory will be added to the
        // middle when the memory to the inside is freed.
    }
    allocation->ptr = NULL;
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
    if ((high_address - low_address) * 4 < (int32_t) length || length % 4 != 0) {
        return NULL;
    }
    uint8_t index = 0;
    int8_t direction = 1;
    if (high) {
        index = CIRCUITPY_SUPERVISOR_ALLOC_COUNT - 1;
        direction = -1;
    }
    for (; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index += direction) {
        if (allocations[index].ptr == NULL) {
            break;
        }
    }
    if (index >= CIRCUITPY_SUPERVISOR_ALLOC_COUNT) {
        return NULL;
    }
    supervisor_allocation* alloc = &allocations[index];
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
