// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/enum.h"
#include "py/obj.h"

typedef enum displayio_colorspace {
    DISPLAYIO_COLORSPACE_RGB888,
    DISPLAYIO_COLORSPACE_RGB565,
    DISPLAYIO_COLORSPACE_RGB555,
    DISPLAYIO_COLORSPACE_RGB565_SWAPPED,
    DISPLAYIO_COLORSPACE_RGB555_SWAPPED,
    DISPLAYIO_COLORSPACE_BGR565,
    DISPLAYIO_COLORSPACE_BGR555,
    DISPLAYIO_COLORSPACE_BGR565_SWAPPED,
    DISPLAYIO_COLORSPACE_BGR555_SWAPPED,
    DISPLAYIO_COLORSPACE_L8,
} displayio_colorspace_t;

void common_hal_displayio_release_displays(void);

extern const mp_obj_type_t displayio_colorspace_type;
extern const cp_enum_obj_t displayio_colorspace_RGB888_obj;


// Used in the various bus displays: BusDisplay, EPaperDisplay and ParallelDisplay
// Supported by shared-module/displayio/bus_core.c
typedef enum {
    DISPLAY_COMMAND,
    DISPLAY_DATA
} display_byte_type_t;

typedef enum {
    CHIP_SELECT_UNTOUCHED,
    CHIP_SELECT_TOGGLE_EVERY_BYTE
} display_chip_select_behavior_t;

typedef bool (*display_bus_bus_reset)(mp_obj_t bus);
typedef bool (*display_bus_bus_free)(mp_obj_t bus);
typedef bool (*display_bus_begin_transaction)(mp_obj_t bus);
typedef void (*display_bus_send)(mp_obj_t bus, display_byte_type_t byte_type,
    display_chip_select_behavior_t chip_select, const uint8_t *data, uint32_t data_length);
typedef void (*display_bus_end_transaction)(mp_obj_t bus);
typedef void (*display_bus_collect_ptrs)(mp_obj_t bus);
