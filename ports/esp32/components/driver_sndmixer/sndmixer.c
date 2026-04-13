#include <sdkconfig.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"
#include "esp_log.h"

#include "driver_i2s.h"

#include "snd_source_wav.h"
#include "snd_source_mod.h"
#include "snd_source_mp3.h"
#include "snd_source_opus.h"
#include "snd_source_synth.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#define TAG "Sndmixer"

#define CHFL_EVICTABLE (1 << 0)
#define CHFL_PAUSED    (1 << 1)
#define CHFL_LOOP      (1 << 2)
#define CHFL_STEREO    (1 << 3)

#define SYNC_NOTE_DIVISOR 8 // Beat synchroniser keeps counts in 1/8 (eighth) notes
#define SYNC_COUNT_BARS 4   // Beat synchroniser counts up to 4 bars (i.e. 4 whole notes, or 32 eighth notes)

typedef enum {
  CMD_QUEUE_WAV = 1,
  CMD_QUEUE_WAV_STREAM,
  CMD_QUEUE_MOD,
  CMD_QUEUE_MP3,
  CMD_QUEUE_MP3_STREAM,
  CMD_QUEUE_OPUS,
  CMD_QUEUE_OPUS_STREAM,
  CMD_QUEUE_SYNTH,
  CMD_LOOP,
  CMD_VOLUME,
  CMD_PLAY,
  CMD_PAUSE,
  CMD_STOP,
  CMD_PAUSE_ALL,
  CMD_RESUME_ALL,
  CMD_FREQ,
  CMD_WAVEFORM,
  CMD_CALLBACK,
  CMD_BEAT_SYNC_START,
  CMD_BEAT_SYNC_STOP,
  CMD_START_AT_NEXT
} sndmixer_cmd_ins_t;

typedef struct {
  sndmixer_cmd_ins_t cmd;
  int id;
  union {
    struct {
      const void *queue_file_start;
      const void *queue_file_end;
      const void *read_func;
            void *stream;
      const void *seek_func;
      const void *callback_func;
      const void *callback_handle;
      int flags;
    };
    struct {
      int param;
    };
  };
} sndmixer_cmd_t;

typedef struct {
  int id;
  const sndmixer_source_t *source;  // or NULL if channel unused
  void *src_ctx;
  int volume;  // 0-256
  uint8_t flags;
  int16_t *buffer;
  int chunksz;
  const void *callback_func;
  const void *callback_handle;
  uint32_t dds_rate;  // Rate; 16.16 fixed
  uint32_t dds_acc;   // DDS accumulator, 16.16 fixed

  // For beat synched playback, the interval of (1/SYNC_NOTE_DIVISOR) (e.g. 1/8th) notes to synchronise to.
  // 1 for starting at a 1/8th note in the default config, 2 for 1/4th, 4 for 1/2, 8 for 1 whole bar, 16 for 2 bars, 32 for 4 bars.
  int8_t start_at_next;
} sndmixer_channel_t;

static sndmixer_channel_t *channel;
static int no_channels;
static int samplerate;
static volatile uint32_t curr_id = 0;
static QueueHandle_t cmd_queue;
static int use_stereo = 0;
static uint8_t beat_sync_count = 0; // At which (1/SYNC_NOTE_DIVISOR)'th note we are currently
static bool beat_sync_enabled = false;
static TickType_t beat_sync_last_tick = 0; // When beat_sync_count was last increased
static uint8_t beat_sync_bpm = 120; // Preconfigured BPM. Can be configured with sndmixer API

// Grabs a new ID by atomically increasing curr_id and returning its value. This is called outside
// of the audio playing thread, hence the atomicity.
static uint32_t new_id(void) {
    uint32_t old_id = __atomic_load_n(&curr_id, __ATOMIC_SEQ_CST);
    uint32_t new_id;
    while (1) {
        new_id = old_id + 1;
        if (__atomic_compare_exchange_n(&curr_id, &old_id, new_id,
                                        false,
                                        __ATOMIC_SEQ_CST,
                                        __ATOMIC_SEQ_CST)) {
            return new_id;
        }
        // on failure, old_id is updated; loop and try again
    }
}

