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

#include "shared-module/usb/utf16le.h"

STATIC void _convert_utf16le_to_utf8(const uint16_t *utf16, size_t utf16_len, uint8_t *utf8, size_t utf8_len) {
    // TODO: Check for runover.
    (void)utf8_len;

    for (size_t i = 0; i < utf16_len; i++) {
        uint16_t chr = utf16[i];
        if (chr < 0x80) {
            *utf8++ = chr & 0xff;
        } else if (chr < 0x800) {
            *utf8++ = (uint8_t)(0xC0 | (chr >> 6 & 0x1F));
            *utf8++ = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
        } else if (chr < 0x10000) {
            // TODO: Verify surrogate.
            *utf8++ = (uint8_t)(0xE0 | (chr >> 12 & 0x0F));
            *utf8++ = (uint8_t)(0x80 | (chr >> 6 & 0x3F));
            *utf8++ = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
        } else {
            // TODO: Handle UTF-16 code points that take two entries.
            uint32_t hc = ((chr & 0xFFFF0000) - 0xD8000000) >> 6;    /* Get high 10 bits */
            chr = (chr & 0xFFFF) - 0xDC00;                  /* Get low 10 bits */
            chr = (hc | chr) + 0x10000;
            *utf8++ = (uint8_t)(0xF0 | (chr >> 18 & 0x07));
            *utf8++ = (uint8_t)(0x80 | (chr >> 12 & 0x3F));
            *utf8++ = (uint8_t)(0x80 | (chr >> 6 & 0x3F));
            *utf8++ = (uint8_t)(0x80 | (chr >> 0 & 0x3F));
        }
    }
}

// Count how many bytes a utf-16-le encoded string will take in utf-8.
STATIC mp_int_t _count_utf8_bytes(const uint16_t *buf, size_t len) {
    size_t total_bytes = 0;
    for (size_t i = 0; i < len; i++) {
        uint16_t chr = buf[i];
        if (chr < 0x80) {
            total_bytes += 1;
        } else if (chr < 0x800) {
            total_bytes += 2;
        } else if (chr < 0x10000) {
            total_bytes += 3;
        } else {
            total_bytes += 4;
        }
    }
    return total_bytes;
}

mp_obj_t utf16le_to_string(const uint16_t *buf, size_t utf16_len) {
    size_t size = _count_utf8_bytes(buf, utf16_len);
    vstr_t vstr;
    vstr_init_len(&vstr, size + 1);
    byte *p = (byte *)vstr.buf;
    // Null terminate.
    p[size] = '\0';
    _convert_utf16le_to_utf8(buf, utf16_len, p, size);
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}
