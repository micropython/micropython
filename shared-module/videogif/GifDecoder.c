// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/videogif/GifDecoder.h"

#include <string.h>

#include "py/mpstate.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "supervisor/port_heap.h"
#include "supervisor/shared/tick.h"

#define PLAYING_HEAD MP_STATE_VM(videogif_playing_list)

// Python code always gets at least this long between two frames, even when
// decoding falls behind the GIF's delays.
#define VIDEOGIF_MIN_GAP_MS 5

static inline void videogif_put_pixel(uint8_t *row, int x, uint16_t value) {
    memcpy(row + x * 2, &value, 2);
}

static displayio_area_t videogif_canvas(videogif_gifdecoder_obj_t *self) {
    const displayio_area_t canvas = {
        .x2 = self->gif->iCanvasWidth,
        .y2 = self->gif->iCanvasHeight,
    };
    return canvas;
}

// Fill area with the background color: the global palette entry the logical
// screen descriptor names.
static void videogif_fill(videogif_gifdecoder_obj_t *self, const displayio_area_t *area) {
    const uint8_t index = self->gif->ucBackground;
    const uint16_t value = self->gif->pPalette[index];
    for (int y = area->y1; y < area->y2; y++) {
        uint8_t *row = self->destination.base + y * self->destination.stride;
        for (int x = area->x1; x < area->x2; x++) {
            videogif_put_pixel(row, x, value);
        }
    }
}

// Clear what the next frame is drawn over: the whole canvas after a restart,
// or the last frame's rect when its disposal method is 2. Waits until the
// frame is known to exist and AnimatedGIF has read the palette.
static void videogif_start_frame(videogif_gifdecoder_obj_t *self) {
    if (self->fill_canvas) {
        const displayio_area_t canvas = videogif_canvas(self);
        videogif_fill(self, &canvas);
    } else if (self->erase_last_frame) {
        videogif_fill(self, &self->frame_area);
    }
    self->fill_canvas = false;
    self->erase_last_frame = false;
}

// AnimatedGIF calls this for each decoded line of a frame. Transparent pixels
// are skipped: the destination still holds the previous frame there.
static void videogif_draw_line(GIFDRAW *draw) {
    videogif_gifdecoder_obj_t *self = draw->pUser;
    if (self->new_frame) {
        self->new_frame = false;
        videogif_start_frame(self);
    }

    const int y = draw->iY + draw->y;
    int width = draw->iWidth;
    if (draw->iX + width > self->gif->iCanvasWidth) {
        width = self->gif->iCanvasWidth - draw->iX;
    }
    if (y >= self->gif->iCanvasHeight || width < 1) {
        return;
    }
    uint8_t *row = self->destination.base + y * self->destination.stride;
    const uint8_t *pixels = draw->pPixels;
    for (int i = 0; i < width; i++) {
        const uint8_t index = pixels[i];
        if (draw->ucHasTransparency && index == draw->ucTransparent) {
            continue;
        }
        videogif_put_pixel(row, draw->iX + i, draw->pPalette[index]);
    }
}

static int32_t videogif_read_file(GIFFILE *gif_file, uint8_t *buffer, int32_t length) {
    // A read that would reach the end stops one byte short, the same as gifio:
    // seeking stops working once the last byte is read.
    if (gif_file->iSize - gif_file->iPos < length) {
        length = gif_file->iSize - gif_file->iPos - 1;
    }
    if (length <= 0) {
        return 0;
    }
    int errcode = 0;
    mp_uint_t bytes_read = mp_stream_rw(MP_OBJ_FROM_PTR(gif_file->fHandle), buffer, length,
        &errcode, MP_STREAM_RW_ONCE);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    gif_file->iPos += bytes_read;
    return bytes_read;
}

static int32_t videogif_seek_file(GIFFILE *gif_file, int32_t position) {
    int errcode = 0;
    mp_off_t new_position = mp_stream_seek(MP_OBJ_FROM_PTR(gif_file->fHandle), position,
        MP_SEEK_SET, &errcode);
    if (new_position < 0) {
        mp_raise_OSError(errcode);
    }
    gif_file->iPos = new_position;
    return new_position;
}

// Read the GIF header from file, which is already open.
static void videogif_open_gif(GIFIMAGE *gif, mp_obj_t file) {
    gif->iError = GIF_SUCCESS;
    gif->pfnRead = videogif_read_file;
    gif->pfnSeek = videogif_seek_file;
    gif->pfnClose = NULL;
    gif->pfnOpen = NULL;
    gif->GIFFile.fHandle = MP_OBJ_TO_PTR(file);

    int errcode = 0;
    mp_off_t size = mp_stream_seek(file, 0, MP_SEEK_END, &errcode);
    if (size < 0) {
        mp_raise_OSError(errcode);
    }
    mp_stream_seek(file, 0, MP_SEEK_SET, &errcode);
    if (errcode != 0) {
        mp_raise_OSError(errcode);
    }
    gif->GIFFile.iSize = (int32_t)size;

    if (GIF_init(gif) != 1) {
        if (gif->iError == GIF_TOO_WIDE) {
            mp_raise_ValueError_varg(MP_ERROR_TEXT("%q must be <= %d"), MP_QSTR_width, MAX_WIDTH);
        }
        mp_arg_error_invalid(MP_QSTR_filename);
    }
}

