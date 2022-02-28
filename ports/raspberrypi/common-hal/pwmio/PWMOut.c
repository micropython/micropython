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

#include "shared/runtime/interrupt_char.h"
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
uint32_t slice_variable_frequency;

#define AB_CHANNELS_PER_SLICE 2
static uint32_t channel_use;
static uint32_t never_reset_channel;

// Per the RP2040 datasheet:
//
// "A CC value of 0 will produce a 0% output, i.e. the output signal
// is always low. A CC value of TOP + 1 (i.e. equal to the period, in
// non-phase-correct mode) will produce a 100% output. For example, if
// TOP is programmed to 254, the counter will have a period of 255
// cycles, and CC values in the range of 0 to 255 inclusive will
// produce duty cycles in the range 0% to 100% inclusive."
//
// So 65534 should be the maximum top value, and we'll set CC to be TOP+1 as appropriate.
#define MAX_TOP 65534

static uint32_t _mask(uint8_t slice, uint8_t ab_channel) {
    return 1 << (slice * AB_CHANNELS_PER_SLICE + ab_channel);
}

bool pwmio_claim_slice_ab_channels(uint8_t slice) {
    uint32_t channel_use_mask_a = _mask(slice, 0);
    uint32_t channel_use_mask_b = _mask(slice, 1);

    if ((channel_use & channel_use_mask_a) != 0) {
        return false;
    }
    if ((channel_use & channel_use_mask_b) != 0) {
        return false;
    }

    channel_use |= channel_use_mask_a;
    channel_use |= channel_use_mask_b;
    return true;
}

void pwmio_release_slice_ab_channels(uint8_t slice) {
    uint32_t channel_mask = _mask(slice, 0);
    channel_use &= ~channel_mask;
    channel_mask = _mask(slice, 1);
    channel_use &= ~channel_mask;
}

void pwmout_never_reset(uint8_t slice, uint8_t ab_channel) {
    never_reset_channel |= _mask(slice, ab_channel);
}

void pwmout_reset_ok(uint8_t slice, uint8_t ab_channel) {
    never_reset_channel &= ~_mask(slice, ab_channel);
}

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
    pwmout_never_reset(self->slice, self->ab_channel);

    never_reset_pin_number(self->pin->number);
}

void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {
    pwmout_reset_ok(self->slice, self->ab_channel);
}

void pwmout_reset(void) {
    // Reset all slices
    for (size_t slice = 0; slice < NUM_PWM_SLICES; slice++) {
        bool reset = true;
        for (size_t ab_channel = 0; ab_channel < AB_CHANNELS_PER_SLICE; ab_channel++) {
            uint32_t channel_use_mask = _mask(slice, ab_channel);
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
        slice_variable_frequency &= ~(1 << slice);
    }
}

pwmout_result_t pwmout_allocate(uint8_t slice, uint8_t ab_channel, bool variable_frequency, uint32_t frequency) {
    uint32_t channel_use_mask = _mask(slice, ab_channel);

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
        if ((slice_variable_frequency & (1 << slice)) != 0) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }
        // If we're both fixed frequency but we don't match target frequencies then we can't share.
        if (target_slice_frequencies[slice] != frequency) {
            return PWMOUT_ALL_TIMERS_ON_PIN_IN_USE;
        }
    }

    channel_use |= channel_use_mask;
    if (variable_frequency) {
        slice_variable_frequency |= 1 << slice;
    }

    return PWMOUT_OK;
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {
    self->pin = pin;
    self->variable_frequency = variable_frequency;
    self->duty_cycle = duty;

    claim_pin(pin);

    if (frequency == 0 || frequency > (common_hal_mcu_processor_get_frequency() / 2)) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    uint8_t slice = pwm_gpio_to_slice_num(pin->number);
    uint8_t ab_channel = pwm_gpio_to_channel(pin->number);

    int r = pwmout_allocate(slice, ab_channel, variable_frequency, frequency);
    if (r != PWMOUT_OK) {
        return r;
    }

    self->slice = slice;
    self->ab_channel = ab_channel;

    if (target_slice_frequencies[slice] != frequency) {
        // Reset the counter and compare values.
        pwm_hw->slice[slice].ctr = PWM_CH0_CTR_RESET;
        common_hal_pwmio_pwmout_set_duty_cycle(self, duty);
        common_hal_pwmio_pwmout_set_frequency(self, frequency);
        pwm_set_enabled(slice, true);
    } else {
        common_hal_pwmio_pwmout_set_frequency(self, frequency);
        common_hal_pwmio_pwmout_set_duty_cycle(self, duty);
    }

    // Connect to the pad last to avoid any glitches from changing settings.
    gpio_set_function(pin->number, GPIO_FUNC_PWM);

    return PWMOUT_OK;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->pin == NULL;
}

