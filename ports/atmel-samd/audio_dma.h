// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "extmod/vfs_fat.h"
#include "py/obj.h"
#include "shared-module/audiocore/RawSample.h"
#include "shared-module/audiocore/WaveFile.h"
#include "supervisor/background_callback.h"

typedef struct {
    mp_obj_t sample;
    uint8_t *buffer[2];
    size_t buffer_length[2];
    DmacDescriptor *descriptor[2];
    DmacDescriptor second_descriptor;
    background_callback_t callback;
    uint8_t dma_channel;
    uint8_t event_channel;
    uint8_t audio_channel;
    uint8_t bytes_per_sample;
    uint8_t beat_size;
    uint8_t spacing;
    uint8_t buffer_to_load; // Index
    bool loop;
    bool single_buffer;
    bool single_channel_output;
    bool signed_to_unsigned;
    bool unsigned_to_signed;
    bool playing_in_progress;
} audio_dma_t;

typedef enum {
    AUDIO_DMA_OK,
    AUDIO_DMA_DMA_BUSY,
    AUDIO_DMA_MEMORY_ERROR,
} audio_dma_result;

uint32_t audiosample_sample_rate(mp_obj_t sample_obj);
uint8_t audiosample_bits_per_sample(mp_obj_t sample_obj);
uint8_t audiosample_channel_count(mp_obj_t sample_obj);

void audio_dma_init(audio_dma_t *dma);
void audio_dma_reset(void);

uint8_t dma_allocate_channel(void);
void dma_free_channel(uint8_t channel);

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
    uint32_t output_register_address,
    uint8_t dma_trigger_source);

void audio_dma_disable_channel(uint8_t channel);
void audio_dma_enable_channel(uint8_t channel);
void audio_dma_stop(audio_dma_t *dma);
bool audio_dma_get_playing(audio_dma_t *dma);
void audio_dma_pause(audio_dma_t *dma);
void audio_dma_resume(audio_dma_t *dma);
bool audio_dma_get_paused(audio_dma_t *dma);

void audio_dma_background(void);

uint8_t find_sync_event_channel_raise(void);

void audio_dma_evsys_handler(void);
