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
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/StatusBar.h"
#include "supervisor/background_callback.h"
#include "supervisor/serial.h"
#include "supervisor/shared/status_bar.h"

#if CIRCUITPY_TERMINALIO
#include "shared-module/terminalio/Terminal.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/web_workflow.h"
#endif

#if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_SERIAL_BLE
#include "supervisor/shared/bluetooth/bluetooth.h"
#endif

static background_callback_t status_bar_background_cb;

static bool _forced_dirty = false;
static bool _suspended = false;

void supervisor_status_bar_init(void) {
    shared_module_supervisor_status_bar_obj.console = true;
    shared_module_supervisor_status_bar_obj.display = true;
    shared_module_supervisor_status_bar_obj.update_in_progress = false;
    shared_module_supervisor_status_bar_obj.written = false;
}

// Clear if possible, but give up if we can't do it now.
void supervisor_status_bar_clear(void) {
    if (!_suspended) {
        supervisor_status_bar_set_update_in_progress(&shared_module_supervisor_status_bar_obj, true);
        serial_write("\x1b" "]0;" "\x1b" "\\");
        supervisor_status_bar_set_update_in_progress(&shared_module_supervisor_status_bar_obj, false);
    }
}

void supervisor_status_bar_update(void) {
    if (_suspended) {
        supervisor_status_bar_request_update(true);
        return;
    }
    _forced_dirty = false;

    supervisor_status_bar_set_update_in_progress(&shared_module_supervisor_status_bar_obj, true);

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

    supervisor_status_bar_set_update_in_progress(&shared_module_supervisor_status_bar_obj, false);
}

static void status_bar_background(void *data) {
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

    if (dirty) {
        supervisor_status_bar_update();
    }
}

void supervisor_status_bar_start(void) {
    status_bar_background_cb.fun = status_bar_background;
    status_bar_background_cb.data = NULL;
    supervisor_status_bar_request_update(true);
}

void supervisor_status_bar_request_update(bool force_dirty) {
    if (force_dirty) {
        _forced_dirty = true;
    }
    background_callback_add_core(&status_bar_background_cb);
}

void supervisor_status_bar_suspend(void) {
    _suspended = true;
}

void supervisor_status_bar_resume(void) {
    _suspended = false;
    supervisor_status_bar_request_update(false);
}
