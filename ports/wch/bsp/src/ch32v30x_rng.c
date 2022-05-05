/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_rng.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the RNG firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/
#include "ch32v30x_rng.h"
#include "ch32v30x_rcc.h"

/*********************************************************************
 * @fn      RNG_Cmd
 *
 * @brief   Enables or disables the RNG peripheral.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RNG_Cmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RNG->CR |= RNG_CR_RNGEN;
    }
    else
    {
        RNG->CR &= ~RNG_CR_RNGEN;
    }
}

/*********************************************************************
 * @fn      RNG_GetRandomNumber
 *
 * @brief   Returns a 32-bit random number.
 *
 * @return  32-bit random number.
 */
uint32_t RNG_GetRandomNumber(void)
{
    return RNG->DR;
}

/*********************************************************************
 * @fn      RNG_ITConfig
 *
 * @brief   Enables or disables the RNG interrupt.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  32-bit random number.
 */
void RNG_ITConfig(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RNG->CR |= RNG_CR_IE;
    }
    else
    {
        RNG->CR &= ~RNG_CR_IE;
    }
}

/*********************************************************************
 * @fn      RNG_GetFlagStatus
 *
 * @brief   Checks whether the specified RNG flag is set or not.
 *
 * @param   RNG_FLAG - specifies the RNG flag to check.
 *            RNG_FLAG_DRDY - Data Ready flag.
 *            RNG_FLAG_CECS - Clock Error Current flag.
 *            RNG_FLAG_SECS - Seed Error Current flag.
 *
 * @return  32-bit random number.
 */
FlagStatus RNG_GetFlagStatus(uint8_t RNG_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((RNG->SR & RNG_FLAG) != (uint8_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      RNG_ClearFlag
 *
 * @brief   Clears the RNG flags.
 *
 * @param   RNG_FLAG - specifies the flag to clear.
 *            RNG_FLAG_CECS - Clock Error Current flag.
 *            RNG_FLAG_SECS - Seed Error Current flag.
 *
 * @return  32-bit random number.
 */
void RNG_ClearFlag(uint8_t RNG_FLAG)
{
    RNG->SR = ~(uint32_t)(((uint32_t)RNG_FLAG) << 4);
}

/*********************************************************************
 * @fn      RNG_GetFlagStatus
 *
 * @brief   Checks whether the specified RNG interrupt has occurred or not.
 *
 * @param   RNG_IT - specifies the RNG interrupt source to check.
 *            RNG_IT_CEI - Clock Error Interrupt.
 *            RNG_IT_SEI - Seed Error Interrupt.
 *
 * @return  bitstatus£ºSET or RESET.
 */
ITStatus RNG_GetITStatus(uint8_t RNG_IT)
{
    ITStatus bitstatus = RESET;

    if((RNG->SR & RNG_IT) != (uint8_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      RNG_ClearITPendingBit
 *
 * @brief   Clears the RNG interrupt pending bit(s).
 *
 * @param   RNG_IT - specifies the RNG interrupt pending bit(s) to clear.
 *            RNG_IT_CEI - Clock Error Interrupt.
 *            RNG_IT_SEI - Seed Error Interrupt.
 *
 * @return  None
 */
void RNG_ClearITPendingBit(uint8_t RNG_IT)
{
    RNG->SR = (uint8_t)~RNG_IT;
}
