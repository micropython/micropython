// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Lucian Copeland for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "peripherals/gpio.h"
#include "stm32f4xx_hal.h"
#include "common-hal/microcontroller/Pin.h"

void stm32_peripherals_gpio_init(void) {
    // * GPIO Ports Clock Enable */
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    // Never reset pins
    // TODO: Move this out of peripherals. These helpers shouldn't reference anything CircuitPython
    // specific.
    never_reset_pin_number(2, 14); // PC14 OSC32_IN
    never_reset_pin_number(2, 15); // PC15 OSC32_OUT

    #if !(BOARD_OVERWRITE_SWD)
    never_reset_pin_number(0, 13); // PA13 SWDIO
    never_reset_pin_number(0, 14); // PA14 SWCLK
    #endif

    // Port H is not included in GPIO port array
    // never_reset_pin_number(5,0); //PH0 JTDO
    // never_reset_pin_number(5,1); //PH1 JTRST
}

// LEDs are inverted on F411 DISCO
void stm32f4_peripherals_status_led(uint8_t led, uint8_t state) {
}
