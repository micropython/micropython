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


#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "hw_memmap.h"
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
#include "moduos.h"

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

void mp_hal_stdout_tx_str(const char *str) {
    mp_hal_stdout_tx_strn(str, strlen(str));
}

void mp_hal_stdout_tx_strn(const char *str, size_t len) {
    if (MP_STATE_PORT(os_term_dup_obj)) {
        if (MP_OBJ_IS_TYPE(MP_STATE_PORT(os_term_dup_obj)->stream_o, &pyb_uart_type)) {
            uart_tx_strn(MP_STATE_PORT(os_term_dup_obj)->stream_o, str, len);
        } else {
            MP_STATE_PORT(os_term_dup_obj)->write[2] = mp_obj_new_str_of_type(&mp_type_str, (const byte *)str, len);
            mp_call_method_n_kw(1, 0, MP_STATE_PORT(os_term_dup_obj)->write);
        }
    }
    // and also to telnet
    telnet_tx_strn(str, len);
}

void mp_hal_stdout_tx_strn_cooked (const char *str, size_t len) {
    int32_t nslen = 0;
    const char *_str = str;

    for (int i = 0; i < len; i++) {
        if (str[i] == '\n') {
            mp_hal_stdout_tx_strn(_str, nslen);
            mp_hal_stdout_tx_strn("\r\n", 2);
            _str += nslen + 1;
            nslen = 0;
        } else {
            nslen++;
        }
    }
    if (_str < str + len) {
        mp_hal_stdout_tx_strn(_str, nslen);
    }
}

int mp_hal_stdin_rx_chr(void) {
    for ( ;; ) {
        // read telnet first
        if (telnet_rx_any()) {
            return telnet_rx_char();
        } else if (MP_STATE_PORT(os_term_dup_obj)) { // then the stdio_dup
            if (MP_OBJ_IS_TYPE(MP_STATE_PORT(os_term_dup_obj)->stream_o, &pyb_uart_type)) {
                if (uart_rx_any(MP_STATE_PORT(os_term_dup_obj)->stream_o)) {
                    return uart_rx_char(MP_STATE_PORT(os_term_dup_obj)->stream_o);
                }
            } else {
                MP_STATE_PORT(os_term_dup_obj)->read[2] = mp_obj_new_int(1);
                mp_obj_t data = mp_call_method_n_kw(1, 0, MP_STATE_PORT(os_term_dup_obj)->read);
                // data len is > 0
                if (mp_obj_is_true(data)) {
                    mp_buffer_info_t bufinfo;
                    mp_get_buffer_raise(data, &bufinfo, MP_BUFFER_READ);
                    return ((int *)(bufinfo.buf))[0];
                }
            }
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
