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

typedef struct {
    const uint16_t *buf;
    size_t len;
} utf16_str;

STATIC uint32_t utf16str_peek_unit(utf16_str *utf) {
    if (!utf->len) {
        return 0;
    }
    return *utf->buf;
}

STATIC uint32_t utf16str_next_unit(utf16_str *utf) {
    uint32_t result = utf16str_peek_unit(utf);
    if (utf->len) {
        utf->len--;
        utf->buf++;
    }
    return result;
}
STATIC uint32_t utf16str_next_codepoint(utf16_str *utf) {
    uint32_t unichr = utf16str_next_unit(utf);
    if (unichr >= 0xd800 && unichr < 0xdc00) {
        uint32_t low_surrogate = utf16str_peek_unit(utf);
        if (low_surrogate >= 0xdc00 && low_surrogate < 0xe000) {
            (void)utf16str_next_unit(utf);
            unichr = (unichr - 0xd800) * 0x400 + low_surrogate - 0xdc00 + 0x10000;
        }
    }
    return unichr;
}

STATIC void _convert_utf16le_to_utf8(vstr_t *vstr, utf16_str *utf) {
    while (utf->len) {
        vstr_add_char(vstr, utf16str_next_codepoint(utf));
    }
}

mp_obj_t utf16le_to_string(const uint16_t *buf, size_t utf16_len) {
    // will grow if necessary, but will never grow for an all-ASCII descriptor
    vstr_t vstr;
    vstr_init(&vstr, utf16_len);
    utf16_str utf = {buf, utf16_len};
    _convert_utf16le_to_utf8(&vstr, &utf);
    return mp_obj_new_str_from_vstr(&mp_type_str, &vstr);
}
