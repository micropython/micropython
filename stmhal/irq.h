/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

// these states correspond to values from query_irq, enable_irq and disable_irq
#define IRQ_STATE_DISABLED (0x00000001)
#define IRQ_STATE_ENABLED  (0x00000000)

static inline mp_uint_t query_irq(void) {
    return __get_PRIMASK();
}

// enable_irq and disable_irq are defined inline in mpconfigport.h

MP_DECLARE_CONST_FUN_OBJ(pyb_wfi_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_disable_irq_obj);
MP_DECLARE_CONST_FUN_OBJ(pyb_enable_irq_obj);

// IRQ priority definitions.
// Lower number implies higher interrupt priority.

#define IRQ_PRI_CAN 0x7
#define IRQ_SUBPRI_CAN 0x0

#define IRQ_PRI_DMA 0x6
#define IRQ_SUBPRI_DMA 0x0

#define IRQ_PRI_EXTINT 0xf
#define IRQ_SUBPRI_EXTINT 0xf

// Flash IRQ must be higher priority than interrupts of all those components
// that rely on the flash storage.
#define IRQ_PRI_FLASH 0x1
#define IRQ_SUBPRI_FLASH 0x1

#define IRQ_PRI_OTG_FS 0x6
#define IRQ_SUBPRI_OTG_FS 0x0

#define IRQ_PRI_OTG_HS 0x6
#define IRQ_SUBPRI_OTG_HS 0x0

// PENDSV should be at the lowst priority so that other interrupts complete
// before exception is raised.
#define IRQ_PRI_PENDSV 0xf
#define IRQ_SUBPRI_PENDSV 0xf

#define IRQ_PRI_RTC_WKUP 0xf
#define IRQ_SUBPRI_RTC_WKUP 0xf

#define IRQ_PRI_TIM3 0x6
#define IRQ_SUBPRI_TIM3 0x0

#define IRQ_PRI_TIM5 0x6
#define IRQ_SUBPRI_TIM5 0x0

// Interrupt priority for non-special timers.
#define IRQ_PRI_TIMX 0xe
#define IRQ_SUBPRI_TIMX 0xe

#define IRQ_PRI_UART 0xd
#define IRQ_SUBPRI_UART 0xd

