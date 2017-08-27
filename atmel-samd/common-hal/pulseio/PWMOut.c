/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2016 Damien P. George
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
#include "common-hal/pulseio/PWMOut.h"
#include "shared-bindings/pulseio/PWMOut.h"

#include "samd21_pins.h"

#undef ENABLE

#  define _TCC_SIZE(n,unused)              TPASTE3(TCC,n,_SIZE),
#  define TCC_SIZES         { MREPEAT(TCC_INST_NUM, _TCC_SIZE, 0) }

uint32_t target_timer_frequencies[TC_INST_NUM + TCC_INST_NUM];
static uint32_t timer_periods[TC_INST_NUM + TCC_INST_NUM];
uint8_t timer_refcount[TC_INST_NUM + TCC_INST_NUM];
const uint16_t prescaler[8] = {1, 2, 4, 8, 16, 64, 256, 1024};

// This bitmask keeps track of which channels of a TCC are currently claimed.
uint8_t tcc_channels[3] = {0xf0, 0xfc, 0xfc};

void pwmout_reset(void) {
    // Reset all but TC5
    for (int i = 0; i < TC_INST_NUM + TCC_INST_NUM; i++) {
        if (i == 5) {
            target_timer_frequencies[i] = 1000;
            timer_refcount[i] = 1;
        } else {
            target_timer_frequencies[i] = 0;
            timer_refcount[i] = 0;
        }
    }
    Tcc *tccs[TCC_INST_NUM] = TCC_INSTS;
    for (int i = 0; i < TCC_INST_NUM; i++) {
        // Disable the module before resetting it.
        if (tccs[i]->CTRLA.bit.ENABLE == 1) {
            tccs[i]->CTRLA.bit.ENABLE = 0;
            while (tccs[i]->SYNCBUSY.bit.ENABLE == 1) {
            }
        }
        // TODO(tannewt): Make this depend on the CMSIS.
        if (i == 0) {
            tcc_channels[i] = 0xf0;
        } else {
            tcc_channels[i] = 0xfc;
        }
        tccs[i]->CTRLA.bit.SWRST = 1;
    }
    Tc *tcs[TC_INST_NUM] = TC_INSTS;
    for (int i = 0; i < TC_INST_NUM; i++) {
        if (tcs[i] == TC5) {
            continue;
        }
        tcs[i]->COUNT16.CTRLA.bit.SWRST = 1;
        while (tcs[i]->COUNT16.CTRLA.bit.SWRST == 1) {
        }
    }
}

bool channel_ok(const pin_timer_t* t, uint8_t index) {
    return (!t->is_tc && (tcc_channels[index] & (1 << t->channel)) == 0) ||
            t->is_tc;
}

static uint8_t timer_index(uint32_t base_timer_address) {
    return (base_timer_address - ((uint32_t) TCC0)) / 0x400;
}

