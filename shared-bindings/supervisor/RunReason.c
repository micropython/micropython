/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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
STATIC MP_DEFINE_CONST_DICT(supervisor_run_reason_locals_dict, supervisor_run_reason_locals_table);

MAKE_PRINTER(supervisor, supervisor_run_reason);

MAKE_ENUM_TYPE(supervisor, RunReason, supervisor_run_reason);
