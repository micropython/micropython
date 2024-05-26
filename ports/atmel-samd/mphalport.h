// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "lib/oofatfs/ff.h"

#include "supervisor/shared/tick.h"

// Global millisecond tick count (driven by SysTick interrupt).
#define mp_hal_ticks_ms()       ((mp_uint_t)supervisor_ticks_ms32())

// Number of bytes in receive buffer
extern volatile uint8_t usb_rx_count;
extern volatile bool mp_cdc_enabled;

int receive_usb(void);

void mp_hal_set_interrupt_char(int c);

void mp_hal_disable_all_interrupts(void);
void mp_hal_enable_all_interrupts(void);
