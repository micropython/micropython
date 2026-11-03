// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/audiofilewriter/AudioFileWriter.h"
#include "shared-bindings/audiocore/__init__.h"
#include "shared-module/audiocore/__init__.h"

#include <string.h>

#include "py/mperrno.h"
#include "py/runtime.h"
#include "py/stream.h"

#include "supervisor/background_callback.h"
#include "supervisor/shared/tick.h"

// ---------------------------------------------------------------------------
// Little-endian header helpers
// ---------------------------------------------------------------------------

static void put_u16le(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static void put_u32le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

// ---------------------------------------------------------------------------
// Active-writer registry, walked once per supervisor tick.
//
// The list head lives in MP_STATE_VM (see MP_REGISTER_ROOT_POINTER below) so
// that (a) active writers and their reg_next chain are GC-rooted while
// recording, and (b) the list is wiped automatically when the VM heap is
// recreated on soft reset.
// ---------------------------------------------------------------------------

#define REGISTRY_HEAD ((audiofilewriter_audiofilewriter_obj_t *)MP_STATE_VM(audiofilewriter_linked_list))

// Add self to the registry. Called from Python (play()) context, so it must
// guard against a background tick walking the list mid-mutation.
static void audiofilewriter_register(audiofilewriter_audiofilewriter_obj_t *self) {
    background_callback_prevent();
    // Avoid double-linking if already present.
    bool present = false;
    for (audiofilewriter_audiofilewriter_obj_t *w = REGISTRY_HEAD; w != NULL; w = w->reg_next) {
        if (w == self) {
            present = true;
            break;
        }
    }
    if (!present) {
        self->reg_next = REGISTRY_HEAD;
        MP_STATE_VM(audiofilewriter_linked_list) = self;
    }
    background_callback_allow();
}

// Remove self from the registry. Callers must ensure no background tick is
// walking the list concurrently: either they are the background tick itself
// (single-threaded, so safe), or they wrap the call in prevent/allow.
static void audiofilewriter_unregister(audiofilewriter_audiofilewriter_obj_t *self) {
    audiofilewriter_audiofilewriter_obj_t **pp = (audiofilewriter_audiofilewriter_obj_t **)&MP_STATE_VM(audiofilewriter_linked_list);
    while (*pp != NULL) {
        if (*pp == self) {
            *pp = self->reg_next;
            self->reg_next = NULL;
            return;
        }
        pp = &(*pp)->reg_next;
    }
}

// ---------------------------------------------------------------------------
// RAM ring
// ---------------------------------------------------------------------------

// Copy len bytes from src into the ring. The caller guarantees there is room.
// 8-bit signed PCM is flipped to unsigned to match the WAV convention.
static void audiofilewriter_ring_write(audiofilewriter_audiofilewriter_obj_t *self, const uint8_t *src, uint32_t len) {
    bool flip = (self->bits_per_sample == 8 && self->samples_signed);
    uint32_t i = 0;
    while (i < len) {
        uint32_t span = self->ring_size - self->ring_head;
        if (span > (len - i)) {
            span = len - i;
        }
        if (flip) {
            for (uint32_t j = 0; j < span; j++) {
                self->ring[self->ring_head + j] = src[i + j] ^ 0x80;
            }
        } else {
            memcpy(self->ring + self->ring_head, src + i, span);
        }
        self->ring_head += span;
        if (self->ring_head == self->ring_size) {
            self->ring_head = 0;
        }
        i += span;
    }
    self->ring_count += len;
}

// Drain the ring to the file. Returns false on a write error. Non-raising:
// safe to call from background-task context.
static bool audiofilewriter_flush(audiofilewriter_audiofilewriter_obj_t *self) {
    while (self->ring_count > 0) {
        uint32_t span = self->ring_size - self->ring_tail;
        if (span > self->ring_count) {
            span = self->ring_count;
        }
        int err = 0;
        mp_uint_t wrote = mp_stream_write_exactly(self->file, self->ring + self->ring_tail, span, &err);
        if (err != 0 || wrote != span) {
            return false;
        }
        self->ring_tail += span;
        if (self->ring_tail == self->ring_size) {
            self->ring_tail = 0;
        }
        self->ring_count -= span;
        self->data_bytes += span;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Header patching + finalize
// ---------------------------------------------------------------------------

static void audiofilewriter_patch_header(audiofilewriter_audiofilewriter_obj_t *self) {
    uint8_t sz[4];
    int err = 0;

    // RIFF chunk size lives at header_offset + 4.
    put_u32le(sz, 36 + self->data_bytes);
    if (mp_stream_seek(self->file, self->header_offset + 4, MP_SEEK_SET, &err) == (mp_off_t)-1) {
        return;
    }
    mp_stream_write_exactly(self->file, sz, 4, &err);

    // data chunk size lives at header_offset + 40.
    put_u32le(sz, self->data_bytes);
    if (mp_stream_seek(self->file, self->header_offset + 40, MP_SEEK_SET, &err) == (mp_off_t)-1) {
        return;
    }
    mp_stream_write_exactly(self->file, sz, 4, &err);

    // Leave the cursor at the end of the PCM so the caller can keep appending
    // or simply close the file.
    mp_stream_seek(self->file, self->header_offset + 44 + self->data_bytes, MP_SEEK_SET, &err);
}

// Stop pumping, drain, patch the header, and release the source. Idempotent:
// only the first call (while playing) does work. Non-raising.
static void audiofilewriter_finalize(audiofilewriter_audiofilewriter_obj_t *self) {
    if (!self->playing) {
        return;
    }
    // Stop the pump first so a background tick can't re-enter us.
    self->playing = false;

    audiofilewriter_flush(self);
    audiofilewriter_patch_header(self);

    supervisor_disable_tick();
    audiofilewriter_unregister(self);
    self->sample = MP_OBJ_NULL;
}

// ---------------------------------------------------------------------------
// The pump: one real-time-paced step per supervisor tick
// ---------------------------------------------------------------------------

static void audiofilewriter_pump(audiofilewriter_audiofilewriter_obj_t *self) {
    if (!self->playing) {
        return;
    }

    // The pull granularity through the chain is one source buffer, so we pace
    // in whole-buffer units.
    int64_t frames_per_pull = (int64_t)(self->source_max_buffer / self->bytes_per_frame);
    if (frames_per_pull < 1) {
        frames_per_pull = 1;
    }

    // Accrue a real-time sample budget from elapsed ticks.
    uint64_t now = supervisor_ticks_ms64();
    uint64_t elapsed = now - self->last_tick_ms;
    self->last_tick_ms = now;
    // Ignore long gaps (GC pause, SD stall) so we don't try to catch up an
    // unbounded backlog all at once.
    if (elapsed > 100) {
        elapsed = 100;
    }
    self->budget_frames += (int64_t)((elapsed * self->sample_rate) / 1000);
    // Never bank more than a single buffer of catch-up. For a LIVE source the
    // captured audio sits in the source's own bounded ring; once we fall more
    // than that behind, the surplus is already gone, so a large banked budget
    // cannot recover it -- it would only burst-drain the source on consecutive
    // ticks and then silence-pad, stretching one stall into a long glitch.
    // Capping at one buffer yields at most one brief discontinuity per stall.
    if (self->budget_frames > frames_per_pull) {
        self->budget_frames = frames_per_pull;
    }

    // Pull one buffer only once a full buffer of real time has elapsed: strict
    // real-time pacing so we never pull ahead of a live source (which would
    // only hand back silence). Also require room for a full source buffer.
    if (!self->source_done && self->budget_frames >= frames_per_pull &&
        (self->ring_size - self->ring_count) >= self->source_max_buffer) {
        uint8_t *buf = NULL;
        uint32_t len = 0;
        audioio_get_buffer_result_t res =
            audiosample_get_buffer(self->sample, false, 0, &buf, &len);
        if (res == GET_BUFFER_ERROR) {
            self->source_done = true;
        } else {
            if (len > 0 && buf != NULL) {
                // Defend against a source that hands back more than it advertised.
                if (len > self->source_max_buffer) {
                    len = self->source_max_buffer;
                }
                audiofilewriter_ring_write(self, buf, len);
                self->budget_frames -= (int64_t)(len / self->bytes_per_frame);
            }
            if (res == GET_BUFFER_DONE) {
                self->source_done = true;
            }
        }
    }

    if (!audiofilewriter_flush(self)) {
        // File write failed; give up gracefully rather than spin.
        self->source_done = true;
    }

    if (self->source_done && self->ring_count == 0) {
        audiofilewriter_finalize(self);
    }
}

void audiofilewriter_background(void) {
    audiofilewriter_audiofilewriter_obj_t *self = REGISTRY_HEAD;
    while (self != NULL) {
        // Capture next before pumping: pump() may finalize self, which unlinks
        // it from the registry (but leaves our saved next pointer valid).
        audiofilewriter_audiofilewriter_obj_t *next = self->reg_next;
        audiofilewriter_pump(self);
        self = next;
    }
}

// Called during soft reset (VM teardown). Any writer still active is abandoned:
// we don't try to touch its file (it may already be gone), we just balance the
// tick-enable count and drop it from the list.
void audiofilewriter_reset(void) {
    audiofilewriter_audiofilewriter_obj_t *self = REGISTRY_HEAD;
    while (self != NULL) {
        audiofilewriter_audiofilewriter_obj_t *next = self->reg_next;
        if (self->playing) {
            self->playing = false;
            supervisor_disable_tick();
        }
        self->reg_next = NULL;
        self = next;
    }
    MP_STATE_VM(audiofilewriter_linked_list) = NULL;
}

// ---------------------------------------------------------------------------
// common-hal surface
// ---------------------------------------------------------------------------

void common_hal_audiofilewriter_audiofilewriter_construct(audiofilewriter_audiofilewriter_obj_t *self,
    mp_obj_t file, uint32_t buffer_size) {
    // The file must be a writable, seekable binary stream (a file or BytesIO).
    mp_get_stream_raise(file, MP_STREAM_OP_WRITE | MP_STREAM_OP_IOCTL);

    self->file = file;
    self->sample = MP_OBJ_NULL;
    self->ring_size = buffer_size;
    self->ring = m_malloc_without_collect(buffer_size);
    self->ring_head = 0;
    self->ring_tail = 0;
    self->ring_count = 0;
    self->playing = false;
    self->source_done = false;
    self->reg_next = NULL;
}

bool common_hal_audiofilewriter_audiofilewriter_deinited(audiofilewriter_audiofilewriter_obj_t *self) {
    return self->ring == NULL;
}

void common_hal_audiofilewriter_audiofilewriter_deinit(audiofilewriter_audiofilewriter_obj_t *self) {
    if (self->playing) {
        common_hal_audiofilewriter_audiofilewriter_stop(self);
    }
    self->ring = NULL;
    self->file = MP_OBJ_NULL;
    self->sample = MP_OBJ_NULL;
}

void common_hal_audiofilewriter_audiofilewriter_play(audiofilewriter_audiofilewriter_obj_t *self, mp_obj_t sample_obj) {
    if (self->playing) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Already in progress"));
    }

    audiosample_base_t *sample = audiosample_check(sample_obj);
    uint32_t rate = audiosample_get_sample_rate(sample);
    uint8_t channels = audiosample_get_channel_count(sample);
    uint8_t bits = audiosample_get_bits_per_sample(sample);
    bool single_buffer, samples_signed;
    uint32_t max_buffer_length;
    uint8_t spacing;
    audiosample_get_buffer_structure(sample, false, &single_buffer, &samples_signed,
        &max_buffer_length, &spacing);

    if ((bits != 8 && bits != 16) || channels < 1 || channels > 2) {
        mp_raise_ValueError(MP_ERROR_TEXT("Only 8/16-bit mono/stereo is supported"));
    }
    if (max_buffer_length == 0 || self->ring_size < max_buffer_length) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer_size too small for source"));
    }

    self->sample_rate = rate;
    self->channel_count = channels;
    self->bits_per_sample = bits;
    self->samples_signed = samples_signed;
    self->bytes_per_frame = (uint8_t)(channels * (bits / 8));
    self->source_max_buffer = max_buffer_length;

    // Remember where the header starts so stop() can patch its size fields,
    // then write a placeholder header with zeroed sizes.
    int err = 0;
    mp_off_t off = mp_stream_seek(self->file, 0, MP_SEEK_CUR, &err);
    if (off == (mp_off_t)-1) {
        mp_raise_OSError(err ? err : MP_EIO);
    }
    self->header_offset = (uint32_t)off;

    uint32_t block_align = self->bytes_per_frame;
    uint32_t byte_rate = rate * block_align;
    uint8_t hdr[44];
    memcpy(hdr + 0, "RIFF", 4);
    put_u32le(hdr + 4, 36);              // RIFF size (patched at stop)
    memcpy(hdr + 8, "WAVE", 4);
    memcpy(hdr + 12, "fmt ", 4);
    put_u32le(hdr + 16, 16);             // fmt chunk size
    put_u16le(hdr + 20, 1);              // PCM
    put_u16le(hdr + 22, channels);
    put_u32le(hdr + 24, rate);
    put_u32le(hdr + 28, byte_rate);
    put_u16le(hdr + 32, (uint16_t)block_align);
    put_u16le(hdr + 34, bits);
    memcpy(hdr + 36, "data", 4);
    put_u32le(hdr + 40, 0);              // data size (patched at stop)

    err = 0;
    mp_uint_t wrote = mp_stream_write_exactly(self->file, hdr, sizeof(hdr), &err);
    if (err != 0 || wrote != sizeof(hdr)) {
        mp_raise_OSError(err ? err : MP_EIO);
    }

    audiosample_reset_buffer(sample_obj, false, 0);

    self->sample = sample_obj;
    self->ring_head = 0;
    self->ring_tail = 0;
    self->ring_count = 0;
    self->budget_frames = 0;
    self->data_bytes = 0;
    self->source_done = false;
    self->last_tick_ms = supervisor_ticks_ms64();
    self->playing = true;

    audiofilewriter_register(self);
    supervisor_enable_tick();
}

void common_hal_audiofilewriter_audiofilewriter_stop(audiofilewriter_audiofilewriter_obj_t *self) {
    if (!self->playing) {
        return;
    }
    // Keep the background pump out while we finalize from Python context.
    background_callback_prevent();
    audiofilewriter_finalize(self);
    background_callback_allow();
}

bool common_hal_audiofilewriter_audiofilewriter_get_playing(audiofilewriter_audiofilewriter_obj_t *self) {
    return self->playing;
}

MP_REGISTER_ROOT_POINTER(mp_obj_t audiofilewriter_linked_list);
