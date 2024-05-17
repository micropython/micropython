// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_NRF_COMMON_HAL_RTC_RTC_H
#define MICROPY_INCLUDED_NRF_COMMON_HAL_RTC_RTC_H

extern void rtc_init(void);
extern void rtc_reset(void);
extern void common_hal_rtc_init(void);

#endif  // MICROPY_INCLUDED_NRF_COMMON_HAL_RTC_RTC_H
