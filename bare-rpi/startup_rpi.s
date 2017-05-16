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
    ldr pc, _reset_h
    ldr pc, _undefined_instruction_vector_h
    ldr pc, _software_interrupt_vector_h
    ldr pc, _prefetch_abort_vector_h
    ldr pc, _data_abort_vector_h
    ldr pc, _unused_handler_h
    ldr pc, _interrupt_vector_h
    ldr pc, _fast_interrupt_vector_h

_reset_h:                           .word   _reset_
_undefined_instruction_vector_h:    .word   undef_vector
_software_interrupt_vector_h:       .word   swi_vector
_prefetch_abort_vector_h:           .word   abort_vector
_data_abort_vector_h:               .word   abort_vector
_unused_handler_h:                  .word   _reset_
_interrupt_vector_h:                .word   irq_vector
_fast_interrupt_vector_h:           .word   fiq_vector

_reset_:
    mov     r0, #0x8000
    mov     r1, #0x0000
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}    /** load 32 Byte worth of data **/
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}    /** store 32 Byte **/
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
    
    bl enable_interrupts
    
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
    
enable_interrupts:
        mrs     r0, cpsr
        bic     r0, r0, #0x80
        msr     cpsr_c, r0

        mov     pc, lr
    

