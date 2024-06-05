// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
// SPDX-FileCopyrightText: Copyright (c) 2019 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiomp3/MP3Decoder.h"

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "shared-module/audiomp3/MP3Decoder.h"
#include "supervisor/background_callback.h"
#include "lib/mp3/src/mp3common.h"

#define MAX_BUFFER_LEN (MAX_NSAMP * MAX_NGRAN * MAX_NCHAN * sizeof(int16_t))

#define DO_DEBUG (0)

#if defined(MICROPY_UNIX_COVERAGE)
#define background_callback_prevent() ((void)0)
#define background_callback_allow() ((void)0)
#define background_callback_add(buf, fn, arg) ((fn)((arg)))
#endif

static bool stream_readable(void *stream) {
    int errcode = 0;
    mp_obj_base_t *o = MP_OBJ_TO_PTR(stream);
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    if (!stream_p->ioctl) {
        return true;
    }

    mp_int_t ret = stream_p->ioctl(stream, MP_STREAM_POLL, MP_STREAM_POLL_RD | MP_STREAM_POLL_ERR | MP_STREAM_POLL_HUP, &errcode);
    if (DO_DEBUG) {
        mp_printf(&mp_plat_print, "stream_readable ioctl() -> %d [errcode=%d]\n", ret, errcode);
    }
    return ret != 0;
}

// This is a near copy of mp_stream_posix_read, but avoiding use of global
// errno value & with added prints for debugging purposes. (circuitpython doesn't
// enable mp_stream_posix_read anyway)
static mp_int_t stream_read(void *stream, void *buf, size_t len) {
    int errcode;
    mp_obj_base_t *o = MP_OBJ_TO_PTR(stream);
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    if (!stream_p->read) {
        return -EINVAL;
    }
    mp_uint_t out_sz = stream_p->read(MP_OBJ_FROM_PTR(stream), buf, len, &errcode);
    if (DO_DEBUG) {
        mp_printf(&mp_plat_print, "stream_read(%d) -> %d\n", (int)len, (int)out_sz);
    }
    if (out_sz == MP_STREAM_ERROR) {
        if (DO_DEBUG) {
            mp_printf(&mp_plat_print, "errcode=%d\n", errcode);
        }
        return -errcode; // CIRCUITPY-CHANGE: returns negative errcode value
    } else {
        return out_sz;
    }
}

// This is a near copy of mp_stream_posix_lseek, but avoiding use of global
// errno value (circuitpython doesn't enable posix stream routines anyway)
static off_t stream_lseek(void *stream, off_t offset, int whence) {
    int errcode;
    const mp_obj_base_t *o = stream;
    const mp_stream_p_t *stream_p = MP_OBJ_TYPE_GET_SLOT(o->type, protocol);
    if (!stream_p->ioctl) {
        return -EINVAL;
    }
    struct mp_stream_seek_t seek_s;
    seek_s.offset = offset;
    seek_s.whence = whence;
    mp_uint_t res = stream_p->ioctl(MP_OBJ_FROM_PTR(stream), MP_STREAM_SEEK, (mp_uint_t)(uintptr_t)&seek_s, &errcode);
    if (res == MP_STREAM_ERROR) {
        return -errcode;
    }
    return seek_s.offset;
}

#define INPUT_BUFFER_AVAILABLE(i) ((i).write_off - (i).read_off)
#define INPUT_BUFFER_SPACE(i) ((i).size - INPUT_BUFFER_AVAILABLE(i))
#define INPUT_BUFFER_READ_PTR(i) ((i).buf + (i).read_off)
#define INPUT_BUFFER_CONSUME(i, n) ((i).read_off += (n))
#define INPUT_BUFFER_CLEAR(i) ((i).read_off = (i).write_off = 0)

/** Fill the input buffer unconditionally.
 *
 * Returns true if the input buffer contains any useful data,
 * false otherwise.  (The input buffer will be padded to the end with
 * 0 bytes, which do not interfere with MP3 decoding)
 *
 * Raises OSError if stream_read fails.
 *
 * Sets self->eof if any read of the file returns 0 bytes
 */
