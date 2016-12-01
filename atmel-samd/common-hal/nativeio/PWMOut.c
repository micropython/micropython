/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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
#include "shared-bindings/nativeio/PWMOut.h"

#include "samd21_pins.h"

void common_hal_nativeio_pwmout_construct(nativeio_pwmout_obj_t* self, const mcu_pin_obj_t* pin, uint16_t duty) {
    self->pin = pin;
    self->using_primary_timer = true;

    if (pin->primary_timer.tc == 0 && pin->secondary_timer.tc == 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "PWM not supported on pin %q", self->pin->name));
    }

    // TODO(tannewt): Support output on multiple timer channels at once.
    const pin_timer_t* t = &pin->primary_timer;
    if (t->is_tc) {
        struct tc_config config_tc;
        tc_get_config_defaults(&config_tc);

        config_tc.counter_size    = TC_COUNTER_SIZE_8BIT;
        config_tc.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
        config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV256;
        config_tc.counter_8_bit.period = 0xff;
        config_tc.counter_8_bit.compare_capture_channel[t->channel] = duty;

        config_tc.pwm_channel[t->wave_output].enabled = true;
        config_tc.pwm_channel[t->wave_output].pin_out = pin->pin;
        config_tc.pwm_channel[t->wave_output].pin_mux = MUX_E;

        tc_init(&self->tc_instance, t->tc, &config_tc);

        tc_enable(&self->tc_instance);
    } else {
        struct tcc_config config_tcc;
        tcc_get_config_defaults(&config_tcc, t->tcc);

        config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV256;
        config_tcc.counter.period = 0xFF;
        config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
        config_tcc.compare.match[t->channel] = duty;

        config_tcc.pins.enable_wave_out_pin[t->wave_output] = true;
        config_tcc.pins.wave_out_pin[t->wave_output] = pin->pin;
        config_tcc.pins.wave_out_pin_mux[t->wave_output] = MUX_E;

        tcc_init(&self->tcc_instance, t->tcc, &config_tcc);

        tcc_enable(&self->tcc_instance);
    }
}

extern void common_hal_nativeio_pwmout_deinit(nativeio_pwmout_obj_t* self) {
    const pin_timer_t* t = &self->pin->primary_timer;
    if (!self->using_primary_timer) {
      t = &self->pin->secondary_timer;
    }
    if (t->is_tc) {
      tc_disable(&self->tc_instance);
    } else {
      tcc_disable(&self->tcc_instance);
    }
}

extern void common_hal_nativeio_pwmout_set_duty_cycle(nativeio_pwmout_obj_t* self, uint16_t duty) {
    const pin_timer_t* t = &self->pin->primary_timer;
    if (!self->using_primary_timer) {
        t = &self->pin->secondary_timer;
    }
    if (t->tcc != 0) {
        tcc_set_compare_value(&self->tcc_instance, t->channel, duty);
    } else {
        tc_set_compare_value(&self->tc_instance, t->channel, duty);
    }
}

uint16_t common_hal_nativeio_pwmout_get_duty_cycle(nativeio_pwmout_obj_t* self) {
    const pin_timer_t* t = &self->pin->primary_timer;
    if (!self->using_primary_timer) {
        t = &self->pin->secondary_timer;
    }
    if (t->tcc != 0) {
        return tcc_get_capture_value(&self->tcc_instance, t->channel);
    } else {
        return tc_get_capture_value(&self->tc_instance, t->channel);
    }
}
