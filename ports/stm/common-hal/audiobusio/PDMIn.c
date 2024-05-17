// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Matthew McGowan for Blues Inc.
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include "common-hal/audiobusio/PDMIn.h"
#include "shared-bindings/audiobusio/PDMIn.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "py/runtime.h"
#include "MEMS_Audio_ll_stm32l4.h"


MemsAudio memsAudio;
MemsAudio_STM32L4SAIPDM memsAudioImpl;
pdm_sample_t pdmBuffer[MEMS_AUDIO_PDM_BUFFER_LENGTH];
audiobusio_pdmin_obj_t *instance;

static bool pdm_data_available(MemsAudio_STM32L4SAIPDM *impl, uint8_t *pdmBuffer, size_t pdmBufferLength);

// Caller validates that pins are free.
void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t *self,
    const mcu_pin_obj_t *clock_pin,
    const mcu_pin_obj_t *data_pin,
    uint32_t sample_rate,
    uint8_t bit_depth,
    bool mono,
    uint8_t oversample) {
    self->sample_rate = sample_rate;
    self->mono = mono;
    self->oversample = oversample;
    self->recording_complete = true;


    if (!mono) {
        mp_raise_ValueError(MP_ERROR_TEXT("only mono is supported"));
    }
    if (sample_rate != 16000) {
        mp_raise_ValueError(MP_ERROR_TEXT("only sample_rate=16000 is supported"));
    }
    if (bit_depth != 16) {
        mp_raise_ValueError(MP_ERROR_TEXT("only bit_depth=16 is supported"));
    }
    if (oversample != 64) {
        mp_raise_ValueError(MP_ERROR_TEXT("only oversample=64 is supported"));
    }

    // wait for the previous instance to finish.
    if (instance) {
        common_hal_audiobusio_pdmin_deinit(instance);
    }
    instance = self;

    memset(&memsAudio, 0, sizeof(memsAudio));
    memset(&memsAudioImpl, 0, sizeof(memsAudioImpl));

    common_hal_mcu_pin_claim(clock_pin);
    self->clock_pin = clock_pin;
    common_hal_mcu_pin_claim(data_pin);
    self->data_pin = data_pin;

    self->audio = &memsAudio;
    self->audio_impl = &memsAudioImpl;
    self->audio_impl->pdmBuffer = pdmBuffer;
    self->audio_impl->pdmBufferLength = sizeof(pdmBuffer) / sizeof(pdmBuffer[0]);
    self->audio_impl->pdm_data_available = pdm_data_available;

    mems_audio_init_stm32l4_sai_pdm(self->audio, self->audio_impl);
    mems_audio_record(self->audio);
    mems_audio_pause(self->audio);
}

bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t *self) {
    return self->clock_pin == NULL;
}

void wait_dma_complete(audiobusio_pdmin_obj_t *self) {
    while (!self->recording_complete) {
        MICROPY_VM_HOOK_LOOP;
    }
}

void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t *self) {
    if (instance != self) {
        return;
    }
    instance = NULL;
    if (self->audio) {
        wait_dma_complete(self);
        mems_audio_stop(self->audio);
        mems_audio_uninit(self->audio);
        self->audio = NULL;
        self->audio_impl = NULL;
    }

    if (self->data_pin) {
        common_hal_reset_pin(self->data_pin);
        self->data_pin = NULL;
    }
    if (self->clock_pin) {
        common_hal_reset_pin(self->clock_pin);
        self->clock_pin = NULL;
    }


}

uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t *self) {
    return 16;
}

uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t *self) {
    return 16000;
}

static bool pdm_data_available(MemsAudio_STM32L4SAIPDM *impl, uint8_t *pdmBuffer, size_t pdmBufferLength) {
    // update the filter with the correct number of samples

    audiobusio_pdmin_obj_t *pdmIn = (audiobusio_pdmin_obj_t *)(impl->audio->userData);
    MemsAudio *audio = impl->audio;

    uint32_t pcmSamplesAvailable = pdmBufferLength * 8 / PDM_IN_DECIMATION_FACTOR;
    if (pcmSamplesAvailable > audio->pcmOutputBufferLength) {
        pcmSamplesAvailable = audio->pcmOutputBufferLength;
    }

    // ensure the filter doesn't try to produce more samples than available
    pdmIn->audio_impl->filter.nSamples = pcmSamplesAvailable;

    return pcmSamplesAvailable > 0;
}

static void pcm_data_available(MemsAudio *audio, int16_t *pcmBuffer, size_t pcmBufferLength) {
    // data is already in the output buffer
    audiobusio_pdmin_obj_t *pdmIn = (audiobusio_pdmin_obj_t *)(audio->userData);

    // if DMA copies more data than will fit into the output buffer, crop the length to what will fit
    if (audio->pcmOutputBufferLength < pcmBufferLength) {
        pcmBufferLength = audio->pcmOutputBufferLength;
    }

    audio->pcmOutputBuffer += pcmBufferLength;
    audio->pcmOutputBufferLength -= pcmBufferLength;
    if (audio->pcmOutputBufferLength == 0) {
        pdmIn->recording_complete = true;
        mems_audio_pause(audio);
    }
}

uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t *self,
    uint16_t *output_buffer, uint32_t output_buffer_length) {

    MemsAudio *audio = self->audio;

    wait_dma_complete(self);

    audio->pcmOutputBuffer = (int16_t *)output_buffer;
    audio->pcmOutputBufferLength = output_buffer_length;
    audio->pcm_data_available = pcm_data_available;
    audio->userData = self;         /// reference back to the PDMIn instance
    self->recording_complete = false;

    mems_audio_err_t err = mems_audio_resume(audio);
    if (!IS_MEMS_AUDIO_ERROR(err)) {
        wait_dma_complete(self);
    }

    mems_audio_pause(audio);
    int samples_output = (int)(output_buffer_length) - audio->pcmOutputBufferLength;

    // convert from signed to unsigned (min-point moves from 0 to 32k)
    for (int i = 0; i < samples_output; i++) {
        output_buffer[i] += 0x8000;
    }

    return samples_output;
}
