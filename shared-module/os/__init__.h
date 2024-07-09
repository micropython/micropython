// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

typedef enum {
    GETENV_OK = 0,
    GETENV_ERR_OPEN,
    GETENV_ERR_UNICODE,
    GETENV_ERR_LENGTH,
    GETENV_ERR_NOT_FOUND,
    GETENV_ERR_UNEXPECTED = 0xff00, // logical or'd with the byte value
} os_getenv_err_t;

// Allocation free version that returns the full length of the value.
// If it fits, the return value is 0-terminated. The passed in buffer
// may be modified even if an error is returned.  Allocation free.
// An error that is not 'open' or 'not found' is printed on the repl.
os_getenv_err_t common_hal_os_getenv_str(const char *key, char *value, size_t value_len);

// Returns GETENV_OK and sets value to the read value.  Returns
// GETENV_ERR_... if the value was not numeric. allocation-free.
// If any error code is returned, value is guaranteed not modified
// An error that is not 'open' or 'not found' is printed on the repl.
os_getenv_err_t common_hal_os_getenv_int(const char *key, mp_int_t *value);
