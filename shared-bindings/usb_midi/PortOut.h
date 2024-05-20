// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-module/usb_midi/PortOut.h"

extern const mp_obj_type_t usb_midi_portout_type;

// Write characters. len is in characters NOT bytes!
extern size_t common_hal_usb_midi_portout_write(usb_midi_portout_obj_t *self,
    const uint8_t *data, size_t len, int *errcode);

extern bool common_hal_usb_midi_portout_ready_to_tx(usb_midi_portout_obj_t *self);
