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

#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

// Make sure a path only has forward slashes.
char *to_unix_path(char *p) {
    if (p != NULL) {
        char *pp = p;
        while (*pp != 0) {
            if (*pp == '\\') {
                *pp = '/';
            }
            ++pp;
        }
    }
    return p;
}

// Implement realpath() using _fullpath and make it use the same error codes as realpath() on unix.
// Also have it return a path with forward slashes only as some code relies on this,
// but _fullpath() returns backward slashes no matter what.
char *realpath(const char *path, char *resolved_path) {
    char *ret = NULL;
    if (path == NULL) {
        errno = EINVAL;
    } else if (access(path, R_OK) == 0) {
        ret = resolved_path;
        if (ret == NULL) {
            ret = malloc(_MAX_PATH);
        }
        if (ret == NULL) {
            errno = ENOMEM;
        } else {
            ret = _fullpath(ret, path, _MAX_PATH);
            if (ret == NULL) {
                errno = EIO;
            }
        }
    }
    return to_unix_path(ret);
}
