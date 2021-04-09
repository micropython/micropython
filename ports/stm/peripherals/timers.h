/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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
#include "py/mphal.h"
#include "peripherals/periph.h"

#include STM32_HAL_H

void tim_clock_enable(uint16_t mask);
void tim_clock_disable(uint16_t mask);
uint32_t stm_peripherals_timer_get_source_freq(TIM_TypeDef *timer);
size_t stm_peripherals_timer_get_irqnum(TIM_TypeDef *instance);
void timers_reset(void);
TIM_TypeDef *stm_peripherals_find_timer(void);
void stm_peripherals_timer_preinit(TIM_TypeDef *instance, uint8_t prio, void (*callback)(void));
void stm_peripherals_timer_reserve(TIM_TypeDef *instance);
void stm_peripherals_timer_free(TIM_TypeDef *instance);
void stm_peripherals_timer_never_reset(TIM_TypeDef *instance);
void stm_peripherals_timer_reset_ok(TIM_TypeDef *instance);
bool stm_peripherals_timer_is_never_reset(TIM_TypeDef *instance);
bool stm_peripherals_timer_is_reserved(TIM_TypeDef *instance);
size_t stm_peripherals_timer_get_index(TIM_TypeDef *instance);
