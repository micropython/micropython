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

#include "common-hal/rotaryio/IncrementalEncoder.h"
#include "common-hal/microcontroller/Pin.h"

#include "py/runtime.h"
#include "supervisor/shared/translate.h"

void common_hal_rotaryio_incrementalencoder_construct(rotaryio_incrementalencoder_obj_t *self,
    const mcu_pin_obj_t *pin_a, const mcu_pin_obj_t *pin_b) {
    claim_pin(pin_a);
    claim_pin(pin_b);

    // Prepare configuration for the PCNT unit
    const pcnt_config_t pcnt_config = {
        // Set PCNT input signal and control GPIOs
        .pulse_gpio_num = pin_a->number,
        .ctrl_gpio_num = pin_b->number,
        .channel = PCNT_CHANNEL_0,
        // What to do on the positive / negative edge of pulse input?
        .pos_mode = PCNT_COUNT_DEC,   // Count up on the positive edge
        .neg_mode = PCNT_COUNT_INC,   // Keep the counter value on the negative edge
        // What to do when control input is low or high?
        .lctrl_mode = PCNT_MODE_REVERSE, // Reverse counting direction if low
        .hctrl_mode = PCNT_MODE_KEEP,    // Keep the primary counter mode if high
    };

    // Initialize PCNT unit
    const int8_t unit = peripherals_pcnt_init(pcnt_config);
    if (unit == -1) {
        mp_raise_RuntimeError(translate("All PCNT units in use"));
    }

    self->pin_a = pin_a->number;
    self->pin_b = pin_b->number;
    self->unit = (pcnt_unit_t)unit;
}

bool common_hal_rotaryio_incrementalencoder_deinited(rotaryio_incrementalencoder_obj_t *self) {
    return self->unit == PCNT_UNIT_MAX;
}

void common_hal_rotaryio_incrementalencoder_deinit(rotaryio_incrementalencoder_obj_t *self) {
    if (common_hal_rotaryio_incrementalencoder_deinited(self)) {
        return;
    }
    reset_pin_number(self->pin_a);
    reset_pin_number(self->pin_b);
    peripherals_pcnt_deinit(&self->unit);
}

mp_int_t common_hal_rotaryio_incrementalencoder_get_position(rotaryio_incrementalencoder_obj_t *self) {
    int16_t count;
    pcnt_get_counter_value(self->unit, &count);
    return (count / 2) + self->position;
}

void common_hal_rotaryio_incrementalencoder_set_position(rotaryio_incrementalencoder_obj_t *self,
    mp_int_t new_position) {
    self->position = new_position;
    pcnt_counter_clear(self->unit);
}
