// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#pragma once

// #include "py/mpconfig.h"
#include "py/obj.h"

#include "shared-module/supervisor/Runtime.h"
#include "shared-module/supervisor/StatusBar.h"

#if CIRCUITPY_USB_DEVICE
#include "supervisor/usb.h"
#endif

typedef struct {
    uint8_t options;
    char filename[];
} supervisor_next_code_info_t;

extern const super_runtime_obj_t common_hal_supervisor_runtime_obj;
extern supervisor_status_bar_obj_t shared_module_supervisor_status_bar_obj;
extern mp_obj_t supervisor_ticks_ms(void);

extern char *prev_traceback_string;

extern supervisor_next_code_info_t *next_code_configuration;

#if CIRCUITPY_USB_DEVICE
extern usb_identification_t *custom_usb_identification;
#endif
