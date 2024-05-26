// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"

#if MICROPY_MAX_STACK_USAGE
extern uint32_t shared_module_ustack_max_stack_usage(void);
#endif
extern uint32_t shared_module_ustack_stack_size(void);
extern uint32_t shared_module_ustack_stack_usage(void);
