#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

static int total_bytes_allocated = 0;

void *m_malloc(int num_bytes) {
    if (num_bytes == 0) {
        return NULL;
    }
    void *ptr = malloc(num_bytes);
    if (ptr == NULL) {
        printf("could not allocate memory, allocating %d bytes\n", num_bytes);
        return NULL;
    }
    total_bytes_allocated += num_bytes;
    return ptr;
}

void *m_malloc0(int num_bytes) {
    if (num_bytes == 0) {
        return NULL;
    }
    void *ptr = calloc(1, num_bytes);
    if (ptr == NULL) {
        printf("could not allocate memory, allocating %d bytes\n", num_bytes);
        return NULL;
    }
    total_bytes_allocated += num_bytes;
    return ptr;
}

void *m_realloc(void *ptr, int old_num_bytes, int new_num_bytes) {
    if (new_num_bytes == 0) {
        free(ptr);
        return NULL;
    }
    ptr = realloc(ptr, new_num_bytes);
    if (ptr == NULL) {
        printf("could not allocate memory, reallocating %d bytes\n", new_num_bytes);
        return NULL;
    }
    // At first thought, "Total bytes allocated" should only grow,
    // after all, it's *total*. But consider for example 2K block
    // shrunk to 1K and then grown to 2K again. It's still 2K
    // allocated total. If we process only positive increments,
    // we'll count 3K.
    total_bytes_allocated += new_num_bytes - old_num_bytes;
    return ptr;
}

void m_free(void *ptr, int num_bytes) {
    if (ptr != NULL) {
        free(ptr);
    }
}

int m_get_total_bytes_allocated(void) {
    return total_bytes_allocated;
}
