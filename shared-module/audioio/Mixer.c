/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/audioio/Mixer.h"

#include <stdint.h>

#include "py/runtime.h"
#include "shared-module/audioio/__init__.h"
#include "shared-module/audioio/RawSample.h"

void common_hal_audioio_mixer_construct(audioio_mixer_obj_t* self,
                                        uint8_t voice_count,
                                        uint32_t buffer_size,
                                        uint8_t bits_per_sample,
                                        bool samples_signed,
                                        uint8_t channel_count,
                                        uint32_t sample_rate) {
    self->len = buffer_size / 2 / sizeof(uint32_t) * sizeof(uint32_t);

    self->first_buffer = m_malloc(self->len, false);
    if (self->first_buffer == NULL) {
        common_hal_audioio_mixer_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, translate("Couldn't allocate first buffer"));
    }

    self->second_buffer = m_malloc(self->len, false);
    if (self->second_buffer == NULL) {
        common_hal_audioio_mixer_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, translate("Couldn't allocate second buffer"));
    }

    self->bits_per_sample = bits_per_sample;
    self->samples_signed = samples_signed;
    self->channel_count = channel_count;
    self->sample_rate = sample_rate;
    self->voice_count = voice_count;

    for (uint8_t i = 0; i < self->voice_count; i++) {
        self->voice[i].sample = NULL;
    }
}

void common_hal_audioio_mixer_deinit(audioio_mixer_obj_t* self) {
    self->first_buffer = NULL;
    self->second_buffer = NULL;
}

bool common_hal_audioio_mixer_deinited(audioio_mixer_obj_t* self) {
    return self->first_buffer == NULL;
}

uint32_t common_hal_audioio_mixer_get_sample_rate(audioio_mixer_obj_t* self) {
    return self->sample_rate;
}

void common_hal_audioio_mixer_play(audioio_mixer_obj_t* self, mp_obj_t sample, uint8_t v, bool loop) {
    if (v >= self->voice_count) {
        mp_raise_ValueError(translate("Voice index too high"));
    }
    if (audiosample_sample_rate(sample) != self->sample_rate) {
        mp_raise_ValueError(translate("The sample's sample rate does not match the mixer's"));
    }
    if (audiosample_channel_count(sample) != self->channel_count) {
        mp_raise_ValueError(translate("The sample's channel count does not match the mixer's"));
    }
    if (audiosample_bits_per_sample(sample) != self->bits_per_sample) {
        mp_raise_ValueError(translate("The sample's bits_per_sample does not match the mixer's"));
    }
    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed,
                                     &max_buffer_length, &spacing);
    if (samples_signed != self->samples_signed) {
        mp_raise_ValueError(translate("The sample's signedness does not match the mixer's"));
    }
    audioio_mixer_voice_t* voice = &self->voice[v];
    voice->sample = sample;
    voice->loop = loop;

    audiosample_reset_buffer(sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(sample, false, 0, (uint8_t**) &voice->remaining_buffer, &voice->buffer_length);
    // Track length in terms of words.
    voice->buffer_length /= sizeof(uint32_t);
    voice->more_data = result == GET_BUFFER_MORE_DATA;
}

void common_hal_audioio_mixer_stop_voice(audioio_mixer_obj_t* self, uint8_t voice) {
    self->voice[voice].sample = NULL;
}

bool common_hal_audioio_mixer_get_playing(audioio_mixer_obj_t* self) {
    for (int32_t v = 0; v < self->voice_count; v++) {
        if (self->voice[v].sample != NULL) {
            return true;
        }
    }
    return false;
}

void audioio_mixer_reset_buffer(audioio_mixer_obj_t* self,
                                bool single_channel,
                                uint8_t channel) {
    for (int32_t i = 0; i < self->voice_count; i++) {
        self->voice[i].sample = NULL;
    }
}

uint32_t add8signed(uint32_t a, uint32_t b) {
    #if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
    return __QADD8(a, b);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 4; i++) {
        int8_t ai = a >> (sizeof(int8_t) * 8 * i);
        int8_t bi = b >> (sizeof(int8_t) * 8 * i);
        int32_t intermediate = (int32_t) ai + bi;
        if (intermediate > CHAR_MAX) {
            intermediate = CHAR_MAX;
        } else if (intermediate < CHAR_MIN) {
            //intermediate = CHAR_MIN;
        }
        result |= (((uint32_t) intermediate) & 0xff) << (sizeof(int8_t) * 8 * i);
    }
    return result;
    #endif
}

uint32_t add8unsigned(uint32_t a, uint32_t b) {
    #if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
    // Subtract out the DC offset, add and then shift back.
    a = __USUB8(a, 0x80808080);
    b = __USUB8(b, 0x80808080);
    uint32_t sum = __QADD8(a, b);
    return __UADD8(sum, 0x80808080);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 4; i++) {
        int8_t ai = (a >> (sizeof(uint8_t) * 8 * i)) - 128;
        int8_t bi = (b >> (sizeof(uint8_t) * 8 * i)) - 128;
        int32_t intermediate = (int32_t) ai + bi;
        if (intermediate > UCHAR_MAX) {
            intermediate = UCHAR_MAX;
        }
        result |= ((uint8_t) intermediate + 128) << (sizeof(uint8_t) * 8 * i);
    }
    return result;
    #endif
}

