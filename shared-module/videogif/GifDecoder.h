// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Mikey Sklar for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "py/obj.h"

#include "lib/AnimatedGIF/AnimatedGIF_circuitpy.h"
#include "shared-module/displayio/area.h"
#include "shared-module/videocore/__init__.h"

typedef struct videogif_gifdecoder_obj {
    mp_obj_base_t base;
    // Next decoder in the list of playing decoders that the background task
    // steps (see videogif_background).
    struct videogif_gifdecoder_obj *next;
    // The open GIF file, or NULL once deinitialized.
    mp_obj_t file;
    // Where frames go, set by videocore.VideoBuffer. base is NULL until the
    // first set_destination of a play.
    videocore_destination_t destination;
    videocore_on_frame_fun on_frame;
    void *on_frame_context;
    // When the next frame is due, in supervisor milliseconds.
    uint64_t next_frame_ms;
    // The last frame's rect and disposal method, needed to restore it to the
    // background before the next frame when its disposal method is 2.
    displayio_area_t frame_area;
    // The GIF's NETSCAPE loop count (0 loops forever, -1 when there is none)
    // and how many times it has played through.
    int32_t repeat_count;
    int32_t loops_done;
    uint8_t disposal;
    bool playing;
    bool paused;
    // Played its last loop, or stopped on an error: no more frames are due.
    bool finished;
    // The supervisor tick is enabled for this decoder.
    bool ticking;
    // A frame has been shown since play. After that, a new destination or a
    // resume ends the stream instead of starting the GIF again.
    bool shown;
    // Start again from the first frame: on play, at the first destination or
    // resume of a play, and after the last frame.
    bool restart;
    // Before the first line of the next frame, fill the canvas with the
    // background color, or restore the last frame's rect to it (disposal
    // method 2). See videogif_start_frame.
    bool fill_canvas;
    bool erase_last_frame;
    // The next line drawn is the first of its frame.
    bool new_frame;
    // AnimatedGIF decoder state, freed by deinit. NULL once deinitialized.
    GIFIMAGE *gif;
} videogif_gifdecoder_obj_t;

// Step every playing decoder whose next frame is due. Runs from the
// supervisor background tick, which is enabled while any decoder has frames
// to show.
void videogif_background(void);

// Stop every playing decoder without touching its file or destination. Called
// on soft reset, before the displays and their framebuffers go away.
void videogif_reset(void);
