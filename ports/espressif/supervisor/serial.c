/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
