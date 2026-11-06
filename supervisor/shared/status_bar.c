// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <string.h>
#include "genhdr/mpversion.h"
#include "py/mpconfig.h"
#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/StatusBar.h"
#include "supervisor/background_callback.h"
#include "supervisor/shared/serial.h"
#include "supervisor/shared/status_bar.h"

#if CIRCUITPY_TERMINALIO
#include "shared-module/terminalio/Terminal.h"
#include "supervisor/shared/display.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/web_workflow.h"
#endif

#if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_BLE_SERIAL_SERVICE
#include "supervisor/shared/bluetooth/bluetooth.h"
#endif

#if CIRCUITPY_USB_KEYBOARD_WORKFLOW
#include "supervisor/usb.h"
#endif

static background_callback_t status_bar_background_cb;

static bool _forced_dirty = false;
static bool _suspended = false;

// Clear if possible, but give up if we can't do it now.
void supervisor_status_bar_clear(void) {
    if (!_suspended) {
        serial_write("\x1b" "]0;" "\x1b" "\\");
    }
}

// Write the title up to the version, with single spaces between segments.
static void write_title(void) {
    // Neighboring "..." "..." are concatenated by the compiler. Without this separation, the hex code
    // doesn't get terminated after two following characters and the value is invalid.
    // This is the OSC command to set the title and the icon text. It can be up to 255 characters
    // but some may be cut off.
    serial_write("\x1b" "]0;");
    serial_write("🐍");

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    usb_keyboard_status();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    supervisor_web_workflow_status();
    serial_write(" ");
    #endif

    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_BLE_SERIAL_SERVICE
    supervisor_bluetooth_status();
    serial_write(" ");
    #endif

    supervisor_execution_status();
    serial_write(" ");
}

void supervisor_status_bar_update(void) {
    if (_suspended) {
        supervisor_status_bar_request_update(true);
        return;
    }
    _forced_dirty = false;

    shared_module_supervisor_status_bar_updated(&shared_module_supervisor_status_bar_obj);

    // Skip the console if supervisor.status_bar.console is False or there is no serial connection.
    // This avoids sending part of the status bar update if the connection comes up during it.
    if (shared_module_supervisor_status_bar_get_console(&shared_module_supervisor_status_bar_obj) &&
        serial_connected()) {
        bool prev_display = serial_display_write_disable(true);
        write_title();
        serial_write(MICROPY_GIT_TAG);
        serial_write("\x1b" "\\");
        serial_display_write_disable(prev_display);
    }

    #if CIRCUITPY_TERMINALIO
    // Skip the display if supervisor.status_bar.display is False. A version that does not fit
    // is cut so ".." is on the last two tiles.
    displayio_tilegrid_t *bar = supervisor_terminal.status_bar;
    if (bar != NULL &&
        shared_module_supervisor_status_bar_get_display(&shared_module_supervisor_status_bar_obj)) {
        bool prev_console = serial_console_write_disable(true);
        write_title();
        size_t room = bar->width_in_tiles - supervisor_terminal.status_x;
        // The terminal moves status_y past the bar once the title reaches its last tile.
        if (supervisor_terminal.status_y >= bar->height_in_tiles || room < 2) {
            supervisor_terminal.status_x = bar->width_in_tiles - 2;
            supervisor_terminal.status_y = 0;
            room = 2;
        }
        if (room >= strlen(MICROPY_GIT_TAG)) {
            serial_write(MICROPY_GIT_TAG);
        } else {
            serial_write_substring(MICROPY_GIT_TAG, room - 2);
            serial_write("..");
        }
        serial_write("\x1b" "\\");
        serial_console_write_disable(prev_console);
    }
    #endif
}

static void status_bar_background(void *data) {
    if (_suspended) {
        return;
    }
    bool dirty = _forced_dirty;

    #if CIRCUITPY_WEB_WORKFLOW
    dirty = dirty || supervisor_web_workflow_status_dirty();
    #endif

    #if CIRCUITPY_BLE_FILE_SERVICE || CIRCUITPY_BLE_SERIAL_SERVICE
    dirty = dirty || supervisor_bluetooth_status_dirty();
    #endif

    if (dirty) {
        supervisor_status_bar_update();
    }
}

void supervisor_status_bar_start(void) {
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

void supervisor_status_bar_init(void) {
    status_bar_background_cb.fun = status_bar_background;
    status_bar_background_cb.data = NULL;

    shared_module_supervisor_status_bar_init(&shared_module_supervisor_status_bar_obj);
}
