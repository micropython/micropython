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
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#if defined(__aarch64__)

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

typedef enum {
    SYNC_EL1T = 0,
    SYNC_EL1H,
    SYNC_EL0_64,
    SYNC_EL0_32,
    IRQ_EL1,
    IRQ_EL0,
    FIQ_EL1,
    FIQ_EL0,
    SERROR,
} exception_kind_t;

static const char *const exception_names[] = {
    "Synchronous Exception (EL1t)",
    "Synchronous Exception (EL1h)",
    "Synchronous Exception (EL0, AArch64)",
    "Synchronous Exception (EL0, AArch32)",
    "IRQ (EL1)",
    "IRQ (EL0)",
    "FIQ (EL1)",
    "FIQ (EL0)",
    "SError (Asynchronous External Abort)",
};

volatile uintptr_t exception_registers[36];

static const char *get_esr_ec_string(uint32_t esr) {
    uint32_t ec = (esr >> 26) & 0x3F;
    switch (ec) {
        case 0x00: return "Unknown reason";
        case 0x01: return "Trapped WFI/WFE";
        case 0x03: return "Trapped MCR/MRC (CP15)";
        case 0x04: return "Trapped MCRR/MRRC (CP15)";
        case 0x05: return "Trapped MCR/MRC (CP14)";
        case 0x06: return "Trapped LDC/STC (CP14)";
        case 0x07: return "Trapped FP access";
        case 0x08: return "Trapped VMRS";
        case 0x0A: return "Trapped BXJ";
        case 0x0C: return "Trapped MRRC (CP14)";
        case 0x0D: return "Trapped SIMD/FP";
        case 0x0E: return "Trapped SIMD/FP (64-bit)";
        case 0x11: return "SVC (AArch32)";
        case 0x12: return "SVC (AArch64)";
        case 0x13: return "Trapped MSR/MRS";
        case 0x15: return "SVE access trap";
        case 0x16: return "SME access trap";
        case 0x18: return "Trapped ERET";
        case 0x19: return "Trapped PAC";
        case 0x1A: return "Trapped SVE/SME";
        case 0x1C: return "SMC (AArch32)";
        case 0x1D: return "SMC (AArch64)";
        case 0x20: return "Instruction Abort (lower EL)";
        case 0x21: return "Instruction Abort (same EL)";
        case 0x22: return "PC Alignment Fault";
        case 0x23: return "Data Abort (lower EL)";
        case 0x24: return "Data Abort (same EL)";
        case 0x25: return "SP Alignment Fault";
        case 0x26: return "FP Exception (AArch32)";
        case 0x28: return "FP Exception (AArch64)";
        case 0x2C: return "Software Step (lower EL)";
        case 0x2D: return "Software Step (same EL)";
        case 0x2E: return "Watchpoint (lower EL)";
        case 0x2F: return "Watchpoint (same EL)";
        case 0x30: return "Breakpoint (lower EL)";
        case 0x31: return "Breakpoint (same EL)";
        case 0x32: return "Software Breakpoint";
        case 0x34: return "Vector Catch (AArch32)";
        case 0x35: return "Vector Catch (AArch64)";
        case 0x38: return "BRK (AArch64)";
        case 0x3A: return "ERET Trap";
        case 0x3C: return "PAC Failure";
        default: return "Reserved/Unknown";
    }
}

void exception_handler_main(uintptr_t kind) {
    if (kind == IRQ_EL1 || kind == IRQ_EL0 || kind == FIQ_EL1 || kind == FIQ_EL0) {
        return;
    }

    printf("\n*** Exception: %s ***\n\n", exception_names[kind]);
    printf("Registers:\n");
    printf("  X0  = 0x%016" PRIxPTR "  X1  = 0x%016" PRIxPTR "\n", exception_registers[0], exception_registers[1]);
    printf("  X2  = 0x%016" PRIxPTR "  X3  = 0x%016" PRIxPTR "\n", exception_registers[2], exception_registers[3]);
    printf("  X4  = 0x%016" PRIxPTR "  X5  = 0x%016" PRIxPTR "\n", exception_registers[4], exception_registers[5]);
    printf("  X6  = 0x%016" PRIxPTR "  X7  = 0x%016" PRIxPTR "\n", exception_registers[6], exception_registers[7]);
    printf("  X8  = 0x%016" PRIxPTR "  X9  = 0x%016" PRIxPTR "\n", exception_registers[8], exception_registers[9]);
    printf("  X10 = 0x%016" PRIxPTR "  X11 = 0x%016" PRIxPTR "\n", exception_registers[10], exception_registers[11]);
    printf("  X12 = 0x%016" PRIxPTR "  X13 = 0x%016" PRIxPTR "\n", exception_registers[12], exception_registers[13]);
    printf("  X14 = 0x%016" PRIxPTR "  X15 = 0x%016" PRIxPTR "\n", exception_registers[14], exception_registers[15]);
    printf("  X16 = 0x%016" PRIxPTR "  X17 = 0x%016" PRIxPTR "\n", exception_registers[16], exception_registers[17]);
    printf("  X18 = 0x%016" PRIxPTR "  X19 = 0x%016" PRIxPTR "\n", exception_registers[18], exception_registers[19]);
    printf("  X20 = 0x%016" PRIxPTR "  X21 = 0x%016" PRIxPTR "\n", exception_registers[20], exception_registers[21]);
    printf("  X22 = 0x%016" PRIxPTR "  X23 = 0x%016" PRIxPTR "\n", exception_registers[22], exception_registers[23]);
    printf("  X24 = 0x%016" PRIxPTR "  X25 = 0x%016" PRIxPTR "\n", exception_registers[24], exception_registers[25]);
    printf("  X26 = 0x%016" PRIxPTR "  X27 = 0x%016" PRIxPTR "\n", exception_registers[26], exception_registers[27]);
    printf("  X28 = 0x%016" PRIxPTR "  X29 = 0x%016" PRIxPTR "\n", exception_registers[28], exception_registers[29]);
    printf("  X30 (LR) = 0x%016" PRIxPTR "\n", exception_registers[30]);
    printf("  SP  = 0x%016" PRIxPTR "\n", exception_registers[31]);
    printf("\nSystem Registers:\n");
    printf("  ESR_EL1  = 0x%016" PRIxPTR "  (EC: %s)\n", exception_registers[32], get_esr_ec_string(exception_registers[32]));
    printf("  FAR_EL1  = 0x%016" PRIxPTR "\n", exception_registers[33]);
    printf("  ELR_EL1  = 0x%016" PRIxPTR "  (Return Address)\n", exception_registers[34]);
    printf("  SPSR_EL1 = 0x%016" PRIxPTR "\n", exception_registers[35]);
    printf("\n*** Halting ***\n");
    while (1) {
        __asm__ volatile ("wfi");
    }
}

#endif
