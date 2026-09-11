/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython Contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

    .section .start
    .type start, %function
    .globl start

start:
    /* Set up exception vector table */
    ldr x1, =exception_vectors
    msr vbar_el1, x1

    /* Enable FPU/NEON (CPACR_EL1: enable FP and SIMD) */
    mrs x0, cpacr_el1
    orr x0, x0, #0x300000
    msr cpacr_el1, x0
    isb

    /* Set up stack */
    movz x0, 0xbbbb, lsl #48
    movk x0, 0xbbbb, lsl #32
    movk x0, 0xbbbb, lsl #16
    movk x0, 0xbbbb, lsl #0
    ldr x1, =_sstack
    ldr x2, =_estack
2:
    str x0, [x1]
    add x1, x1, #8
    cmp x1, x2
    blt 2b

    ldr x9, =_estack
    mov sp, x9

    /* Clear BSS section */
    ldr x1, =_sbss
    ldr x2, =_ebss
3:
    str wzr, [x1]
    add x1, x1, #4
    cmp x1, x2
    blt 3b

    bl _entry_point

    .size start, .-start

    /* Exception vector table (aligned to 0x80 boundary for AArch64)
     * Each entry must be 0x80 bytes apart
     * Layout: 0x000-0x1FF: Current EL with SP0
     *         0x200-0x3FF: Current EL with SPx
     *         0x400-0x5FF: Lower EL using AArch64  <- EL0->EL1 exceptions
     *         0x600-0x7FF: Lower EL using AArch32
     */
    .balign 0x800
