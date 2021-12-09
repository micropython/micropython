/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Jeff Epler for Adafruit Industries
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

#include "common-hal/audiopwmio/PWMAudioOut.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "extmod/vfs_fat.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "shared-bindings/pwmio/PWMOut.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "supervisor/shared/translate.h"

#include "src/rp2040/hardware_structs/include/hardware/structs/dma.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"

#define NUM_DMA_TIMERS 4

// The PWM clock frequency is base_clock_rate / PWM_TOP, typically 125_000_000 / PWM_TOP.
// We pick BITS_PER_SAMPLE so we get a clock frequency that is above what would cause aliasing.
#define BITS_PER_SAMPLE 10
#define SAMPLE_BITS_TO_DISCARD (16 - BITS_PER_SAMPLE)
#define PWM_TOP ((1 << BITS_PER_SAMPLE) - 1)


static uint32_t gcd(uint32_t a, uint32_t b) {
    while (b) {
        uint32_t tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

static uint32_t limit_denominator(uint32_t max_denominator, uint32_t num_in, uint32_t den_in, uint32_t *den_out) {
// Algorithm based on Python's limit_denominator
    uint32_t p0 = 0, q0 = 1, p1 = 1, q1 = 0;
    uint32_t d = den_in, n = num_in;
    uint32_t g = gcd(n, d);
    d /= g;
    n /= g;
    if (d < max_denominator) {
        *den_out = d;
        return n;
    }
    while (1) {
        uint32_t a = n / d;
        uint32_t q2 = q0 + a * q1;
        if (q2 > max_denominator) {
            break;
        }

        uint32_t p_tmp = p0 + a * p1;
        p0 = p1;
        q0 = q1;
        p1 = p_tmp;
        q1 = q2;

        uint32_t d_tmp = n - a * d;
        n = d;
        d = d_tmp;
    }
    uint32_t k = (max_denominator - q0) / q1;
    uint32_t bound1_num = p0 + k * p1, bound1_den = q0 + k * q1;
    uint32_t bound2_num = p1, bound2_den = q1;

    if (fabsf((float)bound1_num / bound1_den - (float)num_in / den_in) <=
        fabsf((float)bound2_num / bound2_den - (float)num_in / den_in)) {
        *den_out = bound2_den;
        return bound2_num;
    }

    *den_out = bound1_den;
    return bound1_num;
}

void audiopwmout_reset() {
    for (size_t i = 0; i < NUM_DMA_TIMERS; i++) {
        dma_hw->timer[i] = 0;
    }
}

// Caller validates that pins are free.
void common_hal_audiopwmio_pwmaudioout_construct(audiopwmio_pwmaudioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t quiescent_value) {

    self->stereo = right_channel != NULL;

    if (self->stereo) {
        if (pwm_gpio_to_slice_num(left_channel->number) != pwm_gpio_to_slice_num(right_channel->number)) {
            mp_raise_ValueError(translate("Pins must share PWM slice"));
        }
        if (pwm_gpio_to_channel(left_channel->number) != 0) {
            mp_raise_ValueError(translate("Stereo left must be on PWM channel A"));
        }
        if (pwm_gpio_to_channel(right_channel->number) != 1) {
            mp_raise_ValueError(translate("Stereo right must be on PWM channel B"));
        }
    }

    // Typically pwmout doesn't let us change frequency with two objects on the
    // same PWM slice. However, we have private access to it so we can do what
    // we want. ;-) We mark ourselves variable only if we're a mono output to
    // prevent other PWM use on the other channel. If stereo, we say fixed
    // frequency so we can allocate with ourselves.

    // We don't actually know our frequency yet. It is set when
    // pwmio_pwmout_set_top() is called. This value is unimportant; it just needs to be valid.
    const uint32_t frequency = 12000000;

    // Make sure the PWMOut's are "deinited" by default.
    self->left_pwm.pin = NULL;
    self->right_pwm.pin = NULL;

    pwmout_result_t result =
        common_hal_pwmio_pwmout_construct(&self->left_pwm, left_channel, 0, frequency, !self->stereo);
    if (result == PWMOUT_OK && right_channel != NULL) {
        result =
            common_hal_pwmio_pwmout_construct(&self->right_pwm, right_channel, 0, frequency, false);
        if (result != PWMOUT_OK) {
            common_hal_pwmio_pwmout_deinit(&self->left_pwm);
        }
    }
    if (result != PWMOUT_OK) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    self->quiescent_value = quiescent_value >> SAMPLE_BITS_TO_DISCARD;
    common_hal_pwmio_pwmout_set_duty_cycle(&self->left_pwm, self->quiescent_value);
    pwmio_pwmout_set_top(&self->left_pwm, PWM_TOP);
    if (self->stereo) {
        common_hal_pwmio_pwmout_set_duty_cycle(&self->right_pwm, self->quiescent_value);
        pwmio_pwmout_set_top(&self->right_pwm, PWM_TOP);
    }

    audio_dma_init(&self->dma);
    self->pacing_timer = NUM_DMA_TIMERS;
}

bool common_hal_audiopwmio_pwmaudioout_deinited(audiopwmio_pwmaudioout_obj_t *self) {
    return common_hal_pwmio_pwmout_deinited(&self->left_pwm);
}

void common_hal_audiopwmio_pwmaudioout_deinit(audiopwmio_pwmaudioout_obj_t *self) {
    if (common_hal_audiopwmio_pwmaudioout_deinited(self)) {
        return;
    }

    if (common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }

    // TODO: ramp the pwm down from quiescent value to 0
    common_hal_pwmio_pwmout_deinit(&self->left_pwm);
    common_hal_pwmio_pwmout_deinit(&self->right_pwm);

    audio_dma_deinit(&self->dma);
}

void common_hal_audiopwmio_pwmaudioout_play(audiopwmio_pwmaudioout_obj_t *self, mp_obj_t sample, bool loop) {

    if (common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }

    // TODO: Share pacing timers based on frequency.
    size_t pacing_timer = NUM_DMA_TIMERS;
    for (size_t i = 0; i < NUM_DMA_TIMERS; i++) {
        if (dma_hw->timer[i] == 0) {
            pacing_timer = i;
            break;
        }
    }
    if (pacing_timer == NUM_DMA_TIMERS) {
        mp_raise_RuntimeError(translate("No DMA pacing timer found"));
    }
    uint32_t tx_register = (uint32_t)&pwm_hw->slice[self->left_pwm.slice].cc;
    if (self->stereo) {
        // Shift the destination if we are outputting to both PWM channels.
        tx_register += self->left_pwm.ab_channel * sizeof(uint16_t);
    }

    self->pacing_timer = pacing_timer;

    // Playback with two independent clocks. One is the sample rate which
    // determines when we push a new sample to the PWM slice. The second is the
    // PWM frequency itself.

    // Determine the DMA divisor. The RP2040 has four pacing timers we can use
    // to trigger the DMA. Each has a 16 bit fractional divisor system clock * X / Y where X and Y
    // are 16-bit.

    uint32_t sample_rate = audiosample_sample_rate(sample);

    uint32_t system_clock = common_hal_mcu_processor_get_frequency();
    uint32_t best_denominator;
    uint32_t best_numerator = limit_denominator(0xffff, sample_rate, system_clock, &best_denominator);

    dma_hw->timer[pacing_timer] = best_numerator << 16 | best_denominator;
    audio_dma_result result = audio_dma_setup_playback(
        &self->dma,
        sample,
        loop,
        false, // single channel
        0, // audio channel
        false,  // output signed
        BITS_PER_SAMPLE,
        (uint32_t)tx_register,  // output register: PWM cc register
        0x3b + pacing_timer); // data request line

    if (result == AUDIO_DMA_DMA_BUSY) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
        mp_raise_RuntimeError(translate("No DMA channel found"));
    }
    if (result == AUDIO_DMA_MEMORY_ERROR) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
        mp_raise_RuntimeError(translate("Unable to allocate buffers for signed conversion"));
    }
    // OK! We got all of the resources we need and dma is ready.
}

