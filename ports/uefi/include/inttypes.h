/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Nicko van Someren
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

// Minimal <inttypes.h> for the freestanding UEFI port. The clang builtin forwards
// to a hosted inttypes.h we don't have, so provide the PRI*/SCN* format macros
// directly. Both targets use the Windows LLP64 model (long is 32-bit; 64-bit and
// pointer-sized quantities are long long), so 64-bit and PTR conversions use "ll".
#ifndef _UEFI_LIBC_INTTYPES_H
#define _UEFI_LIBC_INTTYPES_H

#include <stdint.h>

#define PRId8   "d"
#define PRId16  "d"
#define PRId32  "d"
#define PRId64  "lld"
#define PRIi8   "i"
#define PRIi16  "i"
#define PRIi32  "i"
#define PRIi64  "lli"
#define PRIu8   "u"
#define PRIu16  "u"
#define PRIu32  "u"
#define PRIu64  "llu"
#define PRIo8   "o"
#define PRIo16  "o"
#define PRIo32  "o"
#define PRIo64  "llo"
#define PRIx8   "x"
#define PRIx16  "x"
#define PRIx32  "x"
#define PRIx64  "llx"
#define PRIX8   "X"
#define PRIX16  "X"
#define PRIX32  "X"
#define PRIX64  "llX"

#define PRIdPTR "lld"
#define PRIiPTR "lli"
#define PRIuPTR "llu"
#define PRIoPTR "llo"
#define PRIxPTR "llx"
#define PRIXPTR "llX"

#define PRIdMAX "lld"
#define PRIiMAX "lli"
#define PRIuMAX "llu"
#define PRIoMAX "llo"
#define PRIxMAX "llx"
#define PRIXMAX "llX"

#endif // _UEFI_LIBC_INTTYPES_H
