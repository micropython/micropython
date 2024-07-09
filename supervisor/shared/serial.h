// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "py/mpconfig.h"

#ifdef CIRCUITPY_BOOT_OUTPUT_FILE
#include "py/misc.h"

extern vstr_t *boot_output;
#endif


void serial_early_init(void);
void serial_init(void);
void serial_write(const char *text);
// Only writes up to given length. Does not check for null termination at all.
void serial_write_substring(const char *text, uint32_t length);
char serial_read(void);
uint32_t serial_bytes_available(void);
bool serial_connected(void);

// Used for temporarily suppressing output to the console or display.
bool serial_console_write_disable(bool disabled);
bool serial_display_write_disable(bool disabled);

// These have no-op versions that are weak and the port can override. They work
// in tandem with the cross-port mechanics like USB and BLE.
void port_serial_early_init(void);
void port_serial_init(void);
bool port_serial_connected(void);
char port_serial_read(void);
uint32_t port_serial_bytes_available(void);
void port_serial_write_substring(const char *text, uint32_t length);
void board_serial_early_init(void);
void board_serial_init(void);
bool board_serial_connected(void);
char board_serial_read(void);
uint32_t board_serial_bytes_available(void);
void board_serial_write_substring(const char *text, uint32_t length);

int console_uart_printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
