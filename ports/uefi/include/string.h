// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* Minimal <string.h> for the UEFI freestanding (-windows target) build.
 * Implementations come from shared/libc/string0.c and ports/uefi/libc_extra.c. */
#ifndef MPY_UEFI_STRING_H
#define MPY_UEFI_STRING_H
#include <stddef.h>

void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);

size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
size_t strcspn(const char *s, const char *reject);
size_t strspn(const char *s, const char *accept);

#endif
