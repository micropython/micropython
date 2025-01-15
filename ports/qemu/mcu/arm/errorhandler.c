/*
 * This file is part of the MicroPython project, https://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Alessandro Gatti
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "py/runtime.h"

#if defined(__ARM_ARCH_ISA_THUMB)

typedef enum _exception_kind_t {
    RESET = 1,
    NMI = 2,
    HARD_FAULT = 3,
    MEM_MANAGE = 4,
    BUS_FAULT = 5,
    USAGE_FAULT = 6,
    SV_CALL = 11,
    DEBUG_MONITOR = 12,
    PENDING_SV = 13,
    SYSTEM_TICK = 14,
} exception_kind_t;

static const char *EXCEPTION_NAMES_TABLE[] = {
    "Reserved",
    "Reset",
    "NMI",
    "HardFault",
    "MemManage",
    "BusFault",
    "UsageFault",
    "SVCall",
    "DebugMonitor",
    "PendSV",
    "SysTick",
    "External interrupt"
};

// R0-R15, PSR, Kind
uintptr_t registers_copy[18] = { 0 };

__attribute__((naked)) MP_NORETURN void exception_handler(uintptr_t kind) {
    // Save registers
    __asm volatile (
        "ldr r1, =registers_copy \n"
        "str r0, [r1, #68]       \n" // Kind
        "ldr r0, [sp, #0]        \n" // R0
        "str r0, [r1, #0]        \n"
        "ldr r0, [sp, #4]        \n" // R1
        "str r0, [r1, #4]        \n"
        "ldr r0, [sp, #8]        \n" // R2
        "str r0, [r1, #8]        \n"
        "ldr r0, [sp, #12]       \n" // R3
        "str r0, [r1, #12]       \n"
        "str r4, [r1, #16]       \n"
        "str r5, [r1, #20]       \n"
        "str r6, [r1, #24]       \n"
        "str r7, [r1, #28]       \n"
        "mov r0, r8              \n"
        "str r0, [r1, #32]       \n"
        "mov r0, r9              \n"
        "str r0, [r1, #36]       \n"
        "mov r0, r10             \n"
        "str r0, [r1, #40]       \n"
        "mov r0, r11             \n"
        "str r0, [r1, #44]       \n"
        "ldr r0, [sp, #16]       \n" // R12
        "str r0, [r1, #48]       \n"
        "mov r0, sp              \n"
        "sub r0, #32             \n"
        "str r0, [r1, #52]       \n"
        "ldr r0, [sp, #20]       \n" // R14
        "str r0, [r1, #56]       \n"
        "ldr r0, [sp, #24]       \n" // R15
        "str r0, [r1, #60]       \n"
        "ldr r0, [sp, #28]       \n" // xPSR
        "str r0, [r1, #64]       \n"
        :
        :
        : "memory"
        );

    uintptr_t saved_kind = registers_copy[17];

    switch (saved_kind) {
        case RESET:
        case NMI:
        case HARD_FAULT:
        case MEM_MANAGE:
        case BUS_FAULT:
        case USAGE_FAULT:
        case SV_CALL:
        case DEBUG_MONITOR:
        case PENDING_SV:
        case SYSTEM_TICK:
            printf(EXCEPTION_NAMES_TABLE[saved_kind]);
            break;
        default:
            if (saved_kind >= 16) {
                printf("%s %d", EXCEPTION_NAMES_TABLE[11], saved_kind - 16);
            } else {
                printf(EXCEPTION_NAMES_TABLE[0]);
            }
            break;
    }
    printf(" exception caught:\n");
    printf("R0:   %08X R1:   %08X R2:   %08X R3:   %08X\n", registers_copy[0], registers_copy[1], registers_copy[2], registers_copy[3]);
    printf("R4:   %08X R5:   %08X R6:   %08X R7:   %08X\n", registers_copy[4], registers_copy[5], registers_copy[6], registers_copy[7]);
    printf("R8:   %08X R9:   %08X R10:  %08X R11:  %08X\n", registers_copy[8], registers_copy[9], registers_copy[10], registers_copy[11]);
    printf("R12:  %08X R13:  %08X R14:  %08X R15:  %08X\n", registers_copy[12], registers_copy[13], registers_copy[14], registers_copy[15]);
    printf("xPSR: %08X\n", registers_copy[16]);

    for (;;) {}
}

__attribute__((naked)) MP_NORETURN void NMI_Handler(void) {
    exception_handler(NMI);
}

__attribute__((naked)) MP_NORETURN void HardFault_Handler(void) {
    exception_handler(HARD_FAULT);
}

__attribute__((naked)) MP_NORETURN void MemManage_Handler(void) {
    exception_handler(MEM_MANAGE);
}

__attribute__((naked)) MP_NORETURN void BusFault_Handler(void) {
    exception_handler(BUS_FAULT);
}

__attribute__((naked)) MP_NORETURN void UsageFault_Handler(void) {
    exception_handler(USAGE_FAULT);
}

__attribute__((naked)) MP_NORETURN void SVC_Handler(void) {
    exception_handler(SV_CALL);
}

__attribute__((naked)) MP_NORETURN void DebugMon_Handler(void) {
    exception_handler(DEBUG_MONITOR);
}

__attribute__((naked)) MP_NORETURN void PendSV_Handler(void) {
    exception_handler(PENDING_SV);
}

__attribute__((naked)) MP_NORETURN void SysTick_Handler(void) {
    exception_handler(SYSTEM_TICK);
}

#endif
