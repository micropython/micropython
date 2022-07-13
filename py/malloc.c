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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/mpstate.h"

#if MICROPY_DEBUG_VERBOSE // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

#if MICROPY_MEM_STATS
#if !MICROPY_MALLOC_USES_ALLOCATED_SIZE
#error MICROPY_MEM_STATS requires MICROPY_MALLOC_USES_ALLOCATED_SIZE
#endif
#define UPDATE_PEAK() { if (MP_STATE_MEM(current_bytes_allocated) > MP_STATE_MEM(peak_bytes_allocated)) MP_STATE_MEM(peak_bytes_allocated) = MP_STATE_MEM(current_bytes_allocated); }
#endif

#if MICROPY_ENABLE_GC
#include "py/gc.h"

// We redirect standard alloc functions to GC heap - just for the rest of
// this module. In the rest of MicroPython source, system malloc can be
// freely accessed - for interfacing with system and 3rd-party libs for
// example. On the other hand, some (e.g. bare-metal) ports may use GC
// heap as system heap, so, to avoid warnings, we do undef's first.
#undef malloc
#undef free
#undef realloc
#define malloc(b) gc_alloc((b), false)
#define malloc_with_finaliser(b) gc_alloc((b), true)
#define free gc_free
#define realloc(ptr, n) gc_realloc(ptr, n, true)
#define realloc_ext(ptr, n, mv) gc_realloc(ptr, n, mv)
#else

// GC is disabled.  Use system malloc/realloc/free.

#if MICROPY_ENABLE_FINALISER
#error MICROPY_ENABLE_FINALISER requires MICROPY_ENABLE_GC
#endif

STATIC void *realloc_ext(void *ptr, size_t n_bytes, bool allow_move) {
    if (allow_move) {
        return realloc(ptr, n_bytes);
    } else {
        // We are asked to resize, but without moving the memory region pointed to
        // by ptr.  Unless the underlying memory manager has special provision for
        // this behaviour there is nothing we can do except fail to resize.
        return NULL;
    }
}

#endif // MICROPY_ENABLE_GC

void *m_malloc(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    if (ptr == NULL && num_bytes != 0) {
        m_malloc_fail(num_bytes);
    }
    #if MICROPY_MEM_STATS
    MP_STATE_MEM(total_bytes_allocated) += num_bytes;
    MP_STATE_MEM(current_bytes_allocated) += num_bytes;
    UPDATE_PEAK();
    #endif
    DEBUG_printf("malloc %d : %p\n", num_bytes, ptr);
    return ptr;
}

void *m_malloc_maybe(size_t num_bytes) {
    void *ptr = malloc(num_bytes);
    #if MICROPY_MEM_STATS
    MP_STATE_MEM(total_bytes_allocated) += num_bytes;
    MP_STATE_MEM(current_bytes_allocated) += num_bytes;
    UPDATE_PEAK();
    #endif
    DEBUG_printf("malloc %d : %p\n", num_bytes, ptr);
    return ptr;
}

#if MICROPY_ENABLE_FINALISER
void *m_malloc_with_finaliser(size_t num_bytes) {
    void *ptr = malloc_with_finaliser(num_bytes);
    if (ptr == NULL && num_bytes != 0) {
        m_malloc_fail(num_bytes);
    }
    #if MICROPY_MEM_STATS
    MP_STATE_MEM(total_bytes_allocated) += num_bytes;
    MP_STATE_MEM(current_bytes_allocated) += num_bytes;
    UPDATE_PEAK();
    #endif
    DEBUG_printf("malloc %d : %p\n", num_bytes, ptr);
    return ptr;
}
#endif

void *m_malloc0(size_t num_bytes) {
    void *ptr = m_malloc(num_bytes);
    // If this config is set then the GC clears all memory, so we don't need to.
    #if !MICROPY_GC_CONSERVATIVE_CLEAR
    memset(ptr, 0, num_bytes);
    #endif
    return ptr;
}

#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
void *m_realloc(void *ptr, size_t old_num_bytes, size_t new_num_bytes)
#else
void *m_realloc(void *ptr, size_t new_num_bytes)
#endif
{
    void *new_ptr = realloc(ptr, new_num_bytes);
    if (new_ptr == NULL && new_num_bytes != 0) {
        m_malloc_fail(new_num_bytes);
    }
    #if MICROPY_MEM_STATS
    // At first thought, "Total bytes allocated" should only grow,
    // after all, it's *total*. But consider for example 2K block
    // shrunk to 1K and then grown to 2K again. It's still 2K
    // allocated total. If we process only positive increments,
    // we'll count 3K.
    size_t diff = new_num_bytes - old_num_bytes;
    MP_STATE_MEM(total_bytes_allocated) += diff;
    MP_STATE_MEM(current_bytes_allocated) += diff;
    UPDATE_PEAK();
    #endif
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    DEBUG_printf("realloc %p, %d, %d : %p\n", ptr, old_num_bytes, new_num_bytes, new_ptr);
    #else
    DEBUG_printf("realloc %p, %d : %p\n", ptr, new_num_bytes, new_ptr);
    #endif
    return new_ptr;
}

