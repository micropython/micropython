/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 MicroPython contributors
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

#include <stddef.h>
#include <stdint.h>

#include "py/mpconfig.h"
#include "mphalport.h"

// Minimal freestanding libc helpers.  GCC emits implicit calls to
// memcpy/memset when optimising struct copies and array initialisers,
// so we have to provide them even with -nostdlib.  Lifted from
// lib/dabao-sdk/src/common/bao_stdlib/stdlib.c -- we don't compile
// that SDK file directly because it also defines a weak trap_dispatch
// that would conflict with the real one in hardware_irq/irq.c.

void *memcpy(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    while (n--) {
        *d++ = *s++;
    }
    return dst;
}

void *memset(void *dst, int val, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    while (n--) {
        *d++ = (uint8_t)val;
    }
    return dst;
}

void *memmove(void *dst, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dst;
    const uint8_t *s = (const uint8_t *)src;
    if (d == s || n == 0) {
        return dst;
    }
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        d += n;
        s += n;
        while (n--) {
            *--d = *--s;
        }
    }
    return dst;
}

int memcmp(const void *a, const void *b, size_t n) {
    const uint8_t *p = (const uint8_t *)a;
    const uint8_t *q = (const uint8_t *)b;
    while (n--) {
        if (*p != *q) {
            return *p - *q;
        }
        p++;
        q++;
    }
    return 0;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == (char)c) {
            return (char *)s;
        }
        s++;
    }
    return (c == 0) ? (char *)s : NULL;
}

size_t strlen(const char *s) {
    const char *p = s;
    while (*p) {
        p++;
    }
    return p - s;
}

int strncmp(const char *a, const char *b, size_t n) {
    while (n--) {
        unsigned char ca = (unsigned char)*a++;
        unsigned char cb = (unsigned char)*b++;
        if (ca != cb) {
            return ca - cb;
        }
        if (ca == 0) {
            return 0;
        }
    }
    return 0;
}

int strcmp(const char *a, const char *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return (unsigned char)*a - (unsigned char)*b;
}

// stdout sink.  Phase 1.3 has no UART driver yet, so writes are
// discarded.  Replaced by the real implementation in commit 1.5.
void mp_hal_stdout_tx_strn(const char *str, mp_uint_t len) {
    (void)str;
    (void)len;
}

// Cooked stdout: translate '\n' to "\r\n" and forward.  Same no-op
// behaviour as the bare version until commit 1.5.
void mp_hal_stdout_tx_strn_cooked(const char *str, size_t len) {
    (void)str;
    (void)len;
}

// Interrupt-char stash.  Wired into the UART RX path in commit 1.6.
volatile int mp_interrupt_char;

void mp_hal_set_interrupt_char(int c) {
    mp_interrupt_char = c;
}
