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

#include <stdint.h>
#include <string.h>

#include "extmod/vfs_fat.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/runtime.h"
#include "common-hal/audiopwmio/PWMAudioOut.h"
#include "common-hal/pwmio/PWMOut.h"
#include "shared-bindings/audiopwmio/PWMAudioOut.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/tick.h"
#include "supervisor/shared/translate.h"

// TODO: This should be the same size as PWMOut.c:pwms[], but there's no trivial way to accomplish that
STATIC audiopwmio_pwmaudioout_obj_t *active_audio[4];

#define F_TARGET (62500)
#define F_PWM (16000000)
// return the REFRESH value, store the TOP value in an out-parameter
// Tested for key values (worst relative error = 0.224% = 3.84 cents)
//  8000: top = 250 refresh = 7 [ 8000.0]
// 22050: top = 242 refresh = 2 [22038.5]
// 24000: top = 222 refresh = 2 [24024.0]
// 44100: top = 181 refresh = 1 [44198.8]
// 48000: top = 167 refresh = 1 [47904.1]
STATIC uint32_t calculate_pwm_parameters(uint32_t sample_rate, uint32_t *top_out) {
    // the desired frequency is the closest integer multiple of sample_rate not less than F_TARGET
    uint32_t desired_frequency = (F_TARGET + sample_rate - 1) / sample_rate * sample_rate;
    // The top value is the PWM frequency divided by the desired frequency (round to nearest)
    uint32_t top = (F_PWM + desired_frequency / 2) / desired_frequency;
    // The actual frequency is the PWM frequency divided by the top value (round to nearest)
    uint32_t actual_frequency = (F_PWM + top / 2) / top;
    // The multiplier is the actual frequency divided by the sample rate (round to nearest)
    uint32_t multiplier = (actual_frequency + sample_rate / 2) / sample_rate;
    *top_out = top;
    return multiplier - 1;
}

STATIC void activate_audiopwmout_obj(audiopwmio_pwmaudioout_obj_t *self) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        if (!active_audio[i]) {
            active_audio[i] = self;
            supervisor_enable_tick();
            break;
        }
    }
}
STATIC void deactivate_audiopwmout_obj(audiopwmio_pwmaudioout_obj_t *self) {
    // Turn off the interrupts to the CPU.
    self->pwm->INTENCLR = PWM_INTENSET_SEQSTARTED0_Msk | PWM_INTENSET_SEQSTARTED1_Msk;
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        if (active_audio[i] == self) {
            active_audio[i] = NULL;
            supervisor_disable_tick();
        }
    }
}

void audiopwmout_reset() {
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        if (active_audio[i]) {
            supervisor_disable_tick();
        }
        active_audio[i] = NULL;
    }
}

STATIC void fill_buffers(audiopwmio_pwmaudioout_obj_t *self, int buf) {
    uint16_t *dev_buffer = self->buffers[buf];
    uint8_t *buffer;
    uint32_t buffer_length;
    audioio_get_buffer_result_t get_buffer_result =
        audiosample_get_buffer(self->sample, false, 0,
            &buffer, &buffer_length);
    if (get_buffer_result == GET_BUFFER_ERROR) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
        return;
    }
    uint32_t num_samples = buffer_length / self->bytes_per_sample / self->sample_channel_count;
    uint16_t *end_dev_buffer = dev_buffer + 2 * num_samples;

    if (self->bytes_per_sample == 1) {
        uint8_t offset = self->signed_to_unsigned ? 0x80 : 0;
        uint16_t scale = self->scale;
        while (dev_buffer < end_dev_buffer) {
            uint8_t rawval = (*buffer++ + offset);
            uint16_t val = (uint16_t)(((uint32_t)rawval * (uint32_t)scale) >> 8);
            *dev_buffer++ = val;
            if (self->sample_channel_count == 1) {
                *dev_buffer++ = val;
            }
        }
    } else {
        uint16_t offset = self->signed_to_unsigned ? 0x8000 : 0;
        uint16_t scale = self->scale;
        uint16_t *buffer16 = (uint16_t *)buffer;
        while (dev_buffer < end_dev_buffer) {
            uint16_t rawval = (*buffer16++ + offset);
            uint16_t val = (uint16_t)((rawval * (uint32_t)scale) >> 16);
            *dev_buffer++ = val;
            if (self->sample_channel_count == 1) {
                *dev_buffer++ = val;
            }
        }
    }
    self->pwm->SEQ[buf].PTR = (intptr_t)self->buffers[buf];
    self->pwm->SEQ[buf].CNT = num_samples * 2;

    if (self->loop && get_buffer_result == GET_BUFFER_DONE) {
        audiosample_reset_buffer(self->sample, false, 0);
    } else if (get_buffer_result == GET_BUFFER_DONE) {
        self->pwm->SHORTS = NRF_PWM_SHORT_SEQEND0_STOP_MASK | NRF_PWM_SHORT_SEQEND1_STOP_MASK;
        self->stopping = true;
    }
}

