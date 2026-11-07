// fatfs_port.c - FatFs platform hooks for the GD32 port
//
// lib/oofatfs needs a get_fattime() implementation when FF_FS_NORTC is 0.
// We derive it from the same wall clock the rest of the port uses, so FAT
// timestamps agree with littlefs file times and time.time().

#include "py/mphal.h"
#include "lib/oofatfs/ff.h"
#include "shared/timeutils/timeutils.h"

#if MICROPY_VFS_FAT

// FatFs wants the local timestamp packed into a DWORD:
//   bits 31-25: year since 1980
//   bits 24-21: month (1-12)
//   bits 20-16: day (1-31)
//   bits 15-11: hour (0-23)
//   bits 10-5 : minute (0-59)
//   bits 4-0  : second / 2 (0-29)
DWORD get_fattime(void) {
    timeutils_struct_time_t tm;
    uint64_t epoch_ns = mp_hal_time_ns();
    timeutils_seconds_since_1970_to_struct_time(epoch_ns / 1000000000ull, &tm);

    uint16_t year = tm.tm_year;
    if (year < 1980) {
        // FatFs cannot represent dates before 1980; clamp to the epoch.
        year = 1980;
        tm.tm_mon = 1;
        tm.tm_mday = 1;
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
    }

    return ((DWORD)(year - 1980) << 25)
         | ((DWORD)(tm.tm_mon & 0x0f) << 21)
         | ((DWORD)(tm.tm_mday & 0x1f) << 16)
         | ((DWORD)(tm.tm_hour & 0x1f) << 11)
         | ((DWORD)(tm.tm_min & 0x3f) << 5)
         | ((DWORD)((tm.tm_sec / 2) & 0x1f));
}

#endif // MICROPY_VFS_FAT
