// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
