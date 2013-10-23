#include <stdint.h>
#include "std.h"
#include "mpyconfig.h"
#include "gc.h"

#if 0
static uint32_t mem = 0;

void *malloc(size_t n) {
    if (mem == 0) {
        extern uint32_t _heap_start;
        mem = (uint32_t)&_heap_start; // need to use big ram block so we can execute code from it (is it true that we can't execute from CCM?)
    }
    void *ptr = (void*)mem;
    mem = (mem + n + 3) & (~3);
    if (mem > 0x20000000 + 0x18000) {
        void __fatal_error(const char*);
        __fatal_error("out of memory");
    }
    return ptr;
}

void free(void *ptr) {
}

void *realloc(void *ptr, size_t n) {
    return malloc(n);
}

#endif

void *calloc(size_t sz, size_t n) {
    char *ptr = malloc(sz * n);
    for (int i = 0; i < sz * n; i++) {
        ptr[i] = 0;
    }
    return ptr;
}

void *malloc(size_t n) {
    return gc_alloc(n);
}

void free(void *ptr) {
    gc_free(ptr);
}

void *realloc(void *ptr, size_t n) {
    return gc_realloc(ptr, n);
}

void __assert_func(void) {
    printf("\nASSERT FAIL!");
    for (;;) {
    }
}