void common_hal_videogif_gifdecoder_construct(videogif_gifdecoder_obj_t *self, mp_obj_t file,
    bool reverse_bytes_in_word) {
    const uint8_t palette_type = reverse_bytes_in_word ? GIF_PALETTE_RGB565_BE : GIF_PALETTE_RGB565_LE;
    // Outside the VM heap so it doesn't need a large gap there or grow it. Its
    // only heap pointers, the file and self, are held by self too. The
    // finaliser frees it.
    self->gif = port_malloc(sizeof(GIFIMAGE), false);
    if (self->gif == NULL) {
        m_malloc_fail(sizeof(GIFIMAGE));
    }
    GIF_begin(self->gif, palette_type);
    self->gif->pfnDraw = videogif_draw_line;
    self->file = file;
    videogif_open_gif(self->gif, file);
}

bool common_hal_videogif_gifdecoder_deinited(videogif_gifdecoder_obj_t *self) {
    return self->file == NULL;
}

// Tell the destination no more frames are coming, once per play.
static void videogif_end_stream(videogif_gifdecoder_obj_t *self) {
    if (self->destination.on_stream_end != NULL) {
        self->destination.on_stream_end(self->on_frame_context);
        self->destination.on_stream_end = NULL;
    }
}

void common_hal_videogif_gifdecoder_deinit(videogif_gifdecoder_obj_t *self) {
    videogif_end_stream(self);
    common_hal_videogif_gifdecoder_stop(self);
    if (self->gif != NULL) {
        port_free(self->gif);
        self->gif = NULL;
    }
    if (self->file != NULL) {
        mp_stream_close(self->file);
    }
    self->file = NULL;
}

// The VideoBuffer may still ask a deinitialized decoder for its size.
mp_int_t common_hal_videogif_gifdecoder_get_width(videogif_gifdecoder_obj_t *self) {
    return self->gif != NULL ? self->gif->iCanvasWidth : 0;
}

mp_int_t common_hal_videogif_gifdecoder_get_height(videogif_gifdecoder_obj_t *self) {
    return self->gif != NULL ? self->gif->iCanvasHeight : 0;
}

// Keep the supervisor tick on only while a frame can come due.
static void videogif_update_tick(videogif_gifdecoder_obj_t *self) {
    const bool ticking = self->playing && !self->paused && !self->finished;
    if (ticking == self->ticking) {
        return;
    }
    self->ticking = ticking;
    if (ticking) {
        supervisor_enable_tick();
    } else {
        supervisor_disable_tick();
    }
}

// Start again from the first frame as soon as possible.
static void videogif_restart(videogif_gifdecoder_obj_t *self) {
    self->restart = true;
    self->finished = false;
    self->next_frame_ms = supervisor_ticks_ms64();
    videogif_update_tick(self);
}

// No more frames: the last loop played, or a frame or the file can't be read.
static void videogif_finish(videogif_gifdecoder_obj_t *self) {
    self->finished = true;
    videogif_update_tick(self);
    videogif_end_stream(self);
}

void common_hal_videogif_gifdecoder_play(videogif_gifdecoder_obj_t *self, bool paused) {
    if (self->playing) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("already playing"));
    }
    self->playing = true;
    self->paused = paused;
    self->loops_done = 0;
    const videocore_destination_t none = {};
    self->destination = none;
    self->next = PLAYING_HEAD;
    PLAYING_HEAD = self;
    videogif_restart(self);
}

void common_hal_videogif_gifdecoder_stop(videogif_gifdecoder_obj_t *self) {
    if (!self->playing) {
        return;
    }
    self->playing = false;
    const videocore_destination_t none = {};
    self->destination = none;
    videogif_gifdecoder_obj_t **link = &PLAYING_HEAD;
    while (*link != self) {
        link = &(*link)->next;
    }
    *link = self->next;
    self->next = NULL;
    videogif_update_tick(self);
}

void common_hal_videogif_gifdecoder_pause(videogif_gifdecoder_obj_t *self) {
    self->paused = true;
    videogif_update_tick(self);
}

void common_hal_videogif_gifdecoder_resume(videogif_gifdecoder_obj_t *self) {
    // The destination may have been drawn over while paused.
    self->paused = false;
    videogif_restart(self);
}

