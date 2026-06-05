#include <sdkconfig.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <esp_log.h>
#include <esp_attr.h>
#include <esp_heap_caps.h>

#include "sndmixer.h"
#include "snd_source_wav.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#define CHUNK_SIZE 16
#define TAG "source_wav"

typedef struct {
  const uint8_t *data;  // Pointer to internal buffer (if applicable)
  ssize_t pos;
  uint32_t data_len;
  uint32_t rate;
  uint16_t channels, bits;

  stream_read_type stream_read;
  stream_seek_type seek_func;
  void *stream;  // Pointer to stream
  off_t data_start_offset;
} wav_ctx_t;

typedef struct __attribute__((packed)) {
  int8_t riffmagic[4];
  uint32_t size;
  int8_t wavemagic[4];
} riff_hdr_t;

typedef struct __attribute__((packed)) {
  uint16_t fmtcode;
  uint16_t channels;
  uint32_t samplespersec;
  uint32_t avgbytespersec;
  uint16_t blockalign;
  uint16_t bitspersample;
  uint16_t bsize;
  uint16_t validbitspersample;
  uint32_t channelmask;
  int8_t subformat[16];
} fmt_data_t;

typedef struct __attribute__((packed)) {
  int8_t magic[4];
  int32_t size;
  union {
    fmt_data_t fmt;
    int8_t data[0];
  };
} chunk_hdr_t;

int wav_init_source(const void *data_start, const void *data_end, int req_sample_rate, void **ctx,
                    int *stereo) {
  // Check sanity first
  char *p        = (char *)data_start;
  wav_ctx_t *wav = calloc(1, sizeof(wav_ctx_t));
  if (!wav)
    goto err;
  riff_hdr_t *riff = (riff_hdr_t *)p;
  if (memcmp(riff->riffmagic, "RIFF", 4) != 0)
    goto err;
  if (memcmp(riff->wavemagic, "WAVE", 4) != 0)
    goto err;
  p += sizeof(riff_hdr_t);
  while (p < (char *)data_end) {
    chunk_hdr_t *ch = (chunk_hdr_t *)p;
    if (memcmp(ch->magic, "fmt ", 4) == 0) {
      if (ch->fmt.fmtcode != WAVE_FORMAT_PCM) {
        ESP_LOGW(TAG, "Unsupported wav format: %d\n", ch->fmt.fmtcode);
        goto err;
      }
      wav->rate     = ch->fmt.samplespersec;
      wav->bits     = ch->fmt.bitspersample;
      wav->channels = ch->fmt.channels;
      if (wav->channels == 0)
        wav->channels = 1;
    } else if (memcmp(ch->magic, "data", 4) == 0) {
      wav->data_len = ch->size;
      wav->data     = (uint8_t *)ch->data;
    }
    p += 8 + ch->size;
    if (ch->size & 1)
      p++;  // pad to even address
  }

  if (wav->bits != 8 && wav->bits != 16) {
    ESP_LOGW(TAG, "No fmt chunk or unsupported bits/sample: %d\n", wav->bits);
    goto err;
  }
  ESP_LOGI(TAG, "channels: %d, bits: %d, rate: %d, len: %d", wav->channels, wav->bits, wav->rate, wav->data_len);
  wav->pos = 0;
  *ctx     = (void *)wav;
  *stereo  = (wav->channels >= 2);
  return CHUNK_SIZE;
err:
  free(wav);
  return -1;
}

