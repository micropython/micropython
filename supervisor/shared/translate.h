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

#ifndef MICROPY_INCLUDED_SUPERVISOR_TRANSLATE_H
#define MICROPY_INCLUDED_SUPERVISOR_TRANSLATE_H

#include <stdint.h>

// The format of the compressed data is:
// - the size of the uncompressed string in UTF-8 bytes, encoded as a
//   (compress_max_length_bits)-bit number.  compress_max_length_bits is
//   computed during dictionary generation time, and happens to be 8
//   for all current platforms.  However, it'll probably end up being
//   9 in some translations sometime in the future.  This length excludes
//   the trailing NUL, though notably decompress_length includes it.
//
// - followed by the huffman encoding of the individual UTF-16 code
//   points that make up the string.  The trailing "\0" is not
//   represented by a huffman code, but is implied by the length.
//   (building the huffman encoding on UTF-16 code points gave better
//   compression than building it on UTF-8 bytes)
//
// - code points starting at 128 (word_start) and potentially extending
//   to 255 (word_end) (but never interfering with the target
//   language's used code points) stand for dictionary entries in a
//   dictionary with size up to 256 code points.  The dictionary entries
//   are computed with a heuristic based on frequent substrings of 2 to
//   9 code points.  These are called "words" but are not, grammatically
//   speaking, words.  They're just spans of code points that frequently
//   occur together.
//
// - dictionary entries are non-overlapping, and the _ending_ index of each
//   entry is stored in an array.  Since the index given is the ending
//   index, the array is called "wends".
//
// The "data" / "tail" construct is so that the struct's last member is a
// "flexible array".  However, the _only_ member is not permitted to be
// a flexible member, so we have to declare the first byte as a separte
// member of the structure.
//
// For translations where length needs 8 bits, this saves about 1.5
// bytes per string on average compared to a structure of {uint16_t,
// flexible array}, but is also future-proofed against strings with
// UTF-8 length above 256, with a savings of about 1.375 bytes per
// string.
typedef struct {
    uint8_t data;
    const uint8_t tail[];
} compressed_string_t;

// Return the compressed, translated version of a source string
// Usually, due to LTO, this is optimized into a load of a constant
// pointer.
const compressed_string_t *translate(const char *c);
void serial_write_compressed(const compressed_string_t *compressed);
char *decompress(const compressed_string_t *compressed, char *decompressed);
uint16_t decompress_length(const compressed_string_t *compressed);


// Map MicroPython's error messages to our translations.
#if defined(MICROPY_ENABLE_DYNRUNTIME) && MICROPY_ENABLE_DYNRUNTIME
#define MP_ERROR_TEXT(x) (x)
#else
#define MP_ERROR_TEXT(x) translate(x)
#endif

#endif  // MICROPY_INCLUDED_SUPERVISOR_TRANSLATE_H
