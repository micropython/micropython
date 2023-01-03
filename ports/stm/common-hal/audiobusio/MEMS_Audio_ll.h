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


#ifndef _MEMS_AUDIO_LL_H_
#define _MEMS_AUDIO_LL_H_

#include "MEMS_Audio.h"

#ifdef __cplusplus
extern "C" {
#endif


mems_audio_err_t mems_audio_ll_init(MemsAudio *audio);
mems_audio_err_t mems_audio_ll_uninit(MemsAudio *audio);

/**
 * @brief Asynchronously records audio.
 *
 * @param audio
 * @param pdmBuffer
 * @param pdmBufferLength
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_ll_record(MemsAudio *audio);

/**
 * @brief Pause recording audio.
 */
mems_audio_err_t mems_audio_ll_pause(MemsAudio *audio);

/**
 * @brief Resume recording audio.
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_ll_resume(MemsAudio *audio);

/**
 * @brief Stop recording audio and
 *
 * @param audio
 * @return mems_audio_err_t
 */
mems_audio_err_t mems_audio_ll_stop(MemsAudio *audio);

#ifdef __cplusplus
}
#endif


#endif // _MEMS_AUDIO_LL_H_
