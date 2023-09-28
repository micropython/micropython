/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2021 Lorenzo Cappelletti
 */
#ifndef MICROPY_INCLUDED_STM32F4XX_HAL_CONF_H
#define MICROPY_INCLUDED_STM32F4XX_HAL_CONF_H

// Oscillator values in Hz
#define HSE_VALUE (12000000)
#define EXTERNAL_CLOCK_VALUE (12288000)

// Oscillator timeouts in ms
#define HSE_STARTUP_TIMEOUT (100)
#define LSE_STARTUP_TIMEOUT (0)

#include "boards/stm32f4xx_hal_conf_base.h"

#endif // MICROPY_INCLUDED_STM32F4XX_HAL_CONF_H
