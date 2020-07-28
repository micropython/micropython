#include <time.h>
#include "lib/oofatfs/ff.h"

DWORD get_fattime(void) {
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    return ((1900 + tm->tm_year - 1980) << 25)
           | (tm->tm_mon << 21)
           | (tm->tm_mday << 16)
           | (tm->tm_hour << 11)
           | (tm->tm_min << 5)
           | (tm->tm_sec / 2);
}
