// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

typedef enum {
    RUN_REASON_STARTUP,
    RUN_REASON_AUTO_RELOAD,
    RUN_REASON_SUPERVISOR_RELOAD,
    RUN_REASON_REPL_RELOAD,
} supervisor_run_reason_t;

extern const mp_obj_type_t supervisor_run_reason_type;