void common_hal_audiopwmio_pwmaudioout_stop(audiopwmio_pwmaudioout_obj_t *self) {
    if (self->pacing_timer < NUM_DMA_TIMERS) {
        dma_hw->timer[self->pacing_timer] = 0;
        self->pacing_timer = NUM_DMA_TIMERS;
    }

    audio_dma_stop(&self->dma);

    // Set to quiescent level.
    common_hal_pwmio_pwmout_set_duty_cycle(&self->left_pwm, self->quiescent_value);
    pwmio_pwmout_set_top(&self->left_pwm, PWM_TOP);
    if (self->stereo) {
        common_hal_pwmio_pwmout_set_duty_cycle(&self->right_pwm, self->quiescent_value);
        pwmio_pwmout_set_top(&self->right_pwm, PWM_TOP);
    }
}

bool common_hal_audiopwmio_pwmaudioout_get_playing(audiopwmio_pwmaudioout_obj_t *self) {
    bool playing = audio_dma_get_playing(&self->dma);

    if (!playing && self->pacing_timer < NUM_DMA_TIMERS) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }

    return playing;
}

void common_hal_audiopwmio_pwmaudioout_pause(audiopwmio_pwmaudioout_obj_t *self) {
    audio_dma_pause(&self->dma);
}

void common_hal_audiopwmio_pwmaudioout_resume(audiopwmio_pwmaudioout_obj_t *self) {
    audio_dma_resume(&self->dma);
}

bool common_hal_audiopwmio_pwmaudioout_get_paused(audiopwmio_pwmaudioout_obj_t *self) {
    return audio_dma_get_paused(&self->dma);
}
