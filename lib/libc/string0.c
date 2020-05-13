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

#include <stdint.h>
#include <string.h>

#define likely(x) __builtin_expect((x), 1)

void *memcpy(void *dst, const void *src, size_t n) {
    if (likely(!(((uintptr_t)dst) & 3) && !(((uintptr_t)src) & 3))) {
        // pointers aligned
        uint32_t *d = dst;
        const uint32_t *s = src;

        // copy words first
        for (size_t i = (n >> 2); i; i--) {
            *d++ = *s++;
        }

        if (n & 2) {
            // copy half-word
            *(uint16_t*)d = *(const uint16_t*)s;
            d = (uint32_t*)((uint16_t*)d + 1);
            s = (const uint32_t*)((const uint16_t*)s + 1);
        }

        if (n & 1) {
            // copy byte
            *((uint8_t*)d) = *((const uint8_t*)s);
        }
    } else {
        // unaligned access, copy bytes
        uint8_t *d = dst;
        const uint8_t *s = src;

        for (; n; n--) {
            *d++ = *s++;
        }
    }

    return dst;
}

void *memmove(void *dest, const void *src, size_t n) {
    if (src < dest && (uint8_t*)dest < (const uint8_t*)src + n) {
        // need to copy backwards
        uint8_t *d = (uint8_t*)dest + n - 1;
        const uint8_t *s = (const uint8_t*)src + n - 1;
        for (; n > 0; n--) {
            *d-- = *s--;
        }
        return dest;
    } else {
        // can use normal memcpy
        return memcpy(dest, src, n);
    }
}

void *memset(void *s, int c, size_t n) {
    if (c == 0 && ((uintptr_t)s & 3) == 0) {
        // aligned store of 0
        uint32_t *s32 = s;
        for (size_t i = n >> 2; i > 0; i--) {
            *s32++ = 0;
        }
        if (n & 2) {
            *((uint16_t*)s32) = 0;
            s32 = (uint32_t*)((uint16_t*)s32 + 1);
        }
        if (n & 1) {
            *((uint8_t*)s32) = 0;
        }
    } else {
        uint8_t *s2 = s;
        for (; n > 0; n--) {
            *s2++ = c;
        }
    }
    return s;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *s1_8 = s1;
    const uint8_t *s2_8 = s2;
    while (n--) {
        char c1 = *s1_8++;
        char c2 = *s2_8++;
        if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
    }
    return 0;
}

void *memchr(const void *s, int c, size_t n) {
    if (n != 0) {
        const unsigned char *p = s;

        do {
            if (*p++ == c)
                return ((void *)(p - 1));
        } while (--n != 0);
    }
    return 0;
}

size_t strlen(const char *str) {
    int len = 0;
    for (const char *s = str; *s; s++) {
        len += 1;
    }
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = *s1++; // XXX UTF8 get char, next char
        char c2 = *s2++; // XXX UTF8 get char, next char
        if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
    }
    if (*s2) return -1;
    else if (*s1) return 1;
    else return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while (*s1 && *s2 && n > 0) {
        char c1 = *s1++; // XXX UTF8 get char, next char
        char c2 = *s2++; // XXX UTF8 get char, next char
        n--;
        if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
    }
    if (n == 0) return 0;
    else if (*s2) return -1;
    else if (*s1) return 1;
    else return 0;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

// needed because gcc optimises strcpy + strcat to this
char *stpcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return dest;
}

char *strcat(char *dest, const char *src) {
    char *d = dest;
    while (*d) {
        d++;
    }
    while (*src) {
        *d++ = *src++;
    }
    *d = '\0';
    return dest;
}

// Public Domain implementation of strchr from:
// http://en.wikibooks.org/wiki/C_Programming/Strings#The_strchr_function
char *strchr(const char *s, int c)
{
    /* Scan s for the character.  When this loop is finished,
       s will either point to the end of the string or the
       character we were looking for.  */
    while (*s != '\0' && *s != (char)c)
        s++;
    return ((*s == c) ? (char *) s : 0);
}


// Public Domain implementation of strstr from:
// http://en.wikibooks.org/wiki/C_Programming/Strings#The_strstr_function
char *strstr(const char *haystack, const char *needle)
{
    size_t needlelen;
    /* Check for the null needle case.  */
    if (*needle == '\0')
        return (char *) haystack;
    needlelen = strlen(needle);
    for (; (haystack = strchr(haystack, *needle)) != 0; haystack++)
        if (strncmp(haystack, needle, needlelen) == 0)
            return (char *) haystack;
    return 0;
}

size_t strspn(const char *s, const char *accept) {
    const char *ss = s;
    while (*s && strchr(accept, *s) != NULL) {
        ++s;
    }
    return s - ss;
}

size_t strcspn(const char *s, const char *reject) {
    const char *ss = s;
    while (*s && strchr(reject, *s) == NULL) {
        ++s;
    }
    return s - ss;
}
