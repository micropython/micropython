    .syntax unified
    .cpu cortex-m4
    .thumb
    .text
    .align  2
    .global delay_ms
    .thumb
    .thumb_func
    .type   delay_ms, %function
@ void delay_ms(int ms)
delay_ms:
    @ r0 is argument, trashes r2, r3
    adds    r3, r0, #0
    b       .L2
.L5:
    movw    r2, #55999
    b       .L3
.L4:
    subs    r2, r2, #1
.L3:
    cmp     r2, #0
    bgt     .L4
    subs    r3, r3, #1
.L2:
    cmp     r3, #0
    bgt     .L5
    bx      lr
    .size   delay_ms, .-delay_ms
