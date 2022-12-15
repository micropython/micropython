/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
 * Copyright (c) 2019 Artur Pacholec
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_MICROCONTROLLER_PIN_H
#define MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_MICROCONTROLLER_PIN_H

#include <assert.h>

#include "pins.h"

void reset_all_pins(void);
void claim_pin(const mcu_pin_obj_t *pin);

// List of pins that should never be reset.
extern const mcu_pin_obj_t *mimxrt10xx_reset_forbidden_pins[];

// Allow the board to reset a pin in a board-specific way. This can be used
// for LEDs or enable pins to put them in a state beside the default pull-up,
// or to simply not reset the pin at all.
// Return true to indicate that the pin was handled in a special way. Returning false will lead to
// the port-default reset behavior.
extern bool mimxrt10xx_board_reset_pin_number(const mcu_pin_obj_t *pin);

#endif // MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_MICROCONTROLLER_PIN_H
