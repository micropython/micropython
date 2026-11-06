// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

// Minimal <windows.h> for the freestanding UEFI port. clang compiles our
// *-unknown-windows triple with _WIN32 defined, so mbedtls' platform_util.c takes
// its _WIN32 branch and uses SecureZeroMemory for zeroization. We are not really
// Windows; map it to memset. (Other mbedtls _WIN32 paths — sockets, filesystem
// cert loading, timing, entropy — are disabled by our config, so this is the only
// windows.h consumer that actually compiles.)
#ifndef _UEFI_LIBC_WINDOWS_H
#define _UEFI_LIBC_WINDOWS_H

#include <string.h>

#define SecureZeroMemory(ptr, cnt) memset((ptr), 0, (cnt))

#endif // _UEFI_LIBC_WINDOWS_H
