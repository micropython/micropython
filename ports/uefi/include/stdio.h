// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* Minimal <stdio.h> for the UEFI freestanding build. Most MicroPython output
 * goes through mp_hal_stdout_* / mp_printf, not these; they exist for the few
 * files that include <stdio.h> and for debug builds. */
#ifndef MPY_UEFI_STDIO_H
#define MPY_UEFI_STDIO_H
#include <stddef.h>
#include <stdarg.h>

int printf(const char *fmt, ...);
int snprintf(char *str, size_t size, const char *fmt, ...);
int vsnprintf(char *str, size_t size, const char *fmt, va_list ap);
int vprintf(const char *fmt, va_list ap);
int putchar(int c);
int puts(const char *s);

#endif
