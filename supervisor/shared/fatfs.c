// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "supervisor/fatfs.h"

#include "py/runtime.h"
#include "lib/oofatfs/ff.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "shared-bindings/os/__init__.h"
#include "shared-bindings/rtc/RTC.h"
#include "shared-bindings/time/__init__.h"
#include "shared/timeutils/timeutils.h"

DWORD _time_override = 0;
DWORD get_fattime(void) {
    if (_time_override > 0) {
        return _time_override;
    }
    #if CIRCUITPY_RTC
    timeutils_struct_time_t tm;
    common_hal_rtc_get_time(&tm);
    return ((tm.tm_year - 1980) << 25) | (tm.tm_mon << 21) | (tm.tm_mday << 16) |
           (tm.tm_hour << 11) | (tm.tm_min << 5) | (tm.tm_sec >> 1);
    #else
    return ((2016 - 1980) << 25) | ((9) << 21) | ((1) << 16) | ((16) << 11) | ((43) << 5) | (35 / 2);
    #endif
}

void override_fattime(DWORD time) {
    _time_override = time;
}

DWORD make_volid(void) {
    DWORD result;
    if (!common_hal_os_urandom((uint8_t *)&result, sizeof(result))) {
        #if CIRCUITPY_MICROCONTROLLER && COMMON_HAL_MCU_PROCESSOR_UID_LENGTH >= 4
        uint8_t raw_id[COMMON_HAL_MCU_PROCESSOR_UID_LENGTH];
        common_hal_mcu_processor_get_uid(raw_id);
        memcpy(&result, raw_id, sizeof(result));
        #else
        result = (DWORD)common_hal_time_monotonic_ns();
        #endif
    }
    return result;
}
