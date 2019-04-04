/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include <string.h>

#include "py/mpconfig.h"

#include "supervisor/shared/display.h"
#include "shared-bindings/terminalio/Terminal.h"
#include "supervisor/serial.h"
#include "supervisor/usb.h"

#include "tusb.h"

void serial_init(void) {
    usb_init();
}

bool serial_connected(void) {
    return tud_cdc_connected();
}

char serial_read(void) {
    return (char) tud_cdc_read_char();
}

bool serial_bytes_available(void) {
    return tud_cdc_available() > 0;
}

void serial_write_substring(const char* text, uint32_t length) {
#if CIRCUITPY_DISPLAYIO
    int errcode;
    common_hal_terminalio_terminal_write(&supervisor_terminal, (const uint8_t*) text, length, &errcode);
#endif

    uint32_t count = 0;
    while (count < length && tud_cdc_connected()) {
        count += tud_cdc_write(text + count, length - count);
        usb_background();
    }
}

void serial_write(const char* text) {
    serial_write_substring(text, strlen(text));
}