static void clean_up_channel(sndmixer_channel_t *chan) {
  if(chan->callback_handle) {
    // exec callback
    callback_type do_callback = chan->callback_func;
    do_callback(chan->callback_handle,0);
  }

  if (chan->source) {
    chan->source->deinit_source(chan->src_ctx);
    chan->source = NULL;
  }
  free(chan->buffer);
  chan->buffer = NULL;
  chan->flags  = 0;
  ESP_LOGI(TAG, "Sndmixer: %d: cleaning up done", chan->id);
  chan->id = 0;
}

static int find_free_channel() {
  for (int x = 0; x < no_channels; x++) {
    if (channel[x].source == NULL)
      return x;
  }
  // No free channels. Maybe one is evictable?
  for (int x = 0; x < no_channels; x++) {
    if (channel[x].flags & CHFL_EVICTABLE) {
      clean_up_channel(channel + x);
      return x;
    }
  }
  return -1;  // nothing found :/
}

static int init_source(int* chan_id, const sndmixer_source_t *srcfns, const void *data_start,
                       const void *data_end) {
  int ch = find_free_channel();
  *chan_id = ch;
  if (ch < 0) {
    return 0;  // no free channels
  }
  ESP_LOGI(TAG, "Sndmixer: %d: initialising source\n", ch);
  int stereo = 0;
  int chunksz =
      srcfns->init_source(data_start, data_end, samplerate, &channel[ch].src_ctx, &stereo);
  if (chunksz <= 0)
    return 0;  // failed
  channel[ch].source = srcfns;
  channel[ch].volume = 255;
  channel[ch].buffer = malloc(chunksz * sizeof(channel[ch].buffer[0]) * ((stereo && use_stereo) ? 2 : 1));
  if (!channel[ch].buffer) {
    clean_up_channel(channel + ch);
    return 0;
  }
  channel[ch].chunksz  = chunksz;
  int64_t real_rate    = srcfns->get_sample_rate(channel[ch].src_ctx);
  channel[ch].dds_rate = (real_rate << 16) / samplerate;
  channel[ch].dds_acc  = chunksz << 16;  // to force the main thread to get new data
  channel[ch].flags    = 0;
  if (stereo && use_stereo) {
    ESP_LOGI(TAG, "Starting stereo channel");
    channel[ch].flags |= CHFL_STEREO;
  }
  return 1;
}

static int init_source_stream(int* chan_id, const sndmixer_source_t *srcfns, const stream_read_type read_fn, void *stream, stream_seek_type seek_func) {
  int ch = find_free_channel();
  *chan_id = ch;
  if (ch < 0) {
    return 0;  // no free channels
  }
  ESP_LOGI(TAG, "Sndmixer: %d: initialising source\n", ch);
  int stereo = 0;
  int chunksz =
      srcfns->init_source_stream(read_fn, stream, samplerate, &channel[ch].src_ctx, &stereo, seek_func);
  if (chunksz <= 0)
    return 0;  // failed
  channel[ch].source = srcfns;
  channel[ch].volume = 255;
  int msize = chunksz * sizeof(channel[ch].buffer[0]) * ((stereo && use_stereo) ? 2 : 1);
  channel[ch].buffer = malloc(msize);
  ESP_LOGI(TAG, "init_source_stream: chunksz %d, msize %d, stereo: %d, use_stereo: %d, buffer = %p", chunksz, msize, stereo, use_stereo, channel[ch].buffer);
  if (!channel[ch].buffer) {
    clean_up_channel(channel + ch);
    return 0;
  }
  channel[ch].chunksz  = chunksz;
  int64_t real_rate    = srcfns->get_sample_rate(channel[ch].src_ctx);
  channel[ch].dds_rate = (real_rate << 16) / samplerate;
  channel[ch].dds_acc  = chunksz << 16;  // to force the main thread to get new data
  channel[ch].flags    = 0;
  if (stereo && use_stereo) {
    ESP_LOGI(TAG, "Starting stereo channel");
    channel[ch].flags |= CHFL_STEREO;
  }
  return 1;
}

