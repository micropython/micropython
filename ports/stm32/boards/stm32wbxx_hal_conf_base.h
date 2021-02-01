/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_STM32WBXX_HAL_CONF_BASE_H
#define MICROPY_INCLUDED_STM32WBXX_HAL_CONF_BASE_H

// Include various HAL modules for convenience
#include "stm32wbxx_hal_dma.h"
#include "stm32wbxx_hal_adc.h"
#include "stm32wbxx_hal_cortex.h"
#include "stm32wbxx_hal_flash.h"
#include "stm32wbxx_hal_gpio.h"
#include "stm32wbxx_hal_i2c.h"
#include "stm32wbxx_hal_pcd.h"
#include "stm32wbxx_hal_pwr.h"
#include "stm32wbxx_hal_rcc.h"
#include "stm32wbxx_hal_rtc.h"
#include "stm32wbxx_hal_spi.h"
#include "stm32wbxx_hal_tim.h"
#include "stm32wbxx_hal_uart.h"
#include "stm32wbxx_hal_usart.h"
#include "stm32wbxx_ll_adc.h"
#include "stm32wbxx_ll_rtc.h"

// Enable various HAL modules
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED

// Oscillator values in Hz
#define MSI_VALUE   (4000000)

// SysTick has the highest priority
#define TICK_INT_PRIORITY (0x00)

// Miscellaneous HAL settings
#define DATA_CACHE_ENABLE           1
#define INSTRUCTION_CACHE_ENABLE    1
#define PREFETCH_ENABLE             0
#define USE_SPI_CRC                 0
#define USE_RTOS                    0

// HAL parameter assertions are disabled
#define assert_param(expr) ((void)0)

#endif // MICROPY_INCLUDED_STM32WBXX_HAL_CONF_BASE_H
