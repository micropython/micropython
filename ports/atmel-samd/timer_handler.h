// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#pragma once

#define TC_HANDLER_NO_INTERRUPT 0x0
#define TC_HANDLER_PULSEOUT 0x1
#define TC_HANDLER_PEW 0x2
#define TC_HANDLER_FREQUENCYIN 0x3
#define TC_HANDLER_RGBMATRIX 0x4
#define TC_HANDLER_PULSEIN 0x5

void set_timer_handler(bool is_tc, uint8_t index, uint8_t timer_handler);
void shared_timer_handler(bool is_tc, uint8_t index);

// Mirror of turn_on_clocks() from peripherals/samd. Used by deinit paths that
// release a timer so the generic clock channel and APB clock are freed.
void turn_off_clocks(bool is_tc, uint8_t index, uint32_t gclk_index);