static void handle_cmd(sndmixer_cmd_t *cmd) {
  bool cmd_found = true;
  int chan_id;
  int cmd_success = 1;

  // Global initialisation commands that are not bound to a single channel
  switch(cmd->cmd) {
    case CMD_QUEUE_WAV:
      cmd_success = init_source(&chan_id, &sndmixer_source_wav, cmd->queue_file_start, cmd->queue_file_end);
      break;
    case CMD_QUEUE_WAV_STREAM:
      cmd_success = init_source_stream(&chan_id, &sndmixer_source_wav_stream, cmd->read_func, cmd->stream, cmd->seek_func);
      break;
    case CMD_QUEUE_MOD:
      cmd_success = init_source(&chan_id, &sndmixer_source_mod, cmd->queue_file_start, cmd->queue_file_end);
      break;
    case CMD_QUEUE_MP3:
      cmd_success = init_source(&chan_id, &sndmixer_source_mp3, cmd->queue_file_start, cmd->queue_file_end);
      break;
    case CMD_QUEUE_MP3_STREAM:
      cmd_success = init_source_stream(&chan_id, &sndmixer_source_mp3_stream, cmd->read_func, cmd->stream, cmd->seek_func);
      break;
    case CMD_QUEUE_OPUS:
      cmd_success = init_source(&chan_id, &sndmixer_source_opus, cmd->queue_file_start, cmd->queue_file_end);
      break;
    case CMD_QUEUE_OPUS_STREAM:
      cmd_success = init_source_stream(&chan_id, &sndmixer_source_opus_stream, cmd->read_func, cmd->stream, cmd->seek_func);
      break;
    case CMD_QUEUE_SYNTH:
      cmd_success = init_source(&chan_id, &sndmixer_source_synth, 0, 0);
      break;
    default:
      cmd_found = false;
      break;
  }

  if(cmd_found){
    if(cmd_success){
      channel[chan_id].id = cmd->id;  // success; set ID
      channel[chan_id].flags |= cmd->flags;
    } else {
      if(chan_id < 0){
        ESP_LOGE(TAG, "No more available channels");
      } else {
        ESP_LOGE(TAG, "Failed to initialise source");
      }
    }
    return;
  }

  // Other global commands that are not bound to a single channel
  cmd_found = true;
  switch(cmd->cmd) {
    case CMD_BEAT_SYNC_START:
      beat_sync_enabled = true;
      beat_sync_bpm = (uint8_t) cmd->param;
      break;
    case CMD_BEAT_SYNC_STOP:
      beat_sync_enabled = false;
      break;
    case CMD_PAUSE_ALL:
      for (int x = 0; x < no_channels; x++) {
        channel[x].flags |= CHFL_PAUSED;
      }
      break;
    case CMD_RESUME_ALL:
      for (int x = 0; x < no_channels; x++) {
        channel[x].flags &= ~CHFL_PAUSED;
      }
      break;
    default:
      cmd_found = false;
      break;
  }

  if(cmd_found){ return; }

  // The rest are all commands that act on a certain channel ID. Look up if we have a channel with that ID first.
  chan_id = -1;
  for (int i = 0; i < no_channels; i++) {
    if (channel[i].id == cmd->id) {
      chan_id = i;
      break;
    }
  }
  if (chan_id == -1) {
    ESP_LOGW(TAG, "Channel id %d not found, command not executed.", cmd->id);
    return;  // not playing/queued; can't do any of the following commands.
  }

  // Channel-specific commands
  switch(cmd->cmd) {
    case CMD_LOOP:
      if (cmd->param) {
        channel[chan_id].flags |= CHFL_LOOP;
      } else {
        channel[chan_id].flags &= ~CHFL_LOOP;
      }
      break;
    case CMD_VOLUME:
      channel[chan_id].volume = cmd->param;
      break;
    case CMD_PLAY:
      channel[chan_id].flags &= ~CHFL_PAUSED;
      break;
    case CMD_PAUSE:
      channel[chan_id].flags |= CHFL_PAUSED;
      break;
    case CMD_STOP:
      ESP_LOGI(TAG, "%d: cleaning up source due to external stop request", cmd->id);
      clean_up_channel(channel + chan_id);
      break;
    case CMD_FREQ:
      if (channel[chan_id].source->set_frequency) {
        channel[chan_id].source->set_frequency(channel[chan_id].src_ctx, cmd->param);
      } else {
        ESP_LOGE(TAG, "Not a synth channel!");
      }
      break;
    case CMD_WAVEFORM:
      if (channel[chan_id].source->set_waveform) {
        channel[chan_id].source->set_waveform(channel[chan_id].src_ctx, cmd->param);
      } else {
        ESP_LOGE(TAG, "Not a synth channel!");
      }
      break;
    case CMD_CALLBACK:
      channel[chan_id].callback_handle = cmd->callback_handle;
      channel[chan_id].callback_func =  cmd->callback_func;
      break;
    case CMD_START_AT_NEXT:
      channel[chan_id].start_at_next = cmd->param;
      break;
    default:
      break;
  }
}

