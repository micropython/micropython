/**
 ******************************************************************************
 * @file    services-dynalib.h
 * @author  Matthew McGowan
 ******************************************************************************
  Copyright (c) 2015 Particle Industries, Inc.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, either
  version 3 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************
 */

#pragma once

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "rng_hal.h"
#include "eeprom_hal.h"
#include "delay_hal.h"
#include "timer_hal.h"
#include "rtc_hal.h"
#include "interrupts_hal.h"
#endif

/**
 * Define the dynamic exports for the current platform.
 *
 * Note that once used in released binary, the only permissable changes to the dynamic
 * library definition is to append new functions to the end of the list.  This is
 * so the dynamic library maintains backwards compatibility and can continue to
 * be used by older clients.
 *
 * Each platform may have a distinct dynamic library, via conditional compilation
 * but the rule above still holds.
 */

DYNALIB_BEGIN(hal)

#if PLATFORM_ID > 3
DYNALIB_FN(hal,HAL_RNG_Configuration)
DYNALIB_FN(hal,HAL_RNG_GetRandomNumber)
#endif

DYNALIB_FN(hal,HAL_Delay_Milliseconds)
DYNALIB_FN(hal,HAL_Delay_Microseconds)
DYNALIB_FN(hal,HAL_Timer_Get_Micro_Seconds)
DYNALIB_FN(hal,HAL_Timer_Get_Milli_Seconds)

DYNALIB_FN(hal,HAL_RTC_Configuration)
DYNALIB_FN(hal,HAL_RTC_Get_UnixTime)
DYNALIB_FN(hal,HAL_RTC_Set_UnixTime)
DYNALIB_FN(hal,HAL_RTC_Set_UnixAlarm)

DYNALIB_FN(hal,HAL_EEPROM_Init)
DYNALIB_FN(hal,HAL_EEPROM_Read)
DYNALIB_FN(hal,HAL_EEPROM_Write)
DYNALIB_FN(hal,HAL_EEPROM_Length)

DYNALIB_FN(hal,HAL_disable_irq)
DYNALIB_FN(hal,HAL_enable_irq)
DYNALIB_FN(hal,HAL_RTC_Cancel_UnixAlarm)
DYNALIB_END(hal)
