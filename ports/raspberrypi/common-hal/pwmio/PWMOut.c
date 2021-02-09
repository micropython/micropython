/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/runtime.h"
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/microcontroller/Processor.h"

#include "supervisor/shared/translate.h"

#include "src/rp2040/hardware_regs/include/hardware/platform_defs.h"
#include "src/rp2_common/hardware_clocks/include/hardware/clocks.h"
#include "src/rp2_common/hardware_gpio/include/hardware/gpio.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"

uint32_t target_slice_frequencies[NUM_PWM_SLICES];
uint32_t slice_fixed_frequency;

#define CHANNELS_PER_SLICE 2
static uint32_t channel_use;
static uint32_t never_reset_channel;

static uint32_t _mask(uint8_t slice, uint8_t channel) {
    return 1 << (slice * CHANNELS_PER_SLICE + channel);
}

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
    never_reset_channel |= _mask(self->slice, self->channel);

    never_reset_pin_number(self->pin->number);
}

void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {
    never_reset_channel &= ~_mask(self->slice, self->channel);
}

void pwmout_reset(void) {
    // Reset all slices
    for (size_t slice = 0; slice < NUM_PWM_SLICES; slice++) {
        bool reset = true;
        for (size_t channel = 0; channel < CHANNELS_PER_SLICE; channel++) {
            uint32_t channel_use_mask = _mask(slice, channel);
            if ((never_reset_channel & channel_use_mask) != 0) {
                reset = false;
                continue;
            }
            channel_use &= ~channel_use_mask;
        }
        if (!reset) {
            continue;
        }
        pwm_set_enabled(slice, false);
        target_slice_frequencies[slice] = 0;
        slice_fixed_frequency &= ~(1 << slice);
    }
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t* self,
                                                    const mcu_pin_obj_t* pin,
                                                    uint16_t duty,
                                                    uint32_t frequency,
                                                    bool variable_frequency) {
    self->pin = pin;
    self->variable_frequency = variable_frequency;
    self->duty_cycle = duty;

    if (frequency == 0 || frequency > (common_hal_mcu_processor_get_frequency() / 2)) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    uint8_t slice = pwm_gpio_to_slice_num(pin->number);
    uint8_t channel = pwm_gpio_to_channel(pin->number);
    uint32_t channel_use_mask = _mask(slice, channel);

    // Check the channel first.
    if ((channel_use & channel_use_mask) != 0) {
        return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
    }
    // Now check if the slice is in use and if we can share with it.
    if (target_slice_frequencies[slice] > 0) {
        // If we want to change frequency then we can't share.
        if (variable_frequency) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }
        // If the other user wants a variable frequency then we can't share either.
        if ((slice_fixed_frequency & (1 << slice)) != 0) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }
        // If we're both fixed frequency but we don't match target frequencies then we can't share.
        if (target_slice_frequencies[slice] != frequency) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }
    }
    self->slice = slice;
    self->channel = channel;
    channel_use |= channel_use_mask;
    if (variable_frequency) {
        slice_fixed_frequency |= 1 << slice;
    }

    if (target_slice_frequencies[slice] != frequency) {
        // Reset the counter and compare values.
        pwm_hw->slice[slice].ctr = PWM_CH0_CTR_RESET;
        common_hal_pwmio_pwmout_set_duty_cycle(self, duty);
        common_hal_pwmio_pwmout_set_frequency(self, frequency);
        pwm_set_enabled(slice, true);
    } else {
        common_hal_pwmio_pwmout_set_duty_cycle(self, duty);
    }

    // Connect to the pad last to avoid any glitches from changing settings.
    gpio_set_function(pin->number, GPIO_FUNC_PWM);

    return PWMOUT_OK;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t* self) {
    return self->pin == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t* self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }
    uint32_t channel_mask = _mask(self->slice, self->channel);
    channel_use &= ~channel_mask;
    never_reset_channel &= ~channel_mask;
    uint32_t slice_mask = ((1 << CHANNELS_PER_SLICE) - 1) << (self->slice * CHANNELS_PER_SLICE + self->channel);
    if ((channel_use & slice_mask) == 0) {
        target_slice_frequencies[self->slice] = 0;
        slice_fixed_frequency &= ~(1 << self->slice);
        pwm_set_enabled(self->slice, false);
    }

    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

extern void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t* self, uint16_t duty) {
    self->duty_cycle = duty;
    uint16_t actual_duty = duty * self->top / ((1 << 16) - 1);
    pwm_set_chan_level(self->slice, self->channel, actual_duty);
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t* self) {
    return self->duty_cycle;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t* self, uint32_t frequency) {
    if (frequency == 0 || frequency > (common_hal_mcu_processor_get_frequency() / 2)) {
        mp_raise_ValueError(translate("Invalid PWM frequency"));
    }

    target_slice_frequencies[self->slice] = frequency;

    // For low frequencies use the divider to give us full resolution duty_cycle.
    if (frequency < (common_hal_mcu_processor_get_frequency() / (1 << 16))) {
        // Compute the divisor. It's an 8 bit integer and 4 bit fraction. Therefore,
        // we compute everything * 16 for the fractional part.
        // This is 1 << 12 because 4 bits are the * 16.
        uint64_t frequency16 = ((uint64_t) clock_get_hz(clk_sys)) / (1 << 12);
        uint64_t div16 = frequency16 / frequency;
        // Round the divisor to try and get closest to the target frequency. We could
        // also always round up and use TOP to get us closer. We may not need that though.
        if (frequency16 % frequency >= frequency / 2) {
            div16 += 1;
        }
        if (div16 >= (1 << 12)) {
            div16 = (1 << 12) - 1;
        }
        self->actual_frequency = frequency16 / div16;
        self->top = 1 << 16;
        pwm_set_clkdiv_int_frac(self->slice, div16 / 16, div16 % 16);
        pwm_set_wrap(self->slice, self->top - 1);
    } else {
        uint32_t top = common_hal_mcu_processor_get_frequency() / frequency;
        self->actual_frequency = common_hal_mcu_processor_get_frequency() / top;
        self->top = top;
        pwm_set_clkdiv_int_frac(self->slice, 1, 0);
        pwm_set_wrap(self->slice, self->top - 1);
    }
    common_hal_pwmio_pwmout_set_duty_cycle(self, self->duty_cycle);
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t* self) {
    return self->actual_frequency;
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t* self) {
    return self->variable_frequency;
}
