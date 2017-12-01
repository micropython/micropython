.global start
.section .init

start:
    ldr sp, =stack_top
    
    /// load C15 control register c1-c1,0,0 (c1 Control Register) into r0
    mrc p15, 0, r0, c1, c0, 0
    ldr r1, =0x00400000     /// set bit 22 (U-bit)
    orr r0, r1
    mcr p15, 0, r0, c1, c0, 0
    
    bl main

halt:
    b halt
    
