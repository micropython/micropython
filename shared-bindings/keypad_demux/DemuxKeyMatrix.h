// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 CDarius
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objlist.h"
#include "shared-module/keypad_demux/DemuxKeyMatrix.h"

extern const mp_obj_type_t keypad_demux_demuxkeymatrix_type;

void common_hal_keypad_demux_demuxkeymatrix_construct(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t num_row_addr_pins, const mcu_pin_obj_t *row_addr_pins[], mp_uint_t num_column_pins, const mcu_pin_obj_t *column_pins[], mp_float_t interval, size_t max_events, uint8_t debounce_threshold);

void common_hal_keypad_demux_demuxkeymatrix_deinit(keypad_demux_demuxkeymatrix_obj_t *self);

void common_hal_keypad_demux_demuxkeymatrix_key_number_to_row_column(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t key_number, mp_uint_t *row, mp_uint_t *column);
mp_uint_t common_hal_keypad_demux_demuxkeymatrix_row_column_to_key_number(keypad_demux_demuxkeymatrix_obj_t *self, mp_uint_t row, mp_uint_t column);

size_t common_hal_keypad_demux_demuxkeymatrix_get_column_count(keypad_demux_demuxkeymatrix_obj_t *self);
size_t common_hal_keypad_demux_demuxkeymatrix_get_row_count(keypad_demux_demuxkeymatrix_obj_t *self);
