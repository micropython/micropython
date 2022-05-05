/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v30x_pwr.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the PWR firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
********************************************************************************/
#include "ch32v30x_pwr.h"
#include "ch32v30x_rcc.h"

/* PWR registers bit mask */
/* CTLR register bit mask */
#define CTLR_DS_MASK     ((uint32_t)0xFFFFFFFC)
#define CTLR_PLS_MASK    ((uint32_t)0xFFFFFF1F)

/*********************************************************************
 * @fn      PWR_DeInit
 *
 * @brief   Deinitializes the PWR peripheral registers to their default
 *        reset values.
 *
 * @return  none
 */
void PWR_DeInit(void)
{
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
    RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, DISABLE);
}

/*********************************************************************
 * @fn      PWR_BackupAccessCmd
 *
 * @brief   Enables or disables access to the RTC and backup registers.
 *
 * @param   NewState - new state of the access to the RTC and backup registers,
 *            This parameter can be: ENABLE or DISABLE.
 *
 * @return  none
 */
void PWR_BackupAccessCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CTLR |= (1 << 8);
    }
    else
    {
        PWR->CTLR &= ~(1 << 8);
    }
}

/*********************************************************************
 * @fn      PWR_PVDCmd
 *
 * @brief   Enables or disables the Power Voltage Detector(PVD).
 *
 * @param   NewState - new state of the PVD(ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_PVDCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CTLR |= (1 << 4);
    }
    else
    {
        PWR->CTLR &= ~(1 << 4);
    }
}

/*********************************************************************
 * @fn      PWR_PVDLevelConfig
 *
 * @brief   Configures the voltage threshold detected by the Power Voltage
 *        Detector(PVD).
 *
 * @param   PWR_PVDLevel - specifies the PVD detection level
 *            PWR_PVDLevel_2V2 - PVD detection level set to 2.2V
 *            PWR_PVDLevel_2V3 - PVD detection level set to 2.3V
 *            PWR_PVDLevel_2V4 - PVD detection level set to 2.4V
 *            PWR_PVDLevel_2V5 - PVD detection level set to 2.5V
 *            PWR_PVDLevel_2V6 - PVD detection level set to 2.6V
 *            PWR_PVDLevel_2V7 - PVD detection level set to 2.7V
 *            PWR_PVDLevel_2V8 - PVD detection level set to 2.8V
 *            PWR_PVDLevel_2V9 - PVD detection level set to 2.9V
 *
 * @return  none
 */
void PWR_PVDLevelConfig(uint32_t PWR_PVDLevel)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;
    tmpreg &= CTLR_PLS_MASK;
    tmpreg |= PWR_PVDLevel;
    PWR->CTLR = tmpreg;
}

/*********************************************************************
 * @fn      PWR_WakeUpPinCmd
 *
 * @brief   Enables or disables the WakeUp Pin functionality.
 *
 * @param   NewState - new state of the WakeUp Pin functionality
 *        (ENABLE or DISABLE).
 *
 * @return  none
 */
void PWR_WakeUpPinCmd(FunctionalState NewState)
{
    if(NewState)
    {
        PWR->CSR |= (1 << 8);
    }
    else
    {
        PWR->CSR &= ~(1 << 8);
    }
}

/*********************************************************************
 * @fn      PWR_EnterSTOPMode
 *
 * @brief   Enters STOP mode.
 *
 * @param   PWR_Regulator - specifies the regulator state in STOP mode.
 *            PWR_Regulator_ON - STOP mode with regulator ON
 *            PWR_Regulator_LowPower - STOP mode with regulator in low power mode
 *          PWR_STOPEntry - specifies if STOP mode in entered with WFI or WFE instruction.
 *            PWR_STOPEntry_WFI - enter STOP mode with WFI instruction
 *            PWR_STOPEntry_WFE - enter STOP mode with WFE instruction
 *
 * @return  none
 */
void PWR_EnterSTOPMode(uint32_t PWR_Regulator, uint8_t PWR_STOPEntry)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;
    tmpreg &= CTLR_DS_MASK;
    tmpreg |= PWR_Regulator;
    PWR->CTLR = tmpreg;

    NVIC->SCTLR |= (1 << 2);

    if(PWR_STOPEntry == PWR_STOPEntry_WFI)
    {
        __WFI();
    }
    else
    {
        __WFE();
    }

    NVIC->SCTLR &= ~(1 << 2);
}

