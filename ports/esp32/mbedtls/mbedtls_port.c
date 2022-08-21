#include <py/mpconfig.h>

#ifdef MICROPY_SSL_MBEDTLS

#include <time.h>
#include <sys/time.h>
#include "shared/timeutils/timeutils.h"

#ifdef MICROPY_MBEDTLS_PLATFORM_TIME_ALT

time_t platform_mbedtls_time(time_t *timer) {
    // mbedtls_time requires time in seconds from EPOCH 1970

    struct timeval tv;
    gettimeofday(&tv, NULL);

    return tv.tv_sec + TIMEUTILS_SECONDS_1970_TO_2000;
}

#endif
#endif
