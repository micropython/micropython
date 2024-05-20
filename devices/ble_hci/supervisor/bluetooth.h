// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

void supervisor_start_bluetooth(void);
bool supervisor_bluetooth_hook(ble_evt_t *ble_evt);
void supervisor_bluetooth_background(void);
