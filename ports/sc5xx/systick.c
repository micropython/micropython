/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2019 Analog Devices, Inc.
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "systick.h"
#include "modmachine.h"
#include <services/tmr/adi_tmr.h>

#define SYSTICK_TMR_ID 0

volatile uint64_t system_milliticks = 0;
ADI_TMR_HANDLE timer_handle;
uint8_t timer_instance_memory[ADI_TMR_MEMORY];
uint32_t us_divider;

static void systemtimer_handler(void *pCBParam,
                                uint32_t Event,
                                void *pArg) {
    switch (Event)
    {
        case ADI_TMR_EVENT_DATA_INT:
            system_milliticks++;
            break;

        default:
            break;
    }
}

void sys_tick_init() {
    uint32_t fsclk0 = machine_get_fsclk0();

    // Initialize 1ms timer
    if (adi_tmr_Open(SYSTICK_TMR_ID,
                     timer_instance_memory,
                     ADI_TMR_MEMORY,
                     systemtimer_handler,
                     NULL,
                     &timer_handle) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error initializing timer.");
    }

    // Set the mode to PWM OUT
    if (adi_tmr_SetMode(timer_handle, ADI_TMR_MODE_CONTINUOUS_PWMOUT) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error setting the timer mode.");
    }

    // Set the IRQ mode to get interrupt after timer counts to Delay + Width
    if (adi_tmr_SetIRQMode(timer_handle, ADI_TMR_IRQMODE_WIDTH_DELAY) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error setting the timer IRQ mode.");
    }

    // Set the Period - once per ms
    if (adi_tmr_SetPeriod(timer_handle, fsclk0 / 1000) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error setting the timer period.");
    }

    // Set the timer width
    if (adi_tmr_SetWidth(timer_handle, fsclk0 / 1000 / 2 - 1) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error setting the timer width.");
    }

    // Set the timer delay
    if (adi_tmr_SetDelay(timer_handle, fsclk0 / 1000 / 2) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error setting the timer delay.");
    }

    // Enable the timer
    if ((adi_tmr_Enable(timer_handle, true)) != ADI_TMR_SUCCESS) {
        mp_raise_msg(&mp_type_RuntimeError, "Error enabling the timer.");
    }

    // This is used to calculate the us tick
    us_divider = fsclk0 / 1000000;
}

// Core delay function that does an efficient sleep and may switch thread context.
// If IRQs are enabled then we must have the GIL.
void mp_hal_delay_ms(mp_uint_t Delay) {
//    if (query_irq() == IRQ_STATE_ENABLED) {
        // IRQs enabled, so can use systick counter to do the delay
        uint64_t start = system_milliticks;
        // Wraparound of tick is taken care of by 2's complement arithmetic.
        while (system_milliticks - start < Delay) {
            // This macro will execute the necessary idle behaviour.  It may
            // raise an exception, switch threads or enter sleep mode (waiting for
            // (at least) the SysTick interrupt).
            //MICROPY_EVENT_POLL_HOOK
            __asm("nop");
        }
/*    } else {
        // IRQs disabled, so need to use a busy loop for the delay.
        // To prevent possible overflow of the counter we use a double loop.
        const uint32_t count_1ms = HAL_RCC_GetSysClockFreq() / 4000;
        for (int i = 0; i < Delay; i++) {
            for (uint32_t count = 0; ++count <= count_1ms;) {
            }
        }
    }*/
}

// delay for given number of microseconds
void mp_hal_delay_us(mp_uint_t usec) {
//    if (query_irq() == IRQ_STATE_ENABLED) {
        // IRQs enabled, so can use systick counter to do the delay
        uint64_t start = mp_hal_ticks_us();
        while (mp_hal_ticks_us() - start < usec) {
        }
/*    } else {
        // IRQs disabled, so need to use a busy loop for the delay
        // sys freq is always a multiple of 2MHz, so division here won't lose precision
        const uint32_t ucount = HAL_RCC_GetSysClockFreq() / 2000000 * usec / 2;
        for (uint32_t count = 0; ++count <= ucount;) {
        }
    }*/
}

bool sys_tick_has_passed(uint32_t start_tick, uint32_t delay_ms) {
    return system_milliticks - start_tick >= delay_ms;
}

// waits until at least delay_ms milliseconds have passed from the sampling of
// startTick. Handles overflow properly. Assumes stc was taken from
// HAL_GetTick() some time before calling this function.
void sys_tick_wait_at_least(uint32_t start_tick, uint32_t delay_ms) {
    while (!sys_tick_has_passed(start_tick, delay_ms)) {
        __WFI(); // enter sleep mode, waiting for interrupt
    }
}

mp_uint_t mp_hal_ticks_ms(void) {
    return system_milliticks;
}

mp_uint_t mp_hal_ticks_us(void) {
    // use current count to determine us inside one ms
    uint32_t raw_count;
    // STW to avoid interrupt between 2 reads
    adi_tmr_Enable(timer_handle, false);
    adi_tmr_GetCount(timer_handle, &raw_count);
    // prevent optimization
    volatile uint32_t ticks = system_milliticks * 1000 + raw_count / us_divider;
    adi_tmr_Enable(timer_handle, true);
    return ticks;
}
