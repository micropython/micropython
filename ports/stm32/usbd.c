/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Andrew Leech
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

#if MICROPY_HW_ENABLE_USBDEV && MICROPY_HW_TINYUSB_STACK

#include <string.h>
#include "mp_usbd.h"
#include "mphalport.h"

void mp_usbd_port_get_serial_number(char *serial_buf) {
    // Use the same algorithm as the ST DFU bootloader so that the serial
    // number is consistent across all USB modes.
    MP_STATIC_ASSERT(12 <= MICROPY_HW_USB_DESC_STR_MAX); // 6 derived bytes x 2 hex digits + NUL
    static const char hexdig[] = "0123456789ABCDEF";
    uint8_t *id = (uint8_t *)MP_HAL_UNIQUE_ID_ADDRESS;
    uint8_t bytes[] = {
        id[11], (uint8_t)(id[10] + id[2]), id[9],
        (uint8_t)(id[8] + id[0]), id[7], id[6],
    };
    for (int i = 0; i < 6; i++) {
        serial_buf[i * 2] = hexdig[bytes[i] >> 4];
        serial_buf[i * 2 + 1] = hexdig[bytes[i] & 0x0f];
    }
    serial_buf[12] = '\0';
}

#endif
