/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "samd_soc.h"

typedef void (*ISR)(void);

extern uint32_t _estack, _sidata, _sdata, _edata, _sbss, _ebss;

const ISR isr_vector[];
uint32_t systick_ms;

void Reset_Handler(void) __attribute__((naked));
void Reset_Handler(void) {
    // Set stack pointer
    #if __CORTEX_M >= 0x03
    __asm volatile ("ldr sp, =_estack");
    #else
    __asm volatile ("ldr r0, =_estack");
    __asm volatile ("mov sp, r0");
    #endif
    // Copy .data section from flash to RAM
    for (uint32_t *src = &_sidata, *dest = &_sdata; dest < &_edata;) {
        *dest++ = *src++;
    }
    // Zero out .bss section
    for (uint32_t *dest = &_sbss; dest < &_ebss;) {
        *dest++ = 0;
    }

    // When we get here: stack is initialised, bss is clear, data is copied

    #if __FPU_PRESENT == 1 && __FPU_USED == 1
    // Set CP10 and CP11 Full Access
    SCB->CPACR |= (3UL << 10 * 2) | (3UL << 11 * 2);
    #endif

    // SCB->VTOR
    *((volatile uint32_t*)0xe000ed08) = (uint32_t)&isr_vector;

    // SCB->CCR: enable 8-byte stack alignment for IRQ handlers, in accord with EABI
    *((volatile uint32_t*)0xe000ed14) |= 1 << 9;

    // Initialise the cpu and peripherals
    samd_init();

    // Now that we have a basic system up and running we can call main
    samd_main();

    // we must not return
    for (;;) {
    }
}

void Default_Handler(void) {
    for (;;) {
    }
}

void SysTick_Handler(void) {
    systick_ms += 1;
}

const ISR isr_vector[] __attribute__((section(".isr_vector"))) = {
    (ISR)&_estack,
    &Reset_Handler,
    &Default_Handler, // NMI_Handler
    &Default_Handler, // HardFault_Handler
    &Default_Handler, // MemManage_Handler
    &Default_Handler, // BusFault_Handler
    &Default_Handler, // UsageFault_Handler
    0,
    0,
    0,
    0,
    &Default_Handler, // SVC_Handler
    &Default_Handler, // DebugMon_Handler
    0,
    &Default_Handler, // PendSV_Handler
    &SysTick_Handler, // SysTick_Handler
    0, // line 0
    0,
    0,
    0,
    0,
    0,
    0,
    #if defined(MCU_SAMD21)
    USB_Handler_wrapper, // line 7
    #else
    0,
    #endif
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    #if defined(MCU_SAMD51)
    &USB_0_Handler_wrapper, // line 80
    &USB_1_Handler_wrapper,
    &USB_2_Handler_wrapper,
    &USB_3_Handler_wrapper,
    #else
    0,
    0,
    0,
    0,
    #endif
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
