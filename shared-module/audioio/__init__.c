/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "shared-module/audioio/__init__.h"

#include "py/obj.h"
#include "shared-bindings/audioio/Mixer.h"
#include "shared-bindings/audioio/RawSample.h"
#include "shared-bindings/audioio/WaveFile.h"
#include "shared-module/audioio/Mixer.h"
#include "shared-module/audioio/RawSample.h"
#include "shared-module/audioio/WaveFile.h"

uint32_t audiosample_sample_rate(mp_obj_t sample_obj) {
    if (MP_OBJ_IS_TYPE(sample_obj, &audioio_rawsample_type)) {
        audioio_rawsample_obj_t* sample = MP_OBJ_TO_PTR(sample_obj);
        return sample->sample_rate;
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_wavefile_type)) {
        audioio_wavefile_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        return file->sample_rate;
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_mixer_type)) {
        audioio_mixer_obj_t* mixer = MP_OBJ_TO_PTR(sample_obj);
        return mixer->sample_rate;
    }
    return 16000;
}

uint8_t audiosample_bits_per_sample(mp_obj_t sample_obj) {
    if (MP_OBJ_IS_TYPE(sample_obj, &audioio_rawsample_type)) {
        audioio_rawsample_obj_t* sample = MP_OBJ_TO_PTR(sample_obj);
        return sample->bits_per_sample;
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_wavefile_type)) {
        audioio_wavefile_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        return file->bits_per_sample;
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_mixer_type)) {
        audioio_mixer_obj_t* mixer = MP_OBJ_TO_PTR(sample_obj);
        return mixer->bits_per_sample;
    }
    return 8;
}

uint8_t audiosample_channel_count(mp_obj_t sample_obj) {
    if (MP_OBJ_IS_TYPE(sample_obj, &audioio_rawsample_type)) {
        audioio_rawsample_obj_t* sample = MP_OBJ_TO_PTR(sample_obj);
        return sample->channel_count;
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_wavefile_type)) {
        audioio_wavefile_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        return file->channel_count;
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_mixer_type)) {
        audioio_mixer_obj_t* mixer = MP_OBJ_TO_PTR(sample_obj);
        return mixer->channel_count;
    }
    return 1;
}

void audiosample_reset_buffer(mp_obj_t sample_obj, bool single_channel, uint8_t audio_channel) {
    if (MP_OBJ_IS_TYPE(sample_obj, &audioio_rawsample_type)) {
        audioio_rawsample_obj_t* sample = MP_OBJ_TO_PTR(sample_obj);
        audioio_rawsample_reset_buffer(sample, single_channel, audio_channel);
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_wavefile_type)) {
        audioio_wavefile_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        audioio_wavefile_reset_buffer(file, single_channel, audio_channel);
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_mixer_type)) {
        audioio_mixer_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        audioio_mixer_reset_buffer(file, single_channel, audio_channel);
    }
}

audioio_get_buffer_result_t audiosample_get_buffer(mp_obj_t sample_obj,
                                                   bool single_channel,
                                                   uint8_t channel,
                                                   uint8_t** buffer, uint32_t* buffer_length) {
    if (MP_OBJ_IS_TYPE(sample_obj, &audioio_rawsample_type)) {
        audioio_rawsample_obj_t* sample = MP_OBJ_TO_PTR(sample_obj);
        return audioio_rawsample_get_buffer(sample, single_channel, channel, buffer, buffer_length);
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_wavefile_type)) {
        audioio_wavefile_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        return audioio_wavefile_get_buffer(file, single_channel, channel, buffer, buffer_length);
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_mixer_type)) {
        audioio_mixer_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        return audioio_mixer_get_buffer(file, single_channel, channel, buffer, buffer_length);
    }
    return GET_BUFFER_DONE;
}

void audiosample_get_buffer_structure(mp_obj_t sample_obj, bool single_channel,
                                      bool* single_buffer, bool* samples_signed,
                                      uint32_t* max_buffer_length, uint8_t* spacing) {
    if (MP_OBJ_IS_TYPE(sample_obj, &audioio_rawsample_type)) {
        audioio_rawsample_obj_t* sample = MP_OBJ_TO_PTR(sample_obj);
        audioio_rawsample_get_buffer_structure(sample, single_channel, single_buffer,
                                               samples_signed, max_buffer_length, spacing);
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_wavefile_type)) {
        audioio_wavefile_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        audioio_wavefile_get_buffer_structure(file, single_channel, single_buffer, samples_signed,
                                              max_buffer_length, spacing);
    } else if (MP_OBJ_IS_TYPE(sample_obj, &audioio_mixer_type)) {
        audioio_mixer_obj_t* file = MP_OBJ_TO_PTR(sample_obj);
        audioio_mixer_get_buffer_structure(file, single_channel, single_buffer, samples_signed,
                                              max_buffer_length, spacing);
    }
}
