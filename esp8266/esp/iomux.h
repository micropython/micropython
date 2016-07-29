/** esp/iomux.h
 *
 * Configuration of iomux registers.
 *
 * Part of esp-open-rtos
 * Copyright (C) 2015 Superhouse Automation Pty Ltd
 * BSD Licensed as described in the file LICENSE
 */
#ifndef _ESP_IOMUX_H
#define _ESP_IOMUX_H
#include "esp/types.h"
#include "esp/iomux_regs.h"

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Convert a GPIO pin number to an iomux register index.
 *
 * This function should evaluate to a constant if the gpio_number is
 * known at compile time, or return the result from a lookup table if not.
 *
 */
uint8_t IRAM gpio_to_iomux(const uint8_t gpio_number);

/**
 * Convert an iomux register index to a GPIO pin number.
 *
 * This function should evaluate to a constant if the iomux_num is
 * known at compile time, or return the result from a lookup table if not.
 *
 */
uint8_t IRAM iomux_to_gpio(const uint8_t iomux_num);

/**
 * Directly get the IOMUX register for a particular gpio number
 *
 * ie *gpio_iomux_reg(3) is equivalent to IOMUX_GPIO3
 */
inline static esp_reg_t gpio_iomux_reg(const uint8_t gpio_number)
{
    return &(IOMUX.PIN[gpio_to_iomux(gpio_number)]);
}

inline static void iomux_set_function(uint8_t iomux_num, uint32_t func)
{
    uint32_t prev = IOMUX.PIN[iomux_num] & ~IOMUX_PIN_FUNC_MASK;
    IOMUX.PIN[iomux_num] = IOMUX_FUNC(func) | prev;
}

inline static void iomux_set_direction_flags(uint8_t iomux_num, uint32_t dir_flags)
{
    uint32_t mask = IOMUX_PIN_OUTPUT_ENABLE | IOMUX_PIN_OUTPUT_ENABLE_SLEEP;
    uint32_t prev = IOMUX.PIN[iomux_num] & ~mask;
    IOMUX.PIN[iomux_num] = dir_flags | prev;
}

inline static void iomux_set_pullup_flags(uint8_t iomux_num, uint32_t pullup_flags)
{
    uint32_t mask = IOMUX_PIN_PULLUP | IOMUX_PIN_PULLDOWN | IOMUX_PIN_PULLUP_SLEEP | IOMUX_PIN_PULLDOWN_SLEEP;
    uint32_t prev = IOMUX.PIN[iomux_num] & ~mask;
    IOMUX.PIN[iomux_num] = pullup_flags | prev;
}

/**
 * Set a pin to the GPIO function.
 *
 * This allows you to set pins to GPIO without knowing in advance the
 * exact register masks to use.
 *
 * Sets the function and direction, but leaves the pullup configuration the
 * same as before.
 */
inline static void iomux_set_gpio_function(uint8_t gpio_number, bool output_enable)
{
    const uint8_t iomux_num = gpio_to_iomux(gpio_number);
    const uint32_t func = iomux_num > 11 ? 0 : 3;
    iomux_set_function(iomux_num, func);
    iomux_set_direction_flags(iomux_num, output_enable ? IOMUX_PIN_OUTPUT_ENABLE : 0);
}

#ifdef	__cplusplus
}
#endif

#endif
