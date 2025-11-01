/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2025 Mike Tolkachev <contact@miketolkachev.dev>
 */

#ifndef MICROPY_INCLUDED_STM32F4XX_HAL_CONF_H
#define MICROPY_INCLUDED_STM32F4XX_HAL_CONF_H

// Oscillator values in Hz
#define HSE_VALUE (8000000)
#define LSE_VALUE (32768)
#define EXTERNAL_CLOCK_VALUE (12288000)

// Oscillator timeouts in ms
#define HSE_STARTUP_TIMEOUT (100)
#define LSE_STARTUP_TIMEOUT (5000)

#define HAL_DMA2D_MODULE_ENABLED
#define HAL_LTDC_MODULE_ENABLED
#define HAL_QSPI_MODULE_ENABLED
#define HAL_DSI_MODULE_ENABLED

#include "boards/stm32f4xx_hal_conf_base.h"

#ifdef HAL_DMA2D_MODULE_ENABLED
#include "stm32f4xx_hal_dma2d.h"
#endif /* HAL_DMA2D_MODULE_ENABLED */

#ifdef HAL_LTDC_MODULE_ENABLED
#include "stm32f4xx_hal_ltdc.h"
#endif /* HAL_LTDC_MODULE_ENABLED */

#ifdef HAL_QSPI_MODULE_ENABLED
#include "stm32f4xx_hal_qspi.h"
#endif /* HAL_QSPI_MODULE_ENABLED */

#ifdef HAL_DSI_MODULE_ENABLED
#include "stm32f4xx_hal_dsi.h"
#endif /* HAL_DSI_MODULE_ENABLED */

#endif // MICROPY_INCLUDED_STM32F4XX_HAL_CONF_H
