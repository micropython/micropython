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

void common_hal_audioio_mixervoice_set_gain(audioio_mixervoice_obj_t* self, float gain) {
	self->gain = gain * ((1 << 15)-1);
}
