// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/mpconfig.h"
#include "supervisor/shared/serial.h"

#if CIRCUITPY_ESP_USB_SERIAL_JTAG && CIRCUITPY_CONSOLE_UART
#error CIRCUITPY_ESP_USB_SERIAL_JTAG and CIRCUITPY_CONSOLE_UART cannot both be enabled.
#endif

#if CIRCUITPY_ESP_USB_SERIAL_JTAG
#include "supervisor/usb_serial_jtag.h"
#endif

void port_serial_init(void) {
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    usb_serial_jtag_init();
    #endif
}


bool port_serial_connected(void) {
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    return usb_serial_jtag_connected();
    #else
    return false;
    #endif
}

char port_serial_read(void) {
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    if (usb_serial_jtag_bytes_available() > 0) {
        return usb_serial_jtag_read_char();
    }
    #endif
    return -1;
}

uint32_t port_serial_bytes_available(void) {
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    return usb_serial_jtag_bytes_available();
    #else
    return 0;
    #endif
}

void port_serial_write_substring(const char *text, uint32_t length) {
    #if CIRCUITPY_ESP_USB_SERIAL_JTAG
    usb_serial_jtag_write(text, length);
    #endif
}