STATIC void audiopwmout_background_obj(audiopwmio_pwmaudioout_obj_t *self) {
    if (!common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        return;
    }
    if (self->stopping) {
        bool stopped =
            (self->pwm->EVENTS_SEQEND[0] || !self->pwm->EVENTS_SEQSTARTED[0]) &&
            (self->pwm->EVENTS_SEQEND[1] || !self->pwm->EVENTS_SEQSTARTED[1]);
        if (stopped) {
            self->pwm->TASKS_STOP = 1;
        }
    } else if (!self->paused && !self->single_buffer) {
        if (self->pwm->EVENTS_SEQSTARTED[0]) {
            fill_buffers(self, 1);
            self->pwm->EVENTS_SEQSTARTED[0] = 0;
        }
        if (self->pwm->EVENTS_SEQSTARTED[1]) {
            fill_buffers(self, 0);
            self->pwm->EVENTS_SEQSTARTED[1] = 0;
        }
        NVIC_ClearPendingIRQ(self->pwm_irq);
    }
}

void audiopwmout_background() {
    // Check the NVIC first because it is part of the CPU and fast to read.
    if (!NVIC_GetPendingIRQ(PWM0_IRQn) &&
        !NVIC_GetPendingIRQ(PWM1_IRQn) &&
        !NVIC_GetPendingIRQ(PWM2_IRQn) &&
        !NVIC_GetPendingIRQ(PWM3_IRQn)) {
        return;
    }
    // Check our objects because the PWM could be active for some other reason.
    for (size_t i = 0; i < MP_ARRAY_SIZE(active_audio); i++) {
        if (!active_audio[i]) {
            continue;
        }
        audiopwmout_background_obj(active_audio[i]);
    }
}

// Caller validates that pins are free.
void common_hal_audiopwmio_pwmaudioout_construct(audiopwmio_pwmaudioout_obj_t *self,
    const mcu_pin_obj_t *left_channel, const mcu_pin_obj_t *right_channel, uint16_t quiescent_value) {
    self->pwm = pwmout_allocate(256, PWM_PRESCALER_PRESCALER_DIV_1, true, NULL, NULL,
        &self->pwm_irq);
    if (!self->pwm) {
        mp_raise_RuntimeError(translate("All timers in use"));
    }

    self->pwm->PRESCALER = PWM_PRESCALER_PRESCALER_DIV_1;
    // two uint16_t values per sample when Grouped
    // n.b. SEQ[#].CNT "counts" are 2 per sample (left and right channels)
    self->pwm->DECODER = PWM_DECODER_LOAD_Grouped;

    // we use channels 0 and 2 because these are GROUPED; it lets us save half
    // the space for sample data (no additional optimization is possible for
    // single channel)
    self->pwm->PSEL.OUT[0] = self->left_channel_number = left_channel->number;
    claim_pin(left_channel);

    if (right_channel) {
        self->pwm->PSEL.OUT[2] = self->right_channel_number = right_channel->number;
        claim_pin(right_channel);
    }

    self->quiescent_value = quiescent_value >> 8;

    self->pwm->ENABLE = 1;
    // TODO: Ramp from 0 to quiescent value
}

bool common_hal_audiopwmio_pwmaudioout_deinited(audiopwmio_pwmaudioout_obj_t *self) {
    return !self->pwm;
}

void common_hal_audiopwmio_pwmaudioout_deinit(audiopwmio_pwmaudioout_obj_t *self) {
    if (common_hal_audiopwmio_pwmaudioout_deinited(self)) {
        return;
    }
    deactivate_audiopwmout_obj(self);

    // TODO: ramp the pwm down from quiescent value to 0
    self->pwm->ENABLE = 0;

    if (self->left_channel_number) {
        reset_pin_number(self->left_channel_number);
    }
    if (self->right_channel_number) {
        reset_pin_number(self->right_channel_number);
    }

    pwmout_free_channel(self->pwm, 0);
    pwmout_free_channel(self->pwm, 2);

    self->pwm = NULL;

    m_free(self->buffers[0]);
    self->buffers[0] = NULL;

    m_free(self->buffers[1]);
    self->buffers[1] = NULL;
}

