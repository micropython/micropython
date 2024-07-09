// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// Adds up to 10 pointers from the CPUs registers to regs. This is used to make sure no actively
// used heap memory is freed. Its usually implemented in assembly.
mp_uint_t cpu_get_regs_and_sp(mp_uint_t *regs);
