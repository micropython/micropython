// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* Minimal <stdlib.h> for the UEFI freestanding build. */
#ifndef MPY_UEFI_STDLIB_H
#define MPY_UEFI_STDLIB_H
#include <stddef.h>

long strtol(const char *nptr, char **endptr, int base);
unsigned long strtoul(const char *nptr, char **endptr, int base);
long long strtoll(const char *nptr, char **endptr, int base);
unsigned long long strtoull(const char *nptr, char **endptr, int base);

int abs(int j);
long labs(long j);

void abort(void) __attribute__((noreturn));

// MicroPython manages its own heap (gc); these are provided for the few call
// sites that expect libc names, routed to the gc allocator in libc_extra.c.
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

#endif
