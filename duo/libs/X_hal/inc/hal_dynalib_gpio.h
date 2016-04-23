/**
 ******************************************************************************
 * @file    hal_dynalib_gpio.h
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

#ifndef HAL_DYNALIB_GPIO_H
#define	HAL_DYNALIB_GPIO_H

#include "dynalib.h"

#ifdef DYNALIB_EXPORT
#include "gpio_hal.h"
#include "interrupts_hal.h"
#include "adc_hal.h"
#include "dac_hal.h"
#include "pwm_hal.h"
#endif

// WARNING
// The order of functions must not be changed or older applications will break
// when used with newer system firmware.
// Function signatures shouldn't be changed other than changing pointer types.
// New HAL functions must be added to the end of this list.
// GNINRAW

DYNALIB_BEGIN(hal_gpio)
DYNALIB_FN(hal_gpio,HAL_Pin_Map)
DYNALIB_FN(hal_gpio,HAL_Validate_Pin_Function)
DYNALIB_FN(hal_gpio,HAL_Pin_Mode)
DYNALIB_FN(hal_gpio,HAL_Get_Pin_Mode)
DYNALIB_FN(hal_gpio,HAL_GPIO_Write)
DYNALIB_FN(hal_gpio,HAL_GPIO_Read)
DYNALIB_FN(hal_gpio,HAL_Interrupts_Attach)
DYNALIB_FN(hal_gpio,HAL_Interrupts_Detach)
DYNALIB_FN(hal_gpio,HAL_Interrupts_Enable_All)
DYNALIB_FN(hal_gpio,HAL_Interrupts_Disable_All)

DYNALIB_FN(hal_gpio,HAL_DAC_Write)
DYNALIB_FN(hal_gpio,HAL_ADC_Set_Sample_Time)
DYNALIB_FN(hal_gpio,HAL_ADC_Read)

DYNALIB_FN(hal_gpio,HAL_PWM_Write)
DYNALIB_FN(hal_gpio,HAL_PWM_Get_Frequency)
DYNALIB_FN(hal_gpio,HAL_PWM_Get_AnalogValue)

DYNALIB_FN(hal_gpio, HAL_Set_System_Interrupt_Handler)
DYNALIB_FN(hal_gpio, HAL_Get_System_Interrupt_Handler)
DYNALIB_FN(hal_gpio, HAL_System_Interrupt_Trigger)

DYNALIB_FN(hal_gpio, HAL_Pulse_In)
DYNALIB_FN(hal_gpio, HAL_Interrupts_Suspend)
DYNALIB_FN(hal_gpio, HAL_Interrupts_Restore)

DYNALIB_FN(hal_gpio, HAL_PWM_Write_With_Frequency)
DYNALIB_END(hal_gpio)

#endif	/* HAL_DYNALIB_GPIO_H */

