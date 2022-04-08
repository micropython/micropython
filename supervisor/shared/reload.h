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

#ifndef MICROPY_INCLUDED_SUPERVISOR_AUTORELOAD_H
#define MICROPY_INCLUDED_SUPERVISOR_AUTORELOAD_H

#include "supervisor/memory.h"
#include "py/obj.h"
#include "shared-bindings/supervisor/RunReason.h"

enum {
    SUPERVISOR_NEXT_CODE_OPT_RELOAD_ON_SUCCESS = 0x1,
    SUPERVISOR_NEXT_CODE_OPT_RELOAD_ON_ERROR = 0x2,
    SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_SUCCESS = 0x4,
    SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_ERROR = 0x8,
    SUPERVISOR_NEXT_CODE_OPT_STICKY_ON_RELOAD = 0x10,
    SUPERVISOR_NEXT_CODE_OPT_NEWLY_SET = 0x20,
};

enum {
    AUTORELOAD_SUSPEND_REPL = 0x1,
    AUTORELOAD_SUSPEND_BLE = 0x2,
    AUTORELOAD_SUSPEND_USB = 0x4
};

typedef struct {
    uint8_t options;
    char filename[];
} next_code_info_t;

extern supervisor_allocation *next_code_allocation;

// Helper for exiting the VM and reloading immediately.
void reload_initiate(supervisor_run_reason_t run_reason);

// Enabled state is user controllable and very sticky. We don't reset it.
void autoreload_enable(void);
void autoreload_disable(void);
bool autoreload_is_enabled(void);

// Start the autoreload process.
void autoreload_trigger(void);
// True when the autoreload should occur. (A trigger happened and the delay has
// passed.)
bool autoreload_ready(void);
// Reset the autoreload timer in preparation for another trigger. Call when the
// last trigger starts being executed.
void autoreload_reset(void);
// True when a trigger has occurred but we're still delaying in case another
// trigger occurs.
bool autoreload_pending(void);

// Temporarily turn autoreload off, for the given reason(s). Autoreload triggers
// will still be tracked so resuming with autoreload ready with cause an
// immediate reload.
// Used during the REPL or during parts of BLE workflow.
void autoreload_suspend(uint32_t suspend_reason_mask);
// Allow autoreloads again, for the given reason(s).
void autoreload_resume(uint32_t suspend_reason_mask);

#endif  // MICROPY_INCLUDED_SUPERVISOR_AUTORELOAD_H
