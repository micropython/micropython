// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Nicko van Someren

/* mbedTLS configuration for the MicroPython UEFI port.
 *
 * Selected by the Makefile (TLS=mbedtls) via MBEDTLS_CONFIG_FILE. We are fully
 * freestanding — no OS libc/network/filesystem — so we use the common bare-metal
 * path (GC-tracked calloc/free, hardware-entropy alt, snprintf macro) and add the
 * UEFI RTC as the time source for X.509 validity. See README.md.
 */
#ifndef MICROPY_INCLUDED_UEFI_MBEDTLS_CONFIG_PORT_H
#define MICROPY_INCLUDED_UEFI_MBEDTLS_CONFIG_PORT_H

// Turn on the common config's bare-metal block: MBEDTLS_PLATFORM_MEMORY (calloc/
// free -> m_tracked_calloc/free), MBEDTLS_PLATFORM_NO_STD_FUNCTIONS,
// MBEDTLS_ENTROPY_HARDWARE_ALT + MBEDTLS_NO_PLATFORM_ENTROPY, snprintf macro.
#define MICROPY_MBEDTLS_CONFIG_BARE_METAL (1)

#include "extmod/mbedtls/mbedtls_config_common.h"

// clang targets *-unknown-windows in MSVC-compat mode (defines _MSC_VER, not
// __GNUC__), so mbedtls' platform_util.h would pull in <sal.h> for its
// _Check_return_ annotation. We have no SAL; the annotation is only a compile-time
// warning aid, so disable it (skips the <sal.h> include).
#define MBEDTLS_CHECK_RETURN

// The common config defines MBEDTLS_HAVE_TIME / _DATE (needed for certificate
// notBefore/notAfter checks) but also NO_STD_FUNCTIONS, so route time + gmtime_r
// at our UEFI-RTC implementations (uefi_mbedtls_port.c).
#include <time.h>
time_t mbedtls_uefi_time(time_t *timer);
#define MBEDTLS_PLATFORM_TIME_MACRO       mbedtls_uefi_time
#define MBEDTLS_PLATFORM_TIME_TYPE_MACRO  time_t
#define MBEDTLS_PLATFORM_GMTIME_R_ALT     // we provide mbedtls_platform_gmtime_r

// mbedtls_ms_time() (a monotonic millisecond counter) otherwise takes a Windows
// FILETIME path under _WIN32; provide our own from the port's tick clock instead.
#define MBEDTLS_PLATFORM_MS_TIME_ALT

#endif // MICROPY_INCLUDED_UEFI_MBEDTLS_CONFIG_PORT_H
