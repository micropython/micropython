// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* Minimal <assert.h> for the UEFI freestanding build. */
#ifndef MPY_UEFI_ASSERT_H
#define MPY_UEFI_ASSERT_H

#ifdef NDEBUG
#define assert(e) ((void)0)
#else
void __assert_func(const char *file, int line, const char *func, const char *expr)
__attribute__((noreturn));
#define assert(e) ((e) ? (void)0 : __assert_func(__FILE__, __LINE__, __func__, #e))
#endif

#endif
