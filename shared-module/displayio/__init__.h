// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#if CIRCUITPY_BUSDISPLAY
#include "shared-bindings/busdisplay/BusDisplay.h"
#endif
#if CIRCUITPY_AURORA_EPAPER
#include "shared-module/aurora_epaper/aurora_framebuffer.h"
#endif
#include "shared-bindings/displayio/Group.h"
#if CIRCUITPY_EPAPERDISPLAY
#include "shared-bindings/epaperdisplay/EPaperDisplay.h"
#endif
#if CIRCUITPY_FRAMEBUFFERIO
#include "shared-bindings/framebufferio/FramebufferDisplay.h"
#endif
#if CIRCUITPY_FOURWIRE
#include "shared-bindings/fourwire/FourWire.h"
#endif
#if CIRCUITPY_I2CDISPLAYBUS
#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"
#endif
#if CIRCUITPY_PARALLELDISPLAYBUS
#include "shared-bindings/paralleldisplaybus/ParallelBus.h"
#endif
#if CIRCUITPY_RGBMATRIX
#include "shared-bindings/rgbmatrix/RGBMatrix.h"
#endif
#if CIRCUITPY_IS31FL3741
#include "shared-bindings/is31fl3741/FrameBuffer.h"
#endif
#if CIRCUITPY_SHARPDISPLAY
#include "shared-module/sharpdisplay/SharpMemoryFramebuffer.h"
#endif
#if CIRCUITPY_DOTCLOCKFRAMEBUFFER
#include "common-hal/dotclockframebuffer/DotClockFramebuffer.h"
#endif
// Port unique frame buffers.
#if CIRCUITPY_VIDEOCORE
#include "bindings/videocore/Framebuffer.h"
#endif
#if CIRCUITPY_PICODVI
#include "bindings/picodvi/Framebuffer.h"
#endif

#pragma once

typedef struct {
    union {
        mp_obj_base_t bus_base;
        #if CIRCUITPY_FOURWIRE
        fourwire_fourwire_obj_t fourwire_bus;
        #endif
        #if CIRCUITPY_I2CDISPLAYBUS
        i2cdisplaybus_i2cdisplaybus_obj_t i2cdisplay_bus;
        #endif
        #if CIRCUITPY_PARALLELDISPLAYBUS
        paralleldisplaybus_parallelbus_obj_t parallel_bus;
        #endif
        #if CIRCUITPY_RGBMATRIX
        rgbmatrix_rgbmatrix_obj_t rgbmatrix;
        #endif
        #if CIRCUITPY_IS31FL3741
        is31fl3741_framebuffer_obj_t is31fl3741;
        #endif
        #if CIRCUITPY_SHARPDISPLAY
        sharpdisplay_framebuffer_obj_t sharpdisplay;
        #endif
        #if CIRCUITPY_VIDEOCORE
        videocore_framebuffer_obj_t videocore;
        #endif
        #if CIRCUITPY_PICODVI
        picodvi_framebuffer_obj_t picodvi;
        #endif
        #if CIRCUITPY_DOTCLOCKFRAMEBUFFER
        dotclockframebuffer_framebuffer_obj_t dotclock;
        #endif
        #if CIRCUITPY_AURORA_EPAPER
        aurora_epaper_framebuffer_obj_t aurora_epaper;
        #endif
    };
} primary_display_bus_t;

typedef struct {
    union {
        mp_obj_base_t display_base;
        #if CIRCUITPY_BUSDISPLAY
        busdisplay_busdisplay_obj_t display;
        #endif
        #if CIRCUITPY_EPAPERDISPLAY
        epaperdisplay_epaperdisplay_obj_t epaper_display;
        #endif
        #if CIRCUITPY_FRAMEBUFFERIO
        framebufferio_framebufferdisplay_obj_t framebuffer_display;
        #endif
    };
} primary_display_t;

extern primary_display_bus_t display_buses[CIRCUITPY_DISPLAY_LIMIT];
extern primary_display_t displays[CIRCUITPY_DISPLAY_LIMIT];

extern displayio_group_t circuitpython_splash;

void displayio_background(void);
void reset_displays(void);
void displayio_gc_collect(void);

primary_display_t *allocate_display(void);
primary_display_t *allocate_display_or_raise(void);
primary_display_bus_t *allocate_display_bus(void);
primary_display_bus_t *allocate_display_bus_or_raise(void);
