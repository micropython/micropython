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

// Enable this to get a count for the number of times each irq handler is called,
// accessible via pyb.irq_stats().
#define IRQ_ENABLE_STATS (0)

#if IRQ_ENABLE_STATS
extern uint32_t irq_stats[FPU_IRQn + 1];
#define IRQ_ENTER(irq) ++irq_stats[irq]
#define IRQ_EXIT(irq)
#else
#define IRQ_ENTER(irq)
#define IRQ_EXIT(irq)
#endif

static inline mp_uint_t query_irq(void) {
    return __get_PRIMASK();
}

// enable_irq and disable_irq are defined inline in mpconfigport.h

#if __CORTEX_M >= 0x03

// irqs with a priority value greater or equal to "pri" will be disabled
// "pri" should be between 1 and 15 inclusive
static inline uint32_t raise_irq_pri(uint32_t pri) {
    uint32_t basepri = __get_BASEPRI();
    // If non-zero, the processor does not process any exception with a
    // priority value greater than or equal to BASEPRI.
    // When writing to BASEPRI_MAX the write goes to BASEPRI only if either:
    //   - Rn is non-zero and the current BASEPRI value is 0
    //   - Rn is non-zero and less than the current BASEPRI value
    pri <<= (8 - __NVIC_PRIO_BITS);
    __ASM volatile ("msr basepri_max, %0" : : "r" (pri) : "memory");
    return basepri;
}

// "basepri" should be the value returned from raise_irq_pri
static inline void restore_irq_pri(uint32_t basepri) {
    __set_BASEPRI(basepri);
}

#endif

MP_DECLARE_CONST_FUN_OBJ_0(pyb_wfi_obj);
MP_DECLARE_CONST_FUN_OBJ_0(pyb_disable_irq_obj);
MP_DECLARE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_enable_irq_obj);
MP_DECLARE_CONST_FUN_OBJ_0(pyb_irq_stats_obj);

// IRQ priority definitions.
//
// Lower number implies higher interrupt priority.
//
// The default priority grouping is set to NVIC_PRIORITYGROUP_4 in the
// HAL_Init function. This corresponds to 4 bits for the priority field
// and 0 bits for the sub-priority field (which means that for all intensive
// purposes that the sub-priorities below are ignored).
//
// While a given interrupt is being processed, only higher priority (lower number)
// interrupts will preempt a given interrupt. If sub-priorities are active
// then the sub-priority determines the order that pending interrupts of
// a given priority are executed. This is only meaningful if 2 or more
// interrupts of the same priority are pending at the same time.
//
// The priority of the SysTick timer is determined from the TICK_INT_PRIORITY
// value which is normally set to 0 in the stm32f4xx_hal_conf.h file.
//
// The following interrupts are arranged from highest priority to lowest
// priority to make it a bit easier to figure out.

//                           Priority   Sub-Priority
//                           --------   ------------
//#def  IRQ_PRI_SYSTICK         0
//#def  IRQ_SUBPRI_SYSTICK                  0

// The UARTs have no FIFOs, so if they don't get serviced quickly then characters
// get dropped. The handling for each character only consumes about 0.5 usec
#define IRQ_PRI_UART            1
#define IRQ_SUBPRI_UART                     0

// Flash IRQ must be higher priority than interrupts of all those components
// that rely on the flash storage.
#define IRQ_PRI_FLASH           2
#define IRQ_SUBPRI_FLASH                    0

// SDIO must be higher priority than DMA for SDIO DMA transfers to work.
#define IRQ_PRI_SDIO            4
#define IRQ_SUBPRI_SDIO                     0

// DMA should be higher priority than USB, since USB Mass Storage calls
// into the sdcard driver which waits for the DMA to complete.
#define IRQ_PRI_DMA             5
#define IRQ_SUBPRI_DMA                      0

#define IRQ_PRI_OTG_FS          6
#define IRQ_SUBPRI_OTG_FS                   0

#define IRQ_PRI_OTG_HS          6
#define IRQ_SUBPRI_OTG_HS                   0

#define IRQ_PRI_TIM5            6
#define IRQ_SUBPRI_TIM5                     0

#define IRQ_PRI_CAN             7
#define IRQ_SUBPRI_CAN                      0

// Interrupt priority for non-special timers.
#define IRQ_PRI_TIMX            13
#define IRQ_SUBPRI_TIMX                     0

#define IRQ_PRI_EXTINT          14
#define IRQ_SUBPRI_EXTINT                   0

// PENDSV should be at the lowst priority so that other interrupts complete
// before exception is raised.
#define IRQ_PRI_PENDSV          15
#define IRQ_SUBPRI_PENDSV                   0

#define IRQ_PRI_RTC_WKUP        15
#define IRQ_SUBPRI_RTC_WKUP                 0


