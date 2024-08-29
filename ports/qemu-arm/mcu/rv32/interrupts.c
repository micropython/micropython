/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Alessandro Gatti
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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Vector table

void mtvec_table(void) __attribute__((naked, section(".text.mtvec"), aligned(256)));

// Default interrupts

#define ASSIGN_EMPTY_MACHINE_INTERRUPT(interrupt_name) \
    void interrupt_name(void) __attribute__((alias("mtvec_nop")))

ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_ssi);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_msi);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_sti);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_mti);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_sei);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_mei);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq0);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq1);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq2);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq3);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq4);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq5);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq6);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq7);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq8);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq9);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq10);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq11);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq12);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq13);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq14);
ASSIGN_EMPTY_MACHINE_INTERRUPT(mtvec_plat_irq15);

void mtvec_table(void) {
    __asm volatile (
        ".org mtvec_table + 0       \n"
        "jal  zero, mtvec_exception \n" // Exception Handler
        ".org mtvec_table + 4       \n"
        "jal  zero, mtvec_ssi       \n" // Supervisor Software Interrupt
        ".org mtvec_table + 12      \n"
        "jal  zero, mtvec_msi       \n" // Machine Software Interrupt
        ".org mtvec_table + 20      \n"
        "jal  zero, mtvec_sti       \n" // Supervisor Timer Interrupt
        ".org mtvec_table + 28      \n"
        "jal  zero, mtvec_mti       \n" // Machine Timer Interrupt
        ".org mtvec_table + 36      \n"
        "jal  zero, mtvec_sei       \n" // Supervisor External Interrupt
        ".org mtvec_table + 44      \n"
        "jal  zero, mtvec_mei       \n" // Machine External Interrupt
        // Not sure how many platform interrupts QEMU handles...
        ".org mtvec_table + 48      \n"
        "jal  mtvec_plat_irq0       \n" // Platform Interrupt #0
        "jal  mtvec_plat_irq1       \n" // Platform Interrupt #1
        "jal  mtvec_plat_irq2       \n" // Platform Interrupt #2
        "jal  mtvec_plat_irq3       \n" // Platform Interrupt #3
        "jal  mtvec_plat_irq4       \n" // Platform Interrupt #4
        "jal  mtvec_plat_irq5       \n" // Platform Interrupt #5
        "jal  mtvec_plat_irq6       \n" // Platform Interrupt #6
        "jal  mtvec_plat_irq7       \n" // Platform Interrupt #7
        "jal  mtvec_plat_irq8       \n" // Platform Interrupt #8
        "jal  mtvec_plat_irq9       \n" // Platform Interrupt #9
        "jal  mtvec_plat_irq10      \n" // Platform Interrupt #10
        "jal  mtvec_plat_irq11      \n" // Platform Interrupt #11
        "jal  mtvec_plat_irq12      \n" // Platform Interrupt #12
        "jal  mtvec_plat_irq13      \n" // Platform Interrupt #13
        "jal  mtvec_plat_irq14      \n" // Platform Interrupt #14
        "jal  mtvec_plat_irq15      \n" // Platform Interrupt #15
        :
        :
        : "memory"
        );
}

volatile uint32_t registers_copy[35] = { 0 };

const char *exception_causes[] = {
    "Reserved",                       // 0
    "Supervisor software interrupt",  // 1
    "Machine software interrupt",     // 2
    "Supervisor timer interrupt",     // 3
    "Machine timer interrupt",        // 4
    "Supervisor external interrupt",  // 5
    "Machine external interrupt",     // 6
    "Designated for platform use",    // 7
    "Instruction address misaligned", // 8
    "Instruction address fault",      // 9
    "Illegal instruction",            // 10
    "Breakpoint",                     // 11
    "Load address misaligned",        // 12
    "Load address fault",             // 13
    "Store/AMO address misaligned",   // 14
    "Store/AMO access fault",         // 15
    "Environment call from U-mode",   // 16
    "Environment call from S-mode",   // 17
    "Environment call from M-mode",   // 18
    "Instruction page fault",         // 19
    "Load page fault",                // 20
    "Store/AMO page fault",           // 21
    "Designated for custom use"       // 22
};

const char *lookup_cause(uint32_t mcause) {
    if (mcause & 0x80000000) {
        switch (mcause & 0x7FFFFFFF) {
            case 1:
                return exception_causes[1];
            case 3:
                return exception_causes[2];
            case 5:
                return exception_causes[3];
            case 7:
                return exception_causes[4];
            case 9:
                return exception_causes[5];
            case 11:
                return exception_causes[6];
            default:
                return (mcause >= 16) ?
                       exception_causes[7] :
                       exception_causes[0];
        }
    }

    switch (mcause) {
        case 0:
            return exception_causes[8];
        case 1:
            return exception_causes[9];
        case 2:
            return exception_causes[10];
        case 3:
            return exception_causes[11];
        case 4:
            return exception_causes[12];
        case 5:
            return exception_causes[13];
        case 6:
            return exception_causes[14];
        case 7:
            return exception_causes[15];
        case 8:
            return exception_causes[16];
        case 9:
            return exception_causes[17];
        case 11:
            return exception_causes[18];
        case 12:
            return exception_causes[19];
        case 13:
            return exception_causes[20];
        case 15:
            return exception_causes[21];
        default: {
            if ((mcause >= 24 && mcause <= 31) ||
                (mcause >= 48 && mcause <= 63)) {
                return exception_causes[22];
            }

            return exception_causes[0];
        }
    }
}

#pragma GCC push_options
#pragma GCC optimize ("align-functions=4")

