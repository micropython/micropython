/**
 ******************************************************************************
 * @file    hal_dynalib_spi.h
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

#ifndef HAL_DYNALIB_SPI_H
#define	HAL_DYNALIB_SPI_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "spi_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_spi)
DYNALIB_FN(hal_spi,HAL_SPI_Begin)
DYNALIB_FN(hal_spi,HAL_SPI_End)
DYNALIB_FN(hal_spi,HAL_SPI_Set_Bit_Order)
DYNALIB_FN(hal_spi,HAL_SPI_Set_Data_Mode)
DYNALIB_FN(hal_spi,HAL_SPI_Set_Clock_Divider)
DYNALIB_FN(hal_spi,HAL_SPI_Send_Receive_Data)
DYNALIB_FN(hal_spi,HAL_SPI_Is_Enabled_Old)
DYNALIB_FN(hal_spi,HAL_SPI_Init)
DYNALIB_FN(hal_spi,HAL_SPI_Is_Enabled)
DYNALIB_FN(hal_spi,HAL_SPI_Info)
DYNALIB_FN(hal_spi,HAL_SPI_DMA_Transfer)
DYNALIB_END(hal_spi)


#endif	/* HAL_DYNALIB_SPI_H */

