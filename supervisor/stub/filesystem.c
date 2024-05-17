// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/filesystem.h"


void filesystem_background(void) {
    return;
}

void filesystem_tick(void) {
    return;
}

bool filesystem_init(bool create_allowed, bool force_create) {
    (void)create_allowed;
    (void)force_create;
    return true;
}

void filesystem_flush(void) {
}

void filesystem_set_internal_writable_by_usb(bool writable) {
    (void)writable;
    return;
}

void filesystem_set_writable_by_usb(fs_user_mount_t *vfs, bool usb_writable) {
    (void)vfs;
    (void)usb_writable;
    return;
}

bool filesystem_is_writable_by_python(fs_user_mount_t *vfs) {
    (void)vfs;
    return true;
}

bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs) {
    return true;
}

void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection) {
    (void)concurrent_write_protection;
    return;
}

void filesystem_set_concurrent_write_protection(fs_user_mount_t *vfs, bool concurrent_write_protection) {
    (void)vfs;
    (void)concurrent_write_protection;
    return;
}

bool filesystem_present(void) {
    return false;
}
