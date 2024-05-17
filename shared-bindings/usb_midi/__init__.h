// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_USB_MIDI___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_USB_MIDI___INIT___H

#include "py/obj.h"
#include "shared-module/usb_midi/__init__.h"

extern mp_obj_dict_t usb_midi_module_globals;

bool common_hal_usb_midi_disable(void);
bool common_hal_usb_midi_enable(void);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_USB_MIDI___INIT___H
