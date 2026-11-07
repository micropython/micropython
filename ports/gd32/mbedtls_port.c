// mbedtls_port.c - mbedTLS platform hooks for the GD32F4xx port.
//
// mbedTLS on bare metal needs two time sources: wall-clock seconds for
// certificate date checks, and a millisecond counter for timeout accounting.

#include "py/mpconfig.h"

#if MICROPY_PY_SSL && MICROPY_SSL_MBEDTLS

#include <time.h>

#include "py/mphal.h"
#include "mbedtls/platform_time.h"

// Wall-clock seconds.  Shares the source with mp_hal_time_ns(), which reads the
// RTC when one is configured and otherwise returns a fixed epoch offset by
// uptime.  Exposed via MBEDTLS_PLATFORM_TIME_MACRO in the config header.
time_t gd32_rtctime_seconds(time_t *timer) {
    time_t now = (time_t)(mp_hal_time_ns() / 1000000000ull);
    if (timer != NULL) {
        *timer = now;
    }
    return now;
}

// Millisecond monotonic counter used by MBEDTLS_PLATFORM_MS_TIME_ALT.
// mbedTLS expects mbedtls_ms_time_t (int64_t by default), not uint32_t.
mbedtls_ms_time_t mbedtls_ms_time(void) {
    return (mbedtls_ms_time_t)mp_hal_ticks_ms();
}

#endif // MICROPY_PY_SSL && MICROPY_SSL_MBEDTLS