#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
void *m_realloc_maybe(void *ptr, size_t old_num_bytes, size_t new_num_bytes, bool allow_move)
#else
void *m_realloc_maybe(void *ptr, size_t new_num_bytes, bool allow_move)
#endif
{
    void *new_ptr = realloc_ext(ptr, new_num_bytes, allow_move);
    #if MICROPY_MEM_STATS
    // At first thought, "Total bytes allocated" should only grow,
    // after all, it's *total*. But consider for example 2K block
    // shrunk to 1K and then grown to 2K again. It's still 2K
    // allocated total. If we process only positive increments,
    // we'll count 3K.
    // Also, don't count failed reallocs.
    if (!(new_ptr == NULL && new_num_bytes != 0)) {
        size_t diff = new_num_bytes - old_num_bytes;
        MP_STATE_MEM(total_bytes_allocated) += diff;
        MP_STATE_MEM(current_bytes_allocated) += diff;
        UPDATE_PEAK();
    }
    #endif
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    DEBUG_printf("realloc %p, %d, %d : %p\n", ptr, old_num_bytes, new_num_bytes, new_ptr);
    #else
    DEBUG_printf("realloc %p, %d, %d : %p\n", ptr, new_num_bytes, new_ptr);
    #endif
    return new_ptr;
}

#if MICROPY_MALLOC_USES_ALLOCATED_SIZE
void m_free(void *ptr, size_t num_bytes)
#else
void m_free(void *ptr)
#endif
{
    free(ptr);
    #if MICROPY_MEM_STATS
    MP_STATE_MEM(current_bytes_allocated) -= num_bytes;
    #endif
    #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
    DEBUG_printf("free %p, %d\n", ptr, num_bytes);
    #else
    DEBUG_printf("free %p\n", ptr);
    #endif
}

#if MICROPY_TRACKED_ALLOC

#define MICROPY_TRACKED_ALLOC_STORE_SIZE (!MICROPY_ENABLE_GC)

typedef struct _m_tracked_node_t {
    struct _m_tracked_node_t *prev;
    struct _m_tracked_node_t *next;
    #if MICROPY_TRACKED_ALLOC_STORE_SIZE
    uintptr_t size;
    #endif
    uint8_t data[];
} m_tracked_node_t;

#if MICROPY_DEBUG_VERBOSE
STATIC size_t m_tracked_count_links(size_t *nb) {
    m_tracked_node_t *node = MP_STATE_VM(m_tracked_head);
    size_t n = 0;
    *nb = 0;
    while (node != NULL) {
        ++n;
        #if MICROPY_TRACKED_ALLOC_STORE_SIZE
        *nb += node->size;
        #else
        *nb += gc_nbytes(node);
        #endif
        node = node->next;
    }
    return n;
}
#endif

void *m_tracked_calloc(size_t nmemb, size_t size) {
    m_tracked_node_t *node = m_malloc_maybe(sizeof(m_tracked_node_t) + nmemb * size);
    if (node == NULL) {
        return NULL;
    }
    #if MICROPY_DEBUG_VERBOSE
    size_t nb;
    size_t n = m_tracked_count_links(&nb);
    DEBUG_printf("m_tracked_calloc(%u, %u) -> (%u;%u) %p\n", (int)nmemb, (int)size, (int)n, (int)nb, node);
    #endif
    if (MP_STATE_VM(m_tracked_head) != NULL) {
        MP_STATE_VM(m_tracked_head)->prev = node;
    }
    node->prev = NULL;
    node->next = MP_STATE_VM(m_tracked_head);
    MP_STATE_VM(m_tracked_head) = node;
    #if MICROPY_TRACKED_ALLOC_STORE_SIZE
    node->size = nmemb * size;
    #endif
    #if !MICROPY_GC_CONSERVATIVE_CLEAR
    memset(&node->data[0], 0, nmemb * size);
    #endif
    return &node->data[0];
}

void m_tracked_free(void *ptr_in) {
    if (ptr_in == NULL) {
        return;
    }
    m_tracked_node_t *node = (m_tracked_node_t *)((uint8_t *)ptr_in - sizeof(m_tracked_node_t));
    #if MICROPY_DEBUG_VERBOSE
    size_t data_bytes;
    #if MICROPY_TRACKED_ALLOC_STORE_SIZE
    data_bytes = node->size;
    #else
    data_bytes = gc_nbytes(node);
    #endif
    size_t nb;
    size_t n = m_tracked_count_links(&nb);
    DEBUG_printf("m_tracked_free(%p, [%p, %p], nbytes=%u, links=%u;%u)\n", node, node->prev, node->next, (int)data_bytes, (int)n, (int)nb);
    #endif
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    if (node->prev != NULL) {
        node->prev->next = node->next;
    } else {
        MP_STATE_VM(m_tracked_head) = node->next;
    }
    m_free(node
        #if MICROPY_MALLOC_USES_ALLOCATED_SIZE
        #if MICROPY_TRACKED_ALLOC_STORE_SIZE
        , node->size
        #else
        , gc_nbytes(node)
        #endif
        #endif
        );
}

#endif // MICROPY_TRACKED_ALLOC

#if MICROPY_MEM_STATS
size_t m_get_total_bytes_allocated(void) {
    return MP_STATE_MEM(total_bytes_allocated);
}

size_t m_get_current_bytes_allocated(void) {
    return MP_STATE_MEM(current_bytes_allocated);
}

size_t m_get_peak_bytes_allocated(void) {
    return MP_STATE_MEM(peak_bytes_allocated);
}
#endif
