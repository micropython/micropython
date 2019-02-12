    .syntax unified
    .cpu cortex-m4
    .thumb
    .text
    .align  2

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


@ this next function is now obsolete

    .size   gc_helper_get_regs_and_clean_stack, .-gc_helper_get_regs_and_clean_stack
@ void gc_helper_get_regs_and_clean_stack(r0=uint regs[10], r1=heap_end)
    .global gc_helper_get_regs_and_clean_stack
    .thumb
    .thumb_func
    .type   gc_helper_get_regs_and_clean_stack, %function
gc_helper_get_regs_and_clean_stack:
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

    @ clean the stack from given pointer up to current sp
    movs    r0, #0
    mov     r2, sp
    b.n     .entry
.loop:
    str     r0, [r1], #4
.entry:
    cmp     r1, r2
    bcc.n   .loop
    bx      lr

    .size   gc_helper_get_regs_and_clean_stack, .-gc_helper_get_regs_and_clean_stack
