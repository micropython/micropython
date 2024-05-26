// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_midi/PortIn.h"

extern const mp_obj_type_t usb_midi_portin_type;

// Read characters.
extern size_t common_hal_usb_midi_portin_read(usb_midi_portin_obj_t *self,
    uint8_t *data, size_t len, int *errcode);

extern uint32_t common_hal_usb_midi_portin_bytes_available(usb_midi_portin_obj_t *self);
extern void common_hal_usb_midi_portin_clear_buffer(usb_midi_portin_obj_t *self);
