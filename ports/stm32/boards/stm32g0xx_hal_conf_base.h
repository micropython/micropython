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

#ifndef MICROPY_INCLUDED_STM32G0XX_HAL_CONF_BASE_H
#define MICROPY_INCLUDED_STM32G0XX_HAL_CONF_BASE_H

// Oscillator values in Hz
// These must come before the HAL headers because stm32g0xx_ll_rcc.h will define HSI_VALUE unless already defined
#define HSI_VALUE    (16000000)
#define LSI_VALUE    (32000)
#if defined(STM32G0C1xx) || defined(STM32G0B1xx) || defined(STM32G0B0xx)
#define HSI48_VALUE   48000000
#endif

// Include various HAL modules for convenience

#include "stm32g0xx_hal_rcc.h"
#include "stm32g0xx_hal_gpio.h"
#include "stm32g0xx_hal_dma.h"
#include "stm32g0xx_hal_cortex.h"
#include "stm32g0xx_hal_adc.h"
#include "stm32g0xx_hal_adc_ex.h"
#include "stm32g0xx_hal_cec.h"
#include "stm32g0xx_hal_comp.h"
#include "stm32g0xx_hal_crc.h"
#include "stm32g0xx_hal_cryp.h"
#include "stm32g0xx_hal_dac.h"
#include "stm32g0xx_hal_exti.h"
#include "stm32g0xx_hal_fdcan.h"
#include "stm32g0xx_hal_flash.h"
#include "stm32g0xx_hal_i2c.h"
#include "stm32g0xx_hal_i2s.h"
#include "stm32g0xx_hal_irda.h"
#include "stm32g0xx_hal_iwdg.h"
#include "stm32g0xx_hal_lptim.h"
#include "stm32g0xx_hal_pcd.h"
#include "stm32g0xx_hal_hcd.h"
#include "stm32g0xx_hal_pwr.h"
#include "stm32g0xx_hal_rng.h"
#include "stm32g0xx_hal_rtc.h"
#include "stm32g0xx_hal_smartcard.h"
#include "stm32g0xx_hal_smbus.h"
#include "stm32g0xx_hal_spi.h"
#include "stm32g0xx_hal_tim.h"
#include "stm32g0xx_hal_uart.h"
#include "stm32g0xx_hal_usart.h"
#include "stm32g0xx_hal_wwdg.h"

#include "stm32g0xx_ll_lpuart.h"
#include "stm32g0xx_ll_rtc.h"
#include "stm32g0xx_ll_usart.h"

// Enable various HAL modules
#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED

// SysTick has the highest priority
#define TICK_INT_PRIORITY (0x00)

// Miscellaneous HAL settings
#define  USE_RTOS                     0
#define  PREFETCH_ENABLE              1
#define  INSTRUCTION_CACHE_ENABLE     1
#define  USE_SPI_CRC                  1
#define  USE_HAL_CRYP_SUSPEND_RESUME  1

// HAL parameter assertions are disabled
#define assert_param(expr) ((void)0)

#endif // MICROPY_INCLUDED_STM32G0XX_HAL_CONF_BASE_H
