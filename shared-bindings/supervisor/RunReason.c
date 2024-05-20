// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/enum.h"

#include "shared-bindings/supervisor/RunReason.h"

MAKE_ENUM_VALUE(supervisor_run_reason_type, run_reason, STARTUP, RUN_REASON_STARTUP);
MAKE_ENUM_VALUE(supervisor_run_reason_type, run_reason, AUTO_RELOAD, RUN_REASON_AUTO_RELOAD);
MAKE_ENUM_VALUE(supervisor_run_reason_type, run_reason, SUPERVISOR_RELOAD, RUN_REASON_SUPERVISOR_RELOAD);
MAKE_ENUM_VALUE(supervisor_run_reason_type, run_reason, REPL_RELOAD, RUN_REASON_REPL_RELOAD);

//| class RunReason:
//|     """The reason that CircuitPython started running."""
//|
//|     STARTUP: object
//|     """CircuitPython started the microcontroller started up. See `microcontroller.Processor.reset_reason`
//|        for more detail on why the microcontroller was started."""
//|
//|     AUTO_RELOAD: object
//|     """CircuitPython restarted due to an external write to the filesystem."""
//|
//|     SUPERVISOR_RELOAD: object
//|     """CircuitPython restarted due to a call to `supervisor.reload()`."""
//|
//|     REPL_RELOAD: object
//|     """CircuitPython started due to the user typing CTRL-D in the REPL."""
//|
MAKE_ENUM_MAP(supervisor_run_reason) {
    MAKE_ENUM_MAP_ENTRY(run_reason, STARTUP),
    MAKE_ENUM_MAP_ENTRY(run_reason, AUTO_RELOAD),
    MAKE_ENUM_MAP_ENTRY(run_reason, SUPERVISOR_RELOAD),
    MAKE_ENUM_MAP_ENTRY(run_reason, REPL_RELOAD),
};
static MP_DEFINE_CONST_DICT(supervisor_run_reason_locals_dict, supervisor_run_reason_locals_table);

MAKE_PRINTER(supervisor, supervisor_run_reason);

MAKE_ENUM_TYPE(supervisor, RunReason, supervisor_run_reason);
