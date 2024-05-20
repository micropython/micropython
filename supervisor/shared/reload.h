// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

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
    AUTORELOAD_SUSPEND_USB = 0x4,
    AUTORELOAD_SUSPEND_WEB = 0x8
};

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
