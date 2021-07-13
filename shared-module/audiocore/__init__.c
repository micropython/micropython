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
#include "shared-bindings/audiocore/RawSample.h"
#include "shared-bindings/audiocore/WaveFile.h"
#include "shared-module/audiocore/RawSample.h"
#include "shared-module/audiocore/WaveFile.h"

#include "shared-bindings/audiomixer/Mixer.h"
#include "shared-module/audiomixer/Mixer.h"

uint32_t audiosample_sample_rate(mp_obj_t sample_obj) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    return proto->sample_rate(MP_OBJ_TO_PTR(sample_obj));
}

uint8_t audiosample_bits_per_sample(mp_obj_t sample_obj) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    return proto->bits_per_sample(MP_OBJ_TO_PTR(sample_obj));
}

uint8_t audiosample_channel_count(mp_obj_t sample_obj) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    return proto->channel_count(MP_OBJ_TO_PTR(sample_obj));
}

void audiosample_reset_buffer(mp_obj_t sample_obj, bool single_channel_output, uint8_t audio_channel) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    proto->reset_buffer(MP_OBJ_TO_PTR(sample_obj), single_channel_output, audio_channel);
}

audioio_get_buffer_result_t audiosample_get_buffer(mp_obj_t sample_obj,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **buffer, uint32_t *buffer_length) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    return proto->get_buffer(MP_OBJ_TO_PTR(sample_obj), single_channel_output, channel, buffer, buffer_length);
}

void audiosample_get_buffer_structure(mp_obj_t sample_obj, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {
    const audiosample_p_t *proto = mp_proto_get_or_throw(MP_QSTR_protocol_audiosample, sample_obj);
    proto->get_buffer_structure(MP_OBJ_TO_PTR(sample_obj), single_channel_output, single_buffer,
        samples_signed, max_buffer_length, spacing);
}

void audiosample_convert_u8m_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = (*buffer_in++ - 0x80) << 8;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}


void audiosample_convert_u8s_s16s(int16_t *buffer_out, const uint8_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        int16_t sample = (*buffer_in++ - 0x80) << 8;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s8m_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = (*buffer_in++) << 8;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}


void audiosample_convert_s8s_s16s(int16_t *buffer_out, const int8_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        int16_t sample = (*buffer_in++) << 8;
        *buffer_out++ = sample;
    }
}


void audiosample_convert_u16m_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = *buffer_in++ - 0x8000;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}


void audiosample_convert_u16s_s16s(int16_t *buffer_out, const uint16_t *buffer_in, size_t nframes) {
    size_t nsamples = 2 * nframes;
    for (; nsamples--;) {
        int16_t sample = *buffer_in++ - 0x8000;
        *buffer_out++ = sample;
    }
}

void audiosample_convert_s16m_s16s(int16_t *buffer_out, const int16_t *buffer_in, size_t nframes) {
    for (; nframes--;) {
        int16_t sample = *buffer_in++;
        *buffer_out++ = sample;
        *buffer_out++ = sample;
    }
}