int wav_init_source_stream(
  stream_read_type stream_read_fn,
  void *stream,
  int req_sample_rate,
  void **ctx,
  int *stereo,
  stream_seek_type seek_func
) {
  ESP_LOGI(TAG, "init wav");
  wav_ctx_t *wav = calloc(1, sizeof(wav_ctx_t));
  if (!wav) {
    ESP_LOGE(TAG, "Failed to allocate wave file context");
    return -1;
  }

  wav->stream_read  = stream_read_fn;
  wav->seek_func    = seek_func;
  wav->stream       = stream;

  ESP_LOGI(TAG, "header @ %ld", wav->seek_func(wav->stream, 0, SEEK_CUR));
  riff_hdr_t *riffHdr = calloc(1, sizeof(riff_hdr_t));
  int read = wav->stream_read(wav->stream, riffHdr, sizeof(riff_hdr_t));
  if (read < 7) {
    ESP_LOGW(TAG, "Failed to read WAV header");
    return -1;
  }

  if (memcmp(riffHdr->riffmagic, "RIFF", 4) != 0){
    ESP_LOGW(TAG, "WAV file does not contain RIFF magic bytes");
    return -1;
  }
  if (memcmp(riffHdr->wavemagic, "WAVE", 4) != 0){
    ESP_LOGW(TAG, "WAV file does not contain WAVE magic bytes");
    return -1;
  }

  ESP_LOGI(TAG, "fmt @ %ld", wav->seek_func(wav->stream, 0, SEEK_CUR));
  chunk_hdr_t chunk;
  wav->stream_read(wav->stream, &chunk, 4 + 4);

  if (memcmp(chunk.magic, "fmt ", 4) != 0){
    ESP_LOGW(TAG, "WAV file does not contain format chunk after header");
    return -1;
  }
  ESP_LOGI(TAG, "fmt size %d", chunk.size);

  ESP_LOGI(TAG, "fmt body @ %ld", wav->seek_func(wav->stream, 0, SEEK_CUR));
  fmt_data_t format;
  wav->stream_read(wav->stream, &format, chunk.size);
  if (format.fmtcode != WAVE_FORMAT_PCM) {
    ESP_LOGW(TAG, "Unsupported WAV format: %d\n", format.fmtcode);
    return -1;
  }
  wav->rate     = format.samplespersec;
  wav->bits     = format.bitspersample;
  wav->channels = format.channels;
  if (wav->channels == 0) {
    wav->channels = 1;
  }

  ESP_LOGI(TAG, "channels: %d, bits: %d, rate: %d", wav->channels, wav->bits, wav->rate);

  ESP_LOGI(TAG, "data @ %ld", wav->seek_func(wav->stream, 0, SEEK_CUR));
  wav->stream_read(wav->stream, &chunk, 4 + 4);
  if (memcmp(chunk.magic, "data", 4) != 0){
    ESP_LOGW(TAG, "WAV file does not contain data chunk after format chunk");
    return -1;
  }

  ESP_LOGI(TAG, "seek");
  wav->data_start_offset = wav->seek_func(wav->stream, 0, SEEK_CUR);
  wav->data_len = chunk.size;
  ESP_LOGI(TAG, "WAV data offset: %ld", wav->data_start_offset);

  wav->pos = 0;
  *ctx     = (void *)wav;
  *stereo  = (wav->channels >= 2);
  return CHUNK_SIZE;
}

int wav_get_sample_rate(void *ctx) {
  wav_ctx_t *wav = (wav_ctx_t *)ctx;
  return wav->rate;
}

uint8_t get_sample_byte(wav_ctx_t *wav) {
  uint8_t rv = 0;
  if(wav->stream) {
    int read = wav->stream_read(wav->stream, &rv, 1);
    wav->pos += read;
  } else {
    rv = wav->data[wav->pos];
    wav->pos += 1;
  }
  return rv;
}

int16_t get_sample(wav_ctx_t *wav) {
  int16_t rv = 0;
  if (wav->bits == 8) {
    rv = (get_sample_byte(wav) - 128) << 8;
  } else {
    rv = get_sample_byte(wav) | get_sample_byte(wav) << 8;
  }
  return rv;
}

int wav_fill_buffer(void *ctx, int16_t *buffer, int stereo) {
  wav_ctx_t *wav = (wav_ctx_t *)ctx;
  int channels   = 1;
  if (wav->channels == 2 && stereo) {
    channels = 2;
  }
  if(wav->stream && stereo && wav->bits == 16 && wav->channels <= 2) {
    // Optimisation: if we're streaming a 1 or 2-channel 16 bit file, we can directly copy its contents
    ssize_t read = wav->stream_read(wav->stream, buffer, CHUNK_SIZE * sizeof(uint16_t) * wav->channels);
    wav->pos += read;
    return read / (sizeof(uint16_t) * wav->channels);
  }
  for (int i = 0; i < CHUNK_SIZE; i++) {
    if (wav->pos >= wav->data_len)
      return i;
    if (channels == 2) {
      buffer[i * 2 + 0] = get_sample(wav);
      buffer[i * 2 + 1] = get_sample(wav);
    } else {
      int32_t sum = 0;
      for (int k = 0; k < wav->channels; k++) {
        sum += get_sample(wav);
      }
      buffer[i] = sum / wav->channels;
    }
  }
  return CHUNK_SIZE;
}

int wav_reset_buffer(void *ctx) {
    wav_ctx_t *wav = (wav_ctx_t *)ctx;
    wav->pos = 0;
    return 0;
}

int wav_stream_reset_buffer(void *ctx) {
  wav_ctx_t *wav = (wav_ctx_t *)ctx;
  wav->seek_func(wav->stream, wav->data_start_offset, 0);
  wav->pos = 0;
  return 0;
}


void wav_deinit_source(void *ctx) {
  wav_ctx_t *wav = (wav_ctx_t *)ctx;
  free(wav);
}

const sndmixer_source_t sndmixer_source_wav = {
  .init_source     = wav_init_source,
  .get_sample_rate = wav_get_sample_rate,
  .fill_buffer     = wav_fill_buffer,
  .reset_buffer    = wav_reset_buffer,
  .deinit_source   = wav_deinit_source
};

const sndmixer_source_t sndmixer_source_wav_stream = {
  .init_source_stream = wav_init_source_stream,
  .get_sample_rate    = wav_get_sample_rate,
  .fill_buffer        = wav_fill_buffer,
  .reset_buffer       = wav_stream_reset_buffer,
  .deinit_source      = wav_deinit_source
};

#endif
