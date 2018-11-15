/*
 * MixerVoice.h
 *
 *  Created on: Nov 15, 2018
 *      Author: dean
 */

#ifndef SHARED_MODULE_AUDIOIO_MIXERVOICE_H_
#define SHARED_MODULE_AUDIOIO_MIXERVOICE_H_

#include "py/obj.h"

#include "shared-module/audioio/__init__.h"

typedef struct {
	mp_obj_base_t base;
    mp_obj_t sample;
    bool loop;
    bool more_data;
    uint32_t* remaining_buffer;
    uint32_t buffer_length;
    int16_t gain;
} audioio_mixervoice_obj_t;


#endif /* SHARED_MODULE_AUDIOIO_MIXERVOICE_H_ */
