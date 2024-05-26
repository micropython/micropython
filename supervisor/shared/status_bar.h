// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

void supervisor_status_bar_init(void);

void supervisor_status_bar_start(void);
void supervisor_status_bar_suspend(void);
void supervisor_status_bar_resume(void);

void supervisor_status_bar_clear(void);

// Update the title bar immediately. Useful from main.c where we know state has changed and the code
// will only be run once.
void supervisor_status_bar_update(void);

// Use this if requesting from the background, as code is executing or if the status may not have
// changed.
void supervisor_status_bar_request_update(bool force_dirty);

// Provided by main.c
void supervisor_execution_status(void);