#define CHUNK_SIZE 32

// Sound mixer main loop.
static void sndmixer_task(void *arg) {
  int16_t mixbuf[CHUNK_SIZE * (use_stereo ? 2 : 1)];
  ESP_LOGI(TAG, "Sndmixer task up.\n");

  TickType_t current_ticks;
  uint32_t ticks_per_subnote = (uint32_t)(1000.0 / portTICK_PERIOD_MS) / ((beat_sync_bpm / 60.0) * (SYNC_NOTE_DIVISOR/4));
  while (1) {

    // Keep track of tempo if beat sync is enabled
    if(beat_sync_enabled) {
      current_ticks = xTaskGetTickCount();
      if (current_ticks >= beat_sync_last_tick + ticks_per_subnote) {
        beat_sync_count = (beat_sync_count + 1) % (SYNC_COUNT_BARS * SYNC_NOTE_DIVISOR);
        beat_sync_last_tick = current_ticks;
        if (beat_sync_count % 2 == 0) {
          ESP_LOGV(TAG, "beat %d", (beat_sync_count / 2) % 4);
        }
      }
    }

    // Handle any commands that are sent to us.
    sndmixer_cmd_t cmd;
    while (xQueueReceive(cmd_queue, &cmd, 0) == pdTRUE) {
      handle_cmd(&cmd);
    }

    // Assemble CHUNK_SIZE worth of samples and dump it into the I2S subsystem.
    for (int i = 0; i < CHUNK_SIZE; i++) {
      uint8_t active_channels = 0;

      // current sample value, multiplied by 255 (because of multiplies by channel volume)
      int32_t s[2] = {0, 0};
      for (int ch = 0; ch < no_channels; ch++) {
        sndmixer_channel_t *chan = &channel[ch];

        // If the channel is paused, and is set to start at an interval we are currently in, unpause it
        if(chan->start_at_next > 0 && (chan->flags & CHFL_PAUSED) && beat_sync_count % chan->start_at_next == 0) {
            ESP_LOGI(TAG, "Starting at subnote %d", beat_sync_count);
            chan->flags &= ~CHFL_PAUSED;
        }

        if (chan->source && !(chan->flags & CHFL_PAUSED)) {
          // Channel is active.
          active_channels++;
          chan->dds_acc += chan->dds_rate;  // select next sample
          // dds_acc>>16 now gives us which sample to get from the buffer.
          while ((chan->dds_acc >> 16) >= chan->chunksz && chan->source) {
            // That value is outside the channels chunk buffer. Refill that first.
            int r = chan->source->fill_buffer(chan->src_ctx, chan->buffer, use_stereo);
            if (r == 0) {
              // if loop is enabled, reset buffer position to start when no new samples are available
              if (chan->flags & CHFL_LOOP) {
                ESP_LOGI(TAG, "Looping sample");
                if (chan->source->reset_buffer(chan->src_ctx) < 0) {
                  ESP_LOGE(TAG, "%d: cleaning up source, loop failed", chan->id);
                  clean_up_channel(chan);
                  break;
                } else {
                  r = chan->source->fill_buffer(chan->src_ctx, chan->buffer, use_stereo);
                }
              } else {
                // Source is done and no loops are requested
                ESP_LOGI(TAG, "%d: cleaning up source because of EOF", chan->id);
                clean_up_channel(chan);
                break;
              }
              continue;
            }
			if (chan->source) {
            int64_t real_rate = chan->source->get_sample_rate(chan->src_ctx);
            chan->dds_rate    = (real_rate << 16) / samplerate;
            chan->dds_acc -=
                (chan->chunksz << 16);  // reset dds acc; we have parsed chunksize samples.
            chan->chunksz = r;          // save new chunksize
			} else { ESP_LOGE(TAG, "chan->source is NULL"); }
          }
          if (!chan->source) {
            continue;
          }
          // Multiply by volume, add to cumulative sample.
          uint32_t acc = chan->dds_acc >> 16;
          if (chan->flags & CHFL_STEREO) {
            s[0] += (int32_t)(chan->buffer[acc * 2 + 0]) * chan->volume;
            s[1] += (int32_t)(chan->buffer[acc * 2 + 1]) * chan->volume;
          } else {
            s[0] += (int32_t)(chan->buffer[acc]) * chan->volume;
            s[1] += (int32_t)(chan->buffer[acc]) * chan->volume;
          }
        }
      }

      /***
       * Divide by the max volume of a channel to return to INT16 ranges.
       * Note that before, we divided by the number of active channels here as well,
       * seemingly to prevent clipping. However, channels are mixed additively in music.
       * Dividing by active channels will audibly lower the volume when new channels are started
       * whilst others are playing. Adding a few channels together will not cause clipping for
       * most normal samples, and sound natural. For scenarios where clipping could occur, such as
       * multiple synthesizers at full volume, lower the channel volumes from the app.
       */
      s[0] /= 255;
      s[1] /= 255;

      // Saturate
#define SAT(x, min, max) ((x > max) ? max : (x < min) ? min : x)
      s[0] = SAT(s[0], INT16_MIN, INT16_MAX);
      s[1] = SAT(s[1], INT16_MIN, INT16_MAX);

      if (use_stereo) {
        mixbuf[i * 2 + 0] = s[0];
        mixbuf[i * 2 + 1] = s[1];
      } else {
        mixbuf[i] = (s[0] + s[1]) / 2;
      }
    }
    driver_i2s_sound_push(mixbuf, CHUNK_SIZE, use_stereo);
  }
  // ToDo: de-init channels/buffers/... if we ever implement a deinit cmd
  vTaskDelete(NULL);
}

