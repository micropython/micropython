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

#include <string.h>

#include "py/gc.h"
#include "supervisor/shared/display.h"

enum {
    CIRCUITPY_SUPERVISOR_IMMOVABLE_ALLOC_COUNT =
        // stack + heap
        2
        // next_code_allocation
        + 1

        #if INTERNAL_FLASH_FILESYSTEM == 0
        + 1
        #endif

        #if CIRCUITPY_USB
        + 1 // device_descriptor_allocation
        + 1 // configuration_descriptor_allocation
        + 1 // string_descriptors_allocation
        #endif

        #if CIRCUITPY_USB_HID
        + 1 // hid_report_descriptor_allocation
        + 1 // hid_devices_allocation
        #endif
    ,

    CIRCUITPY_SUPERVISOR_MOVABLE_ALLOC_COUNT =
        0
        #if CIRCUITPY_DISPLAYIO
        #if CIRCUITPY_TERMINALIO
        + 1
        #endif
        + CIRCUITPY_DISPLAY_LIMIT * (
            // Maximum needs of one display: max(4 if RGBMATRIX, 1 if SHARPDISPLAY, 0)
            #if CIRCUITPY_RGBMATRIX
            4
            #elif CIRCUITPY_SHARPDISPLAY
            1
            #else
            0
            #endif
            )
        #endif
    ,

    CIRCUITPY_SUPERVISOR_ALLOC_COUNT = CIRCUITPY_SUPERVISOR_IMMOVABLE_ALLOC_COUNT + CIRCUITPY_SUPERVISOR_MOVABLE_ALLOC_COUNT
};

// The lowest two bits of a valid length are always zero, so we can use them to mark an allocation
// as a hole (freed by the client but not yet reclaimed into the free middle) and as movable.
#define FLAGS 3
#define HOLE 1
#define MOVABLE 2

static supervisor_allocation allocations[CIRCUITPY_SUPERVISOR_ALLOC_COUNT];
supervisor_allocation *old_allocations;

typedef struct _supervisor_allocation_node {
    struct _supervisor_allocation_node *next;
    size_t length;
    // We use uint32_t to ensure word (4 byte) alignment.
    uint32_t data[];
} supervisor_allocation_node;

supervisor_allocation_node *low_head;
supervisor_allocation_node *high_head;

// Intermediate (void*) is to suppress -Wcast-align warning. Alignment will always be correct
// because this only reverses how (alloc)->ptr was obtained as &(node->data[0]).
#define ALLOCATION_NODE(alloc) ((supervisor_allocation_node *)(void *)((char *)((alloc)->ptr) - sizeof(supervisor_allocation_node)))

void free_memory(supervisor_allocation *allocation) {
    if (allocation == NULL || allocation->ptr == NULL) {
        return;
    }
    supervisor_allocation_node *node = ALLOCATION_NODE(allocation);
    if (node == low_head) {
        do {
            low_head = low_head->next;
        } while (low_head != NULL && (low_head->length & HOLE));
    } else if (node == high_head) {
        do {
            high_head = high_head->next;
        } while (high_head != NULL && (high_head->length & HOLE));
    } else {
        // Check if it's in the list of embedded allocations.
        supervisor_allocation_node **emb = &MP_STATE_VM(first_embedded_allocation);
        while (*emb != NULL && *emb != node) {
            emb = &((*emb)->next);
        }
        if (*emb != NULL) {
            // Found, remove it from the list.
            *emb = node->next;
            m_free(node
                #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
                , sizeof(supervisor_allocation_node) + (node->length & ~FLAGS)
                #endif
                );
        } else {
            // Else it must be within the low or high ranges and becomes a hole.
            node->length = ((node->length & ~FLAGS) | HOLE);
        }
    }
    allocation->ptr = NULL;
}

