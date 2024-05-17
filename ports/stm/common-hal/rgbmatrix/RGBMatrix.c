// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
