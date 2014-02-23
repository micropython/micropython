#include <stdlib.h>
#include <stm32f4xx.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "pendsv.h"

static mp_obj_t pendsv_object = MP_OBJ_NULL;

void pendsv_init(void) {
    // set PendSV interrupt at lowest priority
    NVIC_SetPriority(PendSV_IRQn, 0xff);
}

// call this function to raise a pending exception during an interrupt
// it will wait until all interrupts are finished then raise the given
// exception object using nlr_jump in the context of the top-level thread
void pendsv_nlr_jump(mp_obj_t o) {
    pendsv_object = o;
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

// since we play tricks with the stack, the compiler must not generate a
// prelude for this function
void pendsv_isr_handler(void) __attribute__((naked));

void pendsv_isr_handler(void) {
    // re-jig the stack so that when we return from this interrupt handler
    // it returns instead to nlr_jump with argument pendsv_object
    // note that stack has a different layout if DEBUG is enabled
    //
    // on entry to this (naked) function, stack has the following layout:
    //
    // stack layout with DEBUG disabled:
    //   sp[6]: pc
    //   sp[5]: ?
    //   sp[4]: ?
    //   sp[3]: ?
    //   sp[2]: ?
    //   sp[1]: ?
    //   sp[0]: r0
    //
    // stack layout with DEBUG enabled:
    //   sp[8]: pc
    //   sp[7]: lr
    //   sp[6]: ?
    //   sp[5]: ?
    //   sp[4]: ?
    //   sp[3]: ?
    //   sp[2]: r0
    //   sp[1]: 0xfffffff9
    //   sp[0]: ?

    __asm volatile (
        "ldr r0, pendsv_object_ptr\n"
        "ldr r0, [r0]\n"
#if defined(PENDSV_DEBUG)
        "str r0, [sp, #8]\n"
#else
        "str r0, [sp, #0]\n"
#endif
        "ldr r0, nlr_jump_ptr\n"
#if defined(PENDSV_DEBUG)
        "str r0, [sp, #32]\n"
#else
        "str r0, [sp, #24]\n"
#endif
        "bx lr\n"
        ".align 2\n"
        "pendsv_object_ptr: .word pendsv_object\n"
        "nlr_jump_ptr: .word nlr_jump\n"
    );

    /*
    uint32_t x[2] = {0x424242, 0xdeaddead};
    printf("PendSV: %p\n", x);
    for (uint32_t *p = (uint32_t*)(((uint32_t)x - 15) & 0xfffffff0), i = 64; i > 0; p += 4, i -= 4) {
        printf(" %p: %08x %08x %08x %08x\n", p, (uint)p[0], (uint)p[1], (uint)p[2], (uint)p[3]);
    }
    */
}
