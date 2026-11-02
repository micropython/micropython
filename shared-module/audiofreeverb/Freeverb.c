// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Mark Komus
//
// SPDX-License-Identifier: MIT
//
// Based on FreeVerb - https://github.com/sinshu/freeverb/tree/main
// Fixed point ideas from - Paul Stoffregen in the Teensy audio library https://github.com/PaulStoffregen/Audio/blob/master/effect_freeverb.cpp
//
#include "shared-bindings/audiofreeverb/Freeverb.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-module/synthio/__init__.h"

#include <stdint.h>
#include "py/runtime.h"
#include <math.h>

void common_hal_audiofreeverb_freeverb_construct(audiofreeverb_freeverb_obj_t *self, mp_obj_t roomsize, mp_obj_t damp, mp_obj_t pre_filter, mp_obj_t post_filter, mp_obj_t mix,
    uint32_t buffer_size, uint8_t bits_per_sample,
    bool samples_signed, uint8_t channel_count, uint32_t sample_rate) {

    // Basic settings every effect and audio sample has
    // These are the effects values, not the source sample(s)
    self->base.bits_per_sample = bits_per_sample; // Most common is 16, but 8 is also supported in many places
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
        common_hal_audiofreeverb_freeverb_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_maybe_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiofreeverb_freeverb_deinit(self);
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

    // The below section sets up the reverb effect's starting values. For a different effect this section will change
    common_hal_audiofreeverb_freeverb_set_pre_filter(self, pre_filter);
    common_hal_audiofreeverb_freeverb_set_post_filter(self, post_filter);

    if (roomsize == MP_OBJ_NULL) {
        roomsize = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(roomsize, &self->roomsize, MP_QSTR_roomsize);
    common_hal_audiofreeverb_freeverb_set_roomsize(self, roomsize);

    if (damp == MP_OBJ_NULL) {
        damp = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(damp, &self->damp, MP_QSTR_damp);
    common_hal_audiofreeverb_freeverb_set_damp(self, damp);

    if (mix == MP_OBJ_NULL) {
        mix = mp_obj_new_float(MICROPY_FLOAT_CONST(0.5));
    }
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);
    common_hal_audiofreeverb_freeverb_set_mix(self, mix);

    // Set up the comb filters
    // These values come from FreeVerb and are selected for the best reverb sound
    self->combbuffersizes[0] = self->combbuffersizes[8] = 1116;
    self->combbuffersizes[1] = self->combbuffersizes[9] = 1188;
    self->combbuffersizes[2] = self->combbuffersizes[10] = 1277;
    self->combbuffersizes[3] = self->combbuffersizes[11] = 1356;
    self->combbuffersizes[4] = self->combbuffersizes[12] = 1422;
    self->combbuffersizes[5] = self->combbuffersizes[13] = 1491;
    self->combbuffersizes[6] = self->combbuffersizes[14] = 1557;
    self->combbuffersizes[7] = self->combbuffersizes[15] = 1617;
    for (uint32_t i = 0; i < 8 * channel_count; i++) {
        self->combbuffers[i] = m_malloc_maybe_without_collect(self->combbuffersizes[i] * sizeof(uint16_t));
        if (self->combbuffers[i] == NULL) {
            common_hal_audiofreeverb_freeverb_deinit(self);
            m_malloc_fail(self->combbuffersizes[i]);
        }
        memset(self->combbuffers[i], 0, self->combbuffersizes[i] * sizeof(uint16_t));

        self->combbufferindex[i] = 0;
        self->combfitlers[i] = 0;
    }

    // Set up the allpass filters
    // These values come from FreeVerb and are selected for the best reverb sound
    self->allpassbuffersizes[0] = self->allpassbuffersizes[4] = 556;
    self->allpassbuffersizes[1] = self->allpassbuffersizes[5] = 441;
    self->allpassbuffersizes[2] = self->allpassbuffersizes[6] = 341;
    self->allpassbuffersizes[3] = self->allpassbuffersizes[7] = 225;
    for (uint32_t i = 0; i < 4 * channel_count; i++) {
        self->allpassbuffers[i] = m_malloc_maybe_without_collect(self->allpassbuffersizes[i] * sizeof(uint16_t));
        if (self->allpassbuffers[i] == NULL) {
            common_hal_audiofreeverb_freeverb_deinit(self);
            m_malloc_fail(self->allpassbuffersizes[i]);
        }
        memset(self->allpassbuffers[i], 0, self->allpassbuffersizes[i] * sizeof(uint16_t));

        self->allpassbufferindex[i] = 0;
    }
}

bool common_hal_audiofreeverb_freeverb_deinited(audiofreeverb_freeverb_obj_t *self) {
    if (self->buffer[0] == NULL) {
        return true;
    }
    return false;
}

void common_hal_audiofreeverb_freeverb_deinit(audiofreeverb_freeverb_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    audiofilters_deinit_filter_chain(&self->pre_filter);
    audiofilters_deinit_filter_chain(&self->post_filter);
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;
}

mp_obj_t common_hal_audiofreeverb_freeverb_get_roomsize(audiofreeverb_freeverb_obj_t *self) {
    return self->roomsize.obj;
}

void common_hal_audiofreeverb_freeverb_set_roomsize(audiofreeverb_freeverb_obj_t *self, mp_obj_t roomsize_obj) {
    synthio_block_assign_slot(roomsize_obj, &self->roomsize, MP_QSTR_roomsize);
}

int16_t audiofreeverb_freeverb_get_roomsize_fixedpoint(mp_float_t n) {
    if (n > (mp_float_t)MICROPY_FLOAT_CONST(1.0)) {
        n = MICROPY_FLOAT_CONST(1.0);
    } else if (n < (mp_float_t)MICROPY_FLOAT_CONST(0.0)) {
        n = MICROPY_FLOAT_CONST(0.0);
    }

    return (int16_t)(n * (mp_float_t)MICROPY_FLOAT_CONST(9175.04)) + 22937; // 9175.04 = 0.28f in fixed point 22937 = 0.7f
}

mp_obj_t common_hal_audiofreeverb_freeverb_get_damp(audiofreeverb_freeverb_obj_t *self) {
    return self->damp.obj;
}

void common_hal_audiofreeverb_freeverb_set_damp(audiofreeverb_freeverb_obj_t *self, mp_obj_t damp) {
    synthio_block_assign_slot(damp, &self->damp, MP_QSTR_damp);
}

void audiofreeverb_freeverb_get_damp_fixedpoint(mp_float_t n, int16_t *damp1, int16_t *damp2) {
    if (n > (mp_float_t)MICROPY_FLOAT_CONST(1.0)) {
        n = MICROPY_FLOAT_CONST(1.0);
    } else if (n < (mp_float_t)MICROPY_FLOAT_CONST(0.0)) {
        n = MICROPY_FLOAT_CONST(0.0);
    }

    *damp1 = (int16_t)(n * (mp_float_t)MICROPY_FLOAT_CONST(13107.2)); // 13107.2 = 0.4f scaling factor
    *damp2 = (int16_t)(32768 - *damp1); // inverse of x1 damp2 = 1.0 - damp1
}

mp_obj_t common_hal_audiofreeverb_freeverb_get_pre_filter(audiofreeverb_freeverb_obj_t *self) {
    return self->pre_filter.obj;
}

void common_hal_audiofreeverb_freeverb_set_pre_filter(audiofreeverb_freeverb_obj_t *self, mp_obj_t filter_in) {
    audiofilters_assign_filter_chain(&self->pre_filter, filter_in, self->base.channel_count);
}

mp_obj_t common_hal_audiofreeverb_freeverb_get_post_filter(audiofreeverb_freeverb_obj_t *self) {
    return self->post_filter.obj;
}

void common_hal_audiofreeverb_freeverb_set_post_filter(audiofreeverb_freeverb_obj_t *self, mp_obj_t filter_in) {
    audiofilters_assign_filter_chain(&self->post_filter, filter_in, self->base.channel_count);
}

mp_obj_t common_hal_audiofreeverb_freeverb_get_mix(audiofreeverb_freeverb_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiofreeverb_freeverb_set_mix(audiofreeverb_freeverb_obj_t *self, mp_obj_t mix) {
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);
}

void audiofreeverb_freeverb_get_mix_fixedpoint(mp_float_t mix, int16_t *mix_sample, int16_t *mix_effect) {
    mix = mix * (mp_float_t)MICROPY_FLOAT_CONST(2.0);
    *mix_sample = (int16_t)(MIN((mp_float_t)MICROPY_FLOAT_CONST(2.0) - mix, (mp_float_t)MICROPY_FLOAT_CONST(1.0)) * 32767);
    *mix_effect = (int16_t)(MIN(mix, (mp_float_t)MICROPY_FLOAT_CONST(1.0)) * 32767);
}

void audiofreeverb_freeverb_reset_buffer(audiofreeverb_freeverb_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);

    audiofilters_reset_filter_chain(&self->pre_filter, self->base.channel_count);
    audiofilters_reset_filter_chain(&self->post_filter, self->base.channel_count);
}

