// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"

#include "shared-bindings/supervisor/__init__.h"
#include "shared-bindings/supervisor/Runtime.h"
#include "shared-bindings/supervisor/StatusBar.h"

// The singleton supervisor.Runtime object, bound to supervisor.runtime
const super_runtime_obj_t common_hal_supervisor_runtime_obj = {
    .base = {
        .type = &supervisor_runtime_type,
    },
};

// The singleton supervisor.StatusBar object, bound to supervisor.status_bar
supervisor_status_bar_obj_t shared_module_supervisor_status_bar_obj = {
    .base = {
        .type = &supervisor_status_bar_type,
    },
};

// String of the last traceback.
char *prev_traceback_string = NULL;

// Custom settings for next code run.
supervisor_next_code_info_t *next_code_configuration = NULL;

#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_IDENTIFICATION
// Custom USB settings.
usb_identification_t *custom_usb_identification = NULL;
#endif
