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

#pragma once

typedef enum {
    GETENV_OK = 0,
    ENVIRON_ERR_OPEN,
    ENVIRON_ERR_UNICODE,
    ENVIRON_ERR_LENGTH,
    ENVIRON_ERR_NOT_FOUND,
    ENVIRON_ERR_UNEXPECTED = 0xff00, // logical or'd with the byte value
} os_getenv_err_t;

// Allocation free version that returns the full length of the value.
// If it fits, the return value is 0-terminated. If the value doesn't fit,
// *value_len may be an over-estimate but never an under-estimate.
os_getenv_err_t common_hal_os_getenv_str(const char *key, char *value, size_t value_len);

// Returns ENVIRON_ERR_OK and sets value to the read value.  Returns
// ENVIRON_ERR_... if the value was not numeric. allocation-free.
os_getenv_err_t common_hal_os_environ_get_key_int(const char *key, mp_int_t *value);
