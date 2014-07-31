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

#include <string.h>

#include "mpconfigport.h"
#include "ff.h"
#include "ffconf.h"

extern BYTE CurrVol;

// "path" is the path to lookup; will advance this pointer beyond the volume name.
// Returns logical drive number (-1 means invalid path).
int ff_get_ldnumber (const TCHAR** path) {
    if (!(*path)) {
        return -1;
    }

    if (**path != '/') {
#if _FS_RPATH
        return CurrVol;
#else
        return -1;
#endif
    } else if (strncmp(*path, "/flash", 6) == 0) {
        if ((*path)[6] == '/') {
            *path += 6;
        } else if ((*path)[6] == '\0') {
            *path = "/";
        } else {
            return -1;
        }
        return 0;
    } else if (strncmp(*path, "/sd", 3) == 0) {
        if ((*path)[3] == '/') {
            *path += 3;
        } else if ((*path)[3] == '\0') {
            *path = "/";
        } else {
            return -1;
        }
        return 1;
    } else {
        return -1;
    }
}
