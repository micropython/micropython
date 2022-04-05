/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Dan Halbert for Adafruit Industries
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
#include "nrf.h"

#include "py/runtime.h"
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "supervisor/shared/translate.h"

#include "nrf_gpio.h"

#define PWM_MAX_FREQ      (16000000)

STATIC NRF_PWM_Type *pwms[] = {
    #if NRFX_CHECK(NRFX_PWM0_ENABLED)
    NRF_PWM0,
    #endif
    #if NRFX_CHECK(NRFX_PWM1_ENABLED)
    NRF_PWM1,
    #endif
    #if NRFX_CHECK(NRFX_PWM2_ENABLED)
    NRF_PWM2,
    #endif
    #if NRFX_CHECK(NRFX_PWM3_ENABLED)
    NRF_PWM3,
    #endif
};

#define CHANNELS_PER_PWM 4

STATIC uint16_t pwm_seq[MP_ARRAY_SIZE(pwms)][CHANNELS_PER_PWM];

static uint8_t never_reset_pwm[MP_ARRAY_SIZE(pwms)];

STATIC int pwm_idx(NRF_PWM_Type *pwm) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(pwms); i++) {
        if (pwms[i] == pwm) {
            return i;
        }
    }
    return -1;
}

void common_hal_pwmio_pwmout_never_reset(pwmio_pwmout_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(pwms); i++) {
        NRF_PWM_Type *pwm = pwms[i];
        if (pwm == self->pwm) {
            never_reset_pwm[i] += 1;
        }
    }

    common_hal_never_reset_pin(self->pin);
}

void common_hal_pwmio_pwmout_reset_ok(pwmio_pwmout_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(pwms); i++) {
        NRF_PWM_Type *pwm = pwms[i];
        if (pwm == self->pwm) {
            never_reset_pwm[i] -= 1;
        }
    }
}

STATIC void reset_single_pwmout(uint8_t i) {
    NRF_PWM_Type *pwm = pwms[i];

    pwm->ENABLE = 0;
    pwm->MODE = PWM_MODE_UPDOWN_Up;
    pwm->DECODER = PWM_DECODER_LOAD_Individual;
    pwm->LOOP = 0;
    pwm->PRESCALER = PWM_PRESCALER_PRESCALER_DIV_1;       // default is 500 hz
    pwm->COUNTERTOP = (PWM_MAX_FREQ / 500);                   // default is 500 hz

    pwm->SEQ[0].PTR = (uint32_t)pwm_seq[i];
    pwm->SEQ[0].CNT = CHANNELS_PER_PWM;      // default mode is Individual --> count must be 4
    pwm->SEQ[0].REFRESH = 0;
    pwm->SEQ[0].ENDDELAY = 0;

    pwm->SEQ[1].PTR = 0;
    pwm->SEQ[1].CNT = 0;
    pwm->SEQ[1].REFRESH = 0;
    pwm->SEQ[1].ENDDELAY = 0;

    for (int ch = 0; ch < CHANNELS_PER_PWM; ch++) {
        pwm_seq[i][ch] = (1 << 15); // polarity = 0
        pwm->PSEL.OUT[ch] = 0xFFFFFFFF; // disconnnect from I/O
    }
}

void pwmout_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(pwms); i++) {
        if (never_reset_pwm[i] > 0) {
            continue;
        }
        reset_single_pwmout(i);
    }
}

// Find the smallest prescaler value that will allow the divisor to be in range.
// This allows the most accuracy.
STATIC bool convert_frequency(uint32_t frequency, uint16_t *countertop, nrf_pwm_clk_t *base_clock) {
    uint32_t divisor = 1;
    // Use a 32-bit number so we don't overflow the uint16_t;
    uint32_t tentative_countertop;
    for (*base_clock = PWM_PRESCALER_PRESCALER_DIV_1;
         *base_clock <= PWM_PRESCALER_PRESCALER_DIV_128;
         (*base_clock)++) {
        tentative_countertop = PWM_MAX_FREQ / divisor / frequency;
        // COUNTERTOP must be 3..32767, according to datasheet, but 3 doesn't work. 4 does.
        if (tentative_countertop <= 32767 && tentative_countertop >= 4) {
            // In range, OK to return.
            *countertop = tentative_countertop;
            return true;
        }
        divisor *= 2;
    }

    return false;
}

// We store these in an array because we cannot compute them.
static IRQn_Type pwm_irqs[4] = {PWM0_IRQn, PWM1_IRQn, PWM2_IRQn, PWM3_IRQn};

NRF_PWM_Type *pwmout_allocate(uint16_t countertop, nrf_pwm_clk_t base_clock,
    bool variable_frequency, int8_t *channel_out, bool *pwm_already_in_use_out,
    IRQn_Type *irq) {
    for (size_t pwm_index = 0; pwm_index < MP_ARRAY_SIZE(pwms); pwm_index++) {
        NRF_PWM_Type *pwm = pwms[pwm_index];
        bool pwm_already_in_use = pwm->ENABLE & PWM_ENABLE_ENABLE_Msk;
        if (pwm_already_in_use) {
            if (variable_frequency) {
                // Variable frequency requires exclusive use of a PWM, so try the next one.
                continue;
            }

            // PWM is in use, but see if it's set to the same frequency we need. If so,
            // look for a free channel.
            if (pwm->COUNTERTOP == countertop && pwm->PRESCALER == base_clock) {
                for (size_t chan = 0; chan < CHANNELS_PER_PWM; chan++) {
                    if (pwm->PSEL.OUT[chan] == 0xFFFFFFFF) {
                        // Channel is free.
                        if (channel_out) {
                            *channel_out = chan;
                        }
                        if (pwm_already_in_use_out) {
                            *pwm_already_in_use_out = pwm_already_in_use;
                        }
                        if (irq) {
                            *irq = pwm_irqs[pwm_index];
                        }
                        return pwm;
                    }
                }
            }
        } else {
            // PWM not yet in use, so we can start to use it. Use channel 0.
            if (channel_out) {
                *channel_out = 0;
            }
            if (pwm_already_in_use_out) {
                *pwm_already_in_use_out = pwm_already_in_use;
            }
            if (irq) {
                *irq = pwm_irqs[pwm_index];
            }
            return pwm;
        }
    }
    return NULL;
}

