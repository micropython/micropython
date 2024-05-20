// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2024 CDarius
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"

#include "common-hal/digitalio/DigitalInOut.h"
#include "shared-module/keypad/__init__.h"
#include "shared-module/keypad/EventQueue.h"

typedef struct {
    KEYPAD_SCANNER_COMMON_FIELDS;
    mp_obj_tuple_t *row_addr_digitalinouts;
    mp_obj_tuple_t *column_digitalinouts;
} keypad_demux_demuxkeymatrix_obj_t;

void keypad_demux_demuxkeymatrix_scan(keypad_demux_demuxkeymatrix_obj_t *self);
void demuxkeymatrix_never_reset(keypad_demux_demuxkeymatrix_obj_t *self);
