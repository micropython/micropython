/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dbgmcu.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the DBGMCU firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
****************************************************************************************/
#include "ch32v30x_dbgmcu.h"

#define IDCODE_DEVID_MASK    ((uint32_t)0x00000FFF)

/*********************************************************************
 * @fn      DBGMCU_GetREVID
 *
 * @brief   Returns the device revision identifier.
 *
 * @return  Revision identifier.
 */
uint32_t DBGMCU_GetREVID(void)
{
    return ((*(uint32_t *)0x1FFFF704) >> 16);
}

/*********************************************************************
 * @fn      DBGMCU_GetDEVID
 *
 * @brief   Returns the device identifier.
 *
 * @return  Device identifier.
 */
uint32_t DBGMCU_GetDEVID(void)
{
    return ((*(uint32_t *)0x1FFFF704) & IDCODE_DEVID_MASK);
}
