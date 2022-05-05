/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_opa.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the OPA firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
***************************************************************************************/
#include "ch32v30x_opa.h"

#define OPA_MASK         ((uint32_t)0x000F)
#define OPA_Total_NUM    4

/*********************************************************************
 * @fn      OPA_DeInit
 *
 * @brief   Deinitializes the OPA peripheral registers to their default
 *        reset values.
 *
 * @return  none
 */
void OPA_DeInit(void)
{
    OPA->CR = 0;
}

/*********************************************************************
 * @fn      OPA_Init
 *
 * @brief   Initializes the OPA peripheral according to the specified
 *        parameters in the OPA_InitStruct.
 *
 * @param   OPA_InitStruct - pointer to a OPA_InitTypeDef structure
 *
 * @return  none
 */
void OPA_Init(OPA_InitTypeDef *OPA_InitStruct)
{
    uint32_t tmp = 0;
    tmp = OPA->CR;
    tmp &= ~(OPA_MASK << (OPA_InitStruct->OPA_NUM * OPA_Total_NUM));
    tmp |= (((OPA_InitStruct->PSEL << OPA_PSEL_OFFSET) | (OPA_InitStruct->NSEL << OPA_NSEL_OFFSET) | (OPA_InitStruct->Mode << OPA_MODE_OFFSET)) << (OPA_InitStruct->OPA_NUM * OPA_Total_NUM));
    OPA->CR = tmp;
}

/*********************************************************************
 * @fn      OPA_StructInit
 *
 * @brief   Fills each OPA_StructInit member with its reset value.
 *
 * @param   OPA_StructInit - pointer to a OPA_InitTypeDef structure
 *
 * @return  none
 */
void OPA_StructInit(OPA_InitTypeDef *OPA_InitStruct)
{
    OPA_InitStruct->Mode = OUT_IO;
    OPA_InitStruct->PSEL = CHP0;
    OPA_InitStruct->NSEL = CHN0;
    OPA_InitStruct->OPA_NUM = OPA1;
}

/*********************************************************************
 * @fn      OPA_Cmd
 *
 * @brief   Enables or disables the specified OPA peripheral.
 *
 * @param   OPA_NUM - Select OPA
 *            NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void OPA_Cmd(OPA_Num_TypeDef OPA_NUM, FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        OPA->CR |= (1 << (OPA_NUM * OPA_Total_NUM));
    }
    else
    {
        OPA->CR &= ~(1 << (OPA_NUM * OPA_Total_NUM));
    }
}
