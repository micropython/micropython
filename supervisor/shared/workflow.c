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
#include "supervisor/fatfs.h"
#include "supervisor/filesystem.h"
#include "supervisor/workflow.h"
#include "supervisor/shared/serial.h"
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
    bool result = supervisor_start_web_workflow();
    if (workflow_background_cb.fun) {
        if (result) {
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

    #if CIRCUITPY_USB_KEYBOARD_WORKFLOW
    usb_keyboard_init();
    #endif
}

FRESULT supervisor_workflow_move(const char *old_path, const char *new_path) {
    const char *old_mount_path;
    const char *new_mount_path;
    fs_user_mount_t *active_mount = filesystem_for_path(old_path, &old_mount_path);
    fs_user_mount_t *new_mount = filesystem_for_path(new_path, &new_mount_path);
    if (active_mount == NULL || new_mount == NULL || active_mount != new_mount || !filesystem_native_fatfs(active_mount)) {
        return FR_NO_PATH;
    }
    if (!filesystem_lock(active_mount)) {
        return FR_WRITE_PROTECTED;
    }
    FATFS *fs = &active_mount->fatfs;

    FRESULT result = f_rename(fs, old_mount_path, new_mount_path);
    filesystem_unlock(active_mount);
    return result;
}

FRESULT supervisor_workflow_mkdir(DWORD fattime, const char *full_path) {
    const char *mount_path;
    fs_user_mount_t *active_mount = filesystem_for_path(full_path, &mount_path);
    if (active_mount == NULL || !filesystem_native_fatfs(active_mount)) {
        return FR_NO_PATH;
    }

    // If there is a mount on the directory, then the mount_path will be empty.
    if (strlen(mount_path) == 0) {
        return FR_EXIST;
    }

    // Check to see if the directory exists already. We don't care about writing
    // it if it already exists.
    FATFS *fs = &active_mount->fatfs;
    FILINFO file;
    FRESULT result = f_stat(fs, mount_path, &file);
    if (result == FR_OK) {
        return FR_EXIST;
    }

    if (!filesystem_lock(active_mount)) {
        return FR_WRITE_PROTECTED;
    }

    override_fattime(fattime);
    result = f_mkdir(fs, mount_path);
    override_fattime(0);
    filesystem_unlock(active_mount);
    return result;
}

FRESULT supervisor_workflow_mkdir_parents(DWORD fattime, char *path) {
    override_fattime(fattime);
    FRESULT result = FR_OK;
    // Make parent directories.
    for (size_t j = 1; j < strlen(path); j++) {
        if (path[j] == '/') {
            path[j] = '\0';
            result = supervisor_workflow_mkdir(fattime, path);
            path[j] = '/';
            if (result != FR_OK && result != FR_EXIST) {
                break;
            }
        }
    }
    // Make the target directory.
    if (result == FR_OK || result == FR_EXIST) {
        result = supervisor_workflow_mkdir(fattime, path);
        // This may return FR_EXIST when a file with the same name already exists.
        // FATFS does the same thing.
    }
    override_fattime(0);
    return result;
}

STATIC FRESULT supervisor_workflow_delete_directory_contents(FATFS *fs, const TCHAR *path) {
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

FRESULT supervisor_workflow_delete_recursive(const char *full_path) {
    const char *mount_path;
    fs_user_mount_t *active_mount = filesystem_for_path(full_path, &mount_path);
    if (active_mount == NULL || !filesystem_native_fatfs(active_mount)) {
        return FR_NO_PATH;
    }
    if (!filesystem_lock(active_mount)) {
        return FR_WRITE_PROTECTED;
    }
    FATFS *fs = &active_mount->fatfs;
    FILINFO file;
    FRESULT result = f_stat(fs, mount_path, &file);
    if (result == FR_OK) {
        if ((file.fattrib & AM_DIR) != 0) {
            result = supervisor_workflow_delete_directory_contents(fs, mount_path);
        }
        if (result == FR_OK) {
            result = f_unlink(fs, mount_path);
        }
    }
    filesystem_unlock(active_mount);
    return result;
}
