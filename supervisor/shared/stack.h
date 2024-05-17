// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void stack_init(void);
// Actual stack location and size, may be larger than requested.
uint32_t *stack_get_bottom(void);
size_t stack_get_length(void);
bool stack_ok(void);

// Use this after any calls into a library which may use a lot of stack. This will raise a Python
// exception when the stack has likely overwritten a portion of the heap.
void assert_heap_ok(void);

#ifndef STACK_CANARY_VALUE
#define STACK_CANARY_VALUE 0x017829ef
#endif
