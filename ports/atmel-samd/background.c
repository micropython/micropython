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
#include "tick.h"
#include "usb.h"
#include "usb_mass_storage.h"

#include "shared-module/displayio/__init__.h"

volatile uint64_t last_finished_tick = 0;

void run_background_tasks(void) {
    #if (defined(SAMD21) && defined(PIN_PA02)) || defined(SAMD51)
    audio_dma_background();
    #endif
    #ifdef CIRCUITPY_DISPLAYIO
    displayio_refresh_display();
    #endif
    usb_msc_background();
    usb_cdc_background();
    last_finished_tick = ticks_ms;
}

bool background_tasks_ok(void) {
    return ticks_ms - last_finished_tick < 1000;
}
