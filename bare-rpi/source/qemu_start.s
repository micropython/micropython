.section .init
.globl _start
.globl dummy
.globl BOOTUP
.globl getSP
.globl setSP


_start:
    ldr sp, =stack_top
    
    bl main

loop:
    b loop

dummy:
    mov pc, lr
    
BOOTUP:
    push {lr}
    mov pc, r0
    b loop

getSP:
    mov r0, sp
    mov pc, lr

setSP:
    mov sp, r0
    mov pc, lr


