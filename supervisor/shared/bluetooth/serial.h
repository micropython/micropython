// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>

void supervisor_start_bluetooth_serial(void);
void supervisor_stop_bluetooth_serial(void);

bool ble_serial_connected(void);
uint32_t ble_serial_available(void);
char ble_serial_read_char(void);
void ble_serial_write(const char *text, size_t len);
void ble_serial_enable(void);
void ble_serial_disable(void);
