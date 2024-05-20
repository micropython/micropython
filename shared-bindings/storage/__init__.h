// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "shared-module/storage/__init__.h"

void common_hal_storage_mount(mp_obj_t vfs_obj, const char *path, bool readonly);
void common_hal_storage_umount_path(const char *path);
void common_hal_storage_umount_object(mp_obj_t vfs_obj);
void common_hal_storage_remount(const char *path, bool readonly, bool disable_concurrent_write_protection);
mp_obj_t common_hal_storage_getmount(const char *path);
void common_hal_storage_erase_filesystem(bool extended);

bool common_hal_storage_disable_usb_drive(void);
bool common_hal_storage_enable_usb_drive(void);
