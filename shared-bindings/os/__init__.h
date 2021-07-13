/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_OS___INIT___H
#define MICROPY_INCLUDED_SHARED_BINDINGS_OS___INIT___H

#include <stdint.h>
#include <stdbool.h>

#include "py/objtuple.h"

extern const mp_rom_obj_tuple_t common_hal_os_uname_info_obj;

mp_obj_t common_hal_os_uname(void);
void common_hal_os_chdir(const char *path);
mp_obj_t common_hal_os_getcwd(void);
mp_obj_t common_hal_os_listdir(const char *path);
void common_hal_os_mkdir(const char *path);
void common_hal_os_remove(const char *path);
void common_hal_os_rename(const char *old_path, const char *new_path);
void common_hal_os_rmdir(const char *path);
mp_obj_t common_hal_os_stat(const char *path);
mp_obj_t common_hal_os_statvfs(const char *path);

// Returns true if data was correctly sourced from a true random number generator.
bool common_hal_os_urandom(uint8_t *buffer, mp_uint_t length);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_OS___INIT___H