void common_hal_pulseio_pwmout_construct(pulseio_pwmout_obj_t* self,
                                          const mcu_pin_obj_t* pin,
                                          uint16_t duty,
                                          uint32_t frequency,
                                          bool variable_frequency) {
    self->pin = pin;
    self->variable_frequency = variable_frequency;

    if (pin->primary_timer.tc == 0 && pin->secondary_timer.tc == 0) {
        mp_raise_ValueError("Invalid pin");
    }

    if (frequency == 0 || frequency > 6000000) {
        mp_raise_ValueError("Invalid PWM frequency");
    }

    uint16_t primary_timer_index = 0xff;
    uint16_t secondary_timer_index = 0xff;
    if (pin->primary_timer.tc != NULL) {
        primary_timer_index = timer_index((uint32_t) pin->primary_timer.tcc);
    }
    if (pin->secondary_timer.tc != NULL) {
        secondary_timer_index = timer_index((uint32_t) pin->secondary_timer.tcc);
    }

    // Figure out which timer we are using.

    // First see if a timer is already going with the frequency we want and our
    // channel is unused.
    // NOTE(shawcroft): The enable bit is in the same position for TC and TCC so
    // we treat them all as TCC for checking ENABLE.
    const pin_timer_t* t = NULL;
    uint8_t index = 0;
    if (!variable_frequency &&
            primary_timer_index != 0xff &&
            target_timer_frequencies[primary_timer_index] == frequency &&
            pin->primary_timer.tcc->CTRLA.bit.ENABLE == 1 &&
            channel_ok(&pin->primary_timer, primary_timer_index)) {
        t = &pin->primary_timer;
        index = primary_timer_index;
        self->tcc_instance.hw = t->tcc;
        self->tcc_instance.double_buffering_enabled = true;
    } else if (!variable_frequency &&
               secondary_timer_index != 0xff &&
               target_timer_frequencies[secondary_timer_index] == frequency &&
               pin->secondary_timer.tcc->CTRLA.bit.ENABLE == 1 &&
               channel_ok(&pin->secondary_timer, secondary_timer_index)) {
        t = &pin->secondary_timer;
        index = secondary_timer_index;
        self->tcc_instance.hw = t->tcc;
        self->tcc_instance.double_buffering_enabled = true;
    } else {
        // Pick an unused timer if available.

        // Check the secondary timer first since its always a nicer TCC (when it
        // exists)
        if (pin->secondary_timer.tc != 0 &&
            timer_refcount[secondary_timer_index] == 0 &&
            pin->secondary_timer.tcc->CTRLA.bit.ENABLE == 0) {
            t = &pin->secondary_timer;
            index = secondary_timer_index;
        } else if (pin->primary_timer.tc != 0 &&
                   (!pin->primary_timer.is_tc || pin->primary_timer.channel == 1) &&
                   timer_refcount[primary_timer_index] == 0) {
            t = &pin->primary_timer;
            index = primary_timer_index;
        }
        if (t == NULL) {
            mp_raise_RuntimeError("All timers in use");
            return;
        }

        uint8_t resolution = 0;
        if (t->is_tc) {
            resolution = 16;
        } else {
            // TCC resolution varies so look it up.
            const uint8_t _tcc_sizes[TCC_INST_NUM] = TCC_SIZES;
            resolution = _tcc_sizes[index];
        }
        // First determine the divisor that gets us the highest resolution.
        uint32_t system_clock = system_cpu_clock_get_hz();
        uint32_t top;
        uint8_t divisor;
        for (divisor = 0; divisor < 8; divisor++) {
            top = (system_clock / prescaler[divisor] / frequency) - 1;
            if (top < (1u << resolution)) {
                break;
            }
        }
        timer_periods[index] = top;
        if (t->is_tc) {
            struct tc_config config_tc;
            tc_get_config_defaults(&config_tc);

            config_tc.counter_size    = TC_COUNTER_SIZE_16BIT;
            config_tc.clock_prescaler = TC_CTRLA_PRESCALER(divisor);
            config_tc.wave_generation = TC_WAVE_GENERATION_MATCH_PWM;
            config_tc.counter_16_bit.compare_capture_channel[0] = top;

            enum status_code status = tc_init(&self->tc_instance, t->tc, &config_tc);
            if (status != STATUS_OK) {
                mp_raise_RuntimeError("Failed to init timer");
            }
            tc_enable(&self->tc_instance);
        } else {
            struct tcc_config config_tcc;
            tcc_get_config_defaults(&config_tcc, t->tcc);

            config_tcc.counter.clock_prescaler = divisor;
            config_tcc.counter.period = top;
            config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;

            enum status_code status = tcc_init(&self->tcc_instance, t->tcc, &config_tcc);
            if (status != STATUS_OK) {
                mp_raise_RuntimeError("Failed to init timer");
            }
            tcc_enable(&self->tcc_instance);
        }

        target_timer_frequencies[index] = frequency;
        timer_refcount[index]++;
    }

    if (!t->is_tc) {
        if (variable_frequency) {
            // We're changing frequency so claim all of the channels.
            tcc_channels[index] = 0xff;
        } else {
            tcc_channels[index] |= (1 << t->channel);
        }
    }

    self->timer = t;

    // Connect the wave output to the outside world.
    struct system_pinmux_config pin_config;
    system_pinmux_get_config_defaults(&pin_config);
    pin_config.mux_position = &self->pin->primary_timer == t ? MUX_E : MUX_F;
    pin_config.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
    system_pinmux_pin_set_config(pin->pin, &pin_config);

    common_hal_pulseio_pwmout_set_duty_cycle(self, duty);
}

extern void common_hal_pulseio_pwmout_deinit(pulseio_pwmout_obj_t* self) {
    const pin_timer_t* t = self->timer;
    uint8_t index = (((uint32_t) t->tcc) - ((uint32_t) TCC0)) / 0x400;
    timer_refcount[index]--;
    if (!t->is_tc) {
        tcc_channels[index] &= ~(1 << t->channel);
    }
    if (timer_refcount[index] == 0) {
        target_timer_frequencies[index] = 0;
        if (t->is_tc) {
            tc_disable(&self->tc_instance);
        } else {
            if (t->tcc == TCC0) {
                tcc_channels[index] = 0xf0;
            } else {
                tcc_channels[index] = 0xfc;
            }
            tcc_disable(&self->tcc_instance);
            tcc_reset(&self->tcc_instance);
        }
    }
    reset_pin(self->pin->pin);
}

