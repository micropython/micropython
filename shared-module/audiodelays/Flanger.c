// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiodelays/Flanger.h"
#include "shared-bindings/audiocore/__init__.h"

#include <stdint.h>
#include "py/runtime.h"

// The largest delay we will ever read, in frames.
#define FLANGER_MAX_DELAY_FRAMES (65535u)

// Convert a delay in milliseconds to a Q16.16 count of frames, clamped so that both interpolation
// taps always land inside the delay line.
static uint32_t flanger_ms_to_frames_q16(audiodelays_flanger_obj_t *self, mp_float_t ms) {
    mp_float_t frames = ms * self->base.sample_rate / MICROPY_FLOAT_CONST(1000.0);
    mp_float_t max_frames = (mp_float_t)MIN(self->delay_buffer_frames - 2, FLANGER_MAX_DELAY_FRAMES);

    // A delay of less than one frame would read the frame we are about to overwrite
    frames = MIN(MAX(frames, MICROPY_FLOAT_CONST(1.0)), max_frames);

    return (uint32_t)(frames * MICROPY_FLOAT_CONST(65536.0));
}

void common_hal_audiodelays_flanger_construct(audiodelays_flanger_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t min_delay_ms, mp_obj_t rate, mp_obj_t depth, mp_obj_t feedback, mp_obj_t mix, bool invert,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate) {

    // Basic format settings every effect and audio sample has
    self->base.bits_per_sample = bits_per_sample;
    self->base.samples_signed = samples_signed; // Are the samples we provide signed (common is true)
    self->base.channel_count = channel_count; // Channels can be 1 for mono or 2 for stereo
    self->base.sample_rate = sample_rate; // Sample rate for the effect, this generally needs to match all audio objects
    self->base.single_buffer = false;
    self->base.max_buffer_length = buffer_size;

    // To smooth things out as CircuitPython is doing other tasks most audio objects have a buffer
    // A double buffer is set up here so the audio output can use DMA on buffer 1 while we
    // write to and create buffer 2.
    // This buffer is what is passed to the audio component that plays the effect.
    // Samples are set sequentially. For stereo audio they are passed L/R/L/R/...
    self->buffer_len = buffer_size; // in bytes

    self->buffer[0] = m_malloc_maybe_without_collect(self->buffer_len);
    if (self->buffer[0] == NULL) {
        common_hal_audiodelays_flanger_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_maybe_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiodelays_flanger_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[1], 0, self->buffer_len);

    self->last_buf_idx = 1; // Which buffer to use first, toggle between 0 and 1

    // Initialize other values most effects will need.
    self->sample = NULL; // The current playing sample
    self->sample_remaining_buffer = NULL; // Pointer to the start of the sample buffer we have not played
    self->sample_buffer_length = 0; // How many samples do we have left to play (these may be 16 bit!)
    self->loop = false; // When the sample is done do we loop to the start again or stop (e.g. in a wav file)
    self->more_data = false; // Is there still more data to read from the sample or did we finish

    // The below section sets up the flanger effect's starting values.

    // If we did not receive a BlockInput we need to create a default float value
    if (min_delay_ms == MP_OBJ_NULL) {
        min_delay_ms = mp_obj_new_float(MICROPY_FLOAT_CONST(1.0));
    }
    synthio_block_assign_slot(min_delay_ms, &self->min_delay_ms, MP_QSTR_min_delay_ms);

    if (rate == MP_OBJ_NULL) {
        rate = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(rate, &self->rate, MP_QSTR_rate);

    if (depth == MP_OBJ_NULL) {
        depth = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(depth, &self->depth, MP_QSTR_depth);

    if (feedback == MP_OBJ_NULL) {
        feedback = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(feedback, &self->feedback, MP_QSTR_feedback);

    if (mix == MP_OBJ_NULL) {
        mix = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    self->invert = invert;
    self->max_delay_ms = max_delay_ms;

    // calculate the length of a single sample in milliseconds
    self->sample_ms = MICROPY_FLOAT_CONST(1000.0) / self->base.sample_rate;

    // Allocate the delay line for the largest delay we can sweep to. It is always 16-bit and each
    // channel gets its own contiguous region. The two extra frames are interpolation headroom: at
    // the top of the sweep the two taps sit at the far end of the line and without the extra room
    // the older of the two would alias onto the frame we are about to write.
    self->delay_buffer_frames = (uint32_t)(self->base.sample_rate / MICROPY_FLOAT_CONST(1000.0) * max_delay_ms) + 2;
    size_t delay_buffer_size = self->delay_buffer_frames * self->base.channel_count * sizeof(int16_t);
    self->delay_buffer = m_malloc_maybe_without_collect(delay_buffer_size);
    if (self->delay_buffer == NULL) {
        common_hal_audiodelays_flanger_deinit(self);
        m_malloc_fail(delay_buffer_size);
    }
    memset(self->delay_buffer, 0, delay_buffer_size);

    self->delay_buffer_pos[0] = self->delay_buffer_pos[1] = 0;
    self->lfo_phase[0] = self->lfo_phase[1] = 0;
    self->lfo_phase_inc = 0;
}

bool common_hal_audiodelays_flanger_deinited(audiodelays_flanger_obj_t *self) {
    if (self->delay_buffer == NULL) {
        return true;
    }
    return false;
}

void common_hal_audiodelays_flanger_deinit(audiodelays_flanger_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->delay_buffer = NULL;
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiodelays_flanger_get_min_delay_ms(audiodelays_flanger_obj_t *self) {
    return self->min_delay_ms.obj;
}

void common_hal_audiodelays_flanger_set_min_delay_ms(audiodelays_flanger_obj_t *self, mp_obj_t min_delay_ms) {
    synthio_block_assign_slot(min_delay_ms, &self->min_delay_ms, MP_QSTR_min_delay_ms);
}

mp_obj_t common_hal_audiodelays_flanger_get_rate(audiodelays_flanger_obj_t *self) {
    return self->rate.obj;
}

void common_hal_audiodelays_flanger_set_rate(audiodelays_flanger_obj_t *self, mp_obj_t rate) {
    synthio_block_assign_slot(rate, &self->rate, MP_QSTR_rate);
}

mp_obj_t common_hal_audiodelays_flanger_get_depth(audiodelays_flanger_obj_t *self) {
    return self->depth.obj;
}

void common_hal_audiodelays_flanger_set_depth(audiodelays_flanger_obj_t *self, mp_obj_t depth) {
    synthio_block_assign_slot(depth, &self->depth, MP_QSTR_depth);
}

mp_obj_t common_hal_audiodelays_flanger_get_feedback(audiodelays_flanger_obj_t *self) {
    return self->feedback.obj;
}

void common_hal_audiodelays_flanger_set_feedback(audiodelays_flanger_obj_t *self, mp_obj_t feedback) {
    synthio_block_assign_slot(feedback, &self->feedback, MP_QSTR_feedback);
}

mp_obj_t common_hal_audiodelays_flanger_get_mix(audiodelays_flanger_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiodelays_flanger_set_mix(audiodelays_flanger_obj_t *self, mp_obj_t arg) {
    synthio_block_assign_slot(arg, &self->mix, MP_QSTR_mix);
}

// Fold the sawtooth phase accumulator into a triangle in the range 0 to 65535
static uint32_t flanger_phase_to_triangle(uint32_t phase) {
    uint32_t tri = phase >> 15;
    if (tri > 65535) {
        tri = 131071 - tri;
    }
    return tri;
}

mp_float_t common_hal_audiodelays_flanger_get_lfo_value(audiodelays_flanger_obj_t *self) {
    return (mp_float_t)flanger_phase_to_triangle(self->lfo_phase[0]) / MICROPY_FLOAT_CONST(65535.0);
}

bool common_hal_audiodelays_flanger_get_invert(audiodelays_flanger_obj_t *self) {
    return self->invert;
}

void common_hal_audiodelays_flanger_set_invert(audiodelays_flanger_obj_t *self, bool invert) {
    self->invert = invert;
}

void audiodelays_flanger_reset_buffer(audiodelays_flanger_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->delay_buffer, 0, self->delay_buffer_frames * self->base.channel_count * sizeof(int16_t));
    self->delay_buffer_pos[0] = self->delay_buffer_pos[1] = 0;
    self->lfo_phase[0] = self->lfo_phase[1] = 0;
}

bool common_hal_audiodelays_flanger_get_playing(audiodelays_flanger_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiodelays_flanger_play(audiodelays_flanger_obj_t *self, mp_obj_t sample, bool loop) {
    audiosample_must_match(&self->base, sample, false);

    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(self->sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);

    // Track remaining sample length in terms of bytes per sample
    self->sample_buffer_length /= (self->base.bits_per_sample / 8);
    // Store if we have more data in the sample to retrieve
    self->more_data = result == GET_BUFFER_MORE_DATA;
}

void common_hal_audiodelays_flanger_stop(audiodelays_flanger_obj_t *self) {
    self->sample = NULL;
}

audioio_get_buffer_result_t audiodelays_flanger_get_buffer(audiodelays_flanger_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    if (!single_channel_output) {
        channel = 0;
    }

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // If we are using 16 bit samples we need a 16 bit pointer, 8 bit needs an 8 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    int8_t *hword_buffer = self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->base.bits_per_sample / 8);

    uint8_t channel_count = self->base.channel_count;
    uint32_t frames = self->delay_buffer_frames;

    // For single channel output every word in this call belongs to `channel`. Otherwise the
    // channels are interleaved and alternate with every word.
    bool single_right = single_channel_output && channel == 1 && channel_count == 2;

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it
                    self->sample = NULL;
                }
            }
            if (self->sample) {
                // Load another sample buffer to play
                audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
                // Track length in terms of words.
                self->sample_buffer_length /= (self->base.bits_per_sample / 8);
                self->more_data = result == GET_BUFFER_MORE_DATA;
            }
        }

        if (self->sample == NULL) {
            // tick all block inputs so that anything attached to them stays in sync
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, length / channel_count);
            (void)synthio_block_slot_get(&self->min_delay_ms);
            (void)synthio_block_slot_get(&self->rate);
            (void)synthio_block_slot_get(&self->depth);
            (void)synthio_block_slot_get(&self->feedback);
            (void)synthio_block_slot_get(&self->mix);

            if (self->base.samples_signed) {
                memset(word_buffer, 0, length * (self->base.bits_per_sample / 8));
            } else {
                // For unsigned samples set to the middle which is "quiet"
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    uint16_t *uword_buffer = (uint16_t *)word_buffer;
                    while (length--) {
                        *uword_buffer++ = 32768;
                    }
                } else {
                    memset(hword_buffer, 128, length * (self->base.bits_per_sample / 8));
                }
            }

            length = 0;
        } else {
            // we have a sample to play and flange
            // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
            uint32_t num_bytes = MIN(MIN(self->sample_buffer_length, length), SYNTHIO_MAX_DUR * channel_count);

            int16_t *sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            int8_t *sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples

            // get the effect values we need from the BlockInput
            shared_bindings_synthio_lfo_tick(self->base.sample_rate, num_bytes / channel_count);
            mp_float_t f_min_delay_ms = synthio_block_slot_get_limited(&self->min_delay_ms, self->sample_ms, (mp_float_t)self->max_delay_ms);
            mp_float_t f_rate = synthio_block_slot_get_limited(&self->rate, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(20.0));
            mp_float_t f_depth = synthio_block_slot_get_limited(&self->depth, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));
            int32_t feedback = (int32_t)(synthio_block_slot_get_limited(&self->feedback, MICROPY_FLOAT_CONST(-0.95), MICROPY_FLOAT_CONST(0.95)) * 32767);
            int32_t mix = (int32_t)(synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0)) * 32767);

            // The sweep runs upward from min_delay_ms towards max_delay_ms rather than around a
            // centre, so no combination of min_delay_ms and depth can push it out of the delay line.
            mp_float_t sweep_top_ms = f_min_delay_ms + f_depth * ((mp_float_t)self->max_delay_ms - f_min_delay_ms);
            uint32_t delay_min_q16 = flanger_ms_to_frames_q16(self, f_min_delay_ms);
            uint32_t delay_span_q16 = flanger_ms_to_frames_q16(self, sweep_top_ms) - delay_min_q16;

            // How far the LFO advances each frame. A rate at or above half the sample rate is
            // meaningless, and clamping there also keeps the conversion inside a uint32_t.
            mp_float_t phase_inc = MIN(f_rate / self->base.sample_rate, MICROPY_FLOAT_CONST(0.5));
            self->lfo_phase_inc = (uint32_t)(phase_inc * MICROPY_FLOAT_CONST(4294967296.0));

            for (uint32_t i = 0; i < num_bytes; i++) {
                bool right_channel = single_channel_output ? single_right : ((i % channel_count) == 1);

                int32_t sample_word = 0;
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    sample_word = sample_src[i];
                } else {
                    if (self->base.samples_signed) {
                        sample_word = sample_hsrc[i];
                    } else {
                        // changing from an 8 bit unsigned to signed into a 32-bit signed
                        sample_word = (int8_t)(((uint8_t)sample_hsrc[i]) ^ 0x80);
                    }
                }

                // Advance this channel's LFO by one frame. Each channel has its own accumulator so
                // that the sweep runs at `rate` whether the channels arrive interleaved (both
                // accumulators step once per frame) or one channel per call.
                self->lfo_phase[right_channel] += self->lfo_phase_inc;

                uint32_t tri = flanger_phase_to_triangle(self->lfo_phase[right_channel]);

                // The delay for this frame, in Q16.16 frames
                uint32_t delay_q16 = delay_min_q16 + (uint32_t)(((uint64_t)delay_span_q16 * tri) >> 16);
                uint32_t delay_int = delay_q16 >> 16;
                uint32_t delay_frac = delay_q16 & 0xffff;

                int16_t *line = self->delay_buffer + (right_channel ? frames : 0);
                uint32_t w = self->delay_buffer_pos[right_channel];

                // Two taps straddling the fractional delay, counted back from the write head.
                // delay_int is at most frames - 2 so neither index can escape the region.
                uint32_t r0 = w + frames - delay_int;
                if (r0 >= frames) {
                    r0 -= frames;
                }
                uint32_t r1 = r0 ? r0 - 1 : frames - 1;

                // Linear interpolation between them
                int32_t s0 = line[r0];
                int32_t s1 = line[r1];
                int32_t wet = s0 + (((s1 - s0) * (int32_t)delay_frac) >> 16);

                // Write the input plus the regenerated signal back into the line
                line[w] = synthio_sat16(sample_word + synthio_sat16(wet * feedback, 15), 0);
                self->delay_buffer_pos[right_channel] = (w + 1 == frames) ? 0 : w + 1;

                int32_t word;
                if (mix <= 328) { // if mix is zero (0.01 in fixed point), pure sample only
                    // Note that we still ran the delay line above.
                    word = sample_word;
                } else {
                    int32_t wet_word = synthio_sat16(wet * mix, 15);
                    // Add (or, inverted, subtract) original sample + effect
                    word = self->invert ? sample_word - wet_word : sample_word + wet_word;
                    word = synthio_mix_down_sample(word, SYNTHIO_MIX_DOWN_SCALE(2));
                }

                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    word_buffer[i] = word;
                    if (!self->base.samples_signed) {
                        word_buffer[i] ^= 0x8000;
                    }
                } else {
                    // 8-bit samples have no headroom for synthio_mix_down_sample to work with, so
                    // clamp instead or the sum of the dry and wet signals wraps around
                    int8_t out = MIN(MAX(word, -128), 127);
                    if (self->base.samples_signed) {
                        hword_buffer[i] = out;
                    } else {
                        hword_buffer[i] = (uint8_t)out ^ 0x80;
                    }
                }
            }

            // Update the remaining length and the buffer positions based on how much we wrote into our buffer
            length -= num_bytes;
            word_buffer += num_bytes;
            hword_buffer += num_bytes;
            self->sample_remaining_buffer += (num_bytes * (self->base.bits_per_sample / 8));
            self->sample_buffer_length -= num_bytes;
        }
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // Flanger always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
