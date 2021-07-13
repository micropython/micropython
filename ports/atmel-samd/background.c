/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "background.h"

#include "audio_dma.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

#include "py/runtime.h"
#include "shared-module/network/__init__.h"
#include "supervisor/shared/stack.h"
#include "supervisor/port.h"

#ifdef CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

#ifdef MONITOR_BACKGROUND_TASKS
// PB03 is physical pin "SCL" on the Metro M4 express
// so you can't use this code AND an i2c peripheral
// at the same time unless you change this
void port_start_background_task(void) {
    REG_PORT_DIRSET1 = (1 << 3);
    REG_PORT_OUTSET1 = (1 << 3);
}

void port_finish_background_task(void) {
    REG_PORT_OUTCLR1 = (1 << 3);
}
#else
void port_start_background_task(void) {
}
void port_finish_background_task(void) {
}
#endif

void port_background_task(void) {
}
