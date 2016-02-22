    .syntax unified
    .cpu cortex-m4
    .thumb
    .text
    .align  2



@ uint gc_helper_get_sp(void)
    .global gc_helper_get_sp
    .thumb
    .thumb_func
    .type   gc_helper_get_sp, %function
gc_helper_get_sp:
    @ return the sp
    mov     r0, sp
    bx      lr



@ uint gc_helper_get_regs_and_sp(r0=uint regs[10])
    .global gc_helper_get_regs_and_sp
    .thumb
    .thumb_func
    .type   gc_helper_get_regs_and_sp, %function
gc_helper_get_regs_and_sp:
    @ store registers into given array
    str     r4, [r0], #4
    str     r5, [r0], #4
    str     r6, [r0], #4
    str     r7, [r0], #4
    str     r8, [r0], #4
    str     r9, [r0], #4
    str     r10, [r0], #4
    str     r11, [r0], #4
    str     r12, [r0], #4
    str     r13, [r0], #4

    @ return the sp
    mov     r0, sp
    bx      lr
