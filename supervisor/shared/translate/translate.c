/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "supervisor/shared/translate/translate.h"
#include "py/qstr.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifndef NO_QSTR
#include "genhdr/compressed_translations.generated.h"
#endif

#include "py/misc.h"
#include "py/mpprint.h"
#include "supervisor/serial.h"

void serial_write_compressed(mp_rom_error_text_t compressed) {
    mp_printf(MP_PYTHON_PRINTER, "%S", compressed);
}

STATIC void get_word(int n, const mchar_t **pos, const mchar_t **end) {
    int len = minlen;
    int i = 0;
    *pos = words;
    while (wlencount[i] <= n) {
        n -= wlencount[i];
        *pos += len * wlencount[i];
        i++;
        len++;
    }
    *pos += len * n;
    *end = *pos + len;
}

STATIC void put_utf8(vstr_t *vstr, int u) {
    if (u >= translation_offstart) {
        u += translation_offset;
    }
    if (word_start <= u && u <= word_end) {
        uint n = (u - word_start);
        const mchar_t *pos, *end;
        get_word(n, &pos, &end);
        // note that at present, entries in the words table are
        // guaranteed not to represent words themselves, so this adds
        // at most 1 level of recursive call
        for (; pos < end; pos++) {
            put_utf8(vstr, *pos);
        }
        return;
    }
    vstr_add_char(vstr, u);
}

uint16_t decompress_length(mp_rom_error_text_t compressed) {
    #ifndef NO_QSTR
    #if (compress_max_length_bits <= 8)
    return 1 + (compressed->data >> (8 - compress_max_length_bits));
    #else
    return 1 + ((compressed->data * 256 + compressed->tail[0]) >> (16 - compress_max_length_bits));
    #endif
    #endif
}

typedef struct {
    const uint8_t *ptr;
    uint8_t bit;
} bitstream_state_t;

static bool next_bit(bitstream_state_t *st) {
    bool r = *st->ptr & st->bit;
    st->bit >>= 1;
    if (!st->bit) {
        st->bit = 0x80;
        st->ptr++;
    }
    return r;
}

static int get_nbits(bitstream_state_t *st, int n) {
    int r = 0;
    while (n--) {
        r = (r << 1) | next_bit(st);
    }
    return r;
}

// note: the vstr must be a fixed-buffer vstr that matches the decompressed length of the string
static void decompress_vstr(mp_rom_error_text_t compressed, vstr_t *decompressed) {
    bitstream_state_t b = {
        .ptr = &(compressed->data) + (compress_max_length_bits >> 3),
        .bit = 1 << (7 - ((compress_max_length_bits) & 0x7)),
    };

    size_t alloc = decompressed->alloc - 1;
    // Stop one early because the last byte is always NULL.
    for (; decompressed->len < alloc;) {
        uint32_t bits = 0;
        uint8_t bit_length = 0;
        uint32_t max_code = lengths[0];
        uint32_t searched_length = lengths[0];
        while (true) {
            bits = (bits << 1) | next_bit(&b);
            bit_length += 1;
            if (max_code > 0 && bits < max_code) {
                break;
            }
            max_code = (max_code << 1) + lengths[bit_length];
            searched_length += lengths[bit_length];
        }
        int v = values[searched_length + bits - max_code];
        if (v == 1) {
            qstr q = get_nbits(&b, translation_qstr_bits) + 1; // honestly no idea why "+1"...
            vstr_add_str(decompressed, qstr_str(q));
        } else {
            put_utf8(decompressed, v);
        }
    }
}


char *decompress(mp_rom_error_text_t compressed, char *decompressed) {
    vstr_t vstr;
    vstr_init_fixed_buf(&vstr, decompress_length(compressed), decompressed);
    decompress_vstr(compressed, &vstr);
    return vstr_null_terminated_str(&vstr);
}

#if CIRCUITPY_TRANSLATE_OBJECT == 1
#include "supervisor/shared/translate/translate_impl.h"
#endif
