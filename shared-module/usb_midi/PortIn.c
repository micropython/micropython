// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/usb_midi/PortIn.h"
#include "shared-module/usb_midi/PortIn.h"
#include "supervisor/shared/translate/translate.h"
#include "tusb.h"

size_t common_hal_usb_midi_portin_read(usb_midi_portin_obj_t *self, uint8_t *data, size_t len, int *errcode) {
    return tud_midi_stream_read(data, len);
}

uint32_t common_hal_usb_midi_portin_bytes_available(usb_midi_portin_obj_t *self) {
    return tud_midi_available();
}
