// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>

void supervisor_bluetooth_file_transfer_background(void);
void supervisor_start_bluetooth_file_transfer(void);
void supervisor_bluetooth_file_transfer_disconnected(void);
