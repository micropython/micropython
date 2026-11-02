// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Cooper Dalrymple
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiodelays/MultiTapDelay.h"
#include "shared-bindings/audiocore/__init__.h"

#include <stdint.h>
#include "py/runtime.h"
#include <math.h>

void common_hal_audiodelays_multi_tap_delay_construct(audiodelays_multi_tap_delay_obj_t *self, uint32_t max_delay_ms,
    mp_obj_t delay_ms, mp_obj_t decay, mp_obj_t mix, mp_obj_t taps,
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
        common_hal_audiodelays_multi_tap_delay_deinit(self);
        m_malloc_fail(self->buffer_len);
    }
    memset(self->buffer[0], 0, self->buffer_len);

    self->buffer[1] = m_malloc_maybe_without_collect(self->buffer_len);
    if (self->buffer[1] == NULL) {
        common_hal_audiodelays_multi_tap_delay_deinit(self);
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

    // The below section sets up the multi-tap delay effect's starting values. For a different effect this section will change

    // If we did not receive a BlockInput we need to create a default float value
    if (decay == MP_OBJ_NULL) {
        decay = mp_obj_new_float(MICROPY_FLOAT_CONST(0.7));
    }
    synthio_block_assign_slot(decay, &self->decay, MP_QSTR_decay);

    if (mix == MP_OBJ_NULL) {
        mix = mp_obj_new_float(MICROPY_FLOAT_CONST(0.25));
    }
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);

    // Allocate the delay buffer for the max possible delay, delay is always 16-bit
    self->max_delay_ms = max_delay_ms;
    self->max_delay_buffer_len = (uint32_t)(self->base.sample_rate / MICROPY_FLOAT_CONST(1000.0) * max_delay_ms) * (self->base.channel_count * sizeof(uint16_t)); // bytes
    self->delay_buffer = m_malloc_maybe_without_collect(self->max_delay_buffer_len);
    if (self->delay_buffer == NULL) {
        common_hal_audiodelays_multi_tap_delay_deinit(self);
        m_malloc_fail(self->max_delay_buffer_len);
    }
    memset(self->delay_buffer, 0, self->max_delay_buffer_len);

    // calculate the length of a single sample in milliseconds
    self->sample_ms = MICROPY_FLOAT_CONST(1000.0) / self->base.sample_rate;

    // calculate everything needed for the current delay
    common_hal_audiodelays_multi_tap_delay_set_delay_ms(self, delay_ms);
    self->delay_buffer_pos = 0;
    self->delay_buffer_right_pos = 0;

    // Initialize our tap values
    self->tap_positions = NULL;
    self->tap_levels = NULL;
    self->tap_offsets = NULL;
    self->tap_len = 0;
    common_hal_audiodelays_multi_tap_delay_set_taps(self, taps);
}

void common_hal_audiodelays_multi_tap_delay_deinit(audiodelays_multi_tap_delay_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->delay_buffer = NULL;
    self->buffer[0] = NULL;
    self->buffer[1] = NULL;

    self->tap_positions = NULL;
    self->tap_levels = NULL;
    self->tap_offsets = NULL;
}

mp_float_t common_hal_audiodelays_multi_tap_delay_get_delay_ms(audiodelays_multi_tap_delay_obj_t *self) {
    return self->delay_ms;
}

void common_hal_audiodelays_multi_tap_delay_set_delay_ms(audiodelays_multi_tap_delay_obj_t *self, mp_obj_t delay_ms) {
    self->delay_ms = mp_obj_get_float(delay_ms);

    // Require that delay is at least 1 sample long
    self->delay_ms = MAX(self->delay_ms, self->sample_ms);

    // Calculate the current delay buffer length in bytes
    self->delay_buffer_len = (uint32_t)(self->base.sample_rate / MICROPY_FLOAT_CONST(1000.0) * self->delay_ms) * (self->base.channel_count * sizeof(uint16_t));

    // Limit to valid range
    if (self->delay_buffer_len < self->buffer_len) {
        // If the delay buffer is smaller than our audio buffer, weird things happen
        self->delay_buffer_len = self->buffer_len;
    }
    if (self->delay_buffer_len > self->max_delay_buffer_len) {
        self->delay_buffer_len = self->max_delay_buffer_len;
    }

    // Clear the now unused part of the buffer or some weird artifacts appear
    memset(self->delay_buffer + self->delay_buffer_len, 0, self->max_delay_buffer_len - self->delay_buffer_len);

    // Update tap offsets if we have any
    recalculate_tap_offsets(self);
}

