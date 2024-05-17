// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Matthew McGowan for Blues Inc.
//
// SPDX-License-Identifier: MIT

#include <assert.h>
#include <stm32l4xx_hal.h>

#include "MEMS_Audio.h"
#include "MEMS_Audio_ll.h"

static void default_pcm_data_available(MemsAudio *audio, pcm_sample_t *pcmSamples, size_t pcmLength) {
}



/**
 * @brief Initializes the MemsAudio instance. Only one instance can be initialized and used at a given time.
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_init(MemsAudio *audio) {
    if (!audio->pcm_data_available) {
        audio->pcm_data_available = default_pcm_data_available;
    }
    return mems_audio_ll_init(audio);
}

/**
 * @brief Uninitializes the MemsAudio instance.
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_uninit(MemsAudio *audio) {
    return mems_audio_ll_uninit(audio);
}

/**
 * @brief Asynchronously records audio.
 *
 * @param audio
 * @param pdmBuffer
 * @param pdmBufferLength
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_record(MemsAudio *audio) {
    return mems_audio_ll_record(audio);
}

/**
 * @brief Pause recording audio.
 */
mems_audio_err_t mems_audio_pause(MemsAudio *audio) {
    return mems_audio_ll_pause(audio);
}

/**
 * @brief Resume recording audio.
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_resume(MemsAudio *audio) {
    return mems_audio_ll_resume(audio);
}

/**
 * @brief Stop recording audio and
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_stop(MemsAudio *audio) {
    return mems_audio_ll_stop(audio);
}
