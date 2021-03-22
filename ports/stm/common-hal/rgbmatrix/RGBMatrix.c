/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <stddef.h>

#include "common-hal/rgbmatrix/RGBMatrix.h"
#include "timers.h"

#include STM32_HAL_H

extern void _PM_IRQ_HANDLER(void);

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    TIM_TypeDef *timer = stm_peripherals_find_timer();
    stm_peripherals_timer_reserve(timer);
    stm_peripherals_timer_never_reset(timer);
    return timer;
}

void common_hal_rgbmatrix_timer_enable(void *ptr) {
    TIM_TypeDef *tim = (TIM_TypeDef *)ptr;
    HAL_NVIC_EnableIRQ(stm_peripherals_timer_get_irqnum(tim));
}

void common_hal_rgbmatrix_timer_disable(void *ptr) {
    TIM_TypeDef *tim = (TIM_TypeDef *)ptr;
    tim->DIER &= ~TIM_DIER_UIE;
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    TIM_TypeDef *tim = (TIM_TypeDef *)ptr;
    stm_peripherals_timer_free(tim);
    common_hal_rgbmatrix_timer_disable(ptr);
}