mp_obj_t common_hal_audiodelays_multi_tap_delay_get_decay(audiodelays_multi_tap_delay_obj_t *self) {
    return self->decay.obj;
}

void common_hal_audiodelays_multi_tap_delay_set_decay(audiodelays_multi_tap_delay_obj_t *self, mp_obj_t decay) {
    synthio_block_assign_slot(decay, &self->decay, MP_QSTR_decay);
}

mp_obj_t common_hal_audiodelays_multi_tap_delay_get_mix(audiodelays_multi_tap_delay_obj_t *self) {
    return self->mix.obj;
}

void common_hal_audiodelays_multi_tap_delay_set_mix(audiodelays_multi_tap_delay_obj_t *self, mp_obj_t mix) {
    synthio_block_assign_slot(mix, &self->mix, MP_QSTR_mix);
}

mp_obj_t common_hal_audiodelays_multi_tap_delay_get_taps(audiodelays_multi_tap_delay_obj_t *self) {
    if (!self->tap_len) {
        return mp_const_none;
    } else {
        mp_obj_tuple_t *taps = mp_obj_new_tuple(self->tap_len, NULL);
        for (size_t i = 0; i < self->tap_len; i++) {
            mp_obj_tuple_t *pair = mp_obj_new_tuple(2, NULL);
            pair->items[0] = mp_obj_new_float(self->tap_positions[i]);
            pair->items[1] = mp_obj_new_float(self->tap_levels[i]);
            taps->items[i] = pair;
        }
        return taps;
    }
}

void validate_tap_value(mp_obj_t item, qstr arg_name) {
    if (mp_obj_is_small_int(item)) {
        mp_arg_validate_int_range(mp_obj_get_int(item), 0, 1, arg_name);
    } else {
        mp_arg_validate_obj_float_range(item, 0, 1, arg_name);
    }
}

mp_float_t get_tap_value(mp_obj_t item) {
    mp_float_t value;
    if (mp_obj_is_small_int(item)) {
        value = (mp_float_t)mp_obj_get_int(item);
    } else {
        value = mp_obj_float_get(item);
    }
    return value;
}

void common_hal_audiodelays_multi_tap_delay_set_taps(audiodelays_multi_tap_delay_obj_t *self, mp_obj_t taps_in) {
    if (taps_in != mp_const_none && !MP_OBJ_TYPE_HAS_SLOT(mp_obj_get_type(taps_in), iter)) {
        mp_raise_TypeError_varg(
            MP_ERROR_TEXT("%q must be of type %q, not %q"),
            MP_QSTR_taps, MP_QSTR_iterable, mp_obj_get_type(taps_in)->name);
    }

    size_t len, i;
    mp_obj_t *items;

    if (taps_in == mp_const_none) {
        len = 0;
        items = NULL;
    } else {
        // convert object to tuple if it wasn't before
        taps_in = MP_OBJ_TYPE_GET_SLOT(&mp_type_tuple, make_new)(
            &mp_type_tuple, 1, 0, &taps_in);

        mp_obj_tuple_get(taps_in, &len, &items);
        mp_arg_validate_length_min(len, 1, MP_QSTR_items);

        for (i = 0; i < len; i++) {
            mp_obj_t item = items[i];
            if (mp_obj_is_tuple_compatible(item)) {
                size_t len1;
                mp_obj_t *items1;
                mp_obj_tuple_get(item, &len1, &items1);
                mp_arg_validate_length(len1, 2, MP_QSTR_items);

                for (size_t j = 0; j < len1; j++) {
                    validate_tap_value(items1[j], j ? MP_QSTR_level : MP_QSTR_position);
                }
            } else if (mp_obj_is_float(item) || mp_obj_is_small_int(item)) {
                validate_tap_value(item, MP_QSTR_position);
            } else {
                mp_raise_TypeError_varg(
                    MP_ERROR_TEXT("%q in %q must be of type %q or %q, not %q"),
                    MP_QSTR_object,
                    MP_QSTR_taps,
                    MP_QSTR_iterable,
                    MP_QSTR_float,
                    mp_obj_get_type(item)->name);
            }

        }
    }

    self->tap_positions = m_renew(mp_float_t,
        self->tap_positions,
        self->tap_len,
        len);
    self->tap_levels = m_renew(mp_float_t,
        self->tap_levels,
        self->tap_len,
        len);
    self->tap_offsets = m_renew(uint32_t,
        self->tap_offsets,
        self->tap_len,
        len);
    self->tap_len = len;

    for (i = 0; i < len; i++) {
        mp_obj_t item = items[i];
        if (mp_obj_is_tuple_compatible(item)) {
            size_t len1;
            mp_obj_t *items1;
            mp_obj_tuple_get(item, &len1, &items1);

            self->tap_positions[i] = get_tap_value(items1[0]);
            self->tap_levels[i] = get_tap_value(items1[1]);
        } else {
            self->tap_positions[i] = get_tap_value(item);
            self->tap_levels[i] = MICROPY_FLOAT_CONST(1.0);
        }
    }

    recalculate_tap_offsets(self);
}

