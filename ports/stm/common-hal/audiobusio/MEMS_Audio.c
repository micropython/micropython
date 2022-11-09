/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Matthew McGowan for Blues Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
