// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_TOUCH_HANDLER_H
#define MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_TOUCH_HANDLER_H

#include "driver/touch_pad.h"

extern uint16_t peripherals_touch_read(touch_pad_t touchpad);
extern void peripherals_touch_reset(void);
extern void peripherals_touch_never_reset(const bool enable);
extern void peripherals_touch_init(const touch_pad_t touchpad);

#endif  // MICROPY_INCLUDED_ESPRESSIF_PERIPHERALS_TOUCH_HANDLER_H
