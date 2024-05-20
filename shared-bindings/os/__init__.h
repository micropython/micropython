// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "py/objtuple.h"

extern const mp_rom_obj_tuple_t common_hal_os_uname_info_obj;

mp_obj_t common_hal_os_uname(void);
void common_hal_os_chdir(const char *path);
mp_obj_t common_hal_os_getcwd(void);
mp_obj_t common_hal_os_getenv(const char *key, mp_obj_t default_);
mp_obj_t common_hal_os_getenv_path(const char *path, const char *key, mp_obj_t default_);

mp_obj_t common_hal_os_listdir(const char *path);
void common_hal_os_mkdir(const char *path);
void common_hal_os_remove(const char *path);
void common_hal_os_rename(const char *old_path, const char *new_path);
void common_hal_os_rmdir(const char *path);
mp_obj_t common_hal_os_stat(const char *path);
mp_obj_t common_hal_os_statvfs(const char *path);
void common_hal_os_utime(const char *path, mp_obj_t times);

// Returns true if data was correctly sourced from a true random number generator.
bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length);