bool common_hal_videogif_gifdecoder_set_destination(videogif_gifdecoder_obj_t *self,
    const videocore_destination_t *destination, videocore_on_frame_fun on_frame, void *context) {
    // A new destination doesn't hold the previous frame to draw the next one
    // over, so start again from the first frame.
    if (destination->base != self->destination.base || destination->stride != self->destination.stride) {
        videogif_restart(self);
    }
    self->destination = *destination;
    self->on_frame = on_frame;
    self->on_frame_context = context;
    return true;
}

// The end of a loop: start over, unless that was the last one. Like web
// browsers, a GIF without a loop count plays once and a loop count of n
// repeats it n times.
static void videogif_end_loop(videogif_gifdecoder_obj_t *self) {
    self->restart = true;
    if (self->repeat_count != 0 && ++self->loops_done > self->repeat_count) {
        videogif_finish(self);
    }
}

// Draw the next frame into the destination and report what changed.
static void videogif_step(videogif_gifdecoder_obj_t *self) {
    const displayio_area_t canvas = videogif_canvas(self);
    const bool whole_frame = self->restart;
    displayio_area_t changed = {};
    if (whole_frame) {
        self->restart = false;
        self->fill_canvas = true;
        GIF_reset(self->gif);
    } else if (self->disposal == 2) {
        // Restore to background before the next frame, if there is one.
        self->erase_last_frame = true;
        changed = self->frame_area;
    }

    self->new_frame = true;
    self->gif->iError = GIF_SUCCESS;
    int delay_ms;
    const bool more = GIF_playFrame(self->gif, &delay_ms, self) == 1;
    if (self->gif->iError == GIF_EMPTY_FRAME && !whole_frame) {
        // Data after the last frame: the loop is over without another frame,
        // so the last frame isn't disposed of and nothing changed.
        self->erase_last_frame = false;
        videogif_end_loop(self);
        return;
    }
    // A frame missing lines is broken, even when no error is set.
    if (self->gif->iError != GIF_SUCCESS || self->gif->iYCount > 0) {
        // A broken frame: stop without reporting it.
        videogif_finish(self);
        return;
    }
    // In case the frame drew no lines.
    videogif_start_frame(self);
    if (whole_frame) {
        // AnimatedGIF reads the loop count with the first frame.
        self->repeat_count = self->gif->iRepeatCount;
    }

    const displayio_area_t frame = {
        .x1 = self->gif->iX,
        .y1 = self->gif->iY,
        .x2 = self->gif->iX + self->gif->iWidth,
        .y2 = self->gif->iY + self->gif->iHeight,
    };
    if (!displayio_area_compute_overlap(&frame, &canvas, &self->frame_area)) {
        self->frame_area.x1 = self->frame_area.x2;
    }
    // The disposal method is bits 2-4 of the graphic control extension's
    // packed fields.
    self->disposal = (self->gif->ucGIFBits >> 2) & 7;
    displayio_area_union(&changed, &self->frame_area, &changed);
    if (whole_frame) {
        // A rect, not NULL: the next frame may be partial, so this one must
        // not be dropped.
        changed = canvas;
    }

    // Like web browsers, show frames with a delay of 0 or 1 centiseconds for
    // 100 ms. AnimatedGIF already reports 0 as 100 ms. Keep to the GIF's
    // timing, but when decoding falls behind, leave Python a gap first.
    if (delay_ms <= 10) {
        delay_ms = 100;
    }
    self->next_frame_ms = MAX(self->next_frame_ms + delay_ms, supervisor_ticks_ms64() + VIDEOGIF_MIN_GAP_MS);
    self->on_frame(self->on_frame_context, self->destination.base, &changed);
    if (!more) {
        videogif_end_loop(self);
    }
}

void videogif_background(void) {
    if (PLAYING_HEAD == NULL) {
        return;
    }
    const uint64_t now = supervisor_ticks_ms64();
    for (videogif_gifdecoder_obj_t *self = PLAYING_HEAD; self != NULL; self = self->next) {
        if (!self->ticking || self->destination.base == NULL || now < self->next_frame_ms) {
            continue;
        }
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {
            videogif_step(self);
            nlr_pop();
        } else {
            // The file can't be read: stop where it is.
            videogif_finish(self);
        }
    }
}

void videogif_reset(void) {
    videogif_gifdecoder_obj_t *self = PLAYING_HEAD;
    while (self != NULL) {
        videogif_gifdecoder_obj_t *next = self->next;
        self->playing = false;
        // Forget on_stream_end too: at gc_deinit the VideoBuffer may be swept first.
        const videocore_destination_t none = {};
        self->destination = none;
        self->next = NULL;
        videogif_update_tick(self);
        self = next;
    }
    PLAYING_HEAD = NULL;
}

MP_REGISTER_ROOT_POINTER(struct videogif_gifdecoder_obj *videogif_playing_list);
