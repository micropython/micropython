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

#include <stdarg.h>
#include <stdio.h>
#include <malloc.h>

// _snprintf/vsnprintf are fine, except the 'F' specifier is not handled
int snprintf(char *dest, size_t count, const char *format, ...) {
    const size_t fmtLen = strlen(format) + 1;
    char *fixedFmt = alloca(fmtLen);
    for (size_t i = 0; i < fmtLen; ++i)
        fixedFmt[i] = format[i] == 'F' ? 'f' : format[i];

    va_list args;
    va_start(args, format);
    const int ret = vsnprintf(dest, count, fixedFmt, args);
    va_end(args);

    return ret;
}
