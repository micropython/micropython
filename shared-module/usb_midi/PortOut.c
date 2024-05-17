// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/usb_midi/PortOut.h"
#include "shared-module/usb_midi/PortOut.h"
#include "supervisor/shared/translate/translate.h"
#include "tusb.h"

size_t common_hal_usb_midi_portout_write(usb_midi_portout_obj_t *self, const uint8_t *data, size_t len, int *errcode) {
    return tud_midi_stream_write(0, data, len);
}

bool common_hal_usb_midi_portout_ready_to_tx(usb_midi_portout_obj_t *self) {
    return tud_midi_mounted();
}
