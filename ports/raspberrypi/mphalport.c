// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <string.h>

#include "shared/readline/readline.h"
#include "shared/runtime/interrupt_char.h"
#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/smallint.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/time/__init__.h"

#include "mpconfigboard.h"
#include "mphalport.h"
#include "supervisor/shared/tick.h"

#include "src/rp2_common/hardware_timer/include/hardware/timer.h"

extern uint32_t common_hal_mcu_processor_get_frequency(void);

void mp_hal_delay_us(mp_uint_t delay) {
    busy_wait_us_32(delay);
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}
