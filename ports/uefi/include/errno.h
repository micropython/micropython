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

// Minimal <errno.h> for the freestanding UEFI port. MicroPython uses its own error
// codes (MP_Exxx, py/mperrno.h) internally; this exists only so third-party code
// that #includes <errno.h> (e.g. extmod/modtls_mbedtls.c) compiles. Values mirror
// the common Linux numbering used elsewhere in the port.
#ifndef _UEFI_LIBC_ERRNO_H
#define _UEFI_LIBC_ERRNO_H

#define EPERM        1
#define EIO          5
#define EBADF        9
#define EAGAIN       11
#define ENOMEM       12
#define EACCES       13
#define EINVAL       22
#define EWOULDBLOCK  EAGAIN
#define EINPROGRESS  115

#endif // _UEFI_LIBC_ERRNO_H