extern void common_hal_pulseio_pwmout_set_duty_cycle(pulseio_pwmout_obj_t* self, uint16_t duty) {
    const pin_timer_t* t = self->timer;
    uint8_t index;
    if (t->is_tc) {
        index = timer_index((uint32_t) self->timer->tc);
        uint16_t adjusted_duty = timer_periods[index] * duty / 0xffff;
        tc_set_compare_value(&self->tc_instance, t->channel, adjusted_duty);
    } else {
        index = timer_index((uint32_t) self->timer->tcc);
        uint32_t adjusted_duty = ((uint64_t) timer_periods[index]) * duty / 0xffff;
        tcc_set_compare_value(&self->tcc_instance, t->channel, adjusted_duty);
    }
}

uint16_t common_hal_pulseio_pwmout_get_duty_cycle(pulseio_pwmout_obj_t* self) {
    const pin_timer_t* t = self->timer;
    if (t->is_tc) {
        while (tc_is_syncing(&self->tc_instance)) {
            /* Wait for sync */
        }
        uint16_t cv = t->tc->COUNT16.CC[t->channel].reg;
        return cv * 0xffff / timer_periods[timer_index((uint32_t) self->timer->tc)];
    } else {
        uint32_t cv = 0;
        if ((t->tcc->STATUS.vec.CCBV & (1 << t->channel)) != 0) {
            cv = t->tcc->CCB[t->channel].reg;
        } else {
            cv = t->tcc->CC[t->channel].reg;
        }

        uint32_t duty_cycle = ((uint64_t) cv) * 0xffff / timer_periods[timer_index((uint32_t) self->timer->tcc)];

        return duty_cycle;
    }
}


void common_hal_pulseio_pwmout_set_frequency(pulseio_pwmout_obj_t* self,
                                              uint32_t frequency) {
    if (frequency == 0 || frequency > 6000000) {
        mp_raise_ValueError("Invalid PWM frequency");
    }
    const pin_timer_t* t = self->timer;
    uint8_t resolution;
    if (t->is_tc) {
        resolution = 16;
    } else {
        resolution = 24;
    }
    uint32_t system_clock = system_cpu_clock_get_hz();
    uint32_t new_top;
    uint8_t new_divisor;
    for (new_divisor = 0; new_divisor < 8; new_divisor++) {
        new_top = (system_clock / prescaler[new_divisor] / frequency) - 1;
        if (new_top < (1u << resolution)) {
            break;
        }
    }
    uint16_t old_duty = common_hal_pulseio_pwmout_get_duty_cycle(self);
    uint8_t old_divisor;
    uint8_t index;
    if (t->is_tc) {
        index = timer_index((uint32_t) self->timer->tc);
        old_divisor = t->tc->COUNT16.CTRLA.bit.PRESCALER;
    } else {
        index = timer_index((uint32_t) self->timer->tcc);
        old_divisor = t->tcc->CTRLA.bit.PRESCALER;
    }
    if (new_divisor != old_divisor) {
        if (t->is_tc) {
            tc_disable(&self->tc_instance);
            t->tc->COUNT16.CTRLA.bit.PRESCALER = new_divisor;
            tc_enable(&self->tc_instance);
        } else {
            tcc_disable(&self->tcc_instance);
            t->tcc->CTRLA.bit.PRESCALER = new_divisor;
            tcc_enable(&self->tcc_instance);
        }
    }
    timer_periods[index] = new_top;
    if (t->is_tc) {
        while (tc_is_syncing(&self->tc_instance)) {
            /* Wait for sync */
        }
        t->tc->COUNT16.CC[0].reg = new_top;
    } else {
        tcc_set_top_value(&self->tcc_instance, new_top);
    }

    common_hal_pulseio_pwmout_set_duty_cycle(self, old_duty);
}

uint32_t common_hal_pulseio_pwmout_get_frequency(pulseio_pwmout_obj_t* self) {
    uint32_t system_clock = system_cpu_clock_get_hz();
    const pin_timer_t* t = self->timer;
    uint8_t index;
    uint8_t divisor;
    if (t->is_tc) {
        index = timer_index((uint32_t) self->timer->tc);
        divisor = t->tc->COUNT16.CTRLA.bit.PRESCALER;
    } else {
        index = timer_index((uint32_t) self->timer->tcc);
        divisor = t->tcc->CTRLA.bit.PRESCALER;
    }
    uint32_t top = timer_periods[index];
    return (system_clock / prescaler[divisor]) / (top + 1);
}

bool common_hal_pulseio_pwmout_get_variable_frequency(pulseio_pwmout_obj_t* self) {
    return self->variable_frequency;
}
