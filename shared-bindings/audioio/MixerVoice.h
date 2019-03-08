/*
 * MixerVoice.h
 *
 *  Created on: Nov 15, 2018
 *      Author: dean
 */

#ifndef SHARED_BINDINGS_AUDIOIO_MIXERVOICE_H_
#define SHARED_BINDINGS_AUDIOIO_MIXERVOICE_H_

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/audioio/Mixer.h"
#include "shared-bindings/audioio/RawSample.h"
#include "shared-module/audioio/MixerVoice.h"
#include "shared-module/audioio/Mixer.h"

extern const mp_obj_type_t audioio_mixer_type;
extern const mp_obj_type_t audioio_mixervoice_type;

void common_hal_audioio_mixervoice_deinit(audioio_mixervoice_obj_t* self);
void common_hal_audioio_mixervoice_set_parent(audioio_mixervoice_obj_t* self, audioio_mixer_obj_t *parent);
void common_hal_audioio_mixervoice_play(audioio_mixervoice_obj_t* self, mp_obj_t sample, bool loop);
void common_hal_audioio_mixervoice_stop(audioio_mixervoice_obj_t* self);
void common_hal_audioio_mixervoice_set_gain(audioio_mixervoice_obj_t* self, float gain);

bool common_hal_audioio_mixervoice_get_playing(audioio_mixervoice_obj_t* self);

#endif /* SHARED_BINDINGS_AUDIOIO_MIXERVOICE_H_ */
