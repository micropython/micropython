// mbedtls_config_port.h - mbedTLS configuration for the GD32F4xx port.
//
// The chip has 128 KB of SRAM and the MicroPython heap plus peripherals already
// claim ~91 KB, so this configuration is deliberately trimmed:
//
//   * only the curves TLS servers actually use (secp256r1, secp384r1, x25519)
//     instead of the full set the common config enables;
//   * no TLS 1.0/1.1, which no modern server accepts anyway;
//   * a smaller inbound record buffer, since the default 16 KB is most of our
//     remaining RAM.

#ifndef MICROPY_INCLUDED_GD32_MBEDTLS_CONFIG_H
#define MICROPY_INCLUDED_GD32_MBEDTLS_CONFIG_H

// Time source for certificate validation.  The port reads the RTC when it is
// configured, and otherwise reports a fixed epoch offset by uptime.
#include <time.h>
time_t gd32_rtctime_seconds(time_t *timer);
#define MBEDTLS_PLATFORM_TIME_MACRO gd32_rtctime_seconds

// mbedTLS 3.x also needs a millisecond counter (used for timeout accounting).
// The implementation lives in mbedtls_port.c; the prototype comes from
// mbedtls/platform_time.h.
#define MBEDTLS_PLATFORM_MS_TIME_ALT

// Use the bare-metal memory/entropy hooks from the common config.
#define MICROPY_MBEDTLS_CONFIG_BARE_METAL (1)

// Include the MicroPython baseline configuration.
#include "extmod/mbedtls/mbedtls_config_common.h"

// ---- trim the common config to fit in SRAM ----

// Small output record buffer; the common config already limits input.
#undef MBEDTLS_SSL_IN_CONTENT_LEN
#define MBEDTLS_SSL_IN_CONTENT_LEN  (8192)

// Drop the obsolete protocol versions.
#undef MBEDTLS_SSL_PROTO_TLS1
#undef MBEDTLS_SSL_PROTO_TLS1_1

// Keep only the curves used by real-world servers.  secp192/224 and the Koblitz
// curves cost code and RAM for no practical benefit.
#undef MBEDTLS_ECP_DP_SECP192R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
#undef MBEDTLS_ECP_DP_SECP192K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP224K1_ENABLED
#undef MBEDTLS_ECP_DP_SECP256K1_ENABLED
#undef MBEDTLS_ECP_DP_CURVE25519_ENABLED

// mbedTLS needs a window size for the remaining curves; 4 is the compact option.
#define MBEDTLS_ECP_WINDOW_SIZE     (4)
#define MBEDTLS_ECP_FIXED_POINT_OPTIM (1)

// The bare-metal hooks do not provide a filesystem.
#undef MBEDTLS_FS_IO

#endif // MICROPY_INCLUDED_GD32_MBEDTLS_CONFIG_H