exception_vectors:
    /* 0x000: Synchronous exception from current EL with SP0 */
    b Sync_EL1T_Handler
    .balign 0x80
    /* 0x080: IRQ from current EL with SP0 */
    b IRQ_EL1_Handler
    .balign 0x80
    /* 0x100: FIQ from current EL with SP0 */
    b FIQ_EL1_Handler
    .balign 0x80
    /* 0x180: SError from current EL with SP0 */
    b SError_Handler
    .balign 0x80
    /* 0x200: Synchronous exception from current EL with SPx */
    b Sync_EL1H_Handler
    .balign 0x80
    /* 0x280: IRQ from current EL with SPx */
    b IRQ_EL1_Handler
    .balign 0x80
    /* 0x300: FIQ from current EL with SPx */
    b FIQ_EL1_Handler
    .balign 0x80
    /* 0x380: SError from current EL with SPx */
    b SError_Handler
    .balign 0x80
    /* 0x400: Synchronous exception from lower EL using AArch64 */
    b Sync_EL0_64_Handler
    .balign 0x80
    /* 0x480: IRQ from lower EL using AArch64 */
    b IRQ_EL0_Handler
    .balign 0x80
    /* 0x500: FIQ from lower EL using AArch64 */
    b FIQ_EL0_Handler
    .balign 0x80
    /* 0x580: SError from lower EL using AArch64 */
    b SError_Handler
    .balign 0x80
    /* 0x600: Synchronous exception from lower EL using AArch32 */
    b Sync_EL0_32_Handler
    .balign 0x80
    /* 0x680: IRQ from lower EL using AArch32 */
    b IRQ_EL0_Handler
    .balign 0x80
    /* 0x700: FIQ from lower EL using AArch32 */
    b FIQ_EL0_Handler
    .balign 0x80
    /* 0x780: SError from lower EL using AArch32 */
    b SError_Handler

    /* Exception handlers */
    .extern exception_registers
    .extern exception_handler_main

    .macro save_exception_state kind
    /* Use X18 as scratch for the base address, saving its original value first. */
    stp x18, x30, [sp, #-16]!

    /* Load base address into X18 */
    ldr x18, =exception_registers

    /* Save X0-X30 to exception_registers[0-30] */
    stp x0, x1, [x18, #0]
    stp x2, x3, [x18, #16]
    stp x4, x5, [x18, #32]
    stp x6, x7, [x18, #48]
    stp x8, x9, [x18, #64]
    stp x10, x11, [x18, #80]
    stp x12, x13, [x18, #96]
    stp x14, x15, [x18, #112]
    stp x16, x17, [x18, #128]

    /* Restore original X18 from stack and save it */
    ldp x2, x30, [sp], #16
    mov sp, x2
    str x2, [x18, #144]          /* Save original X18 */

    stp x19, x20, [x18, #152]
    stp x21, x22, [x18, #168]
    stp x23, x24, [x18, #184]
    stp x25, x26, [x18, #200]
    stp x27, x28, [x18, #216]
    stp x29, x30, [x18, #232]

    /* Save SP to exception_registers[31] */
    mov x2, sp
    str x2, [x18, #248]

    /* Save ESR_EL1 to exception_registers[32] */
    mrs x2, esr_el1
    str x2, [x18, #256]

    /* Save FAR_EL1 to exception_registers[33] */
    mrs x2, far_el1
    str x2, [x18, #264]

    /* Save ELR_EL1 to exception_registers[34] */
    mrs x2, elr_el1
    str x2, [x18, #272]

    /* Save SPSR_EL1 to exception_registers[35] */
    mrs x2, spsr_el1
    str x2, [x18, #280]

    /* Call exception_handler_main(kind) */
    mov x0, #\kind
    bl exception_handler_main

    /* Restore and return (for IRQ/FIQ) or hang (for sync exceptions) */
    b restore_exception_state
    .endm

restore_exception_state:
    ldr x18, =exception_registers

    /* Restore exception state while X18 still holds the register-save base. */
    ldr x2, [x18, #280]
    msr spsr_el1, x2
    ldr x2, [x18, #272]
    msr elr_el1, x2
    ldr x2, [x18, #248]
    mov sp, x2

    ldp x0, x1, [x18, #0]
    ldp x2, x3, [x18, #16]
    ldp x4, x5, [x18, #32]
    ldp x6, x7, [x18, #48]
    ldp x8, x9, [x18, #64]
    ldp x10, x11, [x18, #80]
    ldp x12, x13, [x18, #96]
    ldp x14, x15, [x18, #112]
    ldp x16, x17, [x18, #128]
    ldp x19, x20, [x18, #152]
    ldp x21, x22, [x18, #168]
    ldp x23, x24, [x18, #184]
    ldp x25, x26, [x18, #200]
    ldp x27, x28, [x18, #216]
    ldp x29, x30, [x18, #232]

    /* Restore X18 last because it holds the register-save base. */
    ldr x18, [x18, #144]

    eret

    .type Sync_EL1T_Handler, %function
Sync_EL1T_Handler:
    save_exception_state 0
    .size Sync_EL1T_Handler, .-Sync_EL1T_Handler

    .type Sync_EL1H_Handler, %function
Sync_EL1H_Handler:
    save_exception_state 1
    .size Sync_EL1H_Handler, .-Sync_EL1H_Handler

    .type Sync_EL0_64_Handler, %function
Sync_EL0_64_Handler:
    save_exception_state 2
    .size Sync_EL0_64_Handler, .-Sync_EL0_64_Handler

    .type Sync_EL0_32_Handler, %function
Sync_EL0_32_Handler:
    save_exception_state 3
    .size Sync_EL0_32_Handler, .-Sync_EL0_32_Handler

    .type IRQ_EL1_Handler, %function
IRQ_EL1_Handler:
    save_exception_state 4
    .size IRQ_EL1_Handler, .-IRQ_EL1_Handler

    .type IRQ_EL0_Handler, %function
IRQ_EL0_Handler:
    save_exception_state 5
    .size IRQ_EL0_Handler, .-IRQ_EL0_Handler

    .type FIQ_EL1_Handler, %function
FIQ_EL1_Handler:
    save_exception_state 6
    .size FIQ_EL1_Handler, .-FIQ_EL1_Handler

    .type FIQ_EL0_Handler, %function
FIQ_EL0_Handler:
    save_exception_state 7
    .size FIQ_EL0_Handler, .-FIQ_EL0_Handler

    .type SError_Handler, %function
SError_Handler:
    save_exception_state 8
    .size SError_Handler, .-SError_Handler
