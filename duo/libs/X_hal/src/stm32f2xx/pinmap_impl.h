/**
 ******************************************************************************
 * @file    pinmap_impl.h
 * @author  Satish Nair, Matthew McGowan
 * @version V1.0.0
 * @date    20-Nov-2014
 * @brief   Implementation details of pins on STM32
 ******************************************************************************
  Copyright (c) 2013-2015 Particle Industries, Inc.  All rights reserved.

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

#ifndef PINMAP_IMPL_H
#define	PINMAP_IMPL_H

#include "stm32f2xx.h"


#ifdef	__cplusplus
extern "C" {
#endif

typedef struct STM32_Pin_Info {
  GPIO_TypeDef* gpio_peripheral;
  pin_t gpio_pin;
  uint8_t gpio_pin_source;
  uint8_t adc_channel;
  uint8_t dac_channel;
  TIM_TypeDef* timer_peripheral;
  uint16_t timer_ch;
  PinMode pin_mode;
  uint16_t timer_ccr;
  int32_t user_property;
} STM32_Pin_Info;


STM32_Pin_Info* HAL_Pin_Map(void);

extern void HAL_GPIO_Save_Pin_Mode(PinMode mode);
extern PinMode HAL_GPIO_Recall_Pin_Mode();

#define CHANNEL_NONE ((uint8_t)(0xFF))
#define ADC_CHANNEL_NONE CHANNEL_NONE
#define DAC_CHANNEL_NONE CHANNEL_NONE

#ifdef	__cplusplus
}
#endif

#endif	/* PINMAP_IMPL_H */