static bool mp3file_update_inbuf_always(audiomp3_mp3file_obj_t *self, bool block_ok) {
    if (self->eof || INPUT_BUFFER_SPACE(self->inbuf) == 0) {
        return INPUT_BUFFER_AVAILABLE(self->inbuf) > 0;
    }

    // We didn't previously reach EOF and we have input buffer space available

    // Move the unconsumed portion of the buffer to the start
    if (self->inbuf.read_off) {
        memmove(self->inbuf.buf, INPUT_BUFFER_READ_PTR(self->inbuf), INPUT_BUFFER_AVAILABLE(self->inbuf));
        self->inbuf.write_off -= self->inbuf.read_off;
        self->inbuf.read_off = 0;
    }

    for (size_t to_read; !self->eof && (to_read = INPUT_BUFFER_SPACE(self->inbuf)) > 0 && (block_ok || stream_readable(self->stream));) {
        uint8_t *write_ptr = self->inbuf.buf + self->inbuf.write_off;
        ssize_t n_read = stream_read(self->stream, write_ptr, to_read);

        if (n_read < 0) {
            int errcode = -n_read;
            if (mp_is_nonblocking_error(errcode) || errcode == MP_ETIMEDOUT) {
                break;
            }
            self->eof = true;
            mp_raise_OSError(errcode);
        }

        if (n_read == 0) {
            self->eof = true;
        }

        self->inbuf.write_off += n_read;
    }

    if (DO_DEBUG) {
        mp_printf(&mp_plat_print, "new avail=%d eof=%d\n", (int)INPUT_BUFFER_AVAILABLE(self->inbuf), self->eof);
    }

    // Return true iff there are at least some useful bytes in the buffer
    return INPUT_BUFFER_AVAILABLE(self->inbuf) > 0;
}

/** Update the inbuf from a background callback.
 *
 * Re-queue if there's still buffer space available to read stream data
 */
static void mp3file_update_inbuf_cb(void *self_in) {
    audiomp3_mp3file_obj_t *self = self_in;
    if (common_hal_audiomp3_mp3file_deinited(self_in)) {
        return;
    }
    if (!self->eof && stream_readable(self->stream)) {
        mp3file_update_inbuf_always(self, false);
    }

    #if !defined(MICROPY_UNIX_COVERAGE)
    if (!self->eof && INPUT_BUFFER_SPACE(self->inbuf) > 512) {
        background_callback_add(
            &self->inbuf_fill_cb,
            mp3file_update_inbuf_cb,
            self);
    }
    #endif
}

/** Fill the input buffer if it is less than half full.
 *
 * Returns the same as mp3file_update_inbuf_always.
 */
static bool mp3file_update_inbuf_half(audiomp3_mp3file_obj_t *self, bool block_ok) {
    // If buffer is over half full, do nothing
    if (INPUT_BUFFER_SPACE(self->inbuf) < self->inbuf.size / 2) {
        return true;
    }

    return mp3file_update_inbuf_always(self, block_ok);
}

#define READ_PTR(self) (INPUT_BUFFER_READ_PTR(self->inbuf))
#define BYTES_LEFT(self) (INPUT_BUFFER_AVAILABLE(self->inbuf))
#define CONSUME(self, n) (INPUT_BUFFER_CONSUME(self->inbuf, n))

// http://id3.org/id3v2.3.0
static void mp3file_skip_id3v2(audiomp3_mp3file_obj_t *self, bool block_ok) {
    mp3file_update_inbuf_half(self, block_ok);
    if (BYTES_LEFT(self) < 10) {
        return;
    }
    uint8_t *data = READ_PTR(self);
    if (!(
        data[0] == 'I' &&
        data[1] == 'D' &&
        data[2] == '3' &&
        data[3] != 0xff &&
        data[4] != 0xff &&
        (data[5] & 0x1f) == 0 &&
        (data[6] & 0x80) == 0 &&
        (data[7] & 0x80) == 0 &&
        (data[8] & 0x80) == 0 &&
        (data[9] & 0x80) == 0)) {
        return;
    }
    int32_t size = (data[6] << 21) | (data[7] << 14) | (data[8] << 7) | (data[9]);
    size += 10; // size excludes the "header" (but not the "extended header")
    // First, deduct from size whatever is left in buffer
    if (DO_DEBUG) {
        mp_printf(&mp_plat_print, "%s:%d id3 size %d\n", __FILE__, __LINE__, size);
    }
    uint32_t to_consume = MIN(size, BYTES_LEFT(self));
    CONSUME(self, to_consume);
    size -= to_consume;

    // Next, seek in the file after the header
    if (stream_lseek(self->stream, SEEK_CUR, size) == 0) {
        return;
    }

    // Couldn't seek (might be a socket), so need to actually read and discard all that data
    while (size > 0 && !self->eof) {
        mp3file_update_inbuf_always(self, true);
        to_consume = MIN(size, BYTES_LEFT(self));
        CONSUME(self, to_consume);
        size -= to_consume;
    }
}