void recalculate_tap_offsets(audiodelays_multi_tap_delay_obj_t *self) {
    if (!self->tap_len) {
        return;
    }

    uint32_t delay_buffer_len = self->delay_buffer_len / self->base.channel_count / sizeof(uint16_t);
    for (size_t i = 0; i < self->tap_len; i++) {
        self->tap_offsets[i] = (uint32_t)(delay_buffer_len * self->tap_positions[i]);
    }
}

void audiodelays_multi_tap_delay_reset_buffer(audiodelays_multi_tap_delay_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {

    memset(self->buffer[0], 0, self->buffer_len);
    memset(self->buffer[1], 0, self->buffer_len);
    memset(self->delay_buffer, 0, self->max_delay_buffer_len);
}

bool common_hal_audiodelays_multi_tap_delay_get_playing(audiodelays_multi_tap_delay_obj_t *self) {
    return self->sample != NULL;
}

void common_hal_audiodelays_multi_tap_delay_play(audiodelays_multi_tap_delay_obj_t *self, mp_obj_t sample, bool loop) {
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

void common_hal_audiodelays_multi_tap_delay_stop(audiodelays_multi_tap_delay_obj_t *self) {
    // When the sample is set to stop playing do any cleanup here
    // For delay we clear the sample but the delay continues until the object reading our effect stops
    self->sample = NULL;
    return;
}

audioio_get_buffer_result_t audiodelays_multi_tap_delay_get_buffer(audiodelays_multi_tap_delay_obj_t *self, bool single_channel_output, uint8_t channel,
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

    // The delay buffer is always stored as a 16-bit value internally
    int16_t *delay_buffer = (int16_t *)self->delay_buffer;
    uint32_t delay_buffer_len = self->delay_buffer_len / self->base.channel_count / sizeof(uint16_t);

    uint32_t delay_buffer_pos = self->delay_buffer_pos;
    if (single_channel_output && channel == 1) {
        delay_buffer_pos = self->delay_buffer_right_pos;
    }

    int32_t mix_down_scale = SYNTHIO_MIX_DOWN_SCALE(self->tap_len);

    // Loop over the entire length of our buffer to fill it, this may require several calls to get data from the sample
    while (length != 0) {
        // Check if there is no more sample to play, we will either load more data, reset the sample if loop is on or clear the sample
        if (self->sample_buffer_length == 0) {
            if (!self->more_data) { // The sample has indicated it has no more data to play
                if (self->loop && self->sample) { // If we are supposed to loop reset the sample to the start
                    audiosample_reset_buffer(self->sample, false, 0);
                } else { // If we were not supposed to loop the sample, stop playing it but we still need to play the delay
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
        mp_float_t mix = synthio_block_slot_get_limited(&self->mix, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0)) * MICROPY_FLOAT_CONST(2.0);
        mp_float_t decay = synthio_block_slot_get_limited(&self->decay, MICROPY_FLOAT_CONST(0.0), MICROPY_FLOAT_CONST(1.0));

        int16_t *sample_src = NULL;
        int8_t *sample_hsrc = NULL;
        if (self->sample != NULL) {
            // we have a sample to play and delay
            sample_src = (int16_t *)self->sample_remaining_buffer; // for 16-bit samples
            sample_hsrc = (int8_t *)self->sample_remaining_buffer; // for 8-bit samples
        }

        for (uint32_t i = 0; i < n; i++) {
            uint32_t delay_buffer_offset = delay_buffer_len * ((single_channel_output && channel == 1) || (!single_channel_output && (i % self->base.channel_count) == 1));

            int32_t sample_word = 0;
            if (self->sample != NULL) {
                if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                    sample_word = sample_src[i];
                } else {
                    if (self->base.samples_signed) {
                        sample_word = sample_hsrc[i];
                    } else {
                        // Be careful here changing from an 8 bit unsigned to signed into a 32-bit signed
                        sample_word = (int8_t)(((uint8_t)sample_hsrc[i]) ^ 0x80);
                    }
                }
            }

            // Pull words from delay buffer at tap positions, apply level and mix down
            int32_t word = 0;
            int32_t delay_word;
            if (self->tap_len) {
                size_t tap_pos;
                for (size_t j = 0; j < self->tap_len; j++) {
                    tap_pos = (delay_buffer_pos + delay_buffer_len - self->tap_offsets[j]) % delay_buffer_len;
                    delay_word = delay_buffer[tap_pos + delay_buffer_offset];
                    word += (int32_t)(delay_word * self->tap_levels[j]);
                }

                if (self->tap_len > 1) {
                    word = synthio_mix_down_sample(word, mix_down_scale);
                }
            }

            // Update delay buffer with sample and decay
            delay_word = delay_buffer[delay_buffer_pos + delay_buffer_offset];

            // If no taps are provided, use as standard delay
            if (!self->tap_len) {
                word = delay_word;
            }

            // Apply decay and add sample
            delay_word = (int32_t)(delay_word * decay) + sample_word;

            if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                delay_word = synthio_mix_down_sample(delay_word, SYNTHIO_MIX_DOWN_SCALE(2));
                delay_buffer[delay_buffer_pos + delay_buffer_offset] = (int16_t)delay_word;
            } else {
                // Do not have mix_down for 8 bit so just hard cap samples into 1 byte
                delay_word = MIN(MAX(delay_word, -128), 127);
                delay_buffer[delay_buffer_pos + delay_buffer_offset] = (int8_t)delay_word;
            }

            // Mix sample with tap output
            word = (int32_t)((sample_word * MIN(MICROPY_FLOAT_CONST(2.0) - mix, MICROPY_FLOAT_CONST(1.0)))
                + (word * MIN(mix, MICROPY_FLOAT_CONST(1.0))));
            word = synthio_mix_down_sample(word, SYNTHIO_MIX_DOWN_SCALE(2));

            if (MP_LIKELY(self->base.bits_per_sample == 16)) {
                word_buffer[i] = (int16_t)word;
                if (!self->base.samples_signed) {
                    word_buffer[i] ^= 0x8000;
                }
            } else {
                int8_t mixed = (int16_t)word;
                if (self->base.samples_signed) {
                    hword_buffer[i] = mixed;
                } else {
                    hword_buffer[i] = (uint8_t)mixed ^ 0x80;
                }
            }

            if ((self->base.channel_count == 1 || single_channel_output || (!single_channel_output && (i % self->base.channel_count) == 1))
                && ++delay_buffer_pos >= delay_buffer_len) {
                delay_buffer_pos = 0;
            }
        }

        // Update the remaining length and the buffer positions based on how much we wrote into our buffer
        length -= n;
        word_buffer += n;
        hword_buffer += n;
        if (self->sample != NULL) {
            self->sample_remaining_buffer += (n * (self->base.bits_per_sample / 8));
            self->sample_buffer_length -= n;
        }
    }

    if (single_channel_output && channel == 1) {
        self->delay_buffer_right_pos = delay_buffer_pos;
    } else {
        self->delay_buffer_pos = delay_buffer_pos;
    }

    // Finally pass our buffer and length to the calling audio function
    *buffer = (uint8_t *)self->buffer[self->last_buf_idx];
    *buffer_length = self->buffer_len;

    // MultiTapDelay always returns more data but some effects may return GET_BUFFER_DONE or GET_BUFFER_ERROR (see audiocore/__init__.h)
    return GET_BUFFER_MORE_DATA;
}