void pwmout_free(uint8_t slice, uint8_t ab_channel) {
    uint32_t channel_mask = _mask(slice, ab_channel);
    channel_use &= ~channel_mask;
    never_reset_channel &= ~channel_mask;
    uint32_t slice_mask = ((1 << AB_CHANNELS_PER_SLICE) - 1) << (slice * AB_CHANNELS_PER_SLICE);
    if ((channel_use & slice_mask) == 0) {
        target_slice_frequencies[slice] = 0;
        slice_variable_frequency &= ~(1 << slice);
        pwm_set_enabled(slice, false);
    }
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }
    pwmout_free(self->slice, self->ab_channel);
    reset_pin_number(self->pin->number);
    self->pin = NULL;
}

extern void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty) {
    self->duty_cycle = duty;
    // Do arithmetic in 32 bits to prevent overflow.
    uint16_t compare_count;
    if (duty == 65535) {
        // Ensure that 100% duty cycle is 100% full on and not rounded down,
        // but do MIN() to keep value in range, just in case.
        compare_count = MIN(UINT16_MAX, (uint32_t)self->top + 1);
    } else {
        compare_count = ((uint32_t)duty * self->top + MAX_TOP / 2) / MAX_TOP;
    }
    // compare_count is the CC register value, which should be TOP+1 for 100% duty cycle.
    pwm_set_chan_level(self->slice, self->ab_channel, compare_count);
    // Wait for wrap so that we know our new cc value has been applied. Clear
    // the internal interrupt and then wait for it to be set. Worst case, we
    // wait a full cycle.
    pwm_hw->intr = 1 << self->slice;
    while ((pwm_hw->en & (1 << self->slice)) != 0 &&
           (pwm_hw->intr & (1 << self->slice)) == 0 &&
           !mp_hal_is_interrupted()) {
    }
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

void pwmio_pwmout_set_top(pwmio_pwmout_obj_t *self, uint16_t top) {
    self->actual_frequency = common_hal_mcu_processor_get_frequency() / top;
    self->top = top;
    pwm_set_clkdiv_int_frac(self->slice, 1, 0);
    pwm_set_wrap(self->slice, self->top);
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency) {
    if (frequency == 0 || frequency > (common_hal_mcu_processor_get_frequency() / 2)) {
        mp_raise_ValueError(translate("Invalid PWM frequency"));
    }

    target_slice_frequencies[self->slice] = frequency;

    // For low frequencies use the divider to give us full resolution duty_cycle.
    if (frequency <= (common_hal_mcu_processor_get_frequency() / (1 << 16))) {
        // Compute the divisor. It's an 8 bit integer and 4 bit fraction. Therefore,
        // we compute everything * 16 for the fractional part.
        // This is 1 << 12 because 4 bits are the * 16.
        uint64_t frequency16 = ((uint64_t)clock_get_hz(clk_sys)) / (1 << 12);
        uint64_t div16 = frequency16 / frequency;
        // Round the divisor to try and get closest to the target frequency. We could
        // also always round up and use TOP to get us closer. We may not need that though.
        if (frequency16 % frequency >= frequency / 2) {
            div16 += 1;
        }
        if (div16 >= (1 << 12)) {
            div16 = (1 << 12) - 1;
        }
        self->actual_frequency = (frequency16 + (div16 / 2)) / div16;
        self->top = MAX_TOP;
        pwm_set_clkdiv_int_frac(self->slice, div16 / 16, div16 % 16);
        pwm_set_wrap(self->slice, self->top);
    } else {
        uint32_t top = common_hal_mcu_processor_get_frequency() / frequency;
        self->actual_frequency = common_hal_mcu_processor_get_frequency() / top;
        self->top = MIN(MAX_TOP, top);
        pwm_set_clkdiv_int_frac(self->slice, 1, 0);
        // Set TOP register. For 100% duty cycle, CC must be set to TOP+1.
        pwm_set_wrap(self->slice, self->top);
    }
    common_hal_pwmio_pwmout_set_duty_cycle(self, self->duty_cycle);
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return self->actual_frequency;
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
