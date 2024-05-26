// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Matthew McGowan for Blues Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   How many milliseconds of audio can fit in the audio buffer(s).
 * Interrupts for received data fire at half this duration / twice the frequency.
 */
#ifndef MEMS_AUDIO_MS_BUFFER
#define MEMS_AUDIO_MS_BUFFER (1)
#endif


/**
 * @brief The number of bits per sample of the PCM output
 */
#define PCM_OUT_RESOLUTION                  16

/**
 * @brief The output frequency of PCM samples in Hz.
 */
#define PCM_OUT_SAMPLING_FREQUENCY          16000

/**
 * @brief type for describing error conditions.
 */
typedef int32_t mems_audio_err_t;

/**
 * @brief The datatype that holds an output PCM sample.
 */
typedef int16_t pcm_sample_t;
_Static_assert(PCM_OUT_RESOLUTION == 16, "Output PCM resolution must be 16-bits");


typedef enum {
    MEMS_AUDIO_OK = 0,
    MEMS_AUDIO_ERROR_ALREADY_INITIALIZED = -1,
    MEMS_AUDIO_ERROR_NOT_INITIALIZED = -2
} mems_audio_err_enum_t;

#define IS_MEMS_AUDIO_ERROR(e) (e)
#define CHECK_MEMS_AUDIO_ERROR(e) { if (IS_MEMS_AUDIO_ERROR(e)) return e; }
#define CHECK_MEMS_AUDIO_INITIALIZED(x) { if (!x) return MEMS_AUDIO_ERROR_NOT_INITIALIZED; }

typedef struct MemsAudio_t MemsAudio;

/**
 * @brief Callback informing that PCM samples are available for processing.
 */
typedef void (*pcm_data_available_t)(MemsAudio *audio, pcm_sample_t *pcmSamples, size_t pcmLength);

/**
 * @brief MemsAudio manages the filter, buffers and callbacks used to capture PDM audio samples and convert to PCM.
 *
 */
typedef struct MemsAudio_t {

    /**
     * @brief The buffer to store PCM audio samples
     */
    volatile pcm_sample_t *volatile pcmOutputBuffer;

    /**
     * @brief The length of the PCM buffer. SHould be at least MEMS_AUDIO_PCM_BUFFER_LENGTH
     */
    volatile size_t pcmOutputBufferLength;

    /**
     * @brief Optional callback for when PCM data is available.
     */
    pcm_data_available_t pcm_data_available;

    void *audioImpl;
    void *userData;
} MemsAudio;


mems_audio_err_t mems_audio_init(MemsAudio *audio);

/**
 * @brief Uninitializes the MemsAudio instance.
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_uninit(MemsAudio *audio);

/**
 * @brief Asynchronously records audio.
 *
 * @param audio
 * @param pdmBuffer
 * @param pdmBufferLength
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_record(MemsAudio *audio);

/**
 * @brief Pause recording audio.
 */
mems_audio_err_t mems_audio_pause(MemsAudio *audio);

/**
 * @brief Resume recording audio.
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_resume(MemsAudio *audio);

/**
 * @brief Stop recording audio and
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_stop(MemsAudio *audio);

#ifdef __cplusplus
}
#endif
