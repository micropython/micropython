/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Daniel Campora
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
#include <stdbool.h>

#include "hal/utils.h"
#include "hal/systick.h"

// assembly functions to handle critical sections, interrupt
// disabling/enabling and sleep mode enter/exit
#include "cc3200_asm.h"

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/

#define HAL_FCPU_MHZ                        80U
#define HAL_FCPU_HZ                         (1000000U * HAL_FCPU_MHZ)
#define HAL_SYSTICK_PERIOD_US               1000U
#define UTILS_DELAY_US_TO_COUNT(us)         (((us) * HAL_FCPU_MHZ) / 6)

#define HAL_NVIC_INT_CTRL_REG               (*((volatile uint32_t *) 0xE000ED04 ) )
#define HAL_VECTACTIVE_MASK                 (0x1FUL)

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/

/******************************************************************************
 DEFINE FUNCTION-LIKE MACROS
 ******************************************************************************/

#define HAL_INTRODUCE_SYNC_BARRIER() {                      \
                                        __asm(" dsb \n"     \
                                              " isb \n");   \
                                     }

#define MICROPY_BEGIN_ATOMIC_SECTION()              disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)           enable_irq(state)

/******************************************************************************
 DECLARE PUBLIC FUNCTIONS
 ******************************************************************************/

extern void HAL_SystemInit (void);
extern void HAL_SystemDeInit (void);
extern void HAL_IncrementTick(void);
extern void mp_hal_set_interrupt_char (int c);

#define mp_hal_stdio_poll(poll_flags) (0) // not implemented
#define mp_hal_delay_us(usec) UtilsDelay(UTILS_DELAY_US_TO_COUNT(usec))
#define mp_hal_ticks_cpu() (SysTickPeriodGet() - SysTickValueGet())
#define mp_hal_time_ns() (0) // not implemented
