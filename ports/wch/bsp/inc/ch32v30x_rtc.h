/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_rtc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the RTC  
*                      firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_RTC_H
#define __CH32V30x_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"


/* RTC_interrupts_define */
#define RTC_IT_OW            ((uint16_t)0x0004)  /* Overflow interrupt */
#define RTC_IT_ALR           ((uint16_t)0x0002)  /* Alarm interrupt */
#define RTC_IT_SEC           ((uint16_t)0x0001)  /* Second interrupt */

/* RTC_interrupts_flags */
#define RTC_FLAG_RTOFF       ((uint16_t)0x0020)  /* RTC Operation OFF flag */
#define RTC_FLAG_RSF         ((uint16_t)0x0008)  /* Registers Synchronized flag */
#define RTC_FLAG_OW          ((uint16_t)0x0004)  /* Overflow flag */
#define RTC_FLAG_ALR         ((uint16_t)0x0002)  /* Alarm flag */
#define RTC_FLAG_SEC         ((uint16_t)0x0001)  /* Second flag */
	 

void RTC_ITConfig(uint16_t RTC_IT, FunctionalState NewState);
void RTC_EnterConfigMode(void);
void RTC_ExitConfigMode(void);
uint32_t  RTC_GetCounter(void);
void RTC_SetCounter(uint32_t CounterValue);
void RTC_SetPrescaler(uint32_t PrescalerValue);
void RTC_SetAlarm(uint32_t AlarmValue);
uint32_t  RTC_GetDivider(void);
void RTC_WaitForLastTask(void);
void RTC_WaitForSynchro(void);
FlagStatus RTC_GetFlagStatus(uint16_t RTC_FLAG);
void RTC_ClearFlag(uint16_t RTC_FLAG);
ITStatus RTC_GetITStatus(uint16_t RTC_IT);
void RTC_ClearITPendingBit(uint16_t RTC_IT);

#ifdef __cplusplus
}
#endif

#endif 

