// Author: Renze Nicolai 2019, badge.team

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
#include "libhelix-mp3/mp3dec.h"

#ifdef CONFIG_DRIVER_SNDMIXER_ENABLE

#define MAX_SAMPLES_PER_FRAME (1152 * 2)
#define CHUNK_SIZE            32
#define INTERNAL_BUFFER_SIZE  1024 * 8
#define INTERNAL_BUFFER_FETCH_WHEN \
  4096  // new data will be fetched when there is less than this amount of data

#define TAG "snd_source_mp3"

typedef struct {
  HMP3Decoder hMP3Decoder;
  unsigned char *dataStart;
  unsigned char *dataCurr;
  unsigned char *dataEnd;
  int lastRate;
  int lastChannels;
  short *buffer;
  int bufferValid;
  int bufferOffset;

  unsigned char *dataPtr;  // Pointer to internal buffer (if applicable)
  stream_read_type stream_read;
  stream_seek_type seek_func;
  void *stream;  // Pointer to stream
} mp3_ctx_t;

void mp3_deinit_source(void *ctx);

void _readData(mp3_ctx_t *mp3) {
  // Fetch data for internal buffer
  int dataAvailable   = mp3->dataEnd - mp3->dataCurr;
  int bufferAvailable = INTERNAL_BUFFER_SIZE - (mp3->dataEnd - mp3->dataStart);
  int amountFetched   = 0;

  if (dataAvailable < INTERNAL_BUFFER_FETCH_WHEN) {
    // 1) Get rid of old data
    if (mp3->dataCurr != mp3->dataStart) {
      // Move available data to the begin of the buffer
      // printf("Moving %d bytes of data from %p to %p.\n", dataAvailable, mp3->dataCurr,
      // mp3->dataStart);
      memmove(mp3->dataStart, mp3->dataCurr, dataAvailable);
      mp3->dataCurr = mp3->dataStart;
      mp3->dataEnd  = mp3->dataStart + dataAvailable;
    }

    amountFetched = mp3->stream_read(mp3->stream, mp3->dataEnd, bufferAvailable);
    mp3->dataEnd += amountFetched;  // Our buffer now (hopefully) contains more data
  }

  // printf("_readData: %d, %d, %d\n", dataAvailable, bufferAvailable, amountFetched);
}

int mp3_decode(void *ctx) {
  mp3_ctx_t *mp3 = (mp3_ctx_t *)ctx;

  if (mp3->stream)
    _readData(mp3);

  int available = mp3->dataEnd - mp3->dataCurr;
  int nextSync  = MP3FindSyncWord(mp3->dataCurr, available);

  if (nextSync >= 0) {
    mp3->dataCurr += nextSync;
    available = mp3->dataEnd - mp3->dataCurr;

    // printf("Next syncword @ %d, available = %d\n", nextSync, available);
    int ret = MP3Decode(mp3->hMP3Decoder, &mp3->dataCurr, &available, mp3->buffer, 0);

    if (ret) {
      ESP_LOGE(TAG, "MP3Decode error %d\n", ret);
      return 0;
    }

    MP3FrameInfo fi;
    MP3GetLastFrameInfo(mp3->hMP3Decoder, &fi);

    mp3->lastRate     = fi.samprate;
    mp3->lastChannels = fi.nChans;
    int validSamples  = fi.outputSamps / mp3->lastChannels;
    mp3->bufferValid  = validSamples;
    mp3->bufferOffset = 0;
    // printf("MP3Decode OK, buffer @ %p, available = %d, rate = %d, channels = %d, validSamples =
    // %d\n", mp3->dataCurr, available, mp3->lastRate, mp3->lastChannels, validSamples);

    return 1;
  } else {
    mp3->dataCurr += available;
    ESP_LOGE(TAG, "No syncword found\n");
    return 0;
  }
}

int mp3_init_source(const void *data_start, const void *data_end, int req_sample_rate, void **ctx,
                    int *stereo) {
  // Allocate space for the information struct
  mp3_ctx_t *mp3 = calloc(sizeof(mp3_ctx_t), 1);
  if (!mp3)
    goto err;

  // Start the MP3 library
  mp3->hMP3Decoder = MP3InitDecoder();
  if (!mp3->hMP3Decoder) {
    ESP_LOGE(TAG, "Out of memory error! hMP3Decoder is NULL\n");
    goto err;
  }

  // Fill the struct with info
  mp3->dataStart    = (unsigned char *)data_start;  // Start of data
  mp3->dataCurr     = (unsigned char *)data_start;  // Current position
  mp3->seek_func    = NULL;
  mp3->dataEnd      = (unsigned char *)data_end;    // End of data
  mp3->lastRate     = 0;
  mp3->lastChannels = 0;
  mp3->buffer       = calloc(MAX_SAMPLES_PER_FRAME, sizeof(short));
  mp3->bufferValid  = 0;
  mp3->bufferOffset = 0;
  mp3->dataPtr      = NULL;
  mp3->stream_read  = NULL;
  mp3->stream       = NULL;

  if (!mp3->buffer) {
    ESP_LOGE(TAG, "Out of memory error! mp3->buffer is NULL\n");
    goto err;
  }

  uint32_t length = data_end - data_start + 1;

  ESP_LOGD(TAG, "MP3 source started, data at %p with size %u!\n", mp3->dataStart, length);

  *ctx    = (void *)mp3;
  *stereo = (mp3->lastChannels == 2);

  mp3_decode(*ctx);  // Decode first part

  return CHUNK_SIZE;  // Chunk size

err:
  mp3_deinit_source(mp3);
  return -1;
}

