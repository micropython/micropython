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
#include "supervisor/shared/tick.h"
#include "supervisor/shared/translate.h"

#include "src/rp2040/hardware_structs/include/hardware/structs/dma.h"
#include "src/rp2_common/hardware_pwm/include/hardware/pwm.h"

#define NUM_DMA_TIMERS 4

void audiopwmout_reset() {
    for (size_t i = 0; i < NUM_DMA_TIMERS; i++) {
        dma_hw->timer[i] = 0;
    }
}

// Caller validates that pins are free.
void common_hal_audiopwmio_pwmaudioout_construct(audiopwmio_pwmaudioout_obj_t* self,
        const mcu_pin_obj_t* left_channel, const mcu_pin_obj_t* right_channel, uint16_t quiescent_value) {
    if (left_channel != NULL && right_channel != NULL) {
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
    bool mono = right_channel == NULL;

    // We don't actually know our frequency yet so just pick one that shouldn't
    // match anyone else. (We'll only know the frequency once we play something
    // back.)
    uint32_t frequency = 12500;

    // Make sure the PWMOut's are "deinited" by default.
    self->left_pwm.pin = NULL;
    self->right_pwm.pin = NULL;

    pwmout_result_t result = common_hal_pwmio_pwmout_construct(&self->left_pwm,
        left_channel, 0, frequency, mono);
    if (result == PWMOUT_OK && right_channel != NULL) {
        result = common_hal_pwmio_pwmout_construct(&self->right_pwm,
            right_channel, 0, frequency, false);
        if (result != PWMOUT_OK) {
            common_hal_pwmio_pwmout_deinit(&self->left_pwm);
        }
    }
    if (result != PWMOUT_OK) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    claim_pin(left_channel);
    if (right_channel != NULL) {
        claim_pin(right_channel);
    }

    audio_dma_init(&self->dma);

    self->quiescent_value = quiescent_value;
}

bool common_hal_audiopwmio_pwmaudioout_deinited(audiopwmio_pwmaudioout_obj_t* self) {
    return common_hal_pwmio_pwmout_deinited(&self->left_pwm);
}

void common_hal_audiopwmio_pwmaudioout_deinit(audiopwmio_pwmaudioout_obj_t* self) {
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

void common_hal_audiopwmio_pwmaudioout_play(audiopwmio_pwmaudioout_obj_t* self, mp_obj_t sample, bool loop) {
    if (common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }

    // TODO: Share pacing timers based on frequency.
    size_t pacing_timer = NUM_DMA_TIMERS;
    for (size_t i = 0; i < NUM_DMA_TIMERS; i++) {
        if (dma_hw->timer[i] == 0) {
            pacing_timer = i;
        }
        break;
    }
    if (pacing_timer == NUM_DMA_TIMERS) {
        mp_raise_RuntimeError(translate("No DMA pacing timer found"));
    }
    uint32_t tx_register = (uint32_t) &pwm_hw->slice[self->left_pwm.slice].cc;
    if (common_hal_pwmio_pwmout_deinited(&self->right_pwm)) {
        // Shift the destination if we are outputting to the second PWM channel.
        tx_register += self->left_pwm.channel * sizeof(uint16_t);
    }

    pwmio_pwmout_set_top(&self->left_pwm, 1023);

    audio_dma_result result = audio_dma_setup_playback(
        &self->dma,
        sample,
        loop,
        false, // single channel
        0, // audio channel
        false,  // output signed
        10,
        (uint32_t) tx_register, // output register
        0x3b + pacing_timer); // data request line

    if (result == AUDIO_DMA_DMA_BUSY) {
        // common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(translate("No DMA channel found"));
    } else if (result == AUDIO_DMA_MEMORY_ERROR) {
        // common_hal_audiobusio_i2sout_stop(self);
        mp_raise_RuntimeError(translate("Unable to allocate buffers for signed conversion"));
    }

    // OK! We got all of the resources we need and dma is ready.
    self->pacing_timer = pacing_timer;

    // Playback with two independent clocks. One is the sample rate which
    // determines when we push a new sample to the PWM slice. The second is the
    // PWM frequency itself.

    // Determine the DMA divisor. The RP2040 has four pacing timers we can use
    // to trigger the DMA. Each has a 16 bit fractional divisor system clock * X / Y where X and Y
    // are 16-bit.

    uint32_t sample_rate = audiosample_sample_rate(sample);
    uint32_t system_clock = common_hal_mcu_processor_get_frequency();
    uint32_t best_numerator = 0;
    uint32_t best_denominator = 0;
    uint32_t best_error = system_clock;

    for (uint32_t denominator = 0xffff; denominator > 0; denominator--) {
        uint32_t numerator = (denominator * sample_rate) / system_clock;
        uint32_t remainder = (denominator * sample_rate) % system_clock;
        if (remainder > (system_clock / 2)) {
            numerator += 1;
            remainder = system_clock - remainder;
        }
        if (remainder < best_error) {
            best_denominator = denominator;
            best_numerator = numerator;
            best_error = remainder;
            // Stop early if we can't do better.
            if (remainder == 0) {
                break;
            }
        }
    }

    dma_hw->timer[pacing_timer] = best_numerator << 16 | best_denominator;
}

void common_hal_audiopwmio_pwmaudioout_stop(audiopwmio_pwmaudioout_obj_t* self) {
    dma_hw->timer[self->pacing_timer] = 0;
    self->pacing_timer = NUM_DMA_TIMERS;

    audio_dma_stop(&self->dma);
}

bool common_hal_audiopwmio_pwmaudioout_get_playing(audiopwmio_pwmaudioout_obj_t* self) {
    bool playing = audio_dma_get_playing(&self->dma);
    if (!playing && self->pacing_timer < NUM_DMA_TIMERS) {
        dma_hw->timer[self->pacing_timer] = 0;
        self->pacing_timer = NUM_DMA_TIMERS;
    }
    return playing;
}

void common_hal_audiopwmio_pwmaudioout_pause(audiopwmio_pwmaudioout_obj_t* self) {
    audio_dma_pause(&self->dma);
}

void common_hal_audiopwmio_pwmaudioout_resume(audiopwmio_pwmaudioout_obj_t* self) {
    audio_dma_resume(&self->dma);
}

bool common_hal_audiopwmio_pwmaudioout_get_paused(audiopwmio_pwmaudioout_obj_t* self) {
    return audio_dma_get_paused(&self->dma);
}
