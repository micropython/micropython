// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/usb.h"
#include "supervisor/shared/stack.h"

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

void port_background_task(void) {
}
void port_background_tick(void) {
}
void port_start_background_tick(void) {
}
void port_finish_background_tick(void) {
}
