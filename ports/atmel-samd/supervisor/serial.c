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

#include "supervisor/serial.h"

#include "common-hal/usb_hid/Device.h"

#include "usb.h"
#include "genhdr/autogen_usb_descriptor.h"

// Serial number as hex characters. This writes directly to the USB
// descriptor.
void load_serial_number(void) {
    char nibble_to_hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                              'A', 'B', 'C', 'D', 'E', 'F'};
    #ifdef SAMD21
    uint32_t* addresses[4] = {(uint32_t *) 0x0080A00C, (uint32_t *) 0x0080A040,
                              (uint32_t *) 0x0080A044, (uint32_t *) 0x0080A048};
    #endif
    #ifdef SAMD51
    uint32_t* addresses[4] = {(uint32_t *) 0x008061FC, (uint32_t *) 0x00806010,
                              (uint32_t *) 0x00806014, (uint32_t *) 0x00806018};
    #endif
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            uint8_t nibble = (*(addresses[i]) >> j * 4) & 0xf;
            // Strings are UTF-16-LE encoded.
            serial_number[i * 16 + j * 2] = nibble_to_hex[nibble];
            serial_number[i * 16 + j * 2 + 1] = 0;
        }
    }
}

void serial_init(void) {
    load_serial_number();
    init_usb();
    usb_hid_init();
}

bool serial_connected(void) {
    return usb_connected();
}

char serial_read(void) {
    return usb_read();
}

bool serial_bytes_available(void) {
    return usb_bytes_available();
}

void serial_write(const char* text) {
    usb_write(text, strlen(text));
}
