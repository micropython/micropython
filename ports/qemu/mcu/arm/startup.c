/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2021 Damien P. George
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

#include "shared/runtime/semihosting_arm.h"
#include "uart.h"

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

__attribute__((naked)) void Reset_Handler(void) {
    // set stack pointer
    __asm volatile ("ldr r0, =_estack");
    __asm volatile ("mov sp, r0");
    // copy .data section from flash to RAM
    for (uint32_t *src = &_sidata, *dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }
    // zero out .bss section
    for (uint32_t *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }
    #if MICROPY_HW_FPU && defined(__ARM_ARCH_ISA_THUMB) && __ARM_ARCH >= 7
    // initialise the FPU (full access to CP10 and CP11, as per B3.2.20)
    *((volatile uint32_t *)0xE000ED88) |= 0x00F00000;
    __asm volatile ("dsb");
    __asm volatile ("isb");
    #endif
    // jump to board initialisation
    void _start(void);
    _start();
}

MP_NORETURN void Default_Handler(void) {
    for (;;) {
    }
}

#if defined(__ARM_ARCH_ISA_ARM)

// ARM architecture with standard ARM ISA.

__attribute__((naked, section(".isr_vector"))) void isr_vector(void) {
    __asm volatile (
        "b Reset_Handler\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        "nop\n"
        "b Default_Handler\n"
        "b Default_Handler\n"
        );
}

#elif defined(__ARM_ARCH_ISA_THUMB)

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);

// ARM architecture with Thumb-only ISA.

const uint32_t isr_vector[] __attribute__((section(".isr_vector"))) = {
    (uint32_t)&_estack,
    (uint32_t)&Reset_Handler,
    (uint32_t)&NMI_Handler,
    (uint32_t)&HardFault_Handler,
    (uint32_t)&MemManage_Handler,
    (uint32_t)&BusFault_Handler,
    (uint32_t)&UsageFault_Handler,
    0,
    0,
    0,
    0,
    (uint32_t)&SVC_Handler,
    (uint32_t)&DebugMon_Handler,
    0,
    (uint32_t)&PendSV_Handler,
    (uint32_t)&SysTick_Handler,
};

#endif

void _start(void) {
    mp_semihosting_init();

    extern void ticks_init(void);
    ticks_init();

    // Enable the UART
    uart_init();

    // Now that we have a basic system up and running we can call main
    extern int main();
    main(0, 0);

    // Finished
    exit(0);
}

void exit(int status) {
    // Force qemu to exit using ARM Semihosting
    mp_semihosting_exit(status);
    for (;;) {
    }
}

#ifndef NDEBUG
void __assert_func(const char *file, int line, const char *func, const char *expr) {
    (void)func;
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    exit(1);
}
#endif
