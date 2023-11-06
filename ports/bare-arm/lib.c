/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Damien P. George
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

// These memory functions are needed when the garbage collector is disabled.
// A full implementation should be provided, or the garbage collector enabled.
// The functions here are very simple.

extern char _heap_start;

void *malloc(size_t n) {
    static char *cur_heap = NULL;
    if (cur_heap == NULL) {
        cur_heap = &_heap_start;
    }
    void *ptr = cur_heap;
    cur_heap += (n + 7) & ~7;
    return ptr;
}

void *realloc(void *ptr, size_t size) {
    void *ptr2 = malloc(size);
    if (ptr && size) {
        memcpy(ptr2, ptr, size); // size may be greater than ptr's region, do copy anyway
    }
    return ptr2;
}

void free(void *p) {
}

// These standard string functions are needed by the runtime, and can be
// provided either by the system or shared/libc/string0.c.  The implementations
// here are very simple.

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *ss1 = s1, *ss2 = s2;
    while (n--) {
        int c = *ss1++ - *ss2++;
        if (c) {
            return c;
        }
    }
    return 0;
}

void *memcpy(void *dest, const void *src, size_t n) {
    return memmove(dest, src, n);
}

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    if (s < d && d < s + n) {
        // Need to copy backwards.
        d += n - 1;
        s += n - 1;
        while (n--) {
            *d-- = *s--;
        }
    } else {
        // Can copy forwards.
        while (n--) {
            *d++ = *s++;
        }
    }
    return dest;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *ss = s;
    while (n--) {
        *ss++ = c;
    }
    return s;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == c) {
            return (char *)s;
        }
        ++s;
    }
    return NULL;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (n > 0 && *s1 && *s2) {
        int c = *s1++ - *s2++;
        --n;
        if (c) {
            return c;
        }
    }
    if (n == 0) {
        return 0;
    }
    return *s1 - *s2;
}

int strcmp(const char *s1, const char *s2) {
    return strncmp(s1, s2, 0x7fffffff);
}

size_t strlen(const char *s) {
    const char *ss = s;
    while (*ss) {
        ++ss;
    }
    return ss - s;
}