bool common_hal_audiofreeverb_freeverb_get_playing(audiofreeverb_freeverb_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiofreeverb_freeverb_play(audiofreeverb_freeverb_obj_t *self, mp_obj_t sample, bool loop) {
    audiosample_must_match(&self->base, sample, false);

    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(self->sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);

    // Track remaining sample length in terms of bytes per sample
    self->sample_buffer_length /= (self->base.bits_per_sample / 8);
    // Store if we have more data in the sample to retrieve
    self->more_data = result == GET_BUFFER_MORE_DATA;

    return;
}

void common_hal_audiofreeverb_freeverb_stop(audiofreeverb_freeverb_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    // For reverb we clear the sample but the reverb continues until the object reading our effect stops
    self->sample = NULL;
    return;
}

audioio_get_buffer_result_t audiofreeverb_freeverb_get_buffer(audiofreeverb_freeverb_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {

    // Switch our buffers to the other buffer
    self->last_buf_idx = !self->last_buf_idx;

    // 16 bit samples we need a 16 bit pointer
    int16_t *word_buffer = (int16_t *)self->buffer[self->last_buf_idx];
    uint32_t length = self->buffer_len / (self->base.bits_per_sample / 8);

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it but we still need to play the reverb
                    self->sample = NULL;
                }
            }
            if (self->sample) {
                // Load another sample buffer to play
                audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0, (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
                if (result == GET_BUFFER_ERROR) {
                    self->sample = NULL;
                    self->sample_buffer_length = 0;
                    self->more_data = false;
                } else {
                    // Track length in terms of words.
                    self->sample_buffer_length /= (self->base.bits_per_sample / 8);
                    self->more_data = result == GET_BUFFER_MORE_DATA;
                }
            }
        }

        // Determine how many bytes we can process to our buffer, the less of the sample we have left and our buffer remaining
        uint32_t n;
        if (self->sample == NULL) {
            n = MIN(length, SYNTHIO_MAX_DUR * self->base.channel_count);
        } else {
            n = MIN(MIN(self->sample_buffer_length, length), SYNTHIO_MAX_DUR * self->base.channel_count);
        }

        // get the effect values we need from the BlockInput. These may change at run time so you need to do bounds checking if required
        shared_bindings_synthio_lfo_tick(self->base.sample_rate, n / self->base.channel_count);
        mp_float_t damp = synthio_block_slot_get_limited(&self->damp, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));
        int16_t damp1, damp2;
        audiofreeverb_freeverb_get_damp_fixedpoint(damp, &damp1, &damp2);

        mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));
        int16_t mix_sample, mix_effect;
        audiofreeverb_freeverb_get_mix_fixedpoint(mix, &mix_sample, &mix_effect);

        mp_float_t roomsize = synthio_block_slot_get_limited(&self->roomsize, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));
        int16_t feedback = audiofreeverb_freeverb_get_roomsize_fixedpoint(roomsize);

        // Tick biquad filters
        audiofilters_tick_filter_chain(&self->pre_filter);
        audiofilters_tick_filter_chain(&self->post_filter);

        int16_t *sample_src = (int16_t *)self->sample_remaining_buffer;

        uint32_t channel_comb_offset = 0, channel_allpass_offset = 0;

        for (uint32_t i = 0; i < n; i++) {
            int32_t sample_word = 0;
            if (self->sample != NULL) {
                sample_word = sample_src[i];
            }

            int32_t word, sum;
            int16_t input, bufout, output;

            // Apply filters as Pre-EQ
            input = (int16_t)audiofilters_process_filter_chain(&self->pre_filter, self->base.channel_count, n % self->base.channel_count, sample_word);

            input = synthio_sat16((int32_t)input * 8738, 17); // Initial input scaled down so we can add reverb
            sum = 0;

            // Calculate each of the 8 comb buffers
            for (uint32_t j = 0 + channel_comb_offset; j < 8 + channel_comb_offset; j++) {
                bufout = self->combbuffers[j][self->combbufferindex[j]];
                sum += bufout;
                self->combfitlers[j] = synthio_sat16(bufout * damp2 + self->combfitlers[j] * damp1, 15);
                self->combbuffers[j][self->combbufferindex[j]] = synthio_sat16(input + synthio_sat16(self->combfitlers[j] * feedback, 15), 0);
                if (++self->combbufferindex[j] >= self->combbuffersizes[j]) {
                    self->combbufferindex[j] = 0;
                }
            }

            output = synthio_sat16(sum * 31457, 17); // 31457 = 0.24f with shift of 17

            // Calculate each of the 4 all pass buffers
            for (uint32_t j = 0 + channel_allpass_offset; j < 4 + channel_allpass_offset; j++) {
                bufout = self->allpassbuffers[j][self->allpassbufferindex[j]];
                self->allpassbuffers[j][self->allpassbufferindex[j]] = output + (bufout >> 1); // bufout >> 1 same as bufout*0.5f
                output = synthio_sat16(bufout - output, 1);
                if (++self->allpassbufferindex[j] >= self->allpassbuffersizes[j]) {
                    self->allpassbufferindex[j] = 0;
                }
            }

            // Apply filters as Post-EQ
            output = (int16_t)audiofilters_process_filter_chain(&self->post_filter, self->base.channel_count, n % self->base.channel_count, (int32_t)output);

            word = output * 30; // Add some volume back don't have to saturate as next step will

            word = synthio_sat16(sample_word * mix_sample, 15) + synthio_sat16(word * mix_effect, 15);
            word = synthio_mix_down_sample(word, SYNTHIO_MIX_DOWN_SCALE(2));
            word_buffer[i] = (int16_t)word;

            if ((self->base.channel_count == 2) && (channel_comb_offset == 0)) {
                channel_comb_offset = 8;
                channel_allpass_offset = 4;
            } else {
                channel_comb_offset = 0;
                channel_allpass_offset = 0;
            }
        }

        // Update the remaining length and the buffer positions based on how much we wrote into our buffer
        length -= n;
        word_buffer += n;
        self->sample_remaining_buffer += (n * (self->base.bits_per_sample / 8));
        self->sample_buffer_length -= n;
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // Reverb always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
