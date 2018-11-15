/*
 * MixerVoice.h
 *
 *  Created on: Nov 15, 2018
 *      Author: dean
 */

#ifndef SHARED_BINDINGS_AUDIOIO_MIXERVOICE_H_
#define SHARED_BINDINGS_AUDIOIO_MIXERVOICE_H_

#include "py/obj.h"

#include "shared-module/audioio/__init__.h"

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/audioio/Mixer.h"
#include "shared-bindings/audioio/RawSample.h"

typedef struct {
    mp_obj_t sample;
    bool loop;
    bool more_data;
    uint32_t* remaining_buffer;
    uint32_t buffer_length;
    int16_t gain;
} audioio_mixervoice_obj_t;

extern const mp_obj_type_t audioio_mixer_type;
extern const mp_obj_type_t audioio_mixervoice_type;

void common_hal_audioio_mixervoice_deinit(audioio_mixervoice_obj_t* self);
void common_hal_audioio_mixervoice_play(audioio_mixervoice_obj_t* self, mp_obj_t sample, bool loop);
void common_hal_audioio_mixervoice_stop(audioio_mixervoice_obj_t* self);
void common_hal_audioio_mixervoice_set_gain(audioio_mixervoice_obj_t* self, float gain);

bool common_hal_audioio_mixervoice_get_playing(audioio_mixervoice_obj_t* self);

#endif /* SHARED_BINDINGS_AUDIOIO_MIXERVOICE_H_ */
