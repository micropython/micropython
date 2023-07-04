/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
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

#if MICROPY_HW_ENABLE_USBDEV

#include "mp_usbd.h"
#include "string.h"
#include "tusb.h"
#include "pico/unique_id.h"

void mp_usbd_port_get_serial_number(char *serial_buf) {
    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);
    // convert to hex
    int hexlen = sizeof(id.id) * 2;
    MP_STATIC_ASSERT(hexlen <= MICROPY_HW_USB_DESC_STR_MAX);
    for (int i = 0; i < hexlen; i += 2) {
        static const char *hexdig = "0123456789abcdef";
        serial_buf[i] = hexdig[id.id[i / 2] >> 4];
        serial_buf[i + 1] = hexdig[id.id[i / 2] & 0x0f];
    }
    serial_buf[hexlen] = 0;
}

#endif
