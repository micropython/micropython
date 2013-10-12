#include <stdint.h>
#include "std.h"

static uint32_t mem = 0;

void *malloc(size_t n) {
    if (mem == 0) {
        mem = 0x20008000; // need to use big ram block so we can execute code from it; start up a bit in case that's where bss is...?
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

void *calloc(size_t sz, size_t n) {
    char *ptr = malloc(sz * n);
    for (int i = 0; i < sz * n; i++) {
        ptr[i] = 0;
    }
    return ptr;
}

void *realloc(void *ptr, size_t n) {
    return malloc(n);
}

void __assert_func() {
    printf("\nASSERT FAIL!");
    for (;;) {
    }
}
