// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "shared-module/usb_midi/__init__.h"

extern mp_obj_dict_t usb_midi_module_globals;

bool common_hal_usb_midi_disable(void);
bool common_hal_usb_midi_enable(void);
