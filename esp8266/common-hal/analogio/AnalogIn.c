/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/binary.h"
#include "py/mphal.h"
#include "common-hal/microcontroller/__init__.h"
#include "shared-bindings/analogio/AnalogIn.h"

#include "user_interface.h"

volatile bool adc_in_use __attribute__((aligned(4))) = false;

void common_hal_analogio_analogin_construct(analogio_analogin_obj_t* self,
        const mcu_pin_obj_t *pin) {
    if (pin != &pin_TOUT) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "Pin %q does not have ADC capabilities", pin->name));
    }
    adc_in_use = true;
}

bool common_hal_analogio_analogin_deinited(analogio_analogin_obj_t* self) {
    return self->deinited;
}

void common_hal_analogio_analogin_deinit(analogio_analogin_obj_t* self) {
    if (common_hal_analogio_analogin_deinited(self)) {
        return;
    }
    adc_in_use = false;
    self->deinited = true;
}

uint16_t common_hal_analogio_analogin_get_value(analogio_analogin_obj_t *self) {
    // ADC is 10 bit so shift by 6 to make it 16-bit.
    return system_adc_read() << 6;
}

float common_hal_analogio_analogin_get_reference_voltage(analogio_analogin_obj_t *self) {
    return 1.0f;
}
