// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
// SPDX-FileCopyrightText: Copyright (c) 2019 Artur Pacholec
//
// SPDX-License-Identifier: MIT

#include "py/mphal.h"
#include "py/mpstate.h"
#include "py/smallint.h"

#include "shared-bindings/microcontroller/__init__.h"
#include "supervisor/shared/tick.h"

#include "fsl_common.h"

void mp_hal_delay_us(mp_uint_t delay) {
    SDK_DelayAtLeastUs(delay, SystemCoreClock);
}

void mp_hal_disable_all_interrupts(void) {
    common_hal_mcu_disable_interrupts();
}

void mp_hal_enable_all_interrupts(void) {
    common_hal_mcu_enable_interrupts();
}
