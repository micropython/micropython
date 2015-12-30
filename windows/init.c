/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#ifdef _MSC_VER
#include <crtdbg.h>
#endif
#include "sleep.h"

extern BOOL WINAPI console_sighandler(DWORD evt);

#ifdef _MSC_VER
void invalid_param_handler(const wchar_t *expr, const wchar_t *fun, const wchar_t *file, unsigned int line, uintptr_t p) {
}
#endif

void init() {
#ifdef _MSC_VER
    // Disable the 'Debug Error!' dialog for assertions failures and the likes,
    // instead write messages to the debugger output and terminate.
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);

    // Disable 'invalid parameter handling' which is for instance invoked when
    // passing invalid file descriptors to functions like lseek() and make the
    // functions called behave properly by setting errno to EBADF/EINVAL/..
    _set_invalid_parameter_handler(invalid_param_handler);
#endif
    SetConsoleCtrlHandler(console_sighandler, TRUE);
    init_sleep();
#ifdef __MINGW32__
    putenv("PRINTF_EXPONENT_DIGITS=2");
#elif _MSC_VER < 1900
    // This is only necessary for Visual Studio versions 2013 and below:
    // https://msdn.microsoft.com/en-us/library/bb531344(v=vs.140).aspx
    _set_output_format(_TWO_DIGIT_EXPONENT);
#endif
}

void deinit() {
    SetConsoleCtrlHandler(console_sighandler, FALSE);
    deinit_sleep();
}
