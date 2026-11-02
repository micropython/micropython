// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "shared-bindings/audiotap/Tap.h"
#include "shared-bindings/audiocore/__init__.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#include "py/runtime.h"

void common_hal_audiotap_tap_construct(audiotap_tap_obj_t *self, mp_float_t capacity) {
    // The format comes from the first sample played. Until then the tap is
    // 16 bit mono at 0 Hz, which nothing plays: a channel count of 0 would
    // mark it deinitialized.
    self->base.bits_per_sample = 16;
    self->base.samples_signed = true;
    self->base.channel_count = 1;
    self->base.sample_rate = 0;
    self->base.single_buffer = false;
    self->base.max_buffer_length = 0;

    self->silence = NULL;
    self->silence_len = 0;

    self->sample = NULL;
    self->sample_remaining_buffer = NULL;
    self->sample_buffer_length = 0;
    self->loop = false;
    self->more_data = false;

    self->capacity_seconds = capacity;
    self->ring = NULL;
    self->capacity = 0;
    self->write_pos = 0;
    self->read_pos = 0;
    self->overflow = false;

    self->peak = 0;
    self->peak_reset = false;
    self->sum_squares = 0;
    self->level_count = 0;
    self->rms_reset = false;
}

void common_hal_audiotap_tap_deinit(audiotap_tap_obj_t *self) {
    audiosample_mark_deinit(&self->base);
    self->sample = NULL;
    self->silence = NULL;
    self->ring = NULL;
    self->capacity = 0;
}

void audiotap_tap_reset_buffer(audiotap_tap_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {
    (void)self;
    (void)single_channel_output;
    (void)channel;
    // Nothing to reset: the tap passes on the source's buffers, and the ring
    // belongs to the reader, so what it holds stays readable.
}

bool common_hal_audiotap_tap_get_playing(audiotap_tap_obj_t *self) {
    return self->sample != NULL;
}

// Takes the format of the first sample played and allocates for it.
static void take_format(audiotap_tap_obj_t *self, mp_obj_t sample) {
    const audiosample_base_t *source = audiosample_check(sample);
    audiosample_check_for_deinit(source);
    if (source->bits_per_sample != 8 && source->bits_per_sample != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("bits_per_sample must be 8 or 16"));
    }
    const uint32_t sample_bytes = source->bits_per_sample / 8;
    const uint32_t frame_bytes = sample_bytes * source->channel_count;

    // Silence for when the source has no audio, no longer than the source's
    // buffers and in whole frames.
    uint32_t silence_len = MIN(MAX(source->max_buffer_length, frame_bytes), 512);
    silence_len -= silence_len % frame_bytes;
    int8_t *silence = m_malloc_without_collect(silence_len);
    if (source->samples_signed) {
        memset(silence, 0, silence_len);
    } else if (sample_bytes == 2) {
        uint16_t *words = (uint16_t *)silence;
        for (uint32_t i = 0; i < silence_len / 2; i++) {
            words[i] = 0x8000;
        }
    } else {
        memset(silence, 0x80, silence_len);
    }

    // Rounded, so that a capacity such as 0.7 s, which a float holds as a hair
    // under, does not lose a frame.
    uint32_t capacity = (uint32_t)(self->capacity_seconds * source->sample_rate + MICROPY_FLOAT_CONST(0.5)) * source->channel_count;
    uint8_t *ring = NULL;
    if (capacity > 0) {
        ring = m_malloc_without_collect(capacity * sample_bytes);
    }

    // Nothing plays the tap before it has a format, so the reader is the
    // only one using these yet.
    self->silence = silence;
    self->silence_len = silence_len;
    self->ring = ring;
    self->capacity = capacity;
    self->write_pos = 0;
    self->read_pos = 0;

    self->base.bits_per_sample = source->bits_per_sample;
    self->base.samples_signed = source->samples_signed;
    self->base.channel_count = source->channel_count;
    self->base.sample_rate = source->sample_rate;
    self->base.max_buffer_length = MAX(source->max_buffer_length, silence_len);
}