void pwmout_free_channel(NRF_PWM_Type *pwm, int8_t channel) {
    // Disconnect pin from channel.
    pwm->PSEL.OUT[channel] = 0xFFFFFFFF;

    for (int i = 0; i < CHANNELS_PER_PWM; i++) {
        if (pwm->PSEL.OUT[i] != 0xFFFFFFFF) {
            // Some channel is still being used, so don't disable.
            return;
        }
    }

    nrf_pwm_disable(pwm);
}

pwmout_result_t common_hal_pwmio_pwmout_construct(pwmio_pwmout_obj_t *self,
    const mcu_pin_obj_t *pin,
    uint16_t duty,
    uint32_t frequency,
    bool variable_frequency) {

    // We don't use the nrfx driver here because we want to dynamically allocate channels
    // as needed in an already-enabled PWM.

    uint16_t countertop;
    nrf_pwm_clk_t base_clock;
    if (frequency == 0 || !convert_frequency(frequency, &countertop, &base_clock)) {
        return PWMOUT_INVALID_FREQUENCY;
    }

    int8_t channel;
    bool pwm_already_in_use;
    self->pwm = pwmout_allocate(countertop, base_clock, variable_frequency,
        &channel, &pwm_already_in_use, NULL);

    if (self->pwm == NULL) {
        return PWMOUT_ALL_TIMERS_IN_USE;
    }

    self->channel = channel;
    self->pin = pin;
    claim_pin(pin);

    self->frequency = frequency;
    self->variable_frequency = variable_frequency;

    // Note this is standard, not strong drive.
    nrf_gpio_cfg_output(self->pin->number);

    // disable before mapping pin channel
    nrf_pwm_disable(self->pwm);

    if (!pwm_already_in_use) {
        reset_single_pwmout(pwm_idx(self->pwm));
        nrf_pwm_configure(self->pwm, base_clock, NRF_PWM_MODE_UP, countertop);
    }

    // Connect channel to pin, without disturbing other channels.
    self->pwm->PSEL.OUT[self->channel] = pin->number;

    nrf_pwm_enable(self->pwm);

    common_hal_pwmio_pwmout_set_duty_cycle(self, duty);
    return PWMOUT_OK;
}

bool common_hal_pwmio_pwmout_deinited(pwmio_pwmout_obj_t *self) {
    return self->pwm == NULL;
}

void common_hal_pwmio_pwmout_deinit(pwmio_pwmout_obj_t *self) {
    if (common_hal_pwmio_pwmout_deinited(self)) {
        return;
    }

    nrf_gpio_cfg_default(self->pin->number);

    NRF_PWM_Type *pwm = self->pwm;
    self->pwm = NULL;

    pwmout_free_channel(pwm, self->channel);

    common_hal_reset_pin(self->pin);
    self->pin = NULL;
}

void common_hal_pwmio_pwmout_set_duty_cycle(pwmio_pwmout_obj_t *self, uint16_t duty_cycle) {
    self->duty_cycle = duty_cycle;

    uint16_t *p_value = ((uint16_t *)self->pwm->SEQ[0].PTR) + self->channel;
    *p_value = ((duty_cycle * self->pwm->COUNTERTOP) / 0xFFFF) | (1 << 15);

    self->pwm->TASKS_SEQSTART[0] = 1;
}

uint16_t common_hal_pwmio_pwmout_get_duty_cycle(pwmio_pwmout_obj_t *self) {
    return self->duty_cycle;
}

void common_hal_pwmio_pwmout_set_frequency(pwmio_pwmout_obj_t *self, uint32_t frequency) {
    // COUNTERTOP is 3..32767, so highest available frequency is PWM_MAX_FREQ / 3.
    uint16_t countertop;
    nrf_pwm_clk_t base_clock;
    if (frequency == 0 || !convert_frequency(frequency, &countertop, &base_clock)) {
        mp_raise_ValueError(translate("Invalid PWM frequency"));
    }
    self->frequency = frequency;

    nrf_pwm_configure(self->pwm, base_clock, NRF_PWM_MODE_UP, countertop);
    // Set the duty cycle again, because it depends on COUNTERTOP, which probably changed.
    // Setting the duty cycle will also do a SEQSTART.
    common_hal_pwmio_pwmout_set_duty_cycle(self, self->duty_cycle);
}

uint32_t common_hal_pwmio_pwmout_get_frequency(pwmio_pwmout_obj_t *self) {
    return self->frequency;
}

bool common_hal_pwmio_pwmout_get_variable_frequency(pwmio_pwmout_obj_t *self) {
    return self->variable_frequency;
}

const mcu_pin_obj_t *common_hal_pwmio_pwmout_get_pin(pwmio_pwmout_obj_t *self) {
    return self->pin;
}
