#include <stdio.h>
#include <stdlib.h>

#include "misc.h"

static int total_bytes_allocated = 0;

void m_free(void *ptr) {
    if (ptr != NULL) {
        free(ptr);
    }
}

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

void *m_realloc(void *ptr, int num_bytes) {
    if (num_bytes == 0) {
        free(ptr);
        return NULL;
    }
    ptr = realloc(ptr, num_bytes);
    if (ptr == NULL) {
        printf("could not allocate memory, reallocating %d bytes\n", num_bytes);
        return NULL;
    }
    total_bytes_allocated += num_bytes;
    return ptr;
}

int m_get_total_bytes_allocated(void) {
    return total_bytes_allocated;
}