void common_hal_audiotap_tap_play(audiotap_tap_obj_t *self, mp_obj_t sample, bool loop) {
    if (self->silence == NULL) {
        take_format(self, sample);
    } else {
        audiosample_must_match(&self->base, sample, false);
    }

    audiosample_reset_buffer(sample, false, 0);
    self->loop = loop;
    self->sample_buffer_length = 0;
    // Ask the source for its first buffer.
    self->more_data = true;
    self->sample = sample;
}

void common_hal_audiotap_tap_stop(audiotap_tap_obj_t *self) {
    self->sample = NULL;
}

// Positions run from 0 to 2 * capacity.
static uint32_t ring_used(uint32_t capacity, uint32_t write, uint32_t read) {
    return write >= read ? write - read : write + 2 * capacity - read;
}

static uint32_t ring_index(uint32_t capacity, uint32_t pos) {
    return pos >= capacity ? pos - capacity : pos;
}

static uint32_t ring_advance(uint32_t capacity, uint32_t pos, uint32_t count) {
    pos += count;
    return pos >= 2 * capacity ? pos - 2 * capacity : pos;
}

// The sample at i, as a signed 16 bit value.
static int32_t sample_at(audiotap_tap_obj_t *self, const uint8_t *data, uint32_t i) {
    if (MP_LIKELY(self->base.bits_per_sample == 16)) {
        uint16_t raw = ((const uint16_t *)data)[i];
        return self->base.samples_signed ? (int16_t)raw : (int16_t)(raw ^ 0x8000);
    }
    uint8_t raw = data[i];
    return (self->base.samples_signed ? (int8_t)raw : (int8_t)(raw ^ 0x80)) * 256;
}

// Called by whatever plays the tap, for each run of samples from the source.
// It may be an interrupt, so it never allocates and changes only write_pos and
// the levels.
static void tap_capture(audiotap_tap_obj_t *self, const uint8_t *data, uint32_t count) {
    if (count == 0) {
        return;
    }

    if (self->peak_reset) {
        self->peak = 0;
        self->peak_reset = false;
    }
    if (self->rms_reset) {
        self->sum_squares = 0;
        self->level_count = 0;
        self->rms_reset = false;
    }
    uint32_t peak = self->peak;
    uint64_t sum = 0;
    for (uint32_t i = 0; i < count; i++) {
        int32_t value = sample_at(self, data, i);
        uint32_t magnitude = value < 0 ? -value : value;
        if (magnitude > peak) {
            peak = magnitude;
        }
        sum += (uint64_t)(value * value);
    }
    self->peak = peak;
    self->sum_squares += (float)sum / (32768.0f * 32768.0f);
    self->level_count += count;

    if (self->ring == NULL) {
        return;
    }
    const uint32_t capacity = self->capacity;
    const uint32_t sample_bytes = self->base.bits_per_sample / 8;
    uint32_t write = self->write_pos;
    uint32_t free = capacity - ring_used(capacity, write, self->read_pos);
    if (count > free) {
        // The reader fell behind. Moving read_pos to keep the newest audio
        // would race with it, so drop what doesn't fit.
        count = free;
        self->overflow = true;
    }
    while (count > 0) {
        uint32_t index = ring_index(capacity, write);
        uint32_t run = MIN(count, capacity - index);
        memcpy(self->ring + index * sample_bytes, data, run * sample_bytes);
        data += run * sample_bytes;
        count -= run;
        write = ring_advance(capacity, write, run);
    }
    // The samples must be in the ring before the reader can see them.
    __sync_synchronize();
    self->write_pos = write;
}

uint32_t common_hal_audiotap_tap_get_available(audiotap_tap_obj_t *self) {
    if (self->ring == NULL) {
        return 0;
    }
    return ring_used(self->capacity, self->write_pos, self->read_pos);
}

