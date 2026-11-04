// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/videogif/GifDecoder.h"

extern const mp_obj_type_t videogif_gifdecoder_type;

// Open the GIF in file, which the constructor opened from its filename. Frames
// are RGB565.
// Raises when the file isn't a GIF or is wider than AnimatedGIF supports.
void common_hal_videogif_gifdecoder_construct(videogif_gifdecoder_obj_t *self, mp_obj_t file,
    bool reverse_bytes_in_word);
// Stop playing, free the decoder and close the file. Safe to call more than
// once.
void common_hal_videogif_gifdecoder_deinit(videogif_gifdecoder_obj_t *self);
bool common_hal_videogif_gifdecoder_deinited(videogif_gifdecoder_obj_t *self);
mp_int_t common_hal_videogif_gifdecoder_get_width(videogif_gifdecoder_obj_t *self);
mp_int_t common_hal_videogif_gifdecoder_get_height(videogif_gifdecoder_obj_t *self);

// Videosource protocol steps, driven by videocore.VideoBuffer. Frames are
// decoded by the supervisor background tick, never in an interrupt. play
// raises when the decoder is already playing; the others never raise.
void common_hal_videogif_gifdecoder_play(videogif_gifdecoder_obj_t *self, bool paused);
void common_hal_videogif_gifdecoder_stop(videogif_gifdecoder_obj_t *self);
void common_hal_videogif_gifdecoder_pause(videogif_gifdecoder_obj_t *self);
void common_hal_videogif_gifdecoder_resume(videogif_gifdecoder_obj_t *self);
bool common_hal_videogif_gifdecoder_set_destination(videogif_gifdecoder_obj_t *self,
    const videocore_destination_t *destination, videocore_on_frame_fun on_frame, void *context);
