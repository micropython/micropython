.section .init
.globl _start
.globl dummy
.globl BOOTUP
.globl getSP
.globl setSP


irq_vector_start:
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
    ldr sp, =stack_top
    
    /// copies interrupt vectors to address 0x0000 0000 
    ldr r0, _start
    //mov     r0, #0x8000
    ldr     r1, =irq_vector_start
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}    /** load 32 Byte worth of data **/
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}    /** store 32 Byte **/
    ldmia   r0!,{r2, r3, r4, r5, r6, r7, r8, r9}
    stmia   r1!,{r2, r3, r4, r5, r6, r7, r8, r9}
    
    mrs r0, cpsr
    bic r1, r0, #0x1f
    orr r1, r1, #0x12   // IRQ mode
    msr cpsr, r1
    ldr sp, =irq_stack_top
    
    bic r1, r1, #0x02   // FIQ mode
    orr r1, r1, #0x01
    msr cpsr, r1
    ldr sp, =fiq_stack_top
    
    bic r0, r0, #0x80       /// enable interrupts and return to supervisor mode
    msr cpsr, r0
    
   /* 
    mov r0, #0
rs:
    cmp r0, #0
    beq rs*/
        
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


