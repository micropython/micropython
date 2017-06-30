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

#include <Winsock2.h>

typedef union {
    unsigned __int64 tm; // time in 100 nanoseconds interval
    FILETIME ft;
} FT;

int gettimeofday(struct timeval *tp, struct timezone *tz) {
    if (tp == NULL) {
        return 0;
    }

    // UTC time
    FT ft;
    ZeroMemory(&ft, sizeof(ft));
    GetSystemTimeAsFileTime(&ft.ft);

    // to microseconds
    ft.tm /= 10;

    // convert to unix format
    // number of microseconds intervals between the 1st january 1601 and the 1st january 1970 (369 years + 89 leap days)
    const unsigned __int64 deltaEpoch = 11644473600000000ull;
    const unsigned __int64 microSecondsToSeconds = 1000000ull;
    tp->tv_usec = ft.tm % microSecondsToSeconds;
    tp->tv_sec = (ft.tm - deltaEpoch) / microSecondsToSeconds;

    // see man gettimeofday: timezone is deprecated and expected to be NULL
    (void)tz;

    return 0;
}
