/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
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

#include <stdbool.h>
#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "supervisor/background_callback.h"
#include "supervisor/serial.h"
#include "supervisor/shared/title_bar.h"

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/web_workflow.h"
#endif

#if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
#include "supervisor/shared/bluetooth/bluetooth.h"
#endif

static background_callback_t title_bar_background_cb;

static bool _forced_dirty = false;
static bool _suspended = false;


void supervisor_title_bar_update(void) {
    #if !CIRCUITPY_STATUS_BAR
    return;
    #endif
    _forced_dirty = false;
    // Neighboring "" "" are concatenated by the compiler. Without this separation, the hex code
    // doesn't get terminated after two following characters and the value is invalid.
    // This is the OSC command to set the title and the icon text. It can be up to 255 characters
    // but some may be cut off.
    serial_write("\x1b" "]0;");
    serial_write("🐍");
    #if CIRCUITPY_WEB_WORKFLOW
    supervisor_web_workflow_status();
    serial_write(" | ");
    #endif
    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    supervisor_bluetooth_status();
    serial_write(" | ");
    #endif
    supervisor_execution_status();
    serial_write(" | ");
    serial_write(MICROPY_GIT_TAG);
    // Send string terminator
    serial_write("\x1b" "\\");
}

static void title_bar_background(void *data) {
    #if !CIRCUITPY_STATUS_BAR
    return;
    #endif
    if (_suspended) {
        return;
    }
    bool dirty = _forced_dirty;

    #if CIRCUITPY_WEB_WORKFLOW
    dirty = dirty || supervisor_web_workflow_status_dirty();
    #endif

    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
    dirty = dirty || supervisor_bluetooth_status_dirty();
    #endif

    if (!dirty) {
        return;
    }
    supervisor_title_bar_update();
}

void supervisor_title_bar_start(void) {
    #if !CIRCUITPY_STATUS_BAR
    return;
    #endif
    title_bar_background_cb.fun = title_bar_background;
    title_bar_background_cb.data = NULL;
    supervisor_title_bar_request_update(true);
}

void supervisor_title_bar_request_update(bool force_dirty) {
    #if !CIRCUITPY_STATUS_BAR
    return;
    #endif
    if (force_dirty) {
        _forced_dirty = true;
    }
    background_callback_add_core(&title_bar_background_cb);
}

void supervisor_title_bar_suspend(void) {
    #if !CIRCUITPY_STATUS_BAR
    return;
    #endif
    _suspended = true;
}

void supervisor_title_bar_resume(void) {
    #if !CIRCUITPY_STATUS_BAR
    return;
    #endif
    _suspended = false;
    supervisor_title_bar_request_update(false);
}
