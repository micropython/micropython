// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Global millisecond tick count (driven by SysTick interrupt).
#define mp_hal_ticks_ms()       ((mp_uint_t)supervisor_ticks_ms32())

void mp_hal_set_interrupt_char(int c);

void mp_hal_disable_all_interrupts(void);
void mp_hal_enable_all_interrupts(void);
