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
#include "common-hal/microcontroller/Pin.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"

uint64_t next_start_tick_ms = 0;
uint32_t next_start_tick_us = 1000;

void common_hal_neopixel_write (const digitalio_digitalinout_obj_t* digitalinout, uint8_t *pixels, 
                                uint32_t numBytes) {
    uint8_t *p = pixels, *end = p + numBytes, pix = *p++, mask = 0x80;
    uint32_t start = 0;
    uint32_t cyc = 0;

    //assumes 800_000Hz frequency
    uint32_t sys_freq = HAL_RCC_GetSysClockFreq();
    uint32_t interval = sys_freq/800000;             // cycles per interval (1.25 us). 210@168MHz
    uint32_t t0 = interval - (sys_freq/2500000);     // 0.4 us
    uint32_t t1 = interval - (sys_freq/1250000);     // 0.8 us

    // This must be called while interrupts are on in case we're waiting for a
    // future ms tick.
    wait_until(next_start_tick_ms, next_start_tick_us);

    __disable_irq();
    // Enable DWT in debug core. Useable when interrupts disabled, as opposed to Systick->VAL
    //ITM->LAR = 0xC5ACCE55; //is this required?
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;

    GPIO_TypeDef * p_port = pin_port(digitalinout->pin->port);
    uint32_t p_mask = pin_mask(digitalinout->pin->number);

    for(;;) {
        LL_GPIO_SetOutputPin(p_port, p_mask);
        cyc = (pix & mask) ? t1 : t0;
        start = DWT->CYCCNT;
        while(DWT->CYCCNT - start < cyc);
        LL_GPIO_ResetOutputPin(p_port, p_mask);
        if(!(mask >>= 1)) { //max has shifted all the way
            if(p >= end) break;
            pix       = *p++;
            mask = 0x80;
        }
        while(DWT->CYCCNT - start < interval); //wait for interval to finish
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