supervisor_allocation *allocation_from_ptr(void *ptr) {
    // When called from the context of supervisor_move_memory() (old_allocations != NULL), search
    // by old pointer to give clients a way of mapping from old to new pointer. But not if
    // ptr == NULL, then the caller wants an allocation whose current ptr is NULL.
    supervisor_allocation *list = (old_allocations && ptr) ? old_allocations : &allocations[0];
    for (size_t index = 0; index < CIRCUITPY_SUPERVISOR_ALLOC_COUNT; index++) {
        if (list[index].ptr == ptr) {
            return &allocations[index];
        }
    }
    return NULL;
}

supervisor_allocation *allocate_remaining_memory(void) {
    return allocate_memory((uint32_t)-1, false, false);
}

static supervisor_allocation_node *find_hole(supervisor_allocation_node *node, size_t length) {
    for (; node != NULL; node = node->next) {
        if (node->length == (length | HOLE)) {
            break;
        }
    }
    return node;
}

static supervisor_allocation_node *allocate_memory_node(uint32_t length, bool high, bool movable) {
    if (CIRCUITPY_SUPERVISOR_MOVABLE_ALLOC_COUNT == 0) {
        assert(!movable);
    }
    // supervisor_move_memory() currently does not support movable allocations on the high side, it
    // must be extended first if this is ever needed.
    assert(!(high && movable));
    uint32_t *low_address = low_head ? low_head->data + low_head->length / 4 : port_heap_get_bottom();
    uint32_t *high_address = high_head ? (uint32_t *)high_head : port_heap_get_top();
    // Special case for allocate_remaining_memory(), avoids computing low/high_address twice.
    if (length == (uint32_t)-1) {
        length = (high_address - low_address) * 4 - sizeof(supervisor_allocation_node);
    }
    if (length == 0 || length % 4 != 0) {
        return NULL;
    }
    // 1. Matching hole on the requested side?
    supervisor_allocation_node *node = find_hole(high ? high_head : low_head, length);
    if (!node) {
        // 2. Enough free space in the middle?
        if ((high_address - low_address) * 4 >= (int32_t)(sizeof(supervisor_allocation_node) + length)) {
            if (high) {
                high_address -= (sizeof(supervisor_allocation_node) + length) / 4;
                node = (supervisor_allocation_node *)high_address;
                node->next = high_head;
                high_head = node;
            } else {
                node = (supervisor_allocation_node *)low_address;
                node->next = low_head;
                low_head = node;
            }
        } else {
            // 3. Matching hole on the other side?
            node = find_hole(high ? low_head : high_head, length);
            if (!node) {
                // 4. GC allocation?
                if (movable && gc_alloc_possible()) {
                    node = m_malloc_maybe(sizeof(supervisor_allocation_node) + length, true);
                    if (node) {
                        node->next = MP_STATE_VM(first_embedded_allocation);
                        MP_STATE_VM(first_embedded_allocation) = node;
                    }
                }
                if (!node) {
                    // 5. Give up.
                    return NULL;
                }
            }
        }
    }
    node->length = length;
    if (movable) {
        node->length |= MOVABLE;
    }
    return node;
}

supervisor_allocation *allocate_memory(uint32_t length, bool high, bool movable) {
    supervisor_allocation_node *node = allocate_memory_node(length, high, movable);
    if (!node) {
        return NULL;
    }
    // Find the first free allocation.
    supervisor_allocation *alloc = allocation_from_ptr(NULL);
    if (!alloc) {
        // We should free node again to avoid leaking, but something is wrong anyway if clients try
        // to make more allocations than available, so don't bother.
        return NULL;
    }
    alloc->ptr = &(node->data[0]);
    return alloc;
}

size_t get_allocation_length(supervisor_allocation *allocation) {
    return ALLOCATION_NODE(allocation)->length & ~FLAGS;
}


