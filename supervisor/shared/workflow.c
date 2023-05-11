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
#include "py/mpstate.h"
#include "py/stackctrl.h"
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
static background_callback_t workflow_background_cb = {NULL, NULL};
#endif


// Called during a VM reset. Doesn't actually reset things.
void supervisor_workflow_reset(void) {
    #if CIRCUITPY_BLEIO
    supervisor_start_bluetooth();
    #endif

    #if CIRCUITPY_WEB_WORKFLOW
    if (workflow_background_cb.fun) {
        if (supervisor_start_web_workflow()) {
            supervisor_workflow_request_background();
        }
    }
    #endif
}

void supervisor_workflow_request_background(void) {
    #if CIRCUITPY_WEB_WORKFLOW
    if (workflow_background_cb.fun) {
        workflow_background_cb.data = NULL;
        background_callback_add_core(&workflow_background_cb);
    } else {
        // Unblock polling thread if necessary
        socketpool_socket_poll_resume();
    }
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
    if (supervisor_start_web_workflow()) {
        // Enable background callbacks if web_workflow startup successful
        memset(&workflow_background_cb, 0, sizeof(workflow_background_cb));
        workflow_background_cb.fun = supervisor_web_workflow_background;
    }
    #endif

}

FRESULT supervisor_workflow_mkdir_parents(FATFS *fs, char *path) {
    FRESULT result = FR_OK;
    // Make parent directories.
    for (size_t j = 1; j < strlen(path); j++) {
        if (path[j] == '/') {
            path[j] = '\0';
            result = f_mkdir(fs, path);
            path[j] = '/';
            if (result != FR_OK && result != FR_EXIST) {
                return result;
            }
        }
    }
    // Make the target directory.
    return f_mkdir(fs, path);
}

FRESULT supervisor_workflow_delete_directory_contents(FATFS *fs, const TCHAR *path) {
    FF_DIR dir;
    FILINFO file_info;
    // Check the stack since we're putting paths on it.
    if (mp_stack_usage() >= MP_STATE_THREAD(stack_limit)) {
        return FR_INT_ERR;
    }
    FRESULT res = FR_OK;
    while (res == FR_OK) {
        res = f_opendir(fs, &dir, path);
        if (res != FR_OK) {
            break;
        }
        res = f_readdir(&dir, &file_info);
        // We close and reopen the directory every time since we're deleting
        // entries and it may invalidate the directory handle.
        f_closedir(&dir);
        if (res != FR_OK || file_info.fname[0] == '\0') {
            break;
        }
        size_t pathlen = strlen(path);
        size_t fnlen = strlen(file_info.fname);
        TCHAR full_path[pathlen + 1 + fnlen];
        memcpy(full_path, path, pathlen);
        full_path[pathlen] = '/';
        size_t full_pathlen = pathlen + 1 + fnlen;
        memcpy(full_path + pathlen + 1, file_info.fname, fnlen);
        full_path[full_pathlen] = '\0';
        if ((file_info.fattrib & AM_DIR) != 0) {
            res = supervisor_workflow_delete_directory_contents(fs, full_path);
        }
        if (res != FR_OK) {
            break;
        }
        res = f_unlink(fs, full_path);
    }
    f_closedir(&dir);
    return res;
}