__attribute__((interrupt("machine"), weak)) void mtvec_nop(void) {
}

__attribute__((interrupt("machine"), weak)) void mtvec_exception(void) {
    __asm volatile (
        "csrrw x31, mscratch, x31  \n" // Save X31
        "la    x31, registers_copy \n" // Load target address
        "sw    x1, 0(x31)          \n" // Save X1
        "sw    x2, 4(x31)          \n" // Save X2
        "sw    x3, 8(x31)          \n" // Save X3
        "sw    x4, 12(x31)         \n" // Save X4
        "sw    x5, 16(x31)         \n" // Save X5
        "sw    x6, 20(x31)         \n" // Save X6
        "sw    x7, 24(x31)         \n" // Save X7
        "sw    x8, 28(x31)         \n" // Save X8
        "sw    x9, 32(x31)         \n" // Save X9
        "sw    x10, 36(x31)        \n" // Save X10
        "sw    x11, 40(x31)        \n" // Save X11
        "sw    x12, 44(x31)        \n" // Save X12
        "sw    x13, 48(x31)        \n" // Save X13
        "sw    x14, 52(x31)        \n" // Save X14
        "sw    x15, 56(x31)        \n" // Save X15
        "sw    x16, 60(x31)        \n" // Save X16
        "sw    x17, 64(x31)        \n" // Save X17
        "sw    x18, 68(x31)        \n" // Save X18
        "sw    x19, 72(x31)        \n" // Save X19
        "sw    x20, 76(x31)        \n" // Save X20
        "sw    x21, 80(x31)        \n" // Save X21
        "sw    x22, 84(x31)        \n" // Save X22
        "sw    x23, 88(x31)        \n" // Save X23
        "sw    x24, 92(x31)        \n" // Save X24
        "sw    x25, 98(x31)        \n" // Save X25
        "sw    x26, 100(x31)       \n" // Save X26
        "sw    x27, 104(x31)       \n" // Save X27
        "sw    x28, 108(x31)       \n" // Save X28
        "sw    x29, 112(x31)       \n" // Save X29
        "sw    x30, 116(x31)       \n" // Save X30
        "csrr  x30, mscratch       \n" // Restore X31
        "sw    x30, 120(x31)       \n" // Save X31
        "csrr  x30, mepc           \n" // Load MEPC
        "sw    x30, 124(x31)       \n" // Save MEPC
        "csrr  x30, mcause         \n" // Load MCAUSE
        "sw    x30, 128(x31)       \n" // Save MCAUSE
        "csrr  x30, mtval          \n" // Load MTVAL
        "sw    x30, 132(x31)       \n" // Save MTVAL
        "csrr  x30, mstatus        \n" // Load MSTATUS
        "sw    x30, 138(x31)       \n" // Save MSTATUS
        "lw    x30, 116(x31)       \n" // Restore X30
        "lw    x31, 120(x31)       \n" // Restore X31
        :
        :
        : "memory"
        );

    printf("\nMACHINE EXCEPTION CAUGHT:\n\n");

    printf(" RA=%08lX  SP=%08lX  GP=%08lX  TP=%08lX  T0=%08lX  T1=%08lX\n",
        registers_copy[0], registers_copy[1], registers_copy[2],
        registers_copy[3], registers_copy[4], registers_copy[5]);
    printf(" T2=%08lX  S0=%08lX  S1=%08lX  A0=%08lX  A1=%08lX  A2=%08lX\n",
        registers_copy[6], registers_copy[7], registers_copy[8],
        registers_copy[9], registers_copy[10], registers_copy[11]);
    printf(" A3=%08lX  A4=%08lX  A5=%08lX  A6=%08lX  A7=%08lX  S2=%08lX\n",
        registers_copy[12], registers_copy[13], registers_copy[14],
        registers_copy[15], registers_copy[16], registers_copy[17]);
    printf(" S3=%08lX  S4=%08lX  S5=%08lX  S6=%08lX  S7=%08lX  S8=%08lX\n",
        registers_copy[18], registers_copy[19], registers_copy[20],
        registers_copy[21], registers_copy[22], registers_copy[23]);
    printf(" S9=%08lX S10=%08lX S11=%08lX  T3=%08lX  T4=%08lX  T5=%08lX\n",
        registers_copy[24], registers_copy[25], registers_copy[26],
        registers_copy[27], registers_copy[28], registers_copy[29]);
    printf(" T6=%08lX\n\n", registers_copy[30]);

    printf(" MEPC=%08lX MTVAL=%08lX MSTATUS=%08lx MCAUSE=%08lx (%s)\n",
        registers_copy[31], registers_copy[33], registers_copy[34],
        registers_copy[32], lookup_cause(registers_copy[32]));

    exit(-1);
}

#pragma GCC pop_options

void set_interrupt_table(void) {
    __asm volatile (
        "csrrci s0, mstatus, 8 \n" // S0 = MSTATUS & ~MIE
        "csrw   mstatus, s0    \n" // Global machine interrupts are disabled
        "csrw   mie, zero      \n" // Disable machine interrupts
        "csrw   mip, zero      \n" // Clear pending machine interrupts
        "addi   s0, %0, 1      \n" // Vectored machine interrupts enabled
        "csrw   mtvec, s0      \n" // Set new machine vector table
        "csrrsi s0, mstatus, 8 \n" // S0 = MSTATUS | MIE
        "csrw   mstatus, s0    \n" // Global machine interrupts are enabled
        :
        : "r" (mtvec_table)
        : "memory"
        );
}
