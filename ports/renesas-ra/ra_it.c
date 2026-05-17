/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2021, 2022 Renesas Electronics Corporation
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

#include "py/obj.h"
#include "py/mphal.h"
#include "ra_it.h"
#include "pendsv.h"
#include "irq.h"
#include "powerctrl.h"
#include "pybthread.h"
#include "gccollect.h"
#include "extint.h"
#include "timer.h"
#include "uart.h"
#include "storage.h"

extern void __fatal_error(const char *);

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

// Set the following to 1 to get some more information on the Hard Fault
// More information about decoding the fault registers can be found here:
// http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0646a/Cihdjcfc.html

static char *fmt_hex(uint32_t val, char *buf) {
    const char *hexDig = "0123456789abcdef";

    buf[0] = hexDig[(val >> 28) & 0x0f];
    buf[1] = hexDig[(val >> 24) & 0x0f];
    buf[2] = hexDig[(val >> 20) & 0x0f];
    buf[3] = hexDig[(val >> 16) & 0x0f];
    buf[4] = hexDig[(val >> 12) & 0x0f];
    buf[5] = hexDig[(val >> 8) & 0x0f];
    buf[6] = hexDig[(val >> 4) & 0x0f];
    buf[7] = hexDig[(val >> 0) & 0x0f];
    buf[8] = '\0';

    return buf;
}

static void print_reg(const char *label, uint32_t val) {
    char hexStr[9];

    mp_hal_stdout_tx_str(label);
    mp_hal_stdout_tx_str(fmt_hex(val, hexStr));
    mp_hal_stdout_tx_str("\r\n");
}

static void print_hex_hex(const char *label, uint32_t val1, uint32_t val2) {
    char hex_str[9];
    mp_hal_stdout_tx_str(label);
    mp_hal_stdout_tx_str(fmt_hex(val1, hex_str));
    mp_hal_stdout_tx_str("  ");
    mp_hal_stdout_tx_str(fmt_hex(val2, hex_str));
    mp_hal_stdout_tx_str("\r\n");
}

// The ARMv7M Architecture manual (section B.1.5.6) says that upon entry
// to an exception, that the registers will be in the following order on the
// // stack: R0, R1, R2, R3, R12, LR, PC, XPSR

typedef struct {
    uint32_t r0, r1, r2, r3, r12, lr, pc, xpsr;
} ExceptionRegisters_t;

int pyb_hard_fault_debug = 0;

void HardFault_C_Handler(ExceptionRegisters_t *regs) {
    if (!pyb_hard_fault_debug) {
        powerctrl_mcu_reset();
    }

    #if MICROPY_HW_ENABLE_USB
    // We need to disable the USB so it doesn't try to write data out on
    // the VCP and then block indefinitely waiting for the buffer to drain.
    pyb_usb_flags = 0;
    #endif

    mp_hal_stdout_tx_str("HardFault\r\n");

    print_reg("R0    ", regs->r0);
    print_reg("R1    ", regs->r1);
    print_reg("R2    ", regs->r2);
    print_reg("R3    ", regs->r3);
    print_reg("R12   ", regs->r12);
    print_reg("SP    ", (uint32_t)regs);
    print_reg("LR    ", regs->lr);
    print_reg("PC    ", regs->pc);
    print_reg("XPSR  ", regs->xpsr);

    #if __CORTEX_M >= 3
    uint32_t cfsr = SCB->CFSR;

    print_reg("HFSR  ", SCB->HFSR);
    print_reg("CFSR  ", cfsr);
    if (cfsr & 0x80) {
        print_reg("MMFAR ", SCB->MMFAR);
    }
    if (cfsr & 0x8000) {
        print_reg("BFAR  ", SCB->BFAR);
    }
    #endif

    if ((void *)&_ram_start <= (void *)regs && (void *)regs < (void *)&_ram_end) {
        mp_hal_stdout_tx_str("Stack:\r\n");
        uint32_t *stack_top = &_estack;
        if ((void *)regs < (void *)&_sstack) {
            // stack not in static stack area so limit the amount we print
            stack_top = (uint32_t *)regs + 32;
        }
        for (uint32_t *sp = (uint32_t *)regs; sp < stack_top; ++sp) {
            print_hex_hex("  ", (uint32_t)sp, *sp);
        }
    }

    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
        __fatal_error("HardFault");
    }
}

// Naked functions have no compiler generated gunk, so are the best thing to
// use for asm functions.
__attribute__((naked))
void HardFault_Handler(void) {

    // From the ARMv7M Architecture Reference Manual, section B.1.5.6
    // on entry to the Exception, the LR register contains, amongst other
    // things, the value of CONTROL.SPSEL. This can be found in bit 3.
    //
    // If CONTROL.SPSEL is 0, then the exception was stacked up using the
    // main stack pointer (aka MSP). If CONTROL.SPSEL is 1, then the exception
    // was stacked up using the process stack pointer (aka PSP).

    #if __CORTEX_M == 0
    __asm volatile (
        " mov r0, lr    \n"
        " lsr r0, r0, #3 \n"    // Shift Bit 3 into carry to see which stack pointer we should use.
        " mrs r0, msp   \n"     // Make R0 point to main stack pointer
        " bcc .use_msp  \n"     // Keep MSP in R0 if SPSEL (carry) is 0
        " mrs r0, psp   \n"     // Make R0 point to process stack pointer
        " .use_msp:     \n"
        " b HardFault_C_Handler \n" // Off to C land
        );
    #else
    __asm volatile (
        " tst lr, #4    \n"     // Test Bit 3 to see which stack pointer we should use.
        " ite eq        \n"     // Tell the assembler that the nest 2 instructions are if-then-else
        " mrseq r0, msp \n"     // Make R0 point to main stack pointer
        " mrsne r0, psp \n"     // Make R0 point to process stack pointer
        " b HardFault_C_Handler \n" // Off to C land
        );
    #endif
}

#if 0
/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void) {
}
#endif

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void) {
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
        __fatal_error("MemManage");
    }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void) {
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
        __fatal_error("BusFault");
    }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void) {
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
        __fatal_error("UsageFault");
    }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void) {
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void) {
}
