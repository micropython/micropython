// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

void usb_serial_jtag_init(void);
bool usb_serial_jtag_connected(void);
char usb_serial_jtag_read_char(void);
uint32_t usb_serial_jtag_bytes_available(void);
void usb_serial_jtag_write(const char *text, uint32_t length);
