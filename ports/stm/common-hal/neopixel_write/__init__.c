/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Lucian Copeland for Adafruit Industries
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

#include "py/mphal.h"
#include "shared-bindings/neopixel_write/__init__.h"

#include "tick.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/microcontroller/Pin.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"

uint64_t next_start_tick_ms = 0;
uint32_t next_start_tick_us = 1000;

//sysclock divisors
#define MAGIC_800_INT  900000  // ~1.11 us  -> 1.2  field
#define MAGIC_800_T0H  2800000  // ~0.36 us -> 0.44 field
#define MAGIC_800_T1H  1350000  // ~0.74 us -> 0.84 field

#pragma GCC push_options
#pragma GCC optimize ("Os")

void common_hal_neopixel_write (const digitalio_digitalinout_obj_t* digitalinout, uint8_t *pixels, 
                                uint32_t numBytes) {
    uint8_t *p = pixels, *end = p + numBytes, pix = *p++, mask = 0x80;
    uint32_t start = 0;
    uint32_t cyc = 0;

    //assumes 800_000Hz frequency
    //Theoretical values here are 800_000 -> 1.25us, 2500000->0.4us, 1250000->0.8us
    //TODO: try to get dynamic weighting working again
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    uint32_t interval = sys_freq/MAGIC_800_INT;
    uint32_t t0 = (sys_freq/MAGIC_800_T0H);
    uint32_t t1 = (sys_freq/MAGIC_800_T1H);

    // This must be called while interrupts are on in case we're waiting for a
    // future ms tick.
    wait_until(next_start_tick_ms, next_start_tick_us);

    GPIO_TypeDef * p_port = pin_port(digitalinout->pin->port);
    uint32_t p_mask = pin_mask(digitalinout->pin->number);

    __disable_irq();
    // Enable DWT in debug core. Useable when interrupts disabled, as opposed to Systick->VAL
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;

    for(;;) {
        cyc = (pix & mask) ? t1 : t0;
        start = DWT->CYCCNT;
        LL_GPIO_SetOutputPin(p_port, p_mask);
        while((DWT->CYCCNT - start) < cyc);
        LL_GPIO_ResetOutputPin(p_port, p_mask);
        while((DWT->CYCCNT - start) < interval);
        if(!(mask >>= 1)) {
            if(p >= end) break;
            pix       = *p++;
            mask = 0x80;
        }
    }

    // Enable interrupts again
    __enable_irq();

    // Update the next start.
    current_tick(&next_start_tick_ms, &next_start_tick_us);
    if (next_start_tick_us < 100) {
        next_start_tick_ms += 1;
        next_start_tick_us = 100 - next_start_tick_us;
    } else {
        next_start_tick_us -= 100;
    }
}

#pragma GCC pop_options