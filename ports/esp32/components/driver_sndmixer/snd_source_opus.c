#include <sdkconfig.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <esp_log.h>
#include <esp_attr.h>
#include <esp_heap_caps.h>

#include "sndmixer.h"
#include "opus.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#define INPUT_BUFFER_SIZE  (2 * 1024)
#define OUTPUT_BUFFER_SIZE (4 * 1024)
#define INTERNAL_BUFFER_FETCH_WHEN \
  1024  // new data will be fetched when there is less than this amount of data
#define CHUNK_SIZE 128

#define TAG "snd_source_opus"

typedef struct {
  OpusDecoder *decoder;
  uint16_t channels;
  uint16_t rate;

  unsigned char *input_start, *input_curr, *input_end;
  int16_t *output_start, *output_curr, *output_end;

  stream_read_type stream_read;
  void *stream;  // Pointer to stream
} opus_ctx_t;

static void opus_deinit_source(void *ctx);

static inline void read_data(opus_ctx_t *ctx) {
  // Fetch data for internal buffer
  int left        = ctx->input_curr - ctx->input_start;
  int buffer_free = INPUT_BUFFER_SIZE - (ctx->input_end - ctx->input_curr);
  int fetched     = 0;

  if (left < INTERNAL_BUFFER_FETCH_WHEN) {
    // 1) Get rid of old data
    if (ctx->input_curr != ctx->input_start) {
      // Move available data to the begin of the buffer
      memmove(ctx->input_start, ctx->input_curr, left);
      ctx->input_curr = ctx->input_start;
      ctx->input_end  = ctx->input_start + left;
    }

    fetched = ctx->stream_read(ctx->stream, ctx->input_end, buffer_free);
    ESP_LOGD(TAG, "fetched %d bytes\r\n", fetched);
    ctx->input_end += fetched;  // Our buffer now (hopefully) contains more data
  }
}

static int IRAM_ATTR ctx_decode(void *_ctx) {
  opus_ctx_t *ctx = (opus_ctx_t *)_ctx;

  if (ctx->stream)
    read_data(ctx);

  int available = ctx->input_end - ctx->input_curr;

  if (available < 4) {
    return 0;
  }
  ESP_LOGD(TAG, "%d bytes available\r\n", available);

  int channels     = ctx->input_curr[0];
  int frequency    = 400 * ctx->input_curr[1];
  int bytes_needed = *(uint16_t *)(ctx->input_curr + 2);

  ESP_LOGD(TAG, "ch:%d, freq:%d, bytes:%d\r\n", channels, frequency, bytes_needed);

  if (bytes_needed > available - 4) {
    return 0;
  }

  if (channels != ctx->channels || frequency != ctx->rate) {
    ESP_LOGD(TAG, "reinitializing\r\n");
    opus_decoder_init(ctx->decoder, frequency, channels);
    ctx->channels = channels;
    ctx->rate     = frequency;
  }

  int ret = opus_decode(ctx->decoder, ctx->input_curr + 4, bytes_needed, ctx->output_start,
                        (OUTPUT_BUFFER_SIZE) / ctx->channels / sizeof(int16_t), 0);

  if (ret < 0) {
    ESP_LOGE(TAG, "opus_decode returned %d", ret);
    return 0;
  }
  ctx->input_curr += 4 + bytes_needed;
  ctx->output_end  = ctx->output_start + ret * ctx->channels;
  ctx->output_curr = ctx->output_start;
  ESP_LOGD(TAG, "decoded %d samples\r\n", ret);
  return 1;
}

int IRAM_ATTR opus_init_source(const void *data_start, const void *data_end, int req_sample_rate, void **_ctx,
                     int *stereo) {
  // Allocate space for the information struct
  opus_ctx_t *ctx = calloc(sizeof(opus_ctx_t), 1);
  if (!ctx)
    goto err;

  int err;
  ctx->decoder = opus_decoder_create(8000, 2, &err);
  if (err != OPUS_OK) {
    ESP_LOGE(TAG, "failed to create opus decode, returned %d", err);
    goto err;
  }

  ctx->input_start  = (unsigned char *)data_start;  // Start of data
  ctx->input_curr   = (unsigned char *)data_start;  // Current position
  ctx->input_end    = (unsigned char *)data_end;    // End of data
  ctx->rate         = 0;
  ctx->channels     = 0;
  ctx->output_start = calloc(OUTPUT_BUFFER_SIZE, sizeof(int16_t));
  ctx->output_end   = ctx->output_start;
  ctx->output_curr  = ctx->output_start;
  ctx->stream_read  = NULL;
  ctx->stream       = NULL;

  uint32_t length = data_end - data_start;

  ESP_LOGD(TAG, "Started opus source, %d bytes of data at %p", length, ctx->input_start);

  if (!ctx_decode(ctx)) {
    goto err;
  }
  *stereo = (ctx->channels == 2);

  *_ctx = (void *)ctx;
  return CHUNK_SIZE;  // Chunk size

err:
  opus_deinit_source(ctx);
  return -1;
}

