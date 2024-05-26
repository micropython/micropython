// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "background.h"

#include "py/runtime.h"
#include "supervisor/port.h"

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#if CIRCUITPY_AUDIOBUSIO
#include "common-hal/audiobusio/I2SOut.h"
#endif

#if CIRCUITPY_AUDIOPWMIO
#include "common-hal/audiopwmio/PWMAudioOut.h"
#endif

void port_start_background_tick(void) {
}

void port_finish_background_tick(void) {
}

void port_background_tick(void) {
    #if CIRCUITPY_AUDIOPWMIO
    audiopwmout_background();
    #endif
    #if CIRCUITPY_AUDIOBUSIO
    i2s_background();
    #endif
}

// Allow boards to override this.
MP_WEAK void board_background_task(void) {
}

void port_background_task(void) {
    board_background_task();
}
