/* This file is part of the MicroPython project, http://micropython.org/
 * The MIT License (MIT)
 * Copyright (c) 2019 Damien P. George
 */
#ifndef MICROPY_INCLUDED_STM32N6XX_HAL_CONF_H
#define MICROPY_INCLUDED_STM32N6XX_HAL_CONF_H

// Oscillator values in Hz
#define HSE_VALUE (48000000)
#define LSE_VALUE (32768)

// Oscillator timeouts in ms
#define HSE_STARTUP_TIMEOUT (100)
#define LSE_STARTUP_TIMEOUT (5000)

// Enable the HAL's SD card 1.8V switch support, for UHS-I mode.
#define USE_SD_TRANSCEIVER (1)

#include "boards/stm32n6xx_hal_conf_base.h"

#endif // MICROPY_INCLUDED_STM32N6XX_HAL_CONF_H