/* If a sync word can be found, advance to it and return true.  Otherwise,
 * return false.
 */
static bool mp3file_find_sync_word(audiomp3_mp3file_obj_t *self, bool block_ok) {
    do {
        mp3file_update_inbuf_half(self, block_ok);
        int offset = MP3FindSyncWord(READ_PTR(self), BYTES_LEFT(self));
        if (offset >= 0) {
            CONSUME(self, offset);
            mp3file_update_inbuf_half(self, block_ok);
            return true;
        }
        CONSUME(self, MAX(0, BYTES_LEFT(self) - 16));
    } while (!self->eof);
    return false;
}

static bool mp3file_get_next_frame_info(audiomp3_mp3file_obj_t *self, MP3FrameInfo *fi, bool block_ok) {
    int err;
    do {
        err = MP3GetNextFrameInfo(self->decoder, fi, READ_PTR(self));
        if (err == ERR_MP3_NONE) {
            break;
        }
        CONSUME(self, 1);
        mp3file_find_sync_word(self, block_ok);
    } while (!self->eof);
    return err == ERR_MP3_NONE;
}

#define DEFAULT_INPUT_BUFFER_SIZE (2048)
#define MIN_USER_BUFFER_SIZE (DEFAULT_INPUT_BUFFER_SIZE + 2 * MAX_BUFFER_LEN)

void common_hal_audiomp3_mp3file_construct(audiomp3_mp3file_obj_t *self,
    mp_obj_t stream,
    uint8_t *buffer,
    size_t buffer_size) {
    // Note: Adafruit_MP3 uses a 2kB input buffer and two 4kB output pcm_buffer.
    // for a whopping total of 10kB pcm_buffer (+mp3 decoder state and frame buffer)
    // At 44kHz, that's 23ms of output audio data.
    //
    // We will choose a slightly different allocation strategy for the output:
    // Make sure the pcm_buffer are sized exactly to match (a multiple of) the
    // frame size; this is typically 2304 * 2 bytes, so a little bit bigger
    // than the two 4kB output pcm_buffer, except that the alignment allows to
    // never allocate that extra frame buffer.

    if ((intptr_t)buffer & 1) {
        buffer += 1;
        buffer_size -= 1;
    }
    if (buffer && buffer_size > MIN_USER_BUFFER_SIZE) {
        self->pcm_buffer[0] = (int16_t *)(void *)buffer;
        self->pcm_buffer[1] = (int16_t *)(void *)(buffer + MAX_BUFFER_LEN);
        self->inbuf.buf = buffer + 2 * MAX_BUFFER_LEN;
        self->inbuf.size = buffer_size - 2 * MAX_BUFFER_LEN;
    } else {
        self->inbuf.size = DEFAULT_INPUT_BUFFER_SIZE;
        self->inbuf.buf = m_malloc(DEFAULT_INPUT_BUFFER_SIZE);
        if (self->inbuf.buf == NULL) {
            common_hal_audiomp3_mp3file_deinit(self);
            m_malloc_fail(DEFAULT_INPUT_BUFFER_SIZE);
        }

        if (buffer_size >= 2 * MAX_BUFFER_LEN) {
            self->pcm_buffer[0] = (int16_t *)(void *)buffer;
            self->pcm_buffer[1] = (int16_t *)(void *)(buffer + MAX_BUFFER_LEN);
        } else {
            self->pcm_buffer[0] = m_malloc(MAX_BUFFER_LEN);
            if (self->pcm_buffer[0] == NULL) {
                common_hal_audiomp3_mp3file_deinit(self);
                m_malloc_fail(MAX_BUFFER_LEN);
            }

            self->pcm_buffer[1] = m_malloc(MAX_BUFFER_LEN);
            if (self->pcm_buffer[1] == NULL) {
                common_hal_audiomp3_mp3file_deinit(self);
                m_malloc_fail(MAX_BUFFER_LEN);
            }
        }
    }
    self->inbuf.read_off = self->inbuf.write_off = 0;

    self->decoder = MP3InitDecoder();
    if (self->decoder == NULL) {
        common_hal_audiomp3_mp3file_deinit(self);
        mp_raise_msg(&mp_type_MemoryError,
            MP_ERROR_TEXT("Couldn't allocate decoder"));
    }

    common_hal_audiomp3_mp3file_set_file(self, stream);
}

