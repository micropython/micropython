.section .init
.globl _start
.globl GET32
.globl PUT32
.globl dummy
.globl BOOTUP
.globl getSP
.globl setSP
.globl source

_start:
    mov r0, #0
    mov r1, #0
    bl main
    
loop:
    b loop
    
PUT32:
    str r1, [r0]
    mov pc, lr

GET32:
    ldr r0, [r0]
    mov pc, lr

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

