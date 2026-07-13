// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Minimal <time.h> shim for the freestanding UEFI port. Provides just the types
// mbedTLS needs (time_t / struct tm); the actual clock comes from the UEFI RTC via
// mbedtls_uefi_time / mbedtls_platform_gmtime_r in uefi_mbedtls_port.c.
#ifndef _UEFI_LIBC_TIME_H
#define _UEFI_LIBC_TIME_H

#include <stddef.h>

typedef long long time_t;

struct tm {
    int tm_sec;     // 0..60
    int tm_min;     // 0..59
    int tm_hour;    // 0..23
    int tm_mday;    // 1..31
    int tm_mon;     // 0..11
    int tm_year;    // years since 1900
    int tm_wday;    // 0..6, 0 = Sunday
    int tm_yday;    // 0..365
    int tm_isdst;   // daylight saving flag
};

#endif // _UEFI_LIBC_TIME_H
