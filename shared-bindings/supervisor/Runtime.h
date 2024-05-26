// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Michael Schroeder
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#include "shared-bindings/supervisor/RunReason.h"
#include "shared-bindings/supervisor/SafeModeReason.h"

extern const mp_obj_type_t supervisor_runtime_type;

supervisor_run_reason_t supervisor_get_run_reason(void);
void supervisor_set_run_reason(supervisor_run_reason_t run_reason);

safe_mode_t supervisor_get_safe_mode(void);
void supervisor_set_safe_mode(safe_mode_t safe_mode);

bool common_hal_supervisor_runtime_get_serial_connected(void);

uint32_t common_hal_supervisor_runtime_get_serial_bytes_available(void);

// TODO: placeholders for future functions
// bool common_hal_get_supervisor_runtime_repl_active(void);
// bool common_hal_get_supervisor_runtime_usb_enumerated(void);
