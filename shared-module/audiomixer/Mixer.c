/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
 *               2018 DeanM for Adafruit Industries
 *               2019 Michael Schroeder
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

#include "shared-bindings/audiomixer/Mixer.h"
#include "shared-bindings/audiomixer/MixerVoice.h"

#include <stdint.h>

#include "py/runtime.h"
#include "shared-module/audiocore/__init__.h"
#include "shared-module/audiocore/RawSample.h"

void common_hal_audiomixer_mixer_construct(audiomixer_mixer_obj_t* self,
                                           uint8_t voice_count,
                                           uint32_t buffer_size,
                                           uint8_t bits_per_sample,
                                           bool samples_signed,
                                           uint8_t channel_count,
                                           uint32_t sample_rate) {
    self->len = buffer_size / 2 / sizeof(uint32_t) * sizeof(uint32_t);

    self->first_buffer = m_malloc(self->len, false);
    if (self->first_buffer == NULL) {
        common_hal_audiomixer_mixer_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, translate("Couldn't allocate first buffer"));
    }

    self->second_buffer = m_malloc(self->len, false);
    if (self->second_buffer == NULL) {
        common_hal_audiomixer_mixer_deinit(self);
        mp_raise_msg(&mp_type_MemoryError, translate("Couldn't allocate second buffer"));
    }

    self->bits_per_sample = bits_per_sample;
    self->samples_signed = samples_signed;
    self->channel_count = channel_count;
    self->sample_rate = sample_rate;
    self->voice_count = voice_count;
}

void common_hal_audiomixer_mixer_deinit(audiomixer_mixer_obj_t* self) {
    self->first_buffer = NULL;
    self->second_buffer = NULL;
}

bool common_hal_audiomixer_mixer_deinited(audiomixer_mixer_obj_t* self) {
    return self->first_buffer == NULL;
}

uint32_t common_hal_audiomixer_mixer_get_sample_rate(audiomixer_mixer_obj_t* self) {
    return self->sample_rate;
}

bool common_hal_audiomixer_mixer_get_playing(audiomixer_mixer_obj_t* self) {
    for (uint8_t v = 0; v < self->voice_count; v++) {
        if (common_hal_audiomixer_mixervoice_get_playing(MP_OBJ_TO_PTR(self->voice[v]))) {
            return true;
        }
    }
    return false;
}

void audiomixer_mixer_reset_buffer(audiomixer_mixer_obj_t* self,
                                   bool single_channel,
                                   uint8_t channel) {
    for (uint8_t i = 0; i < self->voice_count; i++) {
        common_hal_audiomixer_mixervoice_stop(self->voice[i]);
    }
}

uint32_t add8signed(uint32_t a, uint32_t b) {
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    return __SHADD8(a, b);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 4; i++) {
        int8_t ai = a >> (sizeof(int8_t) * 8 * i);
        int8_t bi = b >> (sizeof(int8_t) * 8 * i);
        int32_t intermediate = (int32_t) ai + bi / 2;
        if (intermediate > CHAR_MAX) {
            intermediate = CHAR_MAX;
        } else if (intermediate < CHAR_MIN) {
            intermediate = CHAR_MIN;
        }
        result |= ((uint32_t) intermediate & 0xff) << (sizeof(int8_t) * 8 * i);
    }
    return result;
    #endif
}

uint32_t add8unsigned(uint32_t a, uint32_t b) {
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    return __UHADD8(a, b);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 4; i++) {
        uint8_t ai = (a >> (sizeof(uint8_t) * 8 * i));
        uint8_t bi = (b >> (sizeof(uint8_t) * 8 * i));
        int32_t intermediate = (int32_t) (ai + bi) / 2;
        if (intermediate > UCHAR_MAX) {
            intermediate = UCHAR_MAX;
        }
        result |= ((uint32_t) intermediate & 0xff) << (sizeof(uint8_t) * 8 * i);
    }
    return result;
    #endif
}

uint32_t add16signed(uint32_t a, uint32_t b) {
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    return __SHADD16(a, b);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 2; i++) {
        int16_t ai = a >> (sizeof(int16_t) * 8 * i);
        int16_t bi = b >> (sizeof(int16_t) * 8 * i);
        int32_t intermediate = (int32_t) ai + bi / 2;
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
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    return __UHADD16(a, b);
    #else
    uint32_t result = 0;
    for (int8_t i = 0; i < 2; i++) {
        int16_t ai = (a >> (sizeof(uint16_t) * 8 * i)) - 0x8000;
        int16_t bi = (b >> (sizeof(uint16_t) * 8 * i)) - 0x8000;
        int32_t intermediate = (int32_t) ai + bi / 2;
        if (intermediate > USHRT_MAX) {
            intermediate = USHRT_MAX;
        }
        result |= ((uint32_t) intermediate & 0xffff) << (sizeof(int16_t) * 8 * i);
    }
    return result;
    #endif
}

