/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_PAIRHEAP_H
#define MICROPY_INCLUDED_PY_PAIRHEAP_H

// This is an implementation of a pairing heap.  It is stable and has deletion
// support.  Only the less-than operation needs to be defined on elements.
//
// See original paper for details:
// Michael L. Fredman, Robert Sedjewick, Daniel D. Sleator, and Robert E. Tarjan.
// The Pairing Heap: A New Form of Self-Adjusting Heap.
// Algorithmica 1:111-129, 1986.
// https://www.cs.cmu.edu/~sleator/papers/pairing-heaps.pdf

#include "py/obj.h"

// This struct forms the nodes of the heap and is intended to be extended, by
// placing it first in another struct, to include additional information for the
// element stored in the heap.  It includes "base" so it can be a MicroPython
// object allocated on the heap and the GC can automatically trace all nodes by
// following the tree structure.
typedef struct _mp_pairheap_t {
    mp_obj_base_t base;
    struct _mp_pairheap_t *child;
    struct _mp_pairheap_t *child_last;
    struct _mp_pairheap_t *next;
} mp_pairheap_t;

// This is the function for the less-than operation on nodes/elements.
typedef int (*mp_pairheap_lt_t)(mp_pairheap_t*, mp_pairheap_t*);

// Core functions.
mp_pairheap_t *mp_pairheap_meld(mp_pairheap_lt_t lt, mp_pairheap_t *heap1, mp_pairheap_t *heap2);
mp_pairheap_t *mp_pairheap_pairing(mp_pairheap_lt_t lt, mp_pairheap_t *child);
mp_pairheap_t *mp_pairheap_delete(mp_pairheap_lt_t lt, mp_pairheap_t *heap, mp_pairheap_t *node);

// Create a new heap.
static inline mp_pairheap_t *mp_pairheap_new(mp_pairheap_lt_t lt) {
    (void)lt;
    return NULL;
}

// Test if the heap is empty.
static inline bool mp_pairheap_is_empty(mp_pairheap_lt_t lt, mp_pairheap_t *heap) {
    (void)lt;
    return heap == NULL;
}

// Peek at the top of the heap.  Will return NULL if empty.
static inline mp_pairheap_t *mp_pairheap_peek(mp_pairheap_lt_t lt, mp_pairheap_t *heap) {
    (void)lt;
    return heap;
}

// Push new node onto existing heap.  Returns the new heap.
static inline mp_pairheap_t *mp_pairheap_push(mp_pairheap_lt_t lt, mp_pairheap_t *heap, mp_pairheap_t *node) {
    node->child = NULL;
    node->next = NULL;
    return mp_pairheap_meld(lt, node, heap); // node is first to be stable
}

// Pop the top off the heap, which must not be empty.  Returns the new heap.
static inline mp_pairheap_t *mp_pairheap_pop(mp_pairheap_lt_t lt, mp_pairheap_t *heap) {
    return mp_pairheap_pairing(lt, heap->child);
}

#endif // MICROPY_INCLUDED_PY_PAIRHEAP_H
