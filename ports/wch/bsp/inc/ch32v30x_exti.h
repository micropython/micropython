/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_exti.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      EXTI firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_EXTI_H
#define __CH32V30x_EXTI_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* EXTI mode enumeration */
typedef enum
{
  EXTI_Mode_Interrupt = 0x00,
  EXTI_Mode_Event = 0x04
}EXTIMode_TypeDef;

/* EXTI Trigger enumeration */
typedef enum
{
  EXTI_Trigger_Rising = 0x08,
  EXTI_Trigger_Falling = 0x0C,  
  EXTI_Trigger_Rising_Falling = 0x10
}EXTITrigger_TypeDef;

/* EXTI Init Structure definition */
typedef struct
{
  uint32_t EXTI_Line;               /* Specifies the EXTI lines to be enabled or disabled.
                                       This parameter can be any combination of @ref EXTI_Lines */
   
  EXTIMode_TypeDef EXTI_Mode;       /* Specifies the mode for the EXTI lines.
                                       This parameter can be a value of @ref EXTIMode_TypeDef */

  EXTITrigger_TypeDef EXTI_Trigger; /* Specifies the trigger signal active edge for the EXTI lines.
                                       This parameter can be a value of @ref EXTIMode_TypeDef */

  FunctionalState EXTI_LineCmd;     /* Specifies the new state of the selected EXTI lines.
                                       This parameter can be set either to ENABLE or DISABLE */ 
}EXTI_InitTypeDef;


/* EXTI_Lines */
#define EXTI_Line0       ((uint32_t)0x00001)  /* External interrupt line 0 */
#define EXTI_Line1       ((uint32_t)0x00002)  /* External interrupt line 1 */
#define EXTI_Line2       ((uint32_t)0x00004)  /* External interrupt line 2 */
#define EXTI_Line3       ((uint32_t)0x00008)  /* External interrupt line 3 */
#define EXTI_Line4       ((uint32_t)0x00010)  /* External interrupt line 4 */
#define EXTI_Line5       ((uint32_t)0x00020)  /* External interrupt line 5 */
#define EXTI_Line6       ((uint32_t)0x00040)  /* External interrupt line 6 */
#define EXTI_Line7       ((uint32_t)0x00080)  /* External interrupt line 7 */
#define EXTI_Line8       ((uint32_t)0x00100)  /* External interrupt line 8 */
#define EXTI_Line9       ((uint32_t)0x00200)  /* External interrupt line 9 */
#define EXTI_Line10      ((uint32_t)0x00400)  /* External interrupt line 10 */
#define EXTI_Line11      ((uint32_t)0x00800)  /* External interrupt line 11 */
#define EXTI_Line12      ((uint32_t)0x01000)  /* External interrupt line 12 */
#define EXTI_Line13      ((uint32_t)0x02000)  /* External interrupt line 13 */
#define EXTI_Line14      ((uint32_t)0x04000)  /* External interrupt line 14 */
#define EXTI_Line15      ((uint32_t)0x08000)  /* External interrupt line 15 */
#define EXTI_Line16      ((uint32_t)0x10000)  /* External interrupt line 16 Connected to the PVD Output */
#define EXTI_Line17      ((uint32_t)0x20000)  /* External interrupt line 17 Connected to the RTC Alarm event */
#define EXTI_Line18      ((uint32_t)0x40000)  /* External interrupt line 18 Connected to the USBD/USBFS OTG
                                                 Wakeup from suspend event */                                    
#define EXTI_Line19      ((uint32_t)0x80000)  /* External interrupt line 19 Connected to the Ethernet Wakeup event */
#define EXTI_Line20      ((uint32_t)0x100000) /* External interrupt line 20 Connected to the USBHS Wakeup event */

void EXTI_DeInit(void);
void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
void EXTI_StructInit(EXTI_InitTypeDef* EXTI_InitStruct);
void EXTI_GenerateSWInterrupt(uint32_t EXTI_Line);
FlagStatus EXTI_GetFlagStatus(uint32_t EXTI_Line);
void EXTI_ClearFlag(uint32_t EXTI_Line);
ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
void EXTI_ClearITPendingBit(uint32_t EXTI_Line);

#ifdef __cplusplus
}
#endif

#endif 

