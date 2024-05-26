// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objexcept.h"
#include "py/objtraceback.h"

extern void shared_module_traceback_print_exception(mp_obj_exception_t *exc,
    mp_print_t *print, mp_int_t limit);
