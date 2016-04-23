/**
 ******************************************************************************
 * @file    hal_peripherals_dynalib.h
 * @authors Matthew McGowan
 * @date    16 February 2015
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

/**
 * All "optional" peripherals not used by the system code.
 */

#ifndef HAL_PERIPHERALS_DYNALIB_H
#define	HAL_PERIPHERALS_DYNALIB_H

#include "dynalib.h"
#include "platform_config.h"    // for HAS_SERIAL_FLASH

#ifdef DYNALIB_EXPORT
#include "tone_hal.h"
#include "servo_hal.h"
#include "hw_config.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_peripherals)
DYNALIB_FN(hal_peripherals,HAL_Tone_Start)
DYNALIB_FN(hal_peripherals,HAL_Tone_Stop)
DYNALIB_FN(hal_peripherals,HAL_Tone_Get_Frequency)
DYNALIB_FN(hal_peripherals,HAL_Tone_Is_Stopped)

DYNALIB_FN(hal_peripherals,HAL_Servo_Attach)
DYNALIB_FN(hal_peripherals,HAL_Servo_Detach)
DYNALIB_FN(hal_peripherals,HAL_Servo_Write_Pulse_Width)
DYNALIB_FN(hal_peripherals,HAL_Servo_Read_Pulse_Width)
DYNALIB_FN(hal_peripherals,HAL_Servo_Read_Frequency)

DYNALIB_END(hal_peripherals)

#endif	/* HAL_PERIPHERALS_DYNALIB_H */

