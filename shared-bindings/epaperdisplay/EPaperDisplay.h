// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

#include "shared-module/epaperdisplay/EPaperDisplay.h"
#include "shared-module/displayio/Group.h"

extern const mp_obj_type_t epaperdisplay_epaperdisplay_type;

#define NO_COMMAND 0x100

void common_hal_epaperdisplay_epaperdisplay_construct(epaperdisplay_epaperdisplay_obj_t *self,
    mp_obj_t bus, const uint8_t *start_sequence, uint16_t start_sequence_len, mp_float_t start_up_time,
    const uint8_t *stop_sequence, uint16_t stop_sequence_len,
    uint16_t width, uint16_t height, uint16_t ram_width, uint16_t ram_height,
    int16_t colstart, int16_t rowstart, uint16_t rotation,
    uint16_t set_column_window_command, uint16_t set_row_window_command,
    uint16_t set_current_column_command, uint16_t set_current_row_command,
    uint16_t write_black_ram_command, bool black_bits_inverted,
    uint16_t write_color_ram_command, bool color_bits_inverted, uint32_t highlight_color,
    const uint8_t *refresh_sequence, uint16_t refresh_sequence_len, mp_float_t refresh_time,
    const mcu_pin_obj_t *busy_pin, bool busy_state, mp_float_t seconds_per_frame,
    bool always_toggle_chip_select, bool grayscale, bool acep, bool two_byte_sequence_length,
    bool address_little_endian);

bool common_hal_epaperdisplay_epaperdisplay_refresh(epaperdisplay_epaperdisplay_obj_t *self);

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_root_group(epaperdisplay_epaperdisplay_obj_t *self);
bool common_hal_epaperdisplay_epaperdisplay_set_root_group(epaperdisplay_epaperdisplay_obj_t *self, displayio_group_t *root_group);

// Returns time in milliseconds.
uint32_t common_hal_epaperdisplay_epaperdisplay_get_time_to_refresh(epaperdisplay_epaperdisplay_obj_t *self);
bool common_hal_epaperdisplay_epaperdisplay_get_busy(epaperdisplay_epaperdisplay_obj_t *self);

uint16_t common_hal_epaperdisplay_epaperdisplay_get_width(epaperdisplay_epaperdisplay_obj_t *self);
uint16_t common_hal_epaperdisplay_epaperdisplay_get_height(epaperdisplay_epaperdisplay_obj_t *self);
uint16_t common_hal_epaperdisplay_epaperdisplay_get_rotation(epaperdisplay_epaperdisplay_obj_t *self);
void common_hal_epaperdisplay_epaperdisplay_set_rotation(epaperdisplay_epaperdisplay_obj_t *self, int rotation);

mp_obj_t common_hal_epaperdisplay_epaperdisplay_get_bus(epaperdisplay_epaperdisplay_obj_t *self);
