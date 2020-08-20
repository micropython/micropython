/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#include "py/runtime.h"
#include "supervisor/filesystem.h"
#include "supervisor/shared/stack.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#if CIRCUITPY_DISPLAYIO
#include "shared-module/displayio/__init__.h"
#endif

static bool running_background_tasks = false;

void background_tasks_reset(void) {
    running_background_tasks = false;
}

void run_background_tasks(void) {
    // Don't call ourselves recursively.
    if (running_background_tasks) {
        return;
    }

    // Delay for 1 tick so that we don't starve the idle task.
    // TODO: 1 tick is 10ms which is a long time! Can we delegate to idle for a minimal amount of
    // time?
    vTaskDelay(1);
    running_background_tasks = true;
    filesystem_background();

    // #if CIRCUITPY_DISPLAYIO
    // displayio_background();
    // #endif
    running_background_tasks = false;

    assert_heap_ok();
}
