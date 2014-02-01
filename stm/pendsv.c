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

void pendsv_isr_handler(void) {
    // re-jig the stack so that when we return from this interrupt handler
    // it returns instead to nlr_jump with argument pendsv_object
    __asm volatile (
        "ldr r0, pendsv_object_ptr\n"
        "ldr r0, [r0]\n"
        "str r0, [sp, #0]\n"
        "ldr r0, nlr_jump_ptr\n"
        "str r0, [sp, #24]\n"
        "bx lr\n"
        ".align 2\n"
        "pendsv_object_ptr: .word pendsv_object\n"
        "nlr_jump_ptr: .word nlr_jump\n"
    );

    /*
    sp[0] = r0 = exception to raise
    sp[6] = pc = nlr_jump
    uint32_t x[2] = {0x424242, 0xdeaddead};
    printf("PendSV: %p\n", x);
    for (uint32_t *p = (uint32_t*)(((uint32_t)x - 15) & 0xfffffff0), i = 64; i > 0; p += 4, i -= 4) {
        printf(" %p: %08x %08x %08x %08x\n", p, p[0], p[1], p[2], p[3]);
    }
    */
}
