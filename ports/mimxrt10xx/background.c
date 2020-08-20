/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Artur Pacholec
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

//#include "audio_dma.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/tick.h"
#include "supervisor/usb.h"

#include "py/runtime.h"
#include "shared-module/network/__init__.h"
#include "supervisor/linker.h"
#include "supervisor/shared/stack.h"

#ifdef CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

volatile uint64_t last_finished_tick = 0;

bool stack_ok_so_far = true;

static bool running_background_tasks = false;

void background_tasks_reset(void) {
    running_background_tasks = false;
}

void PLACE_IN_ITCM(run_background_tasks)(void) {
    // Don't call ourselves recursively.
    if (running_background_tasks) {
        return;
    }
    assert_heap_ok();
    running_background_tasks = true;

    #if CIRCUITPY_AUDIOIO || CIRCUITPY_AUDIOBUSIO
    audio_dma_background();
    #endif
    #if CIRCUITPY_DISPLAYIO
    displayio_background();
    #endif

    #if CIRCUITPY_NETWORK
    network_module_background();
    #endif
    filesystem_background();
    usb_background();
    running_background_tasks = false;
    assert_heap_ok();

    last_finished_tick = supervisor_ticks_ms64();
}

bool background_tasks_ok(void) {
    return supervisor_ticks_ms64() - last_finished_tick < 1000;
}
