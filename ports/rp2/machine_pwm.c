/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include "py/runtime.h"
#include "py/mphal.h"
#include "modmachine.h"

#include "hardware/clocks.h"
#include "hardware/pwm.h"

/******************************************************************************/
// MicroPython bindings for machine.PWM

typedef struct _machine_pwm_obj_t {
    mp_obj_base_t base;
    uint8_t slice;
    uint8_t channel;
    uint8_t duty_type;
    mp_int_t duty;
} machine_pwm_obj_t;

enum {
    DUTY_NOT_SET = 0,
    DUTY_U16,
    DUTY_NS
};

STATIC machine_pwm_obj_t machine_pwm_obj[] = {
    {{&machine_pwm_type}, 0, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 0, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 1, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 1, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 2, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 2, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 3, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 3, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 4, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 4, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 5, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 5, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 6, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 6, PWM_CHAN_B, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 7, PWM_CHAN_A, DUTY_NOT_SET, 0},
    {{&machine_pwm_type}, 7, PWM_CHAN_B, DUTY_NOT_SET, 0},
};

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16);
STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns);

STATIC void mp_machine_pwm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    machine_pwm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<PWM slice=%u channel=%u>", self->slice, self->channel);
}

// PWM(pin)
STATIC mp_obj_t mp_machine_pwm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // Check number of arguments
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    // Get GPIO to connect to PWM.
    uint32_t gpio = mp_hal_get_pin_obj(all_args[0]);

    // Get static peripheral object.
    uint slice = pwm_gpio_to_slice_num(gpio);
    uint8_t channel = pwm_gpio_to_channel(gpio);
    machine_pwm_obj_t *self = &machine_pwm_obj[slice * 2 + channel];
    self->duty_type = DUTY_NOT_SET;

    // Select PWM function for given GPIO.
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    return MP_OBJ_FROM_PTR(self);
}

STATIC void mp_machine_pwm_deinit(machine_pwm_obj_t *self) {
    self->duty_type = DUTY_NOT_SET;
    pwm_set_enabled(self->slice, false);
}

// Returns: floor((16*F + offset) / div16)
// Avoids overflow in the numerator that would occur if
//   16*F + offset > 2**32
//   F + offset/16 > 2**28 = 268435456 (approximately, due to flooring)
uint32_t get_slice_hz(uint32_t offset, uint32_t div16) {
    uint32_t source_hz = clock_get_hz(clk_sys);
    if (source_hz + offset / 16 > 268000000) {
        return (16 * (uint64_t)source_hz + offset) / div16;
    } else {
        return (16 * source_hz + offset) / div16;
    }
}

// Returns 16*F / denom, rounded.
uint32_t get_slice_hz_round(uint32_t div16) {
    return get_slice_hz(div16 / 2, div16);
}

// Returns ceil(16*F / denom).
uint32_t get_slice_hz_ceil(uint32_t div16) {
    return get_slice_hz(div16 - 1, div16);
}

STATIC mp_obj_t mp_machine_pwm_freq_get(machine_pwm_obj_t *self) {
    uint32_t div16 = pwm_hw->slice[self->slice].div;
    uint32_t top = pwm_hw->slice[self->slice].top;
    uint32_t pwm_freq = get_slice_hz_round(div16 * (top + 1));
    return MP_OBJ_NEW_SMALL_INT(pwm_freq);
}

STATIC void mp_machine_pwm_freq_set(machine_pwm_obj_t *self, mp_int_t freq) {
    // Set the frequency, making "top" as large as possible for maximum resolution.
    // Maximum "top" is set at 65534 to be able to achieve 100% duty with 65535.
    #define TOP_MAX 65534
    uint32_t source_hz = clock_get_hz(clk_sys);
    uint32_t div16;
    uint32_t top;

    if ((source_hz + freq / 2) / freq < TOP_MAX) {
        // If possible (based on the formula for TOP below), use a DIV of 1.
        // This also prevents overflow in the DIV calculation.
        div16 = 16;

        // Same as get_slice_hz_round() below but canceling the 16s
        // to avoid overflow for high freq.
        top = (source_hz + freq / 2) / freq - 1;
    } else {
        // Otherwise, choose the smallest possible DIV for maximum
        // duty cycle resolution.
        // Constraint: 16*F/(div16*freq) < TOP_MAX
        // So:
        div16 = get_slice_hz_ceil(TOP_MAX * freq);

        // Set TOP as accurately as possible using rounding.
        top = get_slice_hz_round(div16 * freq) - 1;
    }


    if (div16 < 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too large"));
    } else if (div16 >= 256 * 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("freq too small"));
    }
    pwm_hw->slice[self->slice].div = div16;
    pwm_hw->slice[self->slice].top = top;
    if (self->duty_type == DUTY_U16) {
        mp_machine_pwm_duty_set_u16(self, self->duty);
    } else if (self->duty_type == DUTY_NS) {
        mp_machine_pwm_duty_set_ns(self, self->duty);
    }
}

STATIC mp_obj_t mp_machine_pwm_duty_get_u16(machine_pwm_obj_t *self) {
    uint32_t top = pwm_hw->slice[self->slice].top;
    uint32_t cc = pwm_hw->slice[self->slice].cc;
    cc = (cc >> (self->channel ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB)) & 0xffff;

    // Use rounding (instead of flooring) here to give as accurate an
    // estimate as possible.
    return MP_OBJ_NEW_SMALL_INT((cc * 65535 + (top + 1) / 2) / (top + 1));
}

STATIC void mp_machine_pwm_duty_set_u16(machine_pwm_obj_t *self, mp_int_t duty_u16) {
    uint32_t top = pwm_hw->slice[self->slice].top;

    // Use rounding here to set it as accurately as possible.
    uint32_t cc = (duty_u16 * (top + 1) + 65535 / 2) / 65535;
    pwm_set_chan_level(self->slice, self->channel, cc);
    pwm_set_enabled(self->slice, true);
    self->duty = duty_u16;
    self->duty_type = DUTY_U16;
}

STATIC mp_obj_t mp_machine_pwm_duty_get_ns(machine_pwm_obj_t *self) {
    uint32_t slice_hz = get_slice_hz_round(pwm_hw->slice[self->slice].div);
    uint32_t cc = pwm_hw->slice[self->slice].cc;
    cc = (cc >> (self->channel ? PWM_CH0_CC_B_LSB : PWM_CH0_CC_A_LSB)) & 0xffff;
    return MP_OBJ_NEW_SMALL_INT(((uint64_t)cc * 1000000000ULL + slice_hz / 2) / slice_hz);
}

STATIC void mp_machine_pwm_duty_set_ns(machine_pwm_obj_t *self, mp_int_t duty_ns) {
    uint32_t slice_hz = get_slice_hz_round(pwm_hw->slice[self->slice].div);
    uint32_t cc = ((uint64_t)duty_ns * slice_hz + 500000000ULL) / 1000000000ULL;
    if (cc > 65535) {
        mp_raise_ValueError(MP_ERROR_TEXT("duty larger than period"));
    }
    pwm_set_chan_level(self->slice, self->channel, cc);
    pwm_set_enabled(self->slice, true);
    self->duty = duty_ns;
    self->duty_type = DUTY_NS;
}
