/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Matt Trentini
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
#ifndef MICROPY_INCLUDED_STM32C5XX_HAL_CONF_BASE_H
#define MICROPY_INCLUDED_STM32C5XX_HAL_CONF_BASE_H

// Enable various HAL modules (old-style for MicroPython compatibility)
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_ICACHE_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED

// CubeMX2 HAL module enables (for stm32_hal.h conditional includes)
#define USE_HAL_RCC_MODULE          1
#define USE_HAL_GPIO_MODULE         1
#define USE_HAL_DMA_MODULE          1
#define USE_HAL_CORTEX_MODULE       1
#define USE_HAL_ADC_MODULE          1
#define USE_HAL_CRC_MODULE          1
#define USE_HAL_FLASH_MODULE        1
#define USE_HAL_I2C_MODULE          1
#define USE_HAL_ICACHE_MODULE       1
#define USE_HAL_IWDG_MODULE         1
#define USE_HAL_PCD_MODULE          1
#define USE_HAL_PWR_MODULE          1
#define USE_HAL_RTC_MODULE          1
#define USE_HAL_SPI_MODULE          1
#define USE_HAL_TIM_MODULE          1
#define USE_HAL_UART_MODULE         1
#define USE_HAL_WWDG_MODULE         1

// Oscillator values in Hz
// C5 uses HSI at 144MHz (no separate CSI/HSI48)
#define HSI_VALUE (144000000)
#define LSI_VALUE (32000)

// SysTick has the highest priority
#define TICK_INT_PRIORITY (0x00)

// CubeMX2 HAL system configuration
#define USE_HAL_TICK_INT_PRIORITY   0
#define USE_HAL_FLASH_PREFETCH      1U
#define USE_HAL_MUTEX               0U
#define USE_HAL_CHECK_PARAM         0U
#define USE_HAL_SECURE_CHECK_PARAM  0U
#define USE_HAL_CHECK_PROCESS_STATE 0U

// Miscellaneous HAL settings
#define USE_RTOS            0
#define USE_SPI_CRC         1

// HAL parameter assertions are disabled
#define assert_param(expr) ((void)0)

#endif // MICROPY_INCLUDED_STM32C5XX_HAL_CONF_BASE_H
