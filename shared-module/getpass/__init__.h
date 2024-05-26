// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 microDev
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/runtime.h"

extern mp_obj_t shared_module_getpass_getpass(const char *prompt, mp_print_t *print);
