// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/mpprint.h"
#include "py/runtime.h"

void raise_deinited_error(void);
void properties_print_helper(const mp_print_t *print, mp_obj_t self_in, const mp_arg_t *properties, size_t n_properties);
void properties_construct_helper(mp_obj_t self_in, const mp_arg_t *args, const mp_arg_val_t *vals, size_t n_properties);
