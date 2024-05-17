// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_MODULE_GETPASS___INIT___H
#define MICROPY_INCLUDED_SHARED_MODULE_GETPASS___INIT___H

#include "py/runtime.h"

extern mp_obj_t shared_module_getpass_getpass(const char *prompt, mp_print_t *print);

#endif  // MICROPY_INCLUDED_SHARED_MODULE_GETPASS___INIT___H