/*********************************************************************
 * @fn      PWR_EnterSTANDBYMode
 *
 * @brief   Enters STANDBY mode.
 *
 * @return  none
 */
void PWR_EnterSTANDBYMode(void)
{
    PWR->CTLR |= PWR_CTLR_CWUF;
    PWR->CTLR |= PWR_CTLR_PDDS;
    NVIC->SCTLR |= (1 << 2);

    __WFI();
}

/*********************************************************************
 * @fn      PWR_GetFlagStatus
 *
 * @brief   Checks whether the specified PWR flag is set or not.
 *
 * @param   PWR_FLAG - specifies the flag to check.
 *            PWR_FLAG_WU - Wake Up flag
 *            PWR_FLAG_SB - StandBy flag
 *            PWR_FLAG_PVDO - PVD Output
 *
 * @return  none
 */
FlagStatus PWR_GetFlagStatus(uint32_t PWR_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((PWR->CSR & PWR_FLAG) != (uint32_t)RESET)
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
 * @fn      PWR_ClearFlag
 *
 * @brief   Clears the PWR's pending flags.
 *
 * @param   PWR_FLAG - specifies the flag to clear.
 *            PWR_FLAG_WU - Wake Up flag
 *            PWR_FLAG_SB - StandBy flag
 *
 * @return  none
 */
void PWR_ClearFlag(uint32_t PWR_FLAG)
{
    PWR->CTLR |= PWR_FLAG << 2;
}

/*********************************************************************
 * @fn      PWR_EnterSTANDBYMode_RAM
 *
 * @brief   Enters STANDBY mode with RAM data retention function on.
 *
 * @return  none
 */
void PWR_EnterSTANDBYMode_RAM(void)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;

    tmpreg |= PWR_CTLR_CWUF;
    tmpreg |= PWR_CTLR_PDDS;

    //2K+30K in standby w power.
    tmpreg |= (0x1 << 16) | (0x1 << 17);

    PWR->CTLR = tmpreg;

    NVIC->SCTLR |= (1 << 2);

    __WFI();
}

/*********************************************************************
 * @fn      PWR_EnterSTANDBYMode_RAM_LV
 *
 * @brief   Enters STANDBY mode with RAM data retention function and LV mode on.
 *
 * @return  none
 */
void PWR_EnterSTANDBYMode_RAM_LV(void)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;

    tmpreg |= PWR_CTLR_CWUF;
    tmpreg |= PWR_CTLR_PDDS;

    //2K+30K in standby power.
    tmpreg |= (0x1 << 16) | (0x1 << 17);
    //2K+30K in standby LV .
    tmpreg |= (0x1 << 20);

    PWR->CTLR = tmpreg;

    NVIC->SCTLR |= (1 << 2);

    __WFI();
}

/*********************************************************************
 * @fn      PWR_EnterSTANDBYMode_RAM_VBAT_EN
 *
 * @brief   Enters STANDBY mode with RAM data retention function on (VBAT Enable).
 *
 * @return  none
 */
void PWR_EnterSTANDBYMode_RAM_VBAT_EN(void)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;

    tmpreg |= PWR_CTLR_CWUF;
    tmpreg |= PWR_CTLR_PDDS;

    //2K+30K in standby power (VBAT Enable).
    tmpreg |= (0x1 << 18) | (0x1 << 19);

    PWR->CTLR = tmpreg;

    NVIC->SCTLR |= (1 << 2);

    __WFI();
}

/*********************************************************************
 * @fn      PWR_EnterSTANDBYMode_RAM_LV_VBAT_EN
 *
 * @brief   Enters STANDBY mode with RAM data retention function and LV mode on(VBAT Enable).
 *
 * @return  none
 */
void PWR_EnterSTANDBYMode_RAM_LV_VBAT_EN(void)
{
    uint32_t tmpreg = 0;
    tmpreg = PWR->CTLR;

    tmpreg |= PWR_CTLR_CWUF;
    tmpreg |= PWR_CTLR_PDDS;

    //2K+30K in standby power (VBAT Enable).
    tmpreg |= (0x1 << 18) | (0x1 << 19);
    //2K+30K in standby LV .
    tmpreg |= (0x1 << 20);

    PWR->CTLR = tmpreg;

    NVIC->SCTLR |= (1 << 2);

    __WFI();
}
