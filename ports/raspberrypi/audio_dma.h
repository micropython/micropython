// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "supervisor/background_callback.h"

#include "src/rp2_common/hardware_dma/include/hardware/dma.h"

typedef struct {
    mp_obj_t sample;
    uint8_t *buffer[2];
    size_t buffer_length[2];
    uint32_t channels_to_load_mask;
    uint32_t output_register_address;
    background_callback_t callback;
    uint8_t channel[2];
    uint8_t audio_channel;
    uint8_t output_size;
    uint8_t sample_spacing;
    uint8_t output_resolution; // in bits
    uint8_t sample_resolution; // in bits
    bool loop;
    bool single_channel_output;
    bool signed_to_unsigned;
    bool unsigned_to_signed;
    bool output_signed;
    bool playing_in_progress;
    bool swap_channel;
} audio_dma_t;

typedef enum {
    AUDIO_DMA_OK,
    AUDIO_DMA_DMA_BUSY,
    AUDIO_DMA_MEMORY_ERROR,
} audio_dma_result;


void audio_dma_init(audio_dma_t *dma);
void audio_dma_deinit(audio_dma_t *dma);
void audio_dma_reset(void);

// This sets everything up but doesn't start the timer.
// Sample is the python object for the sample to play.
// loop is true if we should loop the sample.
// single_channel_output is true if we only output a single channel. When false, all channels will be
//   output.
// audio_channel is the index of the channel to dma. single_channel_output must be false in this case.
// output_signed is true if the dma'd data should be signed. False and it will be unsigned.
// output_register_address is the address to copy data to.
// dma_trigger_source is the DMA trigger source which cause another copy
audio_dma_result audio_dma_setup_playback(audio_dma_t *dma,
    mp_obj_t sample,
    bool loop,
    bool single_channel_output,
    uint8_t audio_channel,
    bool output_signed,
    uint8_t output_resolution,
    uint32_t output_register_address,
    uint8_t dma_trigger_source,
    bool swap_channel);

void audio_dma_stop(audio_dma_t *dma);
bool audio_dma_get_playing(audio_dma_t *dma);
void audio_dma_pause(audio_dma_t *dma);
void audio_dma_resume(audio_dma_t *dma);
bool audio_dma_get_paused(audio_dma_t *dma);

uint32_t audio_dma_pause_all(void);
void audio_dma_unpause_mask(uint32_t channel_mask);
