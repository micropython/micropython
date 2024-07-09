// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT
#include "background.h"

#include "audio_dma.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

#include "py/runtime.h"
#include "supervisor/shared/stack.h"
#include "supervisor/port.h"

#ifdef CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#ifdef MONITOR_BACKGROUND_TASKS
// PB03 is physical pin "SCL" on the Metro M4 express
// so you can't use this code AND an i2c peripheral
// at the same time unless you change this
void port_start_background_tick(void) {
    REG_PORT_DIRSET1 = (1 << 3);
    REG_PORT_OUTSET1 = (1 << 3);
}

void port_finish_background_tick(void) {
    REG_PORT_OUTCLR1 = (1 << 3);
}
#else
void port_start_background_tick(void) {
}
void port_finish_background_tick(void) {
}
#endif

void port_background_tick(void) {
}

void port_background_task(void) {
}
