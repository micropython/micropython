/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_wwdg.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the WWDG firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
**********************************************************************************/
#include "ch32v30x_wwdg.h"
#include "ch32v30x_rcc.h"

/* CTLR register bit mask */
#define CTLR_WDGA_Set      ((uint32_t)0x00000080)

/* CFGR register bit mask */
#define CFGR_WDGTB_Mask    ((uint32_t)0xFFFFFE7F)
#define CFGR_W_Mask        ((uint32_t)0xFFFFFF80)
#define BIT_Mask           ((uint8_t)0x7F)

/*********************************************************************
 * @fn      WWDG_DeInit
 *
 * @brief   Deinitializes the WWDG peripheral registers to their default reset values
 *
 * @return  none
 */
void WWDG_DeInit(void)
{
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_WWDG, DISABLE);
}

/*********************************************************************
 * @fn      WWDG_SetPrescaler
 *
 * @brief   Sets the WWDG Prescaler
 *
 * @param   WWDG_Prescaler - specifies the WWDG Prescaler
 *            WWDG_Prescaler_1 - WWDG counter clock = (PCLK1/4096)/1
 *            WWDG_Prescaler_2 - WWDG counter clock = (PCLK1/4096)/2
 *            WWDG_Prescaler_4 - WWDG counter clock = (PCLK1/4096)/4
 *            WWDG_Prescaler_8 - WWDG counter clock = (PCLK1/4096)/8
 *
 * @return  none
 */
void WWDG_SetPrescaler(uint32_t WWDG_Prescaler)
{
    uint32_t tmpreg = 0;
    tmpreg = WWDG->CFGR & CFGR_WDGTB_Mask;
    tmpreg |= WWDG_Prescaler;
    WWDG->CFGR = tmpreg;
}

/*********************************************************************
 * @fn      WWDG_SetWindowValue
 *
 * @brief   Sets the WWDG window value
 *
 * @param   WindowValue - specifies the window value to be compared to the
 *        downcounter,which must be lower than 0x80
 *
 * @return  none
 */
void WWDG_SetWindowValue(uint8_t WindowValue)
{
    __IO uint32_t tmpreg = 0;

    tmpreg = WWDG->CFGR & CFGR_W_Mask;

    tmpreg |= WindowValue & (uint32_t)BIT_Mask;

    WWDG->CFGR = tmpreg;
}

/*********************************************************************
 * @fn      WWDG_EnableIT
 *
 * @brief   Enables the WWDG Early Wakeup interrupt(EWI)
 *
 * @return  none
 */
void WWDG_EnableIT(void)
{
    WWDG->CFGR |= (1 << 9);
}

/*********************************************************************
 * @fn      WWDG_SetCounter
 *
 * @brief   Sets the WWDG counter value
 *
 * @param   Counter - specifies the watchdog counter value,which must be a
 *        number between 0x40 and 0x7F
 *
 * @return  none
 */
void WWDG_SetCounter(uint8_t Counter)
{
    WWDG->CTLR = Counter & BIT_Mask;
}

/*********************************************************************
 * @fn      WWDG_Enable
 *
 * @brief   Enables WWDG and load the counter value
 *
 * @param   Counter - specifies the watchdog counter value,which must be a
 *        number between 0x40 and 0x7F
 * @return  none
 */
void WWDG_Enable(uint8_t Counter)
{
    WWDG->CTLR = CTLR_WDGA_Set | Counter;
}

/*********************************************************************
 * @fn      WWDG_GetFlagStatus
 *
 * @brief   Checks whether the Early Wakeup interrupt flag is set or not
 *
 * @return  The new state of the Early Wakeup interrupt flag (SET or RESET)
 */
FlagStatus WWDG_GetFlagStatus(void)
{
    return (FlagStatus)(WWDG->STATR);
}

/*********************************************************************
 * @fn      WWDG_ClearFlag
 *
 * @brief   Clears Early Wakeup interrupt flag
 *
 * @return  none
 */
void WWDG_ClearFlag(void)
{
    WWDG->STATR = (uint32_t)RESET;
}