uint32_t common_hal_audiotap_tap_readinto(audiotap_tap_obj_t *self, uint8_t *buffer, uint32_t length) {
    if (self->ring == NULL) {
        return 0;
    }
    const uint32_t capacity = self->capacity;
    const uint32_t sample_bytes = self->base.bits_per_sample / 8;
    uint32_t read = self->read_pos;
    uint32_t write = self->write_pos;
    __sync_synchronize();
    uint32_t count = MIN(length - length % self->base.channel_count,
        ring_used(capacity, write, read));
    uint32_t done = 0;
    while (done < count) {
        uint32_t index = ring_index(capacity, read);
        uint32_t run = MIN(count - done, capacity - index);
        memcpy(buffer + done * sample_bytes, self->ring + index * sample_bytes, run * sample_bytes);
        done += run;
        read = ring_advance(capacity, read, run);
    }
    // Copy out before the writer may reuse the space.
    __sync_synchronize();
    self->read_pos = read;
    return count;
}

void common_hal_audiotap_tap_clear(audiotap_tap_obj_t *self) {
    self->read_pos = self->write_pos;
}

bool common_hal_audiotap_tap_get_overflow(audiotap_tap_obj_t *self) {
    bool overflow = self->overflow;
    self->overflow = false;
    return overflow;
}

mp_float_t common_hal_audiotap_tap_get_peak(audiotap_tap_obj_t *self) {
    if (self->peak_reset) {
        // No audio passed since the last read.
        return 0;
    }
    mp_float_t peak = (mp_float_t)self->peak / MICROPY_FLOAT_CONST(32768.0);
    self->peak_reset = true;
    return peak;
}

mp_float_t common_hal_audiotap_tap_get_rms(audiotap_tap_obj_t *self) {
    if (self->rms_reset) {
        // No audio passed since the last read.
        return 0;
    }
    uint32_t count = self->level_count;
    float sum_squares = self->sum_squares;
    self->rms_reset = true;
    return count > 0 ? MICROPY_FLOAT_C_FUN(sqrt)((mp_float_t)sum_squares / count) : 0;
}

audioio_get_buffer_result_t audiotap_tap_get_buffer(audiotap_tap_obj_t *self, bool single_channel_output, uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    (void)single_channel_output;
    (void)channel;

    if (self->silence == NULL) {
        // Deinitialized while something still plays it.
        *buffer = NULL;
        *buffer_length = 0;
        return GET_BUFFER_DONE;
    }

    const uint32_t sample_bytes = self->base.bits_per_sample / 8;
    if (self->sample_buffer_length == 0 && self->sample != NULL) {
        if (!self->more_data) {
            if (self->loop) {
                audiosample_reset_buffer(self->sample, false, 0);
            } else {
                self->sample = NULL;
            }
        }
        if (self->sample != NULL) {
            audioio_get_buffer_result_t result = audiosample_get_buffer(self->sample, false, 0,
                (uint8_t **)&self->sample_remaining_buffer, &self->sample_buffer_length);
            if (result == GET_BUFFER_ERROR) {
                self->sample = NULL;
                self->sample_buffer_length = 0;
                self->more_data = false;
            } else {
                self->sample_buffer_length /= sample_bytes;
                self->more_data = result == GET_BUFFER_MORE_DATA;
            }
        }
    }

    if (self->sample == NULL || self->sample_buffer_length == 0) {
        // Like the effects, a tap plays silence when it has no audio, and
        // rather than ask a source that has nothing yet again in a loop.
        *buffer = (uint8_t *)self->silence;
        *buffer_length = self->silence_len;
        return GET_BUFFER_MORE_DATA;
    }

    // Pass on the source's own buffer, in whole frames no longer than what
    // the player expects, which a later sample's buffers may be.
    const uint32_t channel_count = self->base.channel_count;
    const uint32_t max_samples = self->base.max_buffer_length / sample_bytes / channel_count * channel_count;
    uint32_t n = MIN(self->sample_buffer_length, max_samples);
    tap_capture(self, self->sample_remaining_buffer, n);
    *buffer = self->sample_remaining_buffer;
    *buffer_length = n * sample_bytes;
    self->sample_remaining_buffer += n * sample_bytes;
    self->sample_buffer_length -= n;
    return GET_BUFFER_MORE_DATA;
}
