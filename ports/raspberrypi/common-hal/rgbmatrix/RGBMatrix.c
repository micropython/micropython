// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stddef.h>

#include "py/mphal.h"

#include "common-hal/rgbmatrix/RGBMatrix.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-module/rgbmatrix/RGBMatrix.h"

#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"
#include "src/rp2_common/hardware_irq/include/hardware/irq.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    // Choose a PWM channel based on the first RGB pin
    uint8_t slice = pwm_gpio_to_slice_num(self->rgb_pins[0]);
    uint8_t channel = pwm_gpio_to_channel(self->rgb_pins[0]);
    int result = pwmout_allocate(slice, channel, true, 125000000 / 3);
    if (result == PWMOUT_OK) {
        // return value must be nonzero (but slice and channel can both be
        // zero), so set bit 16...
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
    irq_set_enabled(PWM_IRQ_WRAP, false);
    pwm_clear_irq(slice);
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    intptr_t value = (intptr_t)ptr;
    uint8_t slice = value & 0xff;
    uint8_t channel = value >> 8;
    pwm_set_enabled(slice, false);
    irq_set_enabled(PWM_IRQ_WRAP, false);
    pwm_clear_irq(slice);
    pwmout_free(slice, channel);
    return;
}
