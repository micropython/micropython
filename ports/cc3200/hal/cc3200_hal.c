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


 /******************************************************************************
 IMPORTS
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "extmod/misc.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "hw_memmap.h"
#include "rom_map.h"
#include "interrupt.h"
#include "systick.h"
#include "prcm.h"
#include "pin.h"
#include "telnet.h"
#include "pybuart.h"
#include "utils.h"

#ifdef USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif


/******************************************************************************
 DECLARE PRIVATE FUNCTIONS
 ******************************************************************************/
#ifndef USE_FREERTOS
static void hal_TickInit (void);
#endif

/******************************************************************************
 DECLARE LOCAL DATA
 ******************************************************************************/
static volatile uint32_t HAL_tickCount;

/******************************************************************************
 DECLARE IMPORTED DATA
 ******************************************************************************/
extern void (* const g_pfnVectors[256])(void);

/******************************************************************************
 DEFINE PUBLIC FUNCTIONS
 ******************************************************************************/

__attribute__ ((section (".boot")))
void HAL_SystemInit (void) {
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);

    // in the case of a release image, these steps are already performed by
    // the bootloader so we can skip it and gain some code space
#ifdef DEBUG
    MAP_IntMasterEnable();
    PRCMCC3200MCUInit();
#endif

#ifndef USE_FREERTOS
    hal_TickInit();
#endif
}

void HAL_SystemDeInit (void) {
}

void HAL_IncrementTick(void) {
    HAL_tickCount++;
}

mp_uint_t mp_hal_ticks_ms(void) {
    return HAL_tickCount;
}

// The SysTick timer counts down at HAL_FCPU_HZ, so we can use that knowledge
// to grab a microsecond counter.
mp_uint_t mp_hal_ticks_us(void) {
    mp_uint_t irq_state = disable_irq();
    uint32_t counter = SysTickValueGet();
    uint32_t milliseconds = mp_hal_ticks_ms();
    enable_irq(irq_state);

    uint32_t load = SysTickPeriodGet();
    counter = load - counter; // Convert from decrementing to incrementing
    return (milliseconds * 1000) + ((counter * 1000) / load);
}

void mp_hal_delay_ms(mp_uint_t delay) {
    // only if we are not within interrupt context and interrupts are enabled
    if ((HAL_NVIC_INT_CTRL_REG & HAL_VECTACTIVE_MASK) == 0 && query_irq() == IRQ_STATE_ENABLED) {
        MP_THREAD_GIL_EXIT();
        #ifdef USE_FREERTOS
            vTaskDelay (delay / portTICK_PERIOD_MS);
        #else
            uint32_t start = HAL_tickCount;
            // wraparound of tick is taken care of by 2's complement arithmetic.
            while (HAL_tickCount - start < delay) {
                // enter sleep mode, waiting for (at least) the SysTick interrupt.
                __WFI();
            }
        #endif
        MP_THREAD_GIL_ENTER();
    } else {
        for (int ms = 0; ms < delay; ms++) {
            UtilsDelay(UTILS_DELAY_US_TO_COUNT(1000));
        }
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    mp_uint_t ret = len;
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        ret = dupterm_res;
    }
    // and also to telnet
    telnet_tx_strn(str, len);
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for ( ;; ) {
        // read telnet first
        if (telnet_rx_any()) {
            return telnet_rx_char();
        }

        // then dupterm
        int dupterm_c = mp_os_dupterm_rx_chr();
        if (dupterm_c >= 0) {
            return dupterm_c;
        }

        mp_hal_delay_ms(1);
    }
}

/******************************************************************************
 DEFINE PRIVATE FUNCTIONS
 ******************************************************************************/

#ifndef USE_FREERTOS
static void hal_TickInit (void) {
    HAL_tickCount = 0;
    MAP_SysTickIntRegister(HAL_IncrementTick);
    MAP_IntEnable(FAULT_SYSTICK);
    MAP_SysTickIntEnable();
    MAP_SysTickPeriodSet(HAL_FCPU_HZ / HAL_SYSTICK_PERIOD_US);
    // Force a reload of the SysTick counter register
    HWREG(NVIC_ST_CURRENT) = 0;
    MAP_SysTickEnable();
}
#endif

MP_REGISTER_ROOT_POINTER(struct _os_term_dup_obj_t *os_term_dup_obj);
