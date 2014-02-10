#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "mpconfig.h"

#if 0 // print debugging info
#define DEBUG_printf(args...) printf(args)
#else // don't print debugging info
#define DEBUG_printf(args...) (void)0
#endif

#if MICROPY_MEM_STATS
static int total_bytes_allocated = 0;
static int current_bytes_allocated = 0;
static int peak_bytes_allocated = 0;

#define UPDATE_PEAK() { if (current_bytes_allocated > peak_bytes_allocated) peak_bytes_allocated = current_bytes_allocated; }
#endif

void *m_malloc(int num_bytes) {
    if (num_bytes == 0) {
        return NULL;
    }
    void *ptr = malloc(num_bytes);
    if (ptr == NULL) {
        printf("could not allocate memory, allocating %d bytes\n", num_bytes);
        return NULL;
    }
#if MICROPY_MEM_STATS
    total_bytes_allocated += num_bytes;
    current_bytes_allocated += num_bytes;
    UPDATE_PEAK();
#endif
    DEBUG_printf("malloc %d : %p\n", num_bytes, ptr);
    return ptr;
}

void *m_malloc0(int num_bytes) {
    void *ptr = m_malloc(num_bytes);
    if (ptr != NULL) {
        memset(ptr, 0, num_bytes);
    }
    return ptr;
}

void *m_realloc(void *ptr, int old_num_bytes, int new_num_bytes) {
    if (new_num_bytes == 0) {
        free(ptr);
        return NULL;
    }
    void *new_ptr = realloc(ptr, new_num_bytes);
    if (new_ptr == NULL) {
        printf("could not allocate memory, reallocating %d bytes\n", new_num_bytes);
        return NULL;
    }
#if MICROPY_MEM_STATS
    // At first thought, "Total bytes allocated" should only grow,
    // after all, it's *total*. But consider for example 2K block
    // shrunk to 1K and then grown to 2K again. It's still 2K
    // allocated total. If we process only positive increments,
    // we'll count 3K.
    int diff = new_num_bytes - old_num_bytes;
    total_bytes_allocated += diff;
    current_bytes_allocated += diff;
    UPDATE_PEAK();
#endif
    DEBUG_printf("realloc %p, %d, %d : %p\n", ptr, old_num_bytes, new_num_bytes, new_ptr);
    return new_ptr;
}

void m_free(void *ptr, int num_bytes) {
    if (ptr != NULL) {
        free(ptr);
    }
#if MICROPY_MEM_STATS
    current_bytes_allocated -= num_bytes;
#endif
    DEBUG_printf("free %p, %d\n", ptr, num_bytes);
}

int m_get_total_bytes_allocated(void) {
#if MICROPY_MEM_STATS
    return total_bytes_allocated;
#else
    return -1;
#endif
}

int m_get_current_bytes_allocated(void) {
#if MICROPY_MEM_STATS
    return current_bytes_allocated;
#else
    return -1;
#endif
}

int m_get_peak_bytes_allocated(void) {
#if MICROPY_MEM_STATS
    return peak_bytes_allocated;
#else
    return -1;
#endif
}