void common_hal_audiomp3_mp3file_set_file(audiomp3_mp3file_obj_t *self, mp_obj_t stream) {
    background_callback_prevent();

    self->stream = stream;

    INPUT_BUFFER_CLEAR(self->inbuf);
    self->eof = 0;
    self->other_channel = -1;
    mp3file_update_inbuf_half(self, true);
    mp3file_find_sync_word(self, true);
    // It **SHOULD** not be necessary to do this; the buffer should be filled
    // with fresh content before it is returned by get_buffer().  The fact that
    // this is necessary to avoid a glitch at the start of playback of a second
    // track using the same decoder object means there's still a bug in
    // get_buffer() that I didn't understand.
    memset(self->pcm_buffer[0], 0, MAX_BUFFER_LEN);
    memset(self->pcm_buffer[1], 0, MAX_BUFFER_LEN);
    MP3FrameInfo fi;
    bool result = mp3file_get_next_frame_info(self, &fi, true);
    background_callback_allow();
    if (!result) {
        mp_raise_msg(&mp_type_RuntimeError,
            MP_ERROR_TEXT("Failed to parse MP3 file"));
    }

    self->sample_rate = fi.samprate;
    self->channel_count = fi.nChans;
    self->frame_buffer_size = fi.outputSamps * sizeof(int16_t);
    self->len = 2 * self->frame_buffer_size;
    self->samples_decoded = 0;
}

void common_hal_audiomp3_mp3file_deinit(audiomp3_mp3file_obj_t *self) {
    if (self->decoder) {
        MP3FreeDecoder(self->decoder);
    }
    self->decoder = NULL;
    self->inbuf.buf = NULL;
    self->pcm_buffer[0] = NULL;
    self->pcm_buffer[1] = NULL;
    self->stream = mp_const_none;
    self->samples_decoded = 0;
}

bool common_hal_audiomp3_mp3file_deinited(audiomp3_mp3file_obj_t *self) {
    return self->pcm_buffer[0] == NULL;
}

uint32_t common_hal_audiomp3_mp3file_get_sample_rate(audiomp3_mp3file_obj_t *self) {
    return self->sample_rate;
}

void common_hal_audiomp3_mp3file_set_sample_rate(audiomp3_mp3file_obj_t *self,
    uint32_t sample_rate) {
    self->sample_rate = sample_rate;
}

uint8_t common_hal_audiomp3_mp3file_get_bits_per_sample(audiomp3_mp3file_obj_t *self) {
    return 16;
}

uint8_t common_hal_audiomp3_mp3file_get_channel_count(audiomp3_mp3file_obj_t *self) {
    return self->channel_count;
}

void audiomp3_mp3file_reset_buffer(audiomp3_mp3file_obj_t *self,
    bool single_channel_output,
    uint8_t channel) {
    if (single_channel_output && channel == 1) {
        return;
    }
    // We don't reset the buffer index in case we're looping and we have an odd number of buffer
    // loads
    background_callback_prevent();
    if (self->eof && stream_lseek(self->stream, SEEK_SET, 0) == 0) {
        INPUT_BUFFER_CLEAR(self->inbuf);
        self->eof = 0;
        self->samples_decoded = 0;
        self->other_channel = -1;
        mp3file_skip_id3v2(self, false);
        mp3file_find_sync_word(self, false);
    }
    background_callback_allow();
}