void common_hal_audiopwmio_pwmaudioout_play(audiopwmio_pwmaudioout_obj_t *self, mp_obj_t sample, bool loop) {
    if (common_hal_audiopwmio_pwmaudioout_get_playing(self)) {
        common_hal_audiopwmio_pwmaudioout_stop(self);
    }
    self->sample = sample;
    self->loop = loop;

    uint32_t sample_rate = audiosample_sample_rate(sample);
    self->bytes_per_sample = audiosample_bits_per_sample(sample) / 8;

    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, /* single channel */ false,
        &self->single_buffer, &self->signed_to_unsigned, &max_buffer_length,
        &spacing);
    self->sample_channel_count = audiosample_channel_count(sample);

    if (max_buffer_length > UINT16_MAX) {
        mp_raise_ValueError_varg(translate("Buffer length %d too big. It must be less than %d"), max_buffer_length, UINT16_MAX);
    }
    uint16_t buffer_length = (uint16_t)max_buffer_length;
    self->buffers[0] = m_malloc(buffer_length * 2 * sizeof(uint16_t), false);
    if (!self->single_buffer) {
        self->buffers[1] = m_malloc(buffer_length * 2 * sizeof(uint16_t), false);
    }


    uint32_t top;
    self->pwm->SEQ[0].REFRESH = self->pwm->SEQ[1].REFRESH = calculate_pwm_parameters(sample_rate, &top);
    self->scale = top - 1;
    self->pwm->COUNTERTOP = top;

    self->pwm->LOOP = 1;
    audiosample_reset_buffer(self->sample, false, 0);
    activate_audiopwmout_obj(self);
    self->stopping = false;
    self->pwm->SHORTS = NRF_PWM_SHORT_LOOPSDONE_SEQSTART0_MASK;
    fill_buffers(self, 0);
    self->pwm->SEQ[1].PTR = self->pwm->SEQ[0].PTR;
    self->pwm->SEQ[1].CNT = self->pwm->SEQ[0].CNT;
    self->pwm->EVENTS_SEQSTARTED[0] = 0;
    self->pwm->EVENTS_SEQSTARTED[1] = 0;
    self->pwm->EVENTS_SEQEND[0] = 0;
    self->pwm->EVENTS_SEQEND[1] = 0;
    self->pwm->EVENTS_STOPPED = 0;
    // Enable the SEQSTARTED interrupts so that they wake the CPU and keep it awake until serviced.
    // We don't enable them in the NVIC because we don't actually want an interrupt routine to run.
    self->pwm->INTENSET = PWM_INTENSET_SEQSTARTED0_Msk | PWM_INTENSET_SEQSTARTED1_Msk;
    self->pwm->TASKS_SEQSTART[0] = 1;
    self->playing = true;
    self->paused = false;
}

void common_hal_audiopwmio_pwmaudioout_stop(audiopwmio_pwmaudioout_obj_t *self) {
    deactivate_audiopwmout_obj(self);
    self->pwm->TASKS_STOP = 1;
    self->stopping = false;
    self->paused = false;

    m_free(self->buffers[0]);
    self->buffers[0] = NULL;

    m_free(self->buffers[1]);
    self->buffers[1] = NULL;
}

bool common_hal_audiopwmio_pwmaudioout_get_playing(audiopwmio_pwmaudioout_obj_t *self) {
    if (!self->paused && self->pwm->EVENTS_STOPPED) {
        self->playing = false;
        self->pwm->EVENTS_STOPPED = 0;
    }
    return self->playing;
}

/* pause/resume present difficulties for the NRF PWM audio module.
 *
 * A PWM sequence can be stopped in its tracks by sending a TASKS_STOP event,
 * but there's no way to pick up the sequence where it was stopped; you could
 * start at the start of one of the two sequences, but especially for "single buffer"
 * sample, this seems undesirable.
 *
 * Or, you can stop at the end of a sequence so that you don't duplicate anything
 * when restarting, but again this is unsatisfactory for a "single buffer" sample.
 *
 * For now, I've taken the coward's way and left these methods unimplemented.
 * Perhaps the way forward is to divide even "single buffer" samples into tasks of
 * only a few ms long, so that they can be stopped/restarted quickly enough that it
 * feels instant. (This also saves on memory, for long in-memory "single buffer"
 * samples, since we have to locally take a resampled copy!)
 */
void common_hal_audiopwmio_pwmaudioout_pause(audiopwmio_pwmaudioout_obj_t *self) {
    self->paused = true;
    self->pwm->SHORTS = NRF_PWM_SHORT_SEQEND1_STOP_MASK;
}

void common_hal_audiopwmio_pwmaudioout_resume(audiopwmio_pwmaudioout_obj_t *self) {
    self->paused = false;
    self->pwm->SHORTS = NRF_PWM_SHORT_LOOPSDONE_SEQSTART0_MASK;
    if (self->pwm->EVENTS_STOPPED) {
        self->pwm->EVENTS_STOPPED = 0;
        self->pwm->TASKS_SEQSTART[0] = 1;
    }
}

bool common_hal_audiopwmio_pwmaudioout_get_paused(audiopwmio_pwmaudioout_obj_t *self) {
    return self->paused;
}
