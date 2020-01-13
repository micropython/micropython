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

#include "py/pairheap.h"

// The mp_pairheap_t.next pointer can take one of the following values:
//   - NULL: the node is the top of the heap
//   - LSB set: the node is the last of the children and points to its parent node
//   - other: the node is a child and not the last child
// The macros below help manage this pointer.
#define NEXT_MAKE_RIGHTMOST_PARENT(parent) ((void*)((uintptr_t)(parent) | 1))
#define NEXT_IS_RIGHTMOST_PARENT(next) ((uintptr_t)(next) & 1)
#define NEXT_GET_RIGHTMOST_PARENT(next) ((void*)((uintptr_t)(next) & ~1))

// O(1), stable
mp_pairheap_t *mp_pairheap_meld(mp_pairheap_lt_t lt, mp_pairheap_t *heap1, mp_pairheap_t *heap2) {
    if (heap1 == NULL) {
        return heap2;
    }
    if (heap2 == NULL) {
        return heap1;
    }
    if (lt(heap1, heap2)) {
        if (heap1->child == NULL) {
            heap1->child = heap2;
        } else {
            heap1->child_last->next = heap2;
        }
        heap1->child_last = heap2;
        heap2->next = NEXT_MAKE_RIGHTMOST_PARENT(heap1);
        return heap1;
    } else {
        heap1->next = heap2->child;
        heap2->child = heap1;
        if (heap1->next == NULL) {
            heap2->child_last = heap1;
            heap1->next = NEXT_MAKE_RIGHTMOST_PARENT(heap2);
        }
        return heap2;
    }
}

// amortised O(log N), stable
mp_pairheap_t *mp_pairheap_pairing(mp_pairheap_lt_t lt, mp_pairheap_t *child) {
    if (child == NULL) {
        return NULL;
    }
    mp_pairheap_t *heap = NULL;
    while (!NEXT_IS_RIGHTMOST_PARENT(child)) {
        mp_pairheap_t *n1 = child;
        child = child->next;
        if (!NEXT_IS_RIGHTMOST_PARENT(child)) {
            mp_pairheap_t *n2 = child;
            child = child->next;
            n1 = mp_pairheap_meld(lt, n1, n2);
        }
        heap = mp_pairheap_meld(lt, heap, n1);
    }
    heap->next = NULL;
    return heap;
}

// amortised O(log N), stable
mp_pairheap_t *mp_pairheap_delete(mp_pairheap_lt_t lt, mp_pairheap_t *heap, mp_pairheap_t *node) {
    // Simple case of the top being the node to delete
    if (node == heap) {
        return mp_pairheap_pairing(lt, heap->child);
    }

    // Case where node is not in the heap
    if (node->next == NULL) {
        return heap;
    }

    // Find parent of node
    mp_pairheap_t *parent = node;
    while (!NEXT_IS_RIGHTMOST_PARENT(parent->next)) {
        parent = parent->next;
    }
    parent = NEXT_GET_RIGHTMOST_PARENT(parent->next);

    // Replace node with pairing of its children
    mp_pairheap_t *next;
    if (node == parent->child && node->child == NULL) {
        if (NEXT_IS_RIGHTMOST_PARENT(node->next)) {
            parent->child = NULL;
        } else {
            parent->child = node->next;
        }
        node->next = NULL;
        return heap;
    } else if (node == parent->child) {
        next = node->next;
        node->next = NULL;
        node = mp_pairheap_pairing(lt, node->child);
        parent->child = node;
    } else {
        mp_pairheap_t *n = parent->child;
        while (node != n->next) {
            n = n->next;
        }
        next = node->next;
        node->next = NULL;
        node = mp_pairheap_pairing(lt, node->child);
        if (node == NULL) {
            node = n;
        } else {
            n->next = node;
        }
    }
    node->next = next;
    if (NEXT_IS_RIGHTMOST_PARENT(next)) {
        parent->child_last = node;
    }
    return heap;
}
