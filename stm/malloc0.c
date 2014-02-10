#include <stdint.h>
#include "std.h"
#include "mpconfig.h"
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

void __assert_func(void) {
    printf("\nASSERT FAIL!");
    for (;;) {
    }
}
