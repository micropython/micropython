/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Josef Gajdusek
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <string.h>
#include <stdarg.h>

#include "py/mpprint.h"
#include "py/mpstate.h"
#include "py/runtime.h"
#include "shared-bindings/warnings/__init__.h"
#include "shared-module/warnings/__init__.h"

void common_hal_warnings_warn(const char *message, const mp_obj_type_t *category) {
    warnings_action_t action = MP_STATE_THREAD(warnings_action);
    if (action == WARNINGS_IGNORE) {
        return;
    }
    if (action == WARNINGS_ERROR) {
        mp_raise_msg_str(category, message);
        // Doesn't get here
        return;
    }
    mp_printf(MICROPY_ERROR_PRINTER, "%q: %s\n", category->name, message);
}

void warnings_warn(const mp_obj_type_t *category, mp_rom_error_text_t message, ...) {
    warnings_action_t action = MP_STATE_THREAD(warnings_action);
    if (action == WARNINGS_IGNORE) {
        return;
    }
    va_list argptr;
    va_start(argptr, message);
    if (action == WARNINGS_ERROR) {
        mp_raise_msg_vlist(category, message, argptr);
        va_end(argptr);
        // Doesn't get here
        return;
    }

    mp_printf(MICROPY_ERROR_PRINTER, "%q: ", category->name);
    mp_vcprintf(MICROPY_ERROR_PRINTER, message, argptr);
    mp_print_str(MICROPY_ERROR_PRINTER, "\n");
    va_end(argptr);
}

void common_hal_warnings_simplefilter(warnings_action_t action) {
    MP_STATE_THREAD(warnings_action) = action;
}
