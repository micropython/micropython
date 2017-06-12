.section ".init"

.globl start
.globl setSP
.globl getSP

start:
    ldr sp, =stack_top
    bl main

hang:
    b hang
    
getSP:
    mov r0, sp
    mov pc, lr

setSP:
    mov sp, r0
    mov pc, lr    
