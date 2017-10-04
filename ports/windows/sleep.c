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

#include <windows.h>
#include <errno.h>
#include <limits.h>

HANDLE waitTimer = NULL;

void init_sleep(void) {
    waitTimer = CreateWaitableTimer(NULL, TRUE, NULL);
}

void deinit_sleep(void) {
    if (waitTimer != NULL) {
        CloseHandle(waitTimer);
        waitTimer = NULL;
    }
}

int usleep_impl(__int64 usec) {
    if (waitTimer == NULL) {
        errno = EAGAIN;
        return -1;
    }
    if (usec < 0 || usec > LLONG_MAX / 10) {
        errno = EINVAL;
        return -1;
    }

    LARGE_INTEGER ft;
    ft.QuadPart = -10 * usec; // 100 nanosecond interval, negative value = relative time
    if (SetWaitableTimer(waitTimer, &ft, 0, NULL, NULL, 0) == 0) {
        errno = EINVAL;
        return -1;
    }
    if (WaitForSingleObject(waitTimer, INFINITE) != WAIT_OBJECT_0) {
        errno = EAGAIN;
        return -1;
    }
    return 0;
}

#ifdef _MSC_VER // mingw and the likes provide their own usleep()
int usleep(__int64 usec) {
    return usleep_impl(usec);
}
#endif

void msec_sleep(double msec) {
    const double usec = msec * 1000.0;
    usleep_impl(usec > (double)LLONG_MAX ? LLONG_MAX : (__int64)usec);
}
