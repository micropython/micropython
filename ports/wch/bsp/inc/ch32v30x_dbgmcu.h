/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dbgmcu.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      DBGMCU firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_DBGMCU_H
#define __CH32V30x_DBGMCU_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"


uint32_t DBGMCU_GetREVID(void);
uint32_t DBGMCU_GetDEVID(void);

#ifdef __cplusplus
}
#endif

#endif 







