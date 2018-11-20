/*
 * MixerVoice.c
 *
 *  Created on: Nov 15, 2018
 *      Author: dean
 */

#include "shared-bindings/audioio/Mixer.h"

#include <stdint.h>

#include "py/runtime.h"
#include "shared-module/audioio/__init__.h"
#include "shared-module/audioio/RawSample.h"
#include "shared-module/audioio/MixerVoice.h"

void common_hal_audioio_mixervoice_set_parent(audioio_mixervoice_obj_t* self, audioio_mixer_obj_t *parent) {
	self->parent = parent;
}

void common_hal_audioio_mixervoice_set_gain(audioio_mixervoice_obj_t* self, float gain) {
	self->gain = gain * ((1 << 15)-1);
}

void common_hal_audioio_mixervoice_play(audioio_mixervoice_obj_t* self, mp_obj_t sample, bool loop) {
    if (audiosample_sample_rate(sample) != self->parent->sample_rate) {
        mp_raise_ValueError(translate("The sample's sample rate does not match the mixer's"));
    }
    if (audiosample_channel_count(sample) != self->parent->channel_count) {
        mp_raise_ValueError(translate("The sample's channel count does not match the mixer's"));
    }
    if (audiosample_bits_per_sample(sample) != self->parent->bits_per_sample) {
        mp_raise_ValueError(translate("The sample's bits_per_sample does not match the mixer's"));
    }
    bool single_buffer;
    bool samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed,
                                     &max_buffer_length, &spacing);
    if (samples_signed != self->parent->samples_signed) {
        mp_raise_ValueError(translate("The sample's signedness does not match the mixer's"));
    }
    self->sample = sample;
    self->loop = loop;

    audiosample_reset_buffer(sample, false, 0);
    audioio_get_buffer_result_t result = audiosample_get_buffer(sample, false, 0, (uint8_t**) &self->remaining_buffer, &self->buffer_length);
    // Track length in terms of words.
    self->buffer_length /= sizeof(uint32_t);
    self->more_data = result == GET_BUFFER_MORE_DATA;
}

bool common_hal_audioio_mixervoice_get_playing(audioio_mixervoice_obj_t* self) {
	return self->sample != NULL;
}

void common_hal_audioio_mixervoice_stop(audioio_mixervoice_obj_t* self) {
    self->sample = NULL;
}
