// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

void supervisor_bluetooth_background(void);
void supervisor_bluetooth_init(void);
void supervisor_start_bluetooth(void);
void supervisor_stop_bluetooth(void);

// Enable only works if it hasn't been set yet.
void supervisor_bluetooth_enable_workflow(void);
void supervisor_bluetooth_disable_workflow(void);
bool supervisor_bluetooth_workflow_is_enabled(void);

// Title bar status
bool supervisor_bluetooth_status_dirty(void);
void supervisor_bluetooth_status(void);