// Run on core 1 if enabled, core 0 if not.
#define MY_CORE (portNUM_PROCESSORS - 1)

int sndmixer_init(int p_no_channels, int stereo) {
  no_channels = p_no_channels;
  samplerate  = CONFIG_DRIVER_SNDMIXER_SAMPLE_RATE;
  driver_i2s_sound_start();
  channel    = calloc(sizeof(sndmixer_channel_t), no_channels);
  use_stereo = stereo;
  if (!channel)
    return 0;
  curr_id   = 0;
  cmd_queue = xQueueCreate(10, sizeof(sndmixer_cmd_t));
  if (cmd_queue == NULL) {
    free(channel);
    return 0;
  }
  int r = xTaskCreatePinnedToCore(&sndmixer_task, "sndmixer", 5 << 10, NULL, 5, NULL, MY_CORE);
  if (!r) {
    free(channel);
    vQueueDelete(cmd_queue);
    return 0;
  }
  return 1;
}

// The following functions all are essentially wrappers for the axt of pushing a command into the
// command queue.

int sndmixer_queue_wav(const void *wav_start, const void *wav_end, int evictable) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_QUEUE_WAV,
                        .queue_file_start = wav_start,
                        .queue_file_end   = wav_end,
                        .flags            = CHFL_PAUSED | (evictable ? CHFL_EVICTABLE : 0)};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_wav_stream(stream_read_type read_func, stream_seek_type seek_func, void *stream) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
    .cmd              = CMD_QUEUE_WAV_STREAM,
    .read_func        = (void *)read_func,
    .seek_func        = (void *)seek_func,
    .stream           = stream,
    .flags            = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_mod(const void *mod_start, const void *mod_end) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_QUEUE_MOD,
                        .queue_file_start = mod_start,
                        .queue_file_end   = mod_end,
                        .flags            = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_mp3(const void *mp3_start, const void *mp3_end) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_QUEUE_MP3,
                        .queue_file_start = mp3_start,
                        .queue_file_end   = mp3_end,
                        .flags            = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_mp3_stream(stream_read_type read_func, stream_seek_type seek_func, void *stream) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_QUEUE_MP3_STREAM,
                        .read_func        = (void *)read_func,
                        .seek_func        = (void *)seek_func,
                        .stream           = stream,
                        .flags            = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_opus(const void *opus_start, const void *opus_end) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_QUEUE_OPUS,
                        .queue_file_start = opus_start,
                        .queue_file_end   = opus_end,
                        .flags            = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_opus_stream(stream_read_type read_func, void *stream) {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_QUEUE_OPUS_STREAM,
                        .read_func        = (void *)read_func,
                        .stream           = stream,
                        .flags            = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

int sndmixer_queue_synth() {
  int id             = new_id();
  sndmixer_cmd_t cmd = {.id = id, .cmd = CMD_QUEUE_SYNTH, .flags = CHFL_PAUSED};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
  return id;
}

void sndmixer_set_loop(int id, int do_loop) {
  sndmixer_cmd_t cmd = {.cmd = CMD_LOOP, .id = id, .param = do_loop};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_set_volume(int id, int volume) {
  sndmixer_cmd_t cmd = {.cmd = CMD_VOLUME, .id = id, .param = volume};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_play(int id) {
  sndmixer_cmd_t cmd = {
      .cmd = CMD_PLAY,
      .id  = id,
  };
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_pause(int id) {
  sndmixer_cmd_t cmd = {
      .cmd = CMD_PAUSE,
      .id  = id,
  };
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_stop(int id) {
  sndmixer_cmd_t cmd = {
      .cmd = CMD_STOP,
      .id  = id,
  };
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_pause_all() {
  sndmixer_cmd_t cmd = {
      .cmd = CMD_PAUSE_ALL,
  };
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_resume_all() {
  sndmixer_cmd_t cmd = {
      .cmd = CMD_RESUME_ALL,
  };
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_freq(int id, uint16_t frequency) {
  sndmixer_cmd_t cmd = {.cmd = CMD_FREQ, .id = id, .param = frequency};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_waveform(int id, uint8_t waveform) {
  sndmixer_cmd_t cmd = {.cmd = CMD_WAVEFORM, .id = id, .param = waveform};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_set_callback(int id, callback_type callback, void *handle) {
  sndmixer_cmd_t cmd = {.id               = id,
                        .cmd              = CMD_CALLBACK,
                        .callback_func    = (void *)callback,
                        .callback_handle  = handle};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);

}

void sndmixer_beat_sync_start(uint8_t bpm) {
  sndmixer_cmd_t cmd = {.cmd  = CMD_BEAT_SYNC_START};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_beat_sync_stop() {
  sndmixer_cmd_t cmd = {.cmd  = CMD_BEAT_SYNC_STOP};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

void sndmixer_start_at_next(int id, int start_at_next) {
  sndmixer_cmd_t cmd = {.id     = id,
                        .cmd    = CMD_START_AT_NEXT,
                        .param  = start_at_next};
  xQueueSend(cmd_queue, &cmd, portMAX_DELAY);
}

#endif
