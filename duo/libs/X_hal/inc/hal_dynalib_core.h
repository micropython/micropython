/**
 ******************************************************************************
 * @file    hal_dynalib_core.h
 * @authors Matthew McGowan
 * @date    04 March 2015
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

#ifndef HAL_DYNALIB_CORE_H
#define	HAL_DYNALIB_CORE_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "core_hal.h"
#include "deviceid_hal.h"
#include "syshealth_hal.h"
#if PLATFORM_ID == 88
#include "device_name.h"
#endif
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_core)
DYNALIB_FN(hal_core,HAL_core_subsystem_version)
DYNALIB_FN(hal_core,HAL_Core_Init)
DYNALIB_FN(hal_core,HAL_Core_Config)
DYNALIB_FN(hal_core,HAL_Core_Mode_Button_Pressed)
DYNALIB_FN(hal_core,HAL_Core_Mode_Button_Reset)
DYNALIB_FN(hal_core,HAL_Core_System_Reset)
DYNALIB_FN(hal_core,HAL_Core_Factory_Reset)
DYNALIB_FN(hal_core,HAL_Core_Enter_Bootloader)
DYNALIB_FN(hal_core,HAL_Core_Enter_Stop_Mode)
DYNALIB_FN(hal_core,HAL_Core_Execute_Stop_Mode)
DYNALIB_FN(hal_core,HAL_Core_Enter_Standby_Mode)
DYNALIB_FN(hal_core,HAL_Core_Execute_Standby_Mode)
DYNALIB_FN(hal_core,HAL_Core_Compute_CRC32)
DYNALIB_FN(hal_core,HAL_device_ID)
#if PLATFORM_ID == 88
DYNALIB_FN(hal_core,HAL_Local_Name)
#endif

DYNALIB_FN(hal_core,HAL_Get_Sys_Health)
DYNALIB_FN(hal_core,HAL_Set_Sys_Health)
DYNALIB_FN(hal_core,HAL_watchdog_reset_flagged)
DYNALIB_FN(hal_core,HAL_Notify_WDT)
DYNALIB_FN(hal_core,HAL_Bootloader_Get_Flag)
DYNALIB_FN(hal_core,HAL_Bootloader_Lock)
DYNALIB_FN(hal_core,HAL_Core_System_Reset_FlagSet)
DYNALIB_FN(hal_core,HAL_Core_Runtime_Info)
DYNALIB_FN(hal_core,HAL_Set_System_Config)
DYNALIB_FN(hal_core,HAL_Core_Enter_Safe_Mode)
DYNALIB_FN(hal_core,HAL_Feature_Get)
DYNALIB_FN(hal_core,HAL_Feature_Set)
DYNALIB_END(hal_core)





#endif	/* HAL_DYNALIB_CORE_H */

