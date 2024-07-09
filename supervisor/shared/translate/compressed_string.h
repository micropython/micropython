// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

// The format of the compressed data is:
// - the size of the uncompressed string in UTF-8 bytes, encoded as a
//   (compress_max_length_bits)-bit number.  compress_max_length_bits is
//   computed during dictionary generation time, and happens to be 8
//   for all current platforms.  However, it'll probably end up being
//   9 in some translations sometime in the future.  This length excludes
//   the trailing NUL, though notably decompress_length includes it.
//
// - followed by the huffman encoding of the individual code
//   points that make up the string.  The trailing "\0" is not
//   represented by a huffman code, but is implied by the length.
//   (building the huffman encoding on UTF-16 code points gave better
//   compression than building it on UTF-8 bytes)
//
// - If possible, the code points are represented as uint8_t values, with
//   0..127 representing themselves and 160..255 representing another range
//   of Unicode, controlled by translation_offset and translation_offstart.
//   If this is not possible, uint16_t values are used. At present, no translation
//   requires code points not in the BMP, so this is adequate.
//
// - code points starting at 128 (word_start) and potentially extending
//   to 255 (word_end) (but never interfering with the target
//   language's used code points) stand for dictionary entries in a
//   dictionary with size up to 256 code points.  The dictionary entries
//   are computed with a heuristic based on frequent substrings of 2 to
//   9 code points.  These are called "words" but are not, grammatically
//   speaking, words.  They're just spans of code points that frequently
//   occur together.  They are ordered shortest to longest.
//
// - If the translation uses a lot of code points or widely spaced code points,
//   then the huffman table entries are UTF-16 code points. But if the translation
//   uses only ASCII 7-bit code points plus a SMALL range of higher code points that
//   still fit in 8 bits, translation_offset and translation_offstart are used to
//   renumber the code points so that they still fit within 8 bits. (it's very beneficial
//   for mchar_t to be 8 bits instead of 16!)
//
// - dictionary entries are non-overlapping, and the _ending_ index of each
//   entry is stored in an array.  A count of words of each length, from
//   minlen to maxlen, is given in the array called wlencount.  From
//   this small array, the start and end of the N'th word can be
//   calculated by an efficient, small loop.  (A bit of time is traded
//   to reduce the size of this table indicating lengths)
//
// - Value 1 ('\1') is used to indicate that a QSTR number follows. the
//   QSTR is encoded as a fixed number of bits (translation_qstr_bits), e.g.,
//   10 bits if the highest core qstr is from 512 to 1023 inclusive.
//   (maketranslationdata uses a simple heuristic where any qstr >= 3
//   characters long is encoded in this way; this is simple but probably not
//   optimal. In fact, the rule of >= 2 characters is better for SOME languages
//   on SOME boards.)
//
// The "data" / "tail" construct is so that the struct's last member is a
// "flexible array".  However, the _only_ member is not permitted to be
// a flexible member, so we have to declare the first byte as a separate
// member of the structure.
//
// For translations where length needs 8 bits, this saves about 1.5
// bytes per string on average compared to a structure of {uint16_t,
// flexible array}, but is also future-proofed against strings with
// UTF-8 length above 256, with a savings of about 1.375 bytes per
// string.
typedef struct compressed_string {
    uint8_t data;
    const uint8_t tail[];
} const *mp_rom_error_text_t;

// Return the compressed, translated version of a source string
// Usually, due to LTO, this is optimized into a load of a constant
// pointer.
// mp_rom_error_text_t MP_ERROR_TEXT(const char *c);
void serial_write_compressed(mp_rom_error_text_t compressed);
char *decompress(mp_rom_error_text_t compressed, char *decompressed);
uint16_t decompress_length(mp_rom_error_text_t compressed);