static inline uint32_t mult8unsigned(uint32_t val, int32_t mul) {
    // if mul == 0, no need in wasting cycles
    if (mul == 0) {
        return 0;
    }
    /* TODO: workout ARMv7 instructions
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    return val;
    #else*/
    uint32_t result = 0;
    float mod_mul = (float) mul / (float) ((1<<15)-1);
    for (int8_t i = 0; i < 4; i++) {
        uint8_t ai = val >> (sizeof(uint8_t) * 8 * i);
        int32_t intermediate = ai * mod_mul;
        if (intermediate > SHRT_MAX) {
            intermediate = SHRT_MAX;
        }
        result |= ((uint32_t) intermediate & 0xff) << (sizeof(uint8_t) * 8 * i);
    }

    return result;
    //#endif
}

static inline uint32_t mult8signed(uint32_t val, int32_t mul) {
    // if mul == 0, no need in wasting cycles
    if (mul == 0) {
        return 0;
    }
    /* TODO: workout ARMv7 instructions
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    return val;
    #else
    */
    uint32_t result = 0;
    float mod_mul = (float)mul / (float)((1<<15)-1);
    for (int8_t i = 0; i < 4; i++) {
        int16_t ai = val >> (sizeof(int8_t) * 8 * i);
        int32_t intermediate = ai * mod_mul;
        if (intermediate > CHAR_MAX) {
            intermediate = CHAR_MAX;
        } else if (intermediate < CHAR_MIN) {
            intermediate = CHAR_MIN;
        }
        result |= (((uint32_t) intermediate) & 0xff) << (sizeof(int16_t) * 8 * i);
    }
    return result;
    //#endif
}

//TODO:
static inline uint32_t mult16unsigned(uint32_t val, int32_t mul) {
    // if mul == 0, no need in wasting cycles
    if (mul == 0) {
        return 0;
    }
    /* TODO: the below ARMv7m instructions "work", but the amplitude is much higher/louder
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    // there is no unsigned equivalent to the 'SMULWx' ARMv7 Thumb function,
    // so we have to do it by hand.
    uint32_t lo = val & 0xffff;
    uint32_t hi = val >> 16;
    //mp_printf(&mp_plat_print, "pre-asm: (mul: %d)\n\tval: %x\tlo: %x\thi: %x\n", mul, val, lo, hi);
    uint32_t val_lo;
    asm volatile("mul %0, %1, %2" : "=r" (val_lo) : "r" (mul), "r" (lo));
    asm volatile("mla %0, %1, %2, %3" : "=r" (val) : "r" (mul), "r" (hi), "r" (val_lo));
    //mp_printf(&mp_plat_print, "post-asm:\n\tval: %x\tlo: %x\n\n", val, val_lo);
    return val;
    #else
    */
    uint32_t result = 0;
    float mod_mul = (float)mul / (float)((1<<15)-1);
    for (int8_t i = 0; i < 2; i++) {
        int16_t ai = (val >> (sizeof(uint16_t) * 8 * i)) - 0x8000;
        int32_t intermediate = ai * mod_mul;
        if (intermediate > SHRT_MAX) {
            intermediate = SHRT_MAX;
        } else if (intermediate < SHRT_MIN) {
            intermediate = SHRT_MIN;
        }
        result |= (((uint32_t) intermediate) + 0x8000) << (sizeof(int16_t) * 8 * i);
    }
    return result;
    //#endif
}

static inline uint32_t mult16signed(uint32_t val, int32_t mul) {
    // if mul == 0, no need in wasting cycles
    if (mul == 0) {
        return 0;
    }
    #if (defined (__ARM_ARCH_7EM__) && (__ARM_ARCH_7EM__ == 1)) //Cortex-M4 w/FPU
    int32_t hi, lo;
    enum { bits = 16 }; // saturate to 16 bits
    enum { shift = 0 }; // shift is done automatically
    asm volatile("smulwb %0, %1, %2" : "=r" (lo) : "r" (mul), "r" (val));
    asm volatile("smulwt %0, %1, %2" : "=r" (hi) : "r" (mul), "r" (val));
    asm volatile("ssat %0, %1, %2, asr %3" : "=r" (lo) : "I" (bits), "r" (lo), "I" (shift));
    asm volatile("ssat %0, %1, %2, asr %3" : "=r" (hi) : "I" (bits), "r" (hi), "I" (shift));
    asm volatile("pkhbt %0, %1, %2, lsl #16" : "=r" (val) : "r" (lo), "r" (hi)); // pack
    return val;
    #else
    uint32_t result = 0;
    float mod_mul = (float)mul / (float)((1<<15)-1);
    for (int8_t i = 0; i < 2; i++) {
        int16_t ai = val >> (sizeof(int16_t) * 8 * i);
        int32_t intermediate = ai * mod_mul;
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

audioio_get_buffer_result_t audiomixer_mixer_get_buffer(audiomixer_mixer_obj_t* self,
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
            audiomixer_mixervoice_obj_t* voice = MP_OBJ_TO_PTR(self->voice[v]);

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

                // apply the mixer level
            	if (!self->samples_signed) {
                    if (self->bits_per_sample == 8) {
                        sample_value = mult8unsigned(sample_value, voice->level);
                    } else {
                        sample_value = mult16unsigned(sample_value, voice->level);
                    }
                } else {
                    if (self->bits_per_sample == 8) {
                        sample_value = mult8signed(sample_value, voice->level);
                    } else {
                        sample_value = mult16signed(sample_value, voice->level);
                    }
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

void audiomixer_mixer_get_buffer_structure(audiomixer_mixer_obj_t* self, bool single_channel,
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
