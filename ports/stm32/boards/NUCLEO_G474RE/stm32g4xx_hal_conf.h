/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2019 Damien P. George
 */
#ifndef MICROPY_INCLUDED_STM32G4xx_HAL_CONF_H
#define MICROPY_INCLUDED_STM32G4xx_HAL_CONF_H

#include "boards/stm32g4xx_hal_conf_base.h"

// Oscillator values in Hz
#define HSE_VALUE    (24000000UL) 
#define LSE_VALUE    (32768UL)
#define EXTERNAL_CLOCK_VALUE    (24000UL)

// Oscillator timeouts in ms
#define HSE_STARTUP_TIMEOUT    (100UL)
#define LSE_STARTUP_TIMEOUT    (5000UL)

#endif // MICROPY_INCLUDED_STM32G4xx_HAL_CONF_H
