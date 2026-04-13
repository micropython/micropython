#pragma once
#include <stdint.h>
#include "py/obj.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

typedef ssize_t (*stream_read_type)(void *, void *, size_t);
typedef off_t (*stream_seek_type)(void *, off_t, int);

/**
 * @brief Structure describing a sound source
 */
typedef struct {
  /*! Initialize the sound source. Returns size of data returned per call of fill_buffer. */
  int (*init_source)(const void *data_start, const void *data_end, int req_sample_rate, void **ctx,
                     int *stereo);
  int (*init_source_stream)(stream_read_type stream_read_fn, mp_obj_t stream, int req_sample_rate,
                           void **ctx, int *stereo, stream_seek_type seek_func);
  /*! Get the actual sample rate at which the source returns data */
  int (*get_sample_rate)(void *ctx);
  /*! Decode a bufferful of data. Returns 0 when file ended or something went wrong. Returns amount
   * of bytes in buffer (normally what init_source returned) otherwise. */
  int (*fill_buffer)(void *ctx, int16_t *buffer, int stereo);
  /*! Reset buffer, loop sample */
  int (*reset_buffer)(void *ctx);
  /*! Destroy source, free resources */
  void (*deinit_source)(void *ctx);
  /*! Set frequency of synthesizer */
  void (*set_frequency)(void *ctx, uint16_t frequency);
  /*! Set waveform of synthesizer */
  void (*set_waveform)(void *ctx, uint8_t waveform);
} sndmixer_source_t;

/**
 * @brief Initialize the sound mixer
 *
 * @note This function internally calls kchal_sound_start, there is no need to do this in your
 * program if you use this function to initialize the sound mixer.
 *
 * @param no_channels Amount if sounds to be able to be played simultaneously.
 */
int sndmixer_init(int no_channels, int stereo);

/**
 * @brief Queue the data of a .wav file to be played
 *
 * This queues a sound to be played. It will not be actually played until sndmixer_play is called.
 *
 * @param wav_start Start of the wav-file data
 * @param wav_end End of the wav-file data
 * @param evictable If true, if all audio channels are filled and a new sound is queued, this
 *                  sound can be stopped to make room for the new sound.
 * @return The ID of the queued sound, for use with the other functions.
 */
int sndmixer_queue_wav(const void *wav_start, const void *wav_end, int evictable);
int sndmixer_queue_wav_stream(stream_read_type read_func, stream_seek_type seek_func, void *stream);

/**
 * @brief Queue the data of a .mod/.xm/.s3m file to be played
 *
 * This queues a piece of tracked music to be played. It will not be actually played until
 * sndmixer_play is called.
 *
 * @param mod_start Start of the filedata
 * @param mod_end End of the filedata
 * @return The ID of the queued sound, for use with the other functions.
 */
int sndmixer_queue_mod(const void *mod_start, const void *mod_end);

/**
 * @brief Queue the data of a .mp3 file to be played
 *
 * This queues a piece of mp3 music to be played. It will not be actually played until sndmixer_play
 * is called.
 *
 * @param mp3_start Start of the filedata
 * @param mp3_end End of the filedata
 * @return The ID of the queued sound, for use with the other functions.
 */
int sndmixer_queue_mp3(const void *mp3_start, const void *mp3_end);
int sndmixer_queue_opus(const void *mp3_start, const void *mp3_end);
int sndmixer_queue_mp3_stream(stream_read_type read_func, stream_seek_type seek_func, void *stream);
int sndmixer_queue_opus_stream(stream_read_type read_func, void *stream);

/**
 * @brief Set or unset a sound to looping mode
 *
 * @param id ID of the sound, obtained when queueing it
 * @param loop If true, the sound will loop back to the beginning when it ends.
 */
void sndmixer_set_loop(int id, int loop);

/**
 * @brief Set volume of a sound
 *
 * A queued sound will always start off with a volume setting of 255 (max volume). This call can be
 * used to adjust the volume of the sound at any time afterwards.
 *
 * @param id ID of the sound, obtained when queueing it
 * @param volume New volume, between 0 (muted) and 255 (full sound).
 */
void sndmixer_set_volume(int id, int volume);

/**
 * @brief Play a sound
 *
 * When a sound is queued, it is not playing yet. Use this call to start playback. You can also
 * use this call to resume a sound paused by sndmixer_pause or sndmixer_pause_all.
 *
 * @param id ID of the sound, obtained when queueing it
 */
void sndmixer_play(int id);

/**
 * @brief Pause a sound
 *
 * Stops playback of the sound. The sound can be resumed with sndmixer_play().
 *
 * @param id ID of the sound, obtained when queueing it
 */
void sndmixer_pause(int id);

/**
 * @brief Stop a sound, free the sound source and channel it used.
 *
 * Stops playback of the sound and frees all associated structures.
 *
 * @param id ID of the sound, obtained when queueing it
 */
void sndmixer_stop(int id);

/**
 * @brief Pause all playing sounds
 *
 * This can be used when e.g. the game is paused. Sounds can be individually un-paused afterwards
 * and new sounds can still be queued and played, given enough free/evictable channels.
 */
void sndmixer_pause_all();

/**
 * @brief Resume all paused sounds
 *
 * This can be used to undo a sndmixer_pause_all() call.
 */
void sndmixer_resume_all();

// Basic synthesizer
int sndmixer_queue_synth();
void sndmixer_freq(int id, uint16_t frequency);
void sndmixer_waveform(int id, uint8_t waveform);

typedef _Bool (*callback_type)(void *, void *);

/**
 * @brief Set a callback function to execute after the sample has finished
 *
 * @param id ID of the sound, obtained when queueing it
 * @param loop If true, the sound will loop back to the beginning when it ends.
 */
void sndmixer_set_callback(int id, callback_type callback, void *handle);

/**
 * @brief Start beat synchroniser, that can start samples at intervals matched to a given beat.
 * @param bpm Beats per minute to configure the beat syncer to
 */
void sndmixer_beat_sync_start(uint8_t bpm);

/**
 * @brief Stop beat synchroniser
 */
void sndmixer_beat_sync_stop();

/**
 *
 * @param id Channel ID
 * @param start_at_next Next interval to play sample at. 1 for the next 1/8th note, 2 for 1/4th, 4 for a 1/2 note,
 *                      8 for the next bar, 16 for the next two bars, or 32 for the next complete set of 4 bars.
 */
void sndmixer_start_at_next(int id, int start_at_next);

#ifdef __cplusplus
}
#endif
