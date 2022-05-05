/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_rng.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the
*                      RNG firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32V30x_RNG_H
#define __CH32V30x_RNG_H

#ifdef __cplusplus
 extern "C" {
#endif
#include "ch32v30x.h"

 /* RNG_flags_definition*/
#define RNG_FLAG_DRDY               ((uint8_t)0x0001) /* Data ready */
#define RNG_FLAG_CECS               ((uint8_t)0x0002) /* Clock error current status */
#define RNG_FLAG_SECS               ((uint8_t)0x0004) /* Seed error current status */

/* RNG_interrupts_definition */
#define RNG_IT_CEI                  ((uint8_t)0x20) /* Clock error interrupt */
#define RNG_IT_SEI                  ((uint8_t)0x40) /* Seed error interrupt */


void RNG_Cmd(FunctionalState NewState);
uint32_t RNG_GetRandomNumber(void);
void RNG_ITConfig(FunctionalState NewState);
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG);
void RNG_ClearFlag(uint8_t RNG_FLAG);
ITStatus RNG_GetITStatus(uint8_t RNG_IT);
void RNG_ClearITPendingBit(uint8_t RNG_IT);

#ifdef __cplusplus
}
#endif

#endif
