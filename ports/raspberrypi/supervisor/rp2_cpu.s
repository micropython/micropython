.syntax unified
.cpu cortex-m0
.thumb
.text
.align  2

@ uint cpu_get_regs_and_sp(r0=uint regs[10])
.global cpu_get_regs_and_sp
.thumb
.thumb_func
.type   cpu_get_regs_and_sp, %function
cpu_get_regs_and_sp:
@ store registers into given array
str     r4, [r0, #0]
str     r5, [r0, #4]
str     r6, [r0, #8]
str     r7, [r0, #12]
push {r1}
mov     r1, r8
str     r1, [r0, #16]
mov     r1, r9
str     r1, [r0, #20]
mov     r1, r10
str     r1, [r0, #24]
mov     r1, r11
str     r1, [r0, #28]
mov     r1, r12
str     r1, [r0, #32]
mov     r1, r13
str     r1, [r0, #36]
pop {r1}

@ return the sp
mov     r0, sp
bx      lr