void supervisor_move_memory(void) {
    // This whole function is not needed when there are no movable allocations, let it be optimized
    // out.
    if (CIRCUITPY_SUPERVISOR_MOVABLE_ALLOC_COUNT == 0) {
        return;
    }
    // This must be called exactly after freeing the heap, so that the embedded allocations, if any,
    // are now in the free region.
    assert(MP_STATE_VM(first_embedded_allocation) == NULL || (
        (low_head == NULL || low_head < MP_STATE_VM(first_embedded_allocation)) &&
        (high_head == NULL || MP_STATE_VM(first_embedded_allocation) < high_head)));

    // Save the old pointers for allocation_from_ptr().
    supervisor_allocation old_allocations_array[CIRCUITPY_SUPERVISOR_ALLOC_COUNT];
    memcpy(old_allocations_array, allocations, sizeof(allocations));

    // Compact the low side. Traverse the list repeatedly, finding movable allocations preceded by a
    // hole and swapping them, until no more are found. This is not the most runtime-efficient way,
    // but probably the shortest and simplest code.
    bool acted;
    do {
        acted = false;
        supervisor_allocation_node **nodep = &low_head;
        while (*nodep != NULL && (*nodep)->next != NULL) {
            if (((*nodep)->length & MOVABLE) && ((*nodep)->next->length & HOLE)) {
                supervisor_allocation_node *oldnode = *nodep;
                supervisor_allocation_node *start = oldnode->next;
                supervisor_allocation *alloc = allocation_from_ptr(&(oldnode->data[0]));
                assert(alloc != NULL);
                alloc->ptr = &(start->data[0]);
                oldnode->next = start->next;
                size_t holelength = start->length;
                size_t size = sizeof(supervisor_allocation_node) + (oldnode->length & ~FLAGS);
                memmove(start, oldnode, size);
                supervisor_allocation_node *newhole = (supervisor_allocation_node *)(void *)((char *)start + size);
                newhole->next = start;
                newhole->length = holelength;
                *nodep = newhole;
                acted = true;
            }
            nodep = &((*nodep)->next);
        }
    } while (acted);
    // Any holes bubbled to the top can be absorbed into the free middle.
    while (low_head != NULL && (low_head->length & HOLE)) {
        low_head = low_head->next;
    }
    ;

    // Don't bother compacting the high side, there are no movable allocations and no holes there in
    // current usage.

    // Promote the embedded allocations to top-level ones, compacting them at the beginning of the
    // now free region (or possibly in matching holes).
    // The linked list is unordered, but allocations must be processed in order to avoid risking
    // overwriting each other. To that end, repeatedly find the lowest element of the list, remove
    // it from the list, and process it. This ad-hoc selection sort results in substantially shorter
    // code than using the qsort() function from the C library.
    while (MP_STATE_VM(first_embedded_allocation)) {
        // First element is first candidate.
        supervisor_allocation_node **pminnode = &MP_STATE_VM(first_embedded_allocation);
        // Iterate from second element (if any) on.
        for (supervisor_allocation_node **pnode = &(MP_STATE_VM(first_embedded_allocation)->next); *pnode != NULL; pnode = &(*pnode)->next) {
            if (*pnode < *pminnode) {
                pminnode = pnode;
            }
        }
        // Remove from list.
        supervisor_allocation_node *node = *pminnode;
        *pminnode = node->next;
        // Process.
        size_t length = (node->length & ~FLAGS);
        supervisor_allocation *alloc = allocation_from_ptr(&(node->data[0]));
        assert(alloc != NULL);
        // This may overwrite the header of node if it happened to be there already, but not the
        // data.
        supervisor_allocation_node *new_node = allocate_memory_node(length, false, true);
        // There must be enough free space.
        assert(new_node != NULL);
        memmove(&(new_node->data[0]), &(node->data[0]), length);
        alloc->ptr = &(new_node->data[0]);
    }

    // Notify clients that their movable allocations may have moved.
    old_allocations = &old_allocations_array[0];

    #if CIRCUITPY_DISPLAYIO
    supervisor_display_move_memory();
    #endif

    // Add calls to further clients here.
    old_allocations = NULL;
}
