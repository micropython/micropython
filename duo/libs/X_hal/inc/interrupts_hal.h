/**
 ******************************************************************************
 * @file    interrupts_hal.h
 * @author  Satish Nair, Brett Walach
 * @version V1.0.0
 * @date    12-Sept-2014
 * @brief
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INTERRUPTS_HAL_H
#define __INTERRUPTS_HAL_H

/* Includes ------------------------------------------------------------------*/
#include "pinmap_hal.h"
#include "interrupts_irq.h"

/* Exported types ------------------------------------------------------------*/
typedef enum InterruptMode {
  CHANGE,
  RISING,
  FALLING
} InterruptMode;

typedef void (*HAL_InterruptHandler)(void* data);

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

typedef struct HAL_InterruptCallback {
    HAL_InterruptHandler handler;
    void* data;
} HAL_InterruptCallback;

typedef struct HAL_InterruptExtraConfiguration {
  uint8_t size;
  uint8_t IRQChannelPreemptionPriority;
  uint8_t IRQChannelSubPriority;
} HAL_InterruptExtraConfiguration;

#ifdef __cplusplus
extern "C" {
#endif

void HAL_Interrupts_Attach(uint16_t pin, HAL_InterruptHandler handler, void* data, InterruptMode mode, HAL_InterruptExtraConfiguration* config);
void HAL_Interrupts_Detach(uint16_t pin);
void HAL_Interrupts_Enable_All(void);
void HAL_Interrupts_Disable_All(void);

void HAL_Interrupts_Suspend(void);
void HAL_Interrupts_Restore(void);

void HAL_Interrupts_Trigger(uint16_t pin, void* reserved);


uint8_t HAL_Set_System_Interrupt_Handler(hal_irq_t irq, const HAL_InterruptCallback* callback, HAL_InterruptCallback* previous, void* reserved);
uint8_t HAL_Get_System_Interrupt_Handler(hal_irq_t irq, HAL_InterruptCallback* callback, void* reserved);
void HAL_System_Interrupt_Trigger(hal_irq_t irq, void* reserved);

int HAL_disable_irq();
void HAL_enable_irq(int mask);


#ifdef __cplusplus
}
#endif

#endif  /* __INTERRUPTS_HAL_H */
