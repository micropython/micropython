// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2015 Glenn Ruben Bakke
// SPDX-FileCopyrightText: Copyright (c) 2018 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "lib/tinyusb/src/device/usbd.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/gc.h"
#include "supervisor/cpu.h"
#include "supervisor/usb.h"

#include "csr.h"
#include "generated/soc.h"

#include "irq.h"

void mp_hal_delay_us(mp_uint_t delay) {
    mp_hal_delay_ms(delay / 1000);
}

extern void SysTick_Handler(void);

// This value contains the number of times "common_hal_mcu_disable_interrupts()"
// has been called without calling "common_hal_mcu_enable_interrupts()". Since
// this is the interrupt handler, that means we're handling an interrupt, so
// this value should be `0`.
//
// Interrupts should already be disabled when this handler is running, which means
// this value is logically already `1`. If we didn't do this, then interrupts would
// be prematurely enabled by interrupt handlers that enable and disable interrupts.
extern volatile uint32_t nesting_count;

void isr(void);
__attribute__((section(".ramtext")))
void isr(void) {
    uint8_t irqs = irq_pending() & irq_getmask();

    // Increase the "nesting count". Note: This should be going from 0 -> 1.
    nesting_count += 1;
    #ifdef CFG_TUSB_MCU
    if (irqs & (1 << USB_INTERRUPT)) {
        usb_irq_handler(0);
    }
    #endif
    if (irqs & (1 << TIMER0_INTERRUPT)) {
        SysTick_Handler();
    }

    // Decrease the "nesting count". Note: This should be going from 1 -> 0.
    nesting_count -= 1;
}

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs) {
    unsigned long __tmp;
    asm volatile ("mv %0, x2" : "=r" (__tmp));
    return __tmp;
}
