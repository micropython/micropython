/**
 ******************************************************************************
 * @file    hal_dynalib_i2c.h
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

#ifndef HAL_DYNALIB_I2C_H
#define	HAL_DYNALIB_I2C_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "i2c_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_i2c)

// first edition of these functions that were released on the Photon/P1
// They are not needed on other platforms.
#if PLATFORM_ID==6 || PLATFORM_ID==8 || PLATFORM_ID==88
DYNALIB_FN(hal_i2c,HAL_I2C_Set_Speed_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Enable_DMA_Mode_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Stretch_Clock_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Begin_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_End_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Request_Data_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Begin_Transmission_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_End_Transmission_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Write_Data_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Available_Data_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Read_Data_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Peek_Data_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Flush_Data_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Is_Enabled_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Set_Callback_On_Receive_v1)
DYNALIB_FN(hal_i2c,HAL_I2C_Set_Callback_On_Request_v1)
#endif

DYNALIB_FN(hal_i2c,HAL_I2C_Set_Speed)
DYNALIB_FN(hal_i2c,HAL_I2C_Enable_DMA_Mode)
DYNALIB_FN(hal_i2c,HAL_I2C_Stretch_Clock)
DYNALIB_FN(hal_i2c,HAL_I2C_Begin)
DYNALIB_FN(hal_i2c,HAL_I2C_End)
DYNALIB_FN(hal_i2c,HAL_I2C_Request_Data)
DYNALIB_FN(hal_i2c,HAL_I2C_Begin_Transmission)
DYNALIB_FN(hal_i2c,HAL_I2C_End_Transmission)
DYNALIB_FN(hal_i2c,HAL_I2C_Write_Data)
DYNALIB_FN(hal_i2c,HAL_I2C_Available_Data)
DYNALIB_FN(hal_i2c,HAL_I2C_Read_Data)
DYNALIB_FN(hal_i2c,HAL_I2C_Peek_Data)
DYNALIB_FN(hal_i2c,HAL_I2C_Flush_Data)
DYNALIB_FN(hal_i2c,HAL_I2C_Is_Enabled)
DYNALIB_FN(hal_i2c,HAL_I2C_Set_Callback_On_Receive)
DYNALIB_FN(hal_i2c,HAL_I2C_Set_Callback_On_Request)
DYNALIB_FN(hal_i2c,HAL_I2C_Init)
DYNALIB_END(hal_i2c)

#endif	/* HAL_DYNALIB_I2C_H */

