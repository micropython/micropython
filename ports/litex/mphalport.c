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

#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/gc.h"

#include "csr.h"
#include "generated/soc.h"

#include "irq.h"

#ifdef CFG_TUSB_MCU
    void hal_dcd_isr(uint8_t rhport);
#endif

/*------------------------------------------------------------------*/
/* delay
 *------------------------------------------------------------------*/
void mp_hal_delay_ms(mp_uint_t delay) {
    uint64_t start_tick = supervisor_ticks_ms64();
    uint64_t duration = 0;
    while (duration < delay) {
        #ifdef MICROPY_VM_HOOK_LOOP
            MICROPY_VM_HOOK_LOOP
        #endif
        // Check to see if we've been CTRL-Ced by autoreload or the user.
        if(MP_STATE_VM(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_kbd_exception)) ||
           MP_STATE_VM(mp_pending_exception) == MP_OBJ_FROM_PTR(&MP_STATE_VM(mp_reload_exception))) {
            break;
        }
        duration = (supervisor_ticks_ms64() - start_tick);
        // TODO(tannewt): Go to sleep for a little while while we wait.
    }
}

extern void SysTick_Handler(void);

__attribute__((section(".ramtext")))
void isr(void) {
    uint8_t irqs = irq_pending() & irq_getmask();

#ifdef CFG_TUSB_MCU
    if (irqs & (1 << USB_INTERRUPT))
        hal_dcd_isr(0);
#endif
    if (irqs & (1 << TIMER0_INTERRUPT))
        SysTick_Handler();
}

mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs) {
    unsigned long __tmp;
    asm volatile ("mv %0, x2" :"=r"(__tmp));
    return __tmp;
}
