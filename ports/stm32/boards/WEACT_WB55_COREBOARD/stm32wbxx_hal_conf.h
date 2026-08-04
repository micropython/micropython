/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2019 Damien P. George
 */
#ifndef MICROPY_INCLUDED_STM32WBXX_HAL_CONF_H
#define MICROPY_INCLUDED_STM32WBXX_HAL_CONF_H

// Oscillator values in Hz
#define HSE_VALUE (32000000)
#define LSE_VALUE (32768)
#define EXTERNAL_SAI1_CLOCK_VALUE (48000)

// Oscillator timeouts in ms
#define HSE_STARTUP_TIMEOUT (100)
#define LSE_STARTUP_TIMEOUT (5000)

#include "boards/stm32wbxx_hal_conf_base.h"

#endif // MICROPY_INCLUDED_STM32WBXX_HAL_CONF_H
