/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Michael Schroeder
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_RUNTIME_STATUS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_RUNTIME_STATUS_H

#include <stdbool.h>
#include "py/obj.h"

#include "shared-bindings/supervisor/RunReason.h"

extern const mp_obj_type_t supervisor_runtime_type;

supervisor_run_reason_t supervisor_get_run_reason(void);
void supervisor_set_run_reason(supervisor_run_reason_t run_reason);

bool common_hal_supervisor_runtime_get_serial_connected(void);

bool common_hal_supervisor_runtime_get_serial_bytes_available(void);

// TODO: placeholders for future functions
// bool common_hal_get_supervisor_runtime_repl_active(void);
// bool common_hal_get_supervisor_runtime_usb_enumerated(void);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_SUPERVISOR_RUNTIME_H
