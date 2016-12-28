/**
 ******************************************************************************
 * @file    can_hal.h
 * @author  Brian Spranger, Julien Vanier
 * @version V1.0.0
 * @date    04-Jan-2016
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
#ifndef __CAN_HAL_H
#define __CAN_HAL_H

#include <stdbool.h>

/* Includes ------------------------------------------------------------------*/
#include "pinmap_hal.h"

/* Exported defines ----------------------------------------------------------*/
#if PLATFORM_ID == 6 || PLATFORM_ID == 8 || PLATFORM_ID == 88 // Photon and P1 and Duo
 #define TOTAL_CAN   1
 #define HAL_HAS_CAN_D1_D2
#elif PLATFORM_ID == 10 // Electron
 #define TOTAL_CAN   2
 #define HAL_HAS_CAN_D1_D2
 #define HAL_HAS_CAN_C4_C5
#else
  // Core 
 #define TOTAL_CAN   0
#endif

/* Exported constants --------------------------------------------------------*/

typedef enum HAL_CAN_Channel
{
#ifdef HAL_HAS_CAN_D1_D2
   CAN_D1_D2,
#endif
#ifdef HAL_HAS_CAN_C4_C5
   CAN_C4_C5,
#endif
} HAL_CAN_Channel;

// Flags for HAL_CAN_Begin
typedef enum HAL_CAN_Flags {
  CAN_TEST_MODE = 0x0001,
} HAL_CAN_Flags;

typedef enum HAL_CAN_Errors {
  CAN_NO_ERROR,
  CAN_ERROR_PASSIVE,
  CAN_BUS_OFF
} HAL_CAN_Errors;

typedef enum HAL_CAN_Filters {
  CAN_FILTER_STANDARD,
  CAN_FILTER_EXTENDED
} HAL_CAN_Filters;

/* Exported types ------------------------------------------------------------*/

struct CANMessage
{
   uint32_t id;
   uint8_t  size;
   bool     extended;
   bool     rtr;
   uint8_t  len;
   uint8_t  data[8];

#ifdef __cplusplus
   CANMessage()
     : id { 0 },
       size { sizeof(CANMessage) },
       extended { false },
       rtr { false },
       len { 0 },
       data { 0 }
   {
   }
#endif
};

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

void HAL_CAN_Init(HAL_CAN_Channel channel,
                  uint16_t rxQueueSize,
                  uint16_t txQueueSize,
                  void *reserved);
void HAL_CAN_Begin(HAL_CAN_Channel channel,
                   uint32_t baud,
                   uint32_t flags,
                   void *reserved);
void HAL_CAN_End(HAL_CAN_Channel channel,
                 void *reserved);
bool HAL_CAN_Transmit(HAL_CAN_Channel channel,
                      const CANMessage *message,
                      void *reserved);
bool HAL_CAN_Receive(HAL_CAN_Channel channel,
                     CANMessage *message,
                     void *reserved);
uint8_t HAL_CAN_Available_Messages(HAL_CAN_Channel channel,
                                   void *reserved);
bool HAL_CAN_Add_Filter(HAL_CAN_Channel channel,
                        uint32_t id,
                        uint32_t mask,
                        HAL_CAN_Filters type,
                        void *reserved);
void HAL_CAN_Clear_Filters(HAL_CAN_Channel channel,
                           void *reserved);
bool HAL_CAN_Is_Enabled(HAL_CAN_Channel channel);
HAL_CAN_Errors HAL_CAN_Error_Status(HAL_CAN_Channel channel);


#ifdef __cplusplus
}
#endif

#endif  /* __CAN_HAL_H */

