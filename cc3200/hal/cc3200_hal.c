/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "hw_memmap.h"
#include "py/mpstate.h"
#include MICROPY_HAL_H
#include "rom_map.h"
#include "interrupt.h"
#include "systick.h"
#include "prcm.h"
#include "pin.h"
#include "mpexception.h"
#include "telnet.h"
#include "pybuart.h"
#include "utils.h"
#include "irq.h"

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
 DECLARE PUBLIC DATA
 ******************************************************************************/
struct _pyb_uart_obj_t *pyb_stdio_uart;

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

uint32_t HAL_GetTick(void) {
    return HAL_tickCount;
}

void HAL_Delay(uint32_t delay) {
    // only if we are not within interrupt context and interrupts are enabled
    if ((HAL_NVIC_INT_CTRL_REG & HAL_VECTACTIVE_MASK) == 0 && query_irq() == IRQ_STATE_ENABLED) {
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
    } else {
        for (int ms = 0; ms < delay; ms++) {
            UtilsDelay(UTILS_DELAY_US_TO_COUNT(1000));
        }
    }
}

NORETURN void mp_hal_raise(int errno) {
    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, mp_obj_new_int(errno)));
}

void mp_hal_set_interrupt_char (int c) {
    mpexception_set_interrupt_char (c);
}

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, uint32_t len) {
    // send stdout to UART
    if (pyb_stdio_uart != NULL) {
        uart_tx_strn(pyb_stdio_uart, str, len);
    }
    // and also to telnet
    if (telnet_is_active()) {
        telnet_tx_strn(str, len);
    }
}

void mp_hal_stdout_tx_strn_cooked(const char *str, uint32_t len) {
    // send stdout to UART
    if (pyb_stdio_uart != NULL) {
        uart_tx_strn_cooked(pyb_stdio_uart, str, len);
    }
    // and also to telnet
    if (telnet_is_active()) {
        telnet_tx_strn_cooked(str, len);
    }
}

int mp_hal_stdin_rx_chr(void) {
    for ( ;; ) {
        if (telnet_rx_any()) {
            return telnet_rx_char();
        }
        else if (pyb_stdio_uart != NULL && uart_rx_any(pyb_stdio_uart)) {
            return uart_rx_char(pyb_stdio_uart);
        }
        HAL_Delay(1);
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