uint32_t add16signed(uint32_t a, uint32_t b) {
    #if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
    return __QADD16(a, b);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 2; i++) {
        int16_t ai = a >> (sizeof(int16_t) * 8 * i);
        int16_t bi = b >> (sizeof(int16_t) * 8 * i);
        int32_t intermediate = (int32_t) ai + bi;
        if (intermediate > SHRT_MAX) {
            intermediate = SHRT_MAX;
        } else if (intermediate < SHRT_MIN) {
            intermediate = SHRT_MIN;
        }
        result |= (((uint32_t) intermediate) & 0xffff) << (sizeof(int16_t) * 8 * i);
    }
    return result;
    #endif
}

uint32_t add16unsigned(uint32_t a, uint32_t b) {
    #if (defined (__ARM_FEATURE_DSP) && (__ARM_FEATURE_DSP == 1))
    // Subtract out the DC offset, add and then shift back.
    a = __USUB16(a, 0x80008000);
    b = __USUB16(b, 0x80008000);
    uint32_t sum = __QADD16(a, b);
    return __UADD16(sum, 0x80008000);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 2; i++) {
        int16_t ai = (a >> (sizeof(uint16_t) * 8 * i)) - 0x8000;
        int16_t bi = (b >> (sizeof(uint16_t) * 8 * i)) - 0x8000;
        int32_t intermediate = (int32_t) ai + bi;
        if (intermediate > USHRT_MAX) {
            intermediate = USHRT_MAX;
        }
        result |= ((uint16_t) intermediate + 0x8000) << (sizeof(int16_t) * 8 * i);
    }
    return result;
    #endif
}

audioio_get_buffer_result_t audioio_mixer_get_buffer(audioio_mixer_obj_t* self,
                                                     bool single_channel,
                                                     uint8_t channel,
                                                     uint8_t** buffer,
                                                     uint32_t* buffer_length) {
    if (!single_channel) {
        channel = 0;
    }

    uint32_t channel_read_count = self->left_read_count;
    if (channel == 1) {
        channel_read_count = self->right_read_count;
    }
    *buffer_length = self->len;

    bool need_more_data = self->read_count == channel_read_count;
    if (need_more_data) {
        uint32_t* word_buffer;
        if (self->use_first_buffer) {
            *buffer = (uint8_t*) self->first_buffer;
            word_buffer = self->first_buffer;
        } else {
            *buffer = (uint8_t*) self->second_buffer;
            word_buffer = self->second_buffer;
        }
        self->use_first_buffer = !self->use_first_buffer;
        bool voices_active = false;
        for (int32_t v = 0; v < self->voice_count; v++) {
            audioio_mixer_voice_t* voice = &self->voice[v];

            uint32_t j = 0;
            bool voice_done = voice->sample == NULL;
            for (uint32_t i = 0; i < self->len / sizeof(uint32_t); i++) {
                if (!voice_done && j >= voice->buffer_length) {
                    if (!voice->more_data) {
                        if (voice->loop) {
                            audiosample_reset_buffer(voice->sample, false, 0);
                        } else {
                            voice->sample = NULL;
                            voice_done = true;
                        }
                    }
                    if (!voice_done) {
                        // Load another buffer
                        audioio_get_buffer_result_t result = audiosample_get_buffer(voice->sample, false, 0, (uint8_t**) &voice->remaining_buffer, &voice->buffer_length);
                        // Track length in terms of words.
                        voice->buffer_length /= sizeof(uint32_t);
                        voice->more_data = result == GET_BUFFER_MORE_DATA;
                        j = 0;
                    }
                }
                // First active voice gets copied over verbatim.
                uint32_t sample_value;
                if (voice_done) {
                    // Exit early if another voice already set all samples once.
                    if (voices_active) {
                        continue;
                    }
                    sample_value = 0;
                    if (!self->samples_signed) {
                        if (self->bits_per_sample == 8) {
                            sample_value = 0x7f7f7f7f;
                        } else {
                            sample_value = 0x7fff7fff;
                        }
                    }
                } else {
                    sample_value = voice->remaining_buffer[j];
                }

                if (!voices_active) {
                    word_buffer[i] = sample_value;
                } else {
                    if (self->bits_per_sample == 8) {
                        if (self->samples_signed) {
                            word_buffer[i] = add8signed(word_buffer[i], sample_value);
                        } else {
                            word_buffer[i] = add8unsigned(word_buffer[i], sample_value);
                        }
                    } else {
                        if (self->samples_signed) {
                            word_buffer[i] = add16signed(word_buffer[i], sample_value);
                        } else {
                            word_buffer[i] = add16unsigned(word_buffer[i], sample_value);
                        }
                    }
                }
                j++;
            }
            voice->buffer_length -= j;
            voice->remaining_buffer += j;

            voices_active = true;
        }

        self->read_count += 1;
    } else if (!self->use_first_buffer) {
        *buffer = (uint8_t*) self->first_buffer;
    } else {
        *buffer = (uint8_t*) self->second_buffer;
    }


    if (channel == 0) {
        self->left_read_count += 1;
    } else if (channel == 1) {
        self->right_read_count += 1;
        *buffer = *buffer + self->bits_per_sample / 8;
    }
    return GET_BUFFER_MORE_DATA;
}

void audioio_mixer_get_buffer_structure(audioio_mixer_obj_t* self, bool single_channel,
                                        bool* single_buffer, bool* samples_signed,
                                        uint32_t* max_buffer_length, uint8_t* spacing) {
    *single_buffer = false;
    *samples_signed = self->samples_signed;
    *max_buffer_length = self->len;
    if (single_channel) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}
