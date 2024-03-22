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
#ifndef MICROPY_INCLUDED_STM32G4XX_HAL_CONF_BASE_H
#define MICROPY_INCLUDED_STM32G4XX_HAL_CONF_BASE_H

#define HAL_MODULE_ENABLED
#define HAL_ADC_MODULE_ENABLED
#define HAL_COMP_MODULE_ENABLED
#define HAL_CORDIC_MODULE_ENABLED
#define HAL_CORTEX_MODULE_ENABLED
#define HAL_CRC_MODULE_ENABLED
#define HAL_CRYP_MODULE_ENABLED
#define HAL_DAC_MODULE_ENABLED
#define HAL_DMA_MODULE_ENABLED
#define HAL_EXTI_MODULE_ENABLED
#define HAL_FDCAN_MODULE_ENABLED
#define HAL_FLASH_MODULE_ENABLED
#define HAL_FMAC_MODULE_ENABLED
#define HAL_GPIO_MODULE_ENABLED
#define HAL_HRTIM_MODULE_ENABLED
#define HAL_IRDA_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED
#define HAL_I2C_MODULE_ENABLED
#define HAL_I2S_MODULE_ENABLED
#define HAL_LPTIM_MODULE_ENABLED
#define HAL_NAND_MODULE_ENABLED
#define HAL_NOR_MODULE_ENABLED
#define HAL_OPAMP_MODULE_ENABLED
#define HAL_PCD_MODULE_ENABLED
#define HAL_PWR_MODULE_ENABLED
#define HAL_QSPI_MODULE_ENABLED
#define HAL_RCC_MODULE_ENABLED
#define HAL_RNG_MODULE_ENABLED
#define HAL_RTC_MODULE_ENABLED
#define HAL_SAI_MODULE_ENABLED
#define HAL_SMARTCARD_MODULE_ENABLED
#define HAL_SMBUS_MODULE_ENABLED
#define HAL_SPI_MODULE_ENABLED
#define HAL_SRAM_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED
#define HAL_UART_MODULE_ENABLED
#define HAL_USART_MODULE_ENABLED
#define HAL_WWDG_MODULE_ENABLED

// Oscillator values in Hz
#define HSI_VALUE    (16000000)
#define HSI48_VALUE  (48000000)
#define LSI_VALUE    (32000)

// SysTick priority
#define TICK_INT_PRIORITY (0x0F)

// Miscellaneous HAL settings
#define  USE_RTOS                   0
#define  PREFETCH_ENABLE            0
#define  INSTRUCTION_CACHE_ENABLE   1
#define  DATA_CACHE_ENABLE          1
#define  USE_SPI_CRC                1

// Include various HAL modules for convenience
#include "stm32g4xx_hal_rcc.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_dma.h"
#include "stm32g4xx_hal_cortex.h"
#include "stm32g4xx_hal_adc.h"
#include "stm32g4xx_hal_comp.h"
#include "stm32g4xx_hal_cordic.h"
#include "stm32g4xx_hal_crc.h"
#include "stm32g4xx_hal_cryp.h"
#include "stm32g4xx_hal_dac.h"
#include "stm32g4xx_hal_exti.h"
#include "stm32g4xx_hal_fdcan.h"
#include "stm32g4xx_hal_flash.h"
#include "stm32g4xx_hal_fmac.h"
#include "stm32g4xx_hal_hrtim.h"
#include "stm32g4xx_hal_irda.h"
#include "stm32g4xx_hal_iwdg.h"
#include "stm32g4xx_hal_i2c.h"
#include "stm32g4xx_hal_i2s.h"
#include "stm32g4xx_hal_lptim.h"
#include "stm32g4xx_hal_nand.h"
#include "stm32g4xx_hal_nor.h"
#include "stm32g4xx_hal_opamp.h"
#include "stm32g4xx_hal_pcd.h"
#include "stm32g4xx_hal_pwr.h"
#include "stm32g4xx_hal_qspi.h"
#include "stm32g4xx_hal_rng.h"
#include "stm32g4xx_hal_rtc.h"
#include "stm32g4xx_hal_sai.h"
#include "stm32g4xx_hal_smartcard.h"
#include "stm32g4xx_hal_smbus.h"
#include "stm32g4xx_hal_spi.h"
#include "stm32g4xx_hal_sram.h"
#include "stm32g4xx_hal_tim.h"
#include "stm32g4xx_hal_uart.h"
#include "stm32g4xx_hal_usart.h"
#include "stm32g4xx_hal_wwdg.h"
#include "stm32g4xx_ll_adc.h"
#include "stm32g4xx_ll_rtc.h"
#include "stm32g4xx_ll_usart.h"
#include "stm32g4xx_ll_lpuart.h"

// HAL parameter assertions are disabled
#define assert_param(expr) ((void)0)

#endif // MICROPY_INCLUDED_STM32G4XX_HAL_CONF_BASE_H
