// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2020 Mark Olsson <mark@markolsson.se>
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "stm32f7xx_hal.h"
#include "common-hal/microcontroller/Pin.h"

void board_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    __HAL_RCC_GPIOK_CLK_ENABLE();

    /*
     * Turn off the backlight as it is distracting during development.
     * LCD_BL_CTRL = PK3
     */
    GPIO_InitStructure.Pin = GPIO_PIN_3;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOK, &GPIO_InitStructure);
    HAL_GPIO_WritePin(GPIOK, GPIO_PIN_3, GPIO_PIN_RESET);

    never_reset_pin_number(10, 3);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