audioio_get_buffer_result_t audiomp3_mp3file_get_buffer(audiomp3_mp3file_obj_t *self,
    bool single_channel_output,
    uint8_t channel,
    uint8_t **bufptr,
    uint32_t *buffer_length) {
    if (!self->inbuf.buf) {
        *buffer_length = 0;
        if (DO_DEBUG) {
            mp_printf(&mp_plat_print, "%s:%d\n", __FILE__, __LINE__);
        }
        return GET_BUFFER_ERROR;
    }
    if (!single_channel_output) {
        channel = 0;
    }

    size_t frame_buffer_size_bytes = self->frame_buffer_size;
    *buffer_length = frame_buffer_size_bytes;

    if (channel == self->other_channel) {
        *bufptr = (uint8_t *)(self->pcm_buffer[self->other_buffer_index] + channel);
        self->other_channel = -1;
        self->samples_decoded += *buffer_length / sizeof(int16_t);
        if (DO_DEBUG) {
            mp_printf(&mp_plat_print, "%s:%d\n", __FILE__, __LINE__);
        }
        return GET_BUFFER_MORE_DATA;
    }


    self->buffer_index = !self->buffer_index;
    self->other_channel = 1 - channel;
    self->other_buffer_index = self->buffer_index;
    int16_t *buffer = (int16_t *)(void *)self->pcm_buffer[self->buffer_index];
    *bufptr = (uint8_t *)buffer;

    mp3file_skip_id3v2(self, false);
    if (!mp3file_find_sync_word(self, false)) {
        memset(buffer, 0, self->frame_buffer_size);
        *buffer_length = 0;
        return self->eof ? GET_BUFFER_DONE : GET_BUFFER_ERROR;
    }
    int bytes_left = BYTES_LEFT(self);
    uint8_t *inbuf = READ_PTR(self);
    int err = MP3Decode(self->decoder, &inbuf, &bytes_left, buffer, 0);
    if (err != ERR_MP3_INDATA_UNDERFLOW) {
        CONSUME(self, BYTES_LEFT(self) - bytes_left);
    }
    if (err) {
        memset(buffer, 0, frame_buffer_size_bytes);
        if (DO_DEBUG) {
            mp_printf(&mp_plat_print, "%s:%d err=%d\n", __FILE__, __LINE__, err);
        }
        if (err != ERR_MP3_INDATA_UNDERFLOW && err != ERR_MP3_MAINDATA_UNDERFLOW) {
            memset(buffer, 0, self->frame_buffer_size);
            *buffer_length = 0;
            self->eof = true;
            return GET_BUFFER_ERROR;
        }
    }

    self->samples_decoded += frame_buffer_size_bytes / sizeof(int16_t);

    mp3file_skip_id3v2(self, false);
    int result = mp3file_find_sync_word(self, false) ? GET_BUFFER_MORE_DATA : GET_BUFFER_DONE;

    if (DO_DEBUG) {
        mp_printf(&mp_plat_print, "%s:%d result=%d\n", __FILE__, __LINE__, result);
    }
    if (INPUT_BUFFER_SPACE(self->inbuf) > 512) {
        background_callback_add(
            &self->inbuf_fill_cb,
            mp3file_update_inbuf_cb,
            self);
    }

    if (DO_DEBUG) {
        mp_printf(&mp_plat_print, "post-decode avail=%d eof=%d\n", (int)INPUT_BUFFER_AVAILABLE(self->inbuf), self->eof);
    }
    return result;
}

void audiomp3_mp3file_get_buffer_structure(audiomp3_mp3file_obj_t *self, bool single_channel_output,
    bool *single_buffer, bool *samples_signed,
    uint32_t *max_buffer_length, uint8_t *spacing) {
    *single_buffer = false;
    *samples_signed = true;
    *max_buffer_length = self->frame_buffer_size;
    if (single_channel_output) {
        *spacing = self->channel_count;
    } else {
        *spacing = 1;
    }
}

float common_hal_audiomp3_mp3file_get_rms_level(audiomp3_mp3file_obj_t *self) {
    float sumsq = 0.f;
    // Assumes no DC component to the audio.  Is that a safe assumption?
    int16_t *buffer = (int16_t *)(void *)self->pcm_buffer[self->buffer_index];
    for (size_t i = 0; i < self->frame_buffer_size / sizeof(int16_t); i++) {
        sumsq += (float)buffer[i] * buffer[i];
    }
    return sqrtf(sumsq) / (self->frame_buffer_size / sizeof(int16_t));
}

uint32_t common_hal_audiomp3_mp3file_get_samples_decoded(audiomp3_mp3file_obj_t *self) {
    return self->samples_decoded;
}
