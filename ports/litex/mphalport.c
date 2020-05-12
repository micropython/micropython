/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Glenn Ruben Bakke
 * Copyright (c) 2018 Artur Pacholec
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

#include <string.h>

#include "lib/tinyusb/src/device/usbd.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/gc.h"

#include "csr.h"
#include "generated/soc.h"

#include "irq.h"

void mp_hal_delay_us(mp_uint_t delay) {
    mp_hal_delay_ms(delay / 1000);
}

extern void SysTick_Handler(void);

__attribute__((section(".ramtext")))
void isr(void) {
    uint8_t irqs = irq_pending() & irq_getmask();

#ifdef CFG_TUSB_MCU
    if (irqs & (1 << USB_INTERRUPT))
        tud_int_handler(0);
#endif
    if (irqs & (1 << TIMER0_INTERRUPT))
        SysTick_Handler();
}

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs) {
    unsigned long __tmp;
    asm volatile ("mv %0, x2" :"=r"(__tmp));
    return __tmp;
}
