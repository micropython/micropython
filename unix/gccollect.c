#include <stdint.h>
#include <stdio.h>

#include "misc.h"
#include "mpconfig.h"
#include "gc.h"

#if MICROPY_ENABLE_GC

extern void *stack_top;

// We capture here callee-save registers, i.e. ones which may contain
// interesting values held there by our callers. It doesn't make sense
// to capture caller-saved registers, because they, well, put on the
// stack already by the caller.
#ifdef __x86_64__
typedef machine_uint_t regs_t[6];

void gc_helper_get_regs(regs_t arr) {
    register long rbx asm ("rbx");
    register long rbp asm ("rbp");
    register long r12 asm ("r12");
    register long r13 asm ("r13");
    register long r14 asm ("r14");
    register long r15 asm ("r15");
    arr[0] = rbx;
    arr[1] = rbp;
    arr[2] = r12;
    arr[3] = r13;
    arr[4] = r14;
    arr[5] = r15;
}
#endif

#ifdef __i386__
typedef machine_uint_t regs_t[4];

void gc_helper_get_regs(regs_t arr) {
    register long ebx asm ("ebx");
    register long esi asm ("esi");
    register long edi asm ("edi");
    register long ebp asm ("ebp");
    arr[0] = ebx;
    arr[1] = esi;
    arr[2] = edi;
    arr[3] = ebp;
}
#endif

void gc_collect(void) {
    //gc_dump_info();

    gc_collect_start();
    // this traces .data and .bss sections
    extern char __bss_start, _end;
    //printf(".bss: %p-%p\n", &__bss_start, &_end);
    gc_collect_root((void**)&__bss_start, ((machine_uint_t)&_end - (machine_uint_t)&__bss_start) / sizeof(machine_uint_t));
    regs_t regs;
    gc_helper_get_regs(regs);
    // GC stack (and regs because we captured them)
    gc_collect_root((void**)&regs, ((machine_uint_t)stack_top - (machine_uint_t)&regs) / sizeof(machine_uint_t));
    gc_collect_end();

    //printf("-----\n");
    //gc_dump_info();
}

#endif //MICROPY_ENABLE_GC
