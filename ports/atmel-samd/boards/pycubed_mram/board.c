// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/board.h"
#include "mpconfigboard.h"
#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_gpio.h"
#include "shared-bindings/pwmio/PWMOut.h"

void board_init(void) {
    pwmio_pwmout_obj_t pwm;
    common_hal_pwmio_pwmout_construct(&pwm, &pin_PA23, 4096, 2, false);
    common_hal_pwmio_pwmout_never_reset(&pwm);
}

// Use the MP_WEAK supervisor/shared/board.c versions of routines not defined here.
