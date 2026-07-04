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

// Minimal <windows.h> for the freestanding UEFI port. clang compiles our
// *-unknown-windows triple with _WIN32 defined, so mbedtls' platform_util.c takes
// its _WIN32 branch and uses SecureZeroMemory for zeroization. We are not really
// Windows; map it to memset. (Other mbedtls _WIN32 paths — sockets, filesystem
// cert loading, timing, entropy — are disabled by our config, so this is the only
// windows.h consumer that actually compiles.)
#ifndef _UEFI_LIBC_WINDOWS_H
#define _UEFI_LIBC_WINDOWS_H

#include <string.h>

#define SecureZeroMemory(ptr, cnt) memset((ptr), 0, (cnt))

#endif // _UEFI_LIBC_WINDOWS_H
