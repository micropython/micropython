/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_WINDOWS_VERSION_H
#define MICROPY_INCLUDED_WINDOWS_VERSION_H

// This file defines various macros, that are used by windows.h to
// decide what to define.
//
// Any file that is going to include windows.h should include this file first.
//
// This is in a central header, because it's likely that the supported
// Windows version number will need increasing in future, as new versions
// of Windows are released and old versions drop out of support.  Putting
// it in a central place means there is only one place to update it.
// It also means that it is consistent across all files in the project.

// We want all APIs that were supported in Windows 8.
#define WINVER 0x0602
#define _WIN32_WINNT 0x0602

// Don't include WinSock 1 APIs.
// This is required for WinSock 2 to work.
#define WIN32_LEAN_AND_MEAN

#endif // MICROPY_INCLUDED_WINDOWS_VERSION_H
