/* Compiler-level implementation for esp/iomux.h and esp/iomux_private.h
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
 */
#include "esp/iomux.h"
#include "common_macros.h"

const static IRAM_DATA uint32_t IOMUX_TO_GPIO[] = { 12, 13, 14, 15, 3, 1, 6, 7, 8, 9, 10, 11, 0, 2, 4, 5 };
const static IRAM_DATA uint32_t GPIO_TO_IOMUX[] = { 12, 5, 13, 4, 14, 15, 6, 7, 8, 9, 10, 11, 0, 1, 2, 3 };

uint8_t IRAM gpio_to_iomux(const uint8_t gpio_number)
{
    return GPIO_TO_IOMUX[gpio_number];
}

uint8_t IRAM iomux_to_gpio(const uint8_t iomux_number)
{
    return IOMUX_TO_GPIO[iomux_number];
}
