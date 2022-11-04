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
