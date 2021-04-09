/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "peripherals/pcnt.h"

#define PCNT_UNIT_ACTIVE    1
#define PCNT_UNIT_INACTIVE  0

static uint8_t pcnt_unit_state[4];

void peripherals_pcnt_reset(void) {
    for (uint8_t i = 0; i <= 3; i++) {
        pcnt_unit_state[i] = PCNT_UNIT_INACTIVE;
    }
}

int peripherals_pcnt_init(pcnt_config_t pcnt_config) {
    // Look for available pcnt unit
    for (uint8_t i = 0; i <= 3; i++) {
        if (pcnt_unit_state[i] == PCNT_UNIT_INACTIVE) {
            pcnt_config.unit = (pcnt_unit_t)i;
            pcnt_unit_state[i] = PCNT_UNIT_ACTIVE;
            break;
        } else if (i == 3) {
            return -1;
        }
    }

    // Initialize PCNT unit
    pcnt_unit_config(&pcnt_config);

    // Initialize PCNT's counter
    pcnt_counter_pause(pcnt_config.unit);
    pcnt_counter_clear(pcnt_config.unit);

    // Everything is set up, now go to counting
    pcnt_counter_resume(pcnt_config.unit);

    return pcnt_config.unit;
}

void peripherals_pcnt_deinit(pcnt_unit_t *unit) {
    pcnt_unit_state[*unit] = PCNT_UNIT_INACTIVE;
    *unit = PCNT_UNIT_MAX;
}
