// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "peripherals/gpio.h"
#include "common-hal/microcontroller/Pin.h"

void stm32_peripherals_gpio_init(void) {

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Never reset pins
    never_reset_pin_number(2, 13); // PC13 anti tamp
    never_reset_pin_number(2, 14); // PC14 OSC32_IN
    never_reset_pin_number(2, 15); // PC15 OSC32_OUT
    never_reset_pin_number(0, 13); // PA13 SWDIO
    never_reset_pin_number(0, 14); // PA14 SWCLK
    // never_reset_pin_number(0,15); //PA15 JTDI
    // never_reset_pin_number(1,3); //PB3 JTDO
    // never_reset_pin_number(1,4); //PB4 JTRST

    // Port H is not included in GPIO port array
    // never_reset_pin_number(5,0); //PH0 JTDO
    // never_reset_pin_number(5,1); //PH1 JTRST
}
