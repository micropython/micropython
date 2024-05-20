// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include STM32_HAL_H

#define STM32_GPIO_PORT_SIZE 16

void exti_reset(void);
void stm_peripherals_exti_never_reset(uint8_t num);
void stm_peripherals_exti_reset_exti(uint8_t num);
bool stm_peripherals_exti_is_free(uint8_t num);
bool stm_peripherals_exti_reserve(uint8_t num);
void stm_peripherals_exti_enable(uint8_t num);
void stm_peripherals_exti_disable(uint8_t num);
void stm_peripherals_exti_set_callback(void (*callback)(uint8_t num), uint8_t number);
void stm_peripherals_exti_free(uint8_t num);
IRQn_Type stm_peripherals_exti_get_irq(uint8_t num);
