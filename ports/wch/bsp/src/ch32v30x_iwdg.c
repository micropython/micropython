/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_iwdg.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the IWDG firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_iwdg.h"

/* CTLR register bit mask */
#define CTLR_KEY_Reload    ((uint16_t)0xAAAA)
#define CTLR_KEY_Enable    ((uint16_t)0xCCCC)

/*********************************************************************
 * @fn      IWDG_WriteAccessCmd
 *
 * @brief   Enables or disables write access to IWDG_PSCR and IWDG_RLDR registers.
 *
 * @param   WDG_WriteAccess - new state of write access to IWDG_PSCR and
 *        IWDG_RLDR registers.
 *            IWDG_WriteAccess_Enable - Enable write access to IWDG_PSCR and
 *        IWDG_RLDR registers.
 *            IWDG_WriteAccess_Disable - Disable write access to IWDG_PSCR
 *        and IWDG_RLDR registers.
 *
 * @return  none
 */
void IWDG_WriteAccessCmd(uint16_t IWDG_WriteAccess)
{
    IWDG->CTLR = IWDG_WriteAccess;
}

/*********************************************************************
 * @fn      IWDG_SetPrescaler
 *
 * @brief   Sets IWDG Prescaler value.
 *
 * @param   IWDG_Prescaler - specifies the IWDG Prescaler value.
 *             IWDG_Prescaler_4 - IWDG prescaler set to 4.
 *             IWDG_Prescaler_8 - IWDG prescaler set to 8.
 *             IWDG_Prescaler_16 - IWDG prescaler set to 16.
 *             IWDG_Prescaler_32 - IWDG prescaler set to 32.
 *             IWDG_Prescaler_64 - IWDG prescaler set to 64.
 *             IWDG_Prescaler_128 - IWDG prescaler set to 128.
 *             IWDG_Prescaler_256 - IWDG prescaler set to 256.
 *
 * @return  none
 */
void IWDG_SetPrescaler(uint8_t IWDG_Prescaler)
{
    IWDG->PSCR = IWDG_Prescaler;
}

/*********************************************************************
 * @fn      IWDG_SetReload
 *
 * @brief   Sets IWDG Reload value.
 *
 * @param   Reload - specifies the IWDG Reload value.
 *            This parameter must be a number between 0 and 0x0FFF.
 *
 * @return  none
 */
void IWDG_SetReload(uint16_t Reload)
{
    IWDG->RLDR = Reload;
}

/*********************************************************************
 * @fn      IWDG_ReloadCounter
 *
 * @brief   Reloads IWDG counter with value defined in the reload register.
 *
 * @return  none
 */
void IWDG_ReloadCounter(void)
{
    IWDG->CTLR = CTLR_KEY_Reload;
}

/*********************************************************************
 * @fn      IWDG_Enable
 *
 * @brief   Enables IWDG (write access to IWDG_PSCR and IWDG_RLDR registers disabled).
 *
 * @return  none
 */
void IWDG_Enable(void)
{
    IWDG->CTLR = CTLR_KEY_Enable;
}

/*********************************************************************
 * @fn      IWDG_GetFlagStatus
 *
 * @brief   Checks whether the specified IWDG flag is set or not.
 *
 * @param   IWDG_FLAG - specifies the flag to check.
 *            IWDG_FLAG_PVU - Prescaler Value Update on going.
 *            IWDG_FLAG_RVU - Reload Value Update on going.
 *
 * @return  none
 */
FlagStatus IWDG_GetFlagStatus(uint16_t IWDG_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((IWDG->STATR & IWDG_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}
