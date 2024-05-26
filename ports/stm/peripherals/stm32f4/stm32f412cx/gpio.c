// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "peripherals/gpio.h"
#include "common-hal/microcontroller/Pin.h"

void stm32_peripherals_gpio_init(void) {

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Never reset pins
    never_reset_pin_number(0, 13); // PA13 SWDIO
    never_reset_pin_number(0, 14); // PA14 SWCLK
}
