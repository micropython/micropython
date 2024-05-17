// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

#include "extmod/vfs_fat.h"

extern volatile bool filesystem_flush_requested;

void filesystem_background(void);
void filesystem_tick(void);
bool filesystem_init(bool create_allowed, bool force_create);
void filesystem_flush(void);
bool filesystem_present(void);
void filesystem_set_internal_writable_by_usb(bool usb_writable);
void filesystem_set_internal_concurrent_write_protection(bool concurrent_write_protection);
void filesystem_set_writable_by_usb(fs_user_mount_t *vfs, bool usb_writable);
void filesystem_set_concurrent_write_protection(fs_user_mount_t *vfs, bool concurrent_write_protection);

// Whether user code can modify the filesystem. It doesn't depend on the state
// of USB. Don't use this for a workflow. In workflows, grab the shared file
// system lock.
bool filesystem_is_writable_by_python(fs_user_mount_t *vfs);

// This controls whether USB tries to grab the underlying block device lock
// during enumeration. If another workflow is modifying the filesystem when this
// happens, then USB will be readonly.
bool filesystem_is_writable_by_usb(fs_user_mount_t *vfs);

fs_user_mount_t *filesystem_circuitpy(void);
fs_user_mount_t *filesystem_for_path(const char *path_in, const char **path_under_mount);
bool filesystem_native_fatfs(fs_user_mount_t *fs_mount);

// We have two levels of locking. filesystem_* calls grab a shared blockdev lock to allow
// CircuitPython's fatfs code to edit the blocks. blockdev_* calls grab a lock to mutate blocks
// directly, excluding any filesystem_* locks.

bool filesystem_lock(fs_user_mount_t *fs_mount);
void filesystem_unlock(fs_user_mount_t *fs_mount);

bool blockdev_lock(fs_user_mount_t *fs_mount);
void blockdev_unlock(fs_user_mount_t *fs_mount);