int mp3_init_source_stream(
  stream_read_type stream_read_fn,
  void *stream,
  int req_sample_rate,
  void **ctx,
  int *stereo,
  stream_seek_type seek_func
) {
  // Allocate space for the information struct
  mp3_ctx_t *mp3 = calloc(sizeof(mp3_ctx_t), 1);
  if (!mp3) {
    ESP_LOGE(TAG, "Out of memory error! mp3 is NULL\n");
    goto err;
  }

  // Start the MP3 library
  mp3->hMP3Decoder = MP3InitDecoder();
  if (!mp3->hMP3Decoder) {
    ESP_LOGE(TAG, "Out of memory error! hMP3Decoder is NULL\n");
    goto err;
  }

  // Fill the struct with info
  mp3->lastRate     = 0;
  mp3->lastChannels = 0;
  mp3->buffer       = calloc(MAX_SAMPLES_PER_FRAME, sizeof(short));
  mp3->bufferValid  = 0;
  mp3->bufferOffset = 0;

  mp3->stream_read = (stream_read_type)stream_read_fn;
  mp3->seek_func   = (stream_seek_type)seek_func;
  mp3->stream      = (void *)stream;
  ESP_LOGD(TAG, "stream read fn @ %p and stream at %p\n", mp3->stream_read, mp3->stream);
  mp3->dataPtr   = heap_caps_malloc(INTERNAL_BUFFER_SIZE, MALLOC_CAP_DMA);
  mp3->dataStart = mp3->dataPtr;
  mp3->dataCurr  = mp3->dataPtr;
  mp3->dataEnd   = mp3->dataPtr;

  if (!mp3->buffer) {
    ESP_LOGE(TAG, "Out of memory error! mp3->buffer is NULL\n");
    goto err;
  }

  if (!mp3->dataPtr) {
    ESP_LOGE(TAG, "Out of memory error! mp3->dataPtr is NULL\n");
    goto err;
  }

  *ctx      = (void *)mp3;
  int tries = 5;
  do {
    ESP_LOGI(TAG, "Finding chunk of mp3 data\r\n");
    _readData(mp3);
  } while (!mp3_decode(*ctx) && --tries);
  if (!tries) {
    goto err;
  }
  ESP_LOGD(TAG, "MP3 stream source started, data at %p, %d Hz, %d channels!\n", mp3->dataStart,
           mp3->lastRate, mp3->lastChannels);

  *stereo = mp3->lastChannels == 2;
  return CHUNK_SIZE;  // Chunk size

err:
  mp3_deinit_source(mp3);
  return -1;
}

int mp3_get_sample_rate(void *ctx) {
  mp3_ctx_t *mp3 = (mp3_ctx_t *)ctx;
  return mp3->lastRate;
}

int mp3_fill_buffer(void *ctx, int16_t *buffer, int stereo) {
  mp3_ctx_t *mp3 = (mp3_ctx_t *)ctx;
  if (mp3->bufferValid <= 0)
    mp3_decode(ctx);
  if (mp3->bufferValid > 0) {
    int len = mp3->bufferValid;
    if (len > CHUNK_SIZE)
      len = CHUNK_SIZE;
    for (int i = 0; i < len; i++) {
      if (stereo && (mp3->lastChannels == 2)) {
        buffer[i * 2 + 0] = mp3->buffer[mp3->bufferOffset + i * 2 + 0];
        buffer[i * 2 + 1] = mp3->buffer[mp3->bufferOffset + i * 2 + 1];
      } else {
        buffer[i] = mp3->buffer[mp3->bufferOffset + i * mp3->lastChannels];
      }
    }
    mp3->bufferValid -= len;
    mp3->bufferOffset += len * mp3->lastChannels;
    return len;
  }

  return 0;
}

void mp3_deinit_source(void *ctx) {
  mp3_ctx_t *mp3 = (mp3_ctx_t *)ctx;
  if (mp3) {
    MP3FreeDecoder(mp3->hMP3Decoder);
    if (mp3->buffer)
      free(mp3->buffer);
    if (mp3->dataPtr)
      free(mp3->dataPtr);  // Stream
    free(mp3);
  }
}

int mp3_reset_buffer(void *ctx) {
  mp3_ctx_t *mp3 = (mp3_ctx_t *)ctx;
  mp3->dataCurr = mp3->dataStart;
  mp3->bufferValid  = 0;
  mp3->bufferOffset = 0;
  return 0;
}

int mp3_stream_reset_buffer(void *ctx) {
  mp3_ctx_t *mp3 = (mp3_ctx_t *)ctx;
  mp3->seek_func(mp3->stream,0,0);
  return 0;
}


const sndmixer_source_t sndmixer_source_mp3 = {
  .init_source     = mp3_init_source,
  .get_sample_rate = mp3_get_sample_rate,
  .fill_buffer     = mp3_fill_buffer,
  .reset_buffer    = mp3_reset_buffer,
  .deinit_source   = mp3_deinit_source
};

const sndmixer_source_t sndmixer_source_mp3_stream = {
  .init_source_stream = mp3_init_source_stream,
  .get_sample_rate    = mp3_get_sample_rate,
  .fill_buffer        = mp3_fill_buffer,
  .reset_buffer       = mp3_stream_reset_buffer,
  .deinit_source   = mp3_deinit_source
};

#endif
