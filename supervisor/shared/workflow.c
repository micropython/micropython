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

#include <stdbool.h>
#include "py/mpconfig.h"
#include "supervisor/background_callback.h"
#include "supervisor/workflow.h"
#include "supervisor/serial.h"
#include "supervisor/shared/workflow.h"

#if CIRCUITPY_BLEIO
#include "shared-bindings/_bleio/__init__.h"
#include "supervisor/shared/bluetooth/bluetooth.h"
#endif

#if CIRCUITPY_USB
#include "supervisor/usb.h"
#include "tusb.h"
#endif

#if CIRCUITPY_WEB_WORKFLOW
#include "supervisor/shared/web_workflow/web_workflow.h"
#endif
static background_callback_t workflow_background_cb;

#if CIRCUITPY_STATUS_BAR
static void supervisor_workflow_update_status_bar(void) {
    // Neighboring "" "" are concatenated by the compiler. Without this separation, the hex code
    // doesn't get terminated after two following characters and the value is invalid.
    // This is the OSC command to set the title and the icon text. It can be up to 255 characters
    // but some may be cut off.
    serial_write("\x1b" "]0;");
    serial_write("🐍 ");
    #if CIRCUITPY_WEB_WORKFLOW
    supervisor_web_workflow_status();
    #endif
    // Send string terminator
    serial_write("\x1b" "\\");
}
#endif

static void workflow_background(void *data) {
    #if CIRCUITPY_STATUS_BAR
    supervisor_workflow_update_status_bar();
    #endif
}

// Called during a VM reset. Doesn't actually reset things.
void supervisor_workflow_reset(void) {
    #if CIRCUITPY_BLEIO
    supervisor_start_bluetooth();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    supervisor_start_web_workflow();
    #endif

    workflow_background_cb.fun = workflow_background;
    workflow_background_cb.data = NULL;
    supervisor_workflow_request_background();
}

void supervisor_workflow_request_background(void) {
    background_callback_add_core(&workflow_background_cb);
}

// Return true as soon as USB communication with host has started,
// even before enumeration is done.
// Not that some chips don't notice when USB is unplugged after first being plugged in,
// so this is not perfect, but tud_suspended() check helps.
bool supervisor_workflow_connecting(void) {
    #if CIRCUITPY_USB
    return tud_connected() && !tud_suspended();
    #else
    return false;
    #endif
}

// Return true if host has completed connection to us (such as USB enumeration).
bool supervisor_workflow_active(void) {
    #if CIRCUITPY_USB
    // Eventually there might be other non-USB workflows, such as BLE.
    // tud_ready() checks for usb mounted and not suspended.
    return tud_ready();
    #else
    return false;
    #endif
}

void supervisor_workflow_start(void) {
    // Start USB after giving boot.py a chance to tweak behavior.
    #if CIRCUITPY_USB
    // Setup USB connection after heap is available.
    // It needs the heap to build descriptors.
    usb_init();
    #endif

    // Set up any other serial connection.
    serial_init();

    #if CIRCUITPY_BLEIO
    bleio_reset();
    supervisor_bluetooth_enable_workflow();
    supervisor_start_bluetooth();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    supervisor_start_web_workflow();
    #endif
}
