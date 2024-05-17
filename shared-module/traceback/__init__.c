// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#include "shared-module/traceback/__init__.h"

void shared_module_traceback_print_exception(mp_obj_exception_t *exc, mp_print_t *print, mp_int_t limit) {
    mp_obj_print_exception_with_limit(print, exc, limit);
}