int IRAM_ATTR opus_init_source_stream(stream_read_type stream_read_fn, void *stream, int req_sample_rate,
                           void **_ctx, int *stereo, stream_seek_type seek_func) {
  // Allocate space for the information struct
  opus_ctx_t *ctx = heap_caps_calloc(sizeof(opus_ctx_t), 1, MALLOC_CAP_DMA);
  if (!ctx)
    goto err;

  // Start the MP3 library
  int err;
  ctx->decoder = opus_decoder_create(req_sample_rate, 2, &err);
  if (err != OPUS_OK) {
    ESP_LOGE(TAG, "failed to create opus decode, returned %d", err);
    goto err;
  }

  ctx->input_start  = (unsigned char *)heap_caps_malloc(INPUT_BUFFER_SIZE, MALLOC_CAP_DMA);
  ctx->input_curr   = ctx->input_start;  // Current position
  ctx->input_end    = ctx->input_start;  // End of data
  ctx->rate         = 0;
  ctx->channels     = 0;
  ctx->output_start = heap_caps_calloc(OUTPUT_BUFFER_SIZE, sizeof(int16_t), MALLOC_CAP_DMA);
  ctx->output_end   = ctx->output_start;
  ctx->output_curr  = ctx->output_start;
  ctx->stream_read  = (stream_read_type)stream_read_fn;
  ctx->stream       = (void *)stream;

  ESP_LOGD(TAG, "Started opus stream source");

  *_ctx     = (void *)ctx;
  int tries = 5;
  do {
    read_data(ctx);
  } while (!ctx_decode(ctx) && --tries);
  if (!tries) {
    ESP_LOGE(TAG, "Failed to decode start of opus stream");
    goto err;
  }
  *stereo = (ctx->channels == 2);

  return CHUNK_SIZE;  // Chunk size

err:
  opus_deinit_source(ctx);
  return -1;
}

int IRAM_ATTR opus_get_sample_rate(void *_ctx) {
  opus_ctx_t *ctx = (opus_ctx_t *)_ctx;
  return ctx->rate;
}

int IRAM_ATTR opus_fill_buffer(void *_ctx, int16_t *buffer, int stereo) {
  opus_ctx_t *ctx = (opus_ctx_t *)_ctx;
  if (ctx->output_curr == ctx->output_end) {
    ctx_decode(ctx);
  }
  if (ctx->output_curr != ctx->output_end) {
    int len = (ctx->output_end - ctx->output_curr) / ctx->channels;
    if (len > CHUNK_SIZE) {
      len = CHUNK_SIZE;
    }
    for (int i = 0; i < len; i++) {
      if (stereo && (ctx->channels == 2)) {
        buffer[i * 2 + 0] = *(ctx->output_curr++);
        buffer[i * 2 + 1] = *(ctx->output_curr++);
      } else if (stereo) {
        buffer[i * 2 + 0] = *ctx->output_curr;
        buffer[i * 2 + 1] = *ctx->output_curr;
        ctx->output_curr++;
      } else {
        buffer[i] = *ctx->output_curr;
        ctx->output_curr += ctx->channels;
      }
    }
    return len;
  }

  return 0;
}

static void opus_deinit_source(void *_ctx) {
  opus_ctx_t *ctx = (opus_ctx_t *)_ctx;
  if (ctx) {
    if (ctx->decoder) {
      opus_decoder_destroy(ctx->decoder);
    }
    if (ctx->input_start && ctx->stream) {
      free(ctx->input_start);
    }
    if (ctx->output_start) {
      free(ctx->output_start);
    }
    free(ctx);
  }
}

int opus_reset_buffer(void *ctx) {
  printf("Not implemented yet!");
  return -1;
}

const sndmixer_source_t sndmixer_source_opus = {
  .init_source     = opus_init_source,
  .get_sample_rate = opus_get_sample_rate,
  .fill_buffer     = opus_fill_buffer,
  .reset_buffer    = opus_reset_buffer,
  .deinit_source   = opus_deinit_source
};

const sndmixer_source_t sndmixer_source_opus_stream = {
  .init_source_stream = opus_init_source_stream,
  .get_sample_rate    = opus_get_sample_rate,
  .fill_buffer        = opus_fill_buffer,
  .reset_buffer       = opus_reset_buffer,
  .deinit_source      = opus_deinit_source
};

#endif
