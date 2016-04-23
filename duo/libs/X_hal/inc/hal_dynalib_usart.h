/**
 ******************************************************************************
 * @file    hal_dynalib_usart.h
 * @authors mat
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

#ifndef HAL_DYNALIB_USART_H
#define	HAL_DYNALIB_USART_H

#include "dynalib.h"
#include "usb_config_hal.h"

#ifdef DYNALIB_EXPORT
#include "usart_hal.h"
#include "usb_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_usart)

#ifdef USB_CDC_ENABLE
DYNALIB_FN(hal_usart,USB_USART_Init)
DYNALIB_FN(hal_usart,USB_USART_Available_Data)
DYNALIB_FN(hal_usart,USB_USART_Receive_Data)
DYNALIB_FN(hal_usart,USB_USART_Send_Data)
DYNALIB_FN(hal_usart,USB_USART_Baud_Rate)
DYNALIB_FN(hal_usart,USB_USART_LineCoding_BitRate_Handler)
#endif

DYNALIB_FN(hal_usart,HAL_USART_Init)
DYNALIB_FN(hal_usart,HAL_USART_Begin)
DYNALIB_FN(hal_usart,HAL_USART_End)
DYNALIB_FN(hal_usart,HAL_USART_Write_Data)
DYNALIB_FN(hal_usart,HAL_USART_Available_Data)
DYNALIB_FN(hal_usart,HAL_USART_Read_Data)
DYNALIB_FN(hal_usart,HAL_USART_Peek_Data)
DYNALIB_FN(hal_usart,HAL_USART_Flush_Data)
DYNALIB_FN(hal_usart,HAL_USART_Is_Enabled)
DYNALIB_FN(hal_usart,HAL_USART_Half_Duplex)
DYNALIB_FN(hal_usart,HAL_USART_Available_Data_For_Write)

DYNALIB_END(hal_usart)

#endif	/* HAL_DYNALIB_USART_H */

