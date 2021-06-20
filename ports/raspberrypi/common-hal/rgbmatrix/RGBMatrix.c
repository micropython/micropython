/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <stddef.h>

#include "py/mphal.h"

#include "common-hal/rgbmatrix/RGBMatrix.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-module/rgbmatrix/RGBMatrix.h"

#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    // Choose a PWM channel based on the first RGB pin
    uint8_t slice = pwm_gpio_to_slice_num(self->rgb_pins[0]);
    uint8_t channel = pwm_gpio_to_channel(self->rgb_pins[0]);
    int result = pwmout_allocate(slice, channel, true, 125000000 / 3);
    if (result == PWMOUT_OK) {
        // return value must be nonzero (but slice and channel can both be
        // zero), so set bit 16...
        pwmout_never_reset(slice, channel);
        return (void *)(intptr_t)(slice | (channel << 8) | 0x10000);
    }
    return NULL;
}

void common_hal_rgbmatrix_timer_enable(void *ptr) {
    int8_t slice = ((intptr_t)ptr) & 0xff;
    pwm_set_enabled(slice, false);
    pwm_clear_irq(slice);
    pwm_set_enabled(slice, true);
}

void common_hal_rgbmatrix_timer_disable(void *ptr) {
    int8_t slice = ((intptr_t)ptr) & 0xff;
    pwm_set_enabled(slice, false);
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    intptr_t value = (intptr_t)ptr;
    uint8_t slice = value & 0xff;
    uint8_t channel = value >> 8;
    pwm_set_enabled(slice, false);
    pwmout_free(slice, channel);
    return;
}
