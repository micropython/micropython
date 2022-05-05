/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_bkp.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the BKP firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_bkp.h"
#include "ch32v30x_rcc.h"

/* BKP registers bit mask */

/* OCTLR register bit mask */
#define OCTLR_CAL_MASK    ((uint16_t)0xFF80)
#define OCTLR_MASK        ((uint16_t)0xFC7F)

/*********************************************************************
 * @fn      BKP_DeInit
 *
 * @brief   Deinitializes the BKP peripheral registers to their default reset values.
 *
 * @return  none
 */
void BKP_DeInit(void)
{
    RCC_BackupResetCmd(ENABLE);
    RCC_BackupResetCmd(DISABLE);
}

/*********************************************************************
 * @fn      BKP_TamperPinLevelConfig
 *
 * @brief   Configures the Tamper Pin active level.
 *
 * @param   BKP_TamperPinLevel: specifies the Tamper Pin active level.
 *            BKP_TamperPinLevel_High - Tamper pin active on high level.
 *            BKP_TamperPinLevel_Low - Tamper pin active on low level.
 *
 * @return  none
 */
void BKP_TamperPinLevelConfig(uint16_t BKP_TamperPinLevel)
{
    if(BKP_TamperPinLevel)
    {
        BKP->TPCTLR |= (1 << 1);
    }
    else
    {
        BKP->TPCTLR &= ~(1 << 1);
    }
}

/*********************************************************************
 * @fn      BKP_TamperPinCmd
 *
 * @brief   Enables or disables the Tamper Pin activation.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void BKP_TamperPinCmd(FunctionalState NewState)
{
    if(NewState)
    {
        BKP->TPCTLR |= (1 << 0);
    }
    else
    {
        BKP->TPCTLR &= ~(1 << 0);
    }
}

/*********************************************************************
 * @fn      BKP_ITConfig
 *
 * @brief   Enables or disables the Tamper Pin Interrupt.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void BKP_ITConfig(FunctionalState NewState)
{
    if(NewState)
    {
        BKP->TPCSR |= (1 << 2);
    }
    else
    {
        BKP->TPCSR &= ~(1 << 2);
    }
}

/*********************************************************************
 * @fn      BKP_RTCOutputConfig
 *
 * @brief   Select the RTC output source to output on the Tamper pin.
 *
 * @param   BKP_RTCOutputSource - specifies the RTC output source.
 *            BKP_RTCOutputSource_None - no RTC output on the Tamper pin.
 *            BKP_RTCOutputSource_CalibClock - output the RTC clock with
 *        frequency divided by 64 on the Tamper pin.
 *            BKP_RTCOutputSource_Alarm - output the RTC Alarm pulse signal
 *        on the Tamper pin.
 *            BKP_RTCOutputSource_Second - output the RTC Second pulse
 *        signal on the Tamper pin.
 *
 * @return  none
 */
void BKP_RTCOutputConfig(uint16_t BKP_RTCOutputSource)
{
    uint16_t tmpreg = 0;

    tmpreg = BKP->OCTLR;
    tmpreg &= OCTLR_MASK;
    tmpreg |= BKP_RTCOutputSource;
    BKP->OCTLR = tmpreg;
}

/*********************************************************************
 * @fn      BKP_SetRTCCalibrationValue
 *
 * @brief   Sets RTC Clock Calibration value.
 *
 * @param   CalibrationValue - specifies the RTC Clock Calibration value.
 *            This parameter must be a number between 0 and 0x1F.
 *
 * @return  none
 */
void BKP_SetRTCCalibrationValue(uint8_t CalibrationValue)
{
    uint16_t tmpreg = 0;

    tmpreg = BKP->OCTLR;
    tmpreg &= OCTLR_CAL_MASK;
    tmpreg |= CalibrationValue;
    BKP->OCTLR = tmpreg;
}

/*********************************************************************
 * @fn      BKP_WriteBackupRegister
 *
 * @brief   Writes user data to the specified Data Backup Register.
 *
 * @param   BKP_DR - specifies the Data Backup Register.
 *          Data - data to write.
 *
 * @return  none
 */
void BKP_WriteBackupRegister(uint16_t BKP_DR, uint16_t Data)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)BKP_BASE;
    tmp += BKP_DR;
    *(__IO uint32_t *)tmp = Data;
}

/*********************************************************************
 * @fn      BKP_ReadBackupRegister
 *
 * @brief   Reads data from the specified Data Backup Register.
 *
 * @param   BKP_DR - specifies the Data Backup Register.
 *            This parameter can be BKP_DRx where x=[1, 42].
 *
 * @return  none
 */
uint16_t BKP_ReadBackupRegister(uint16_t BKP_DR)
{
    __IO uint32_t tmp = 0;

    tmp = (uint32_t)BKP_BASE;
    tmp += BKP_DR;

    return (*(__IO uint16_t *)tmp);
}

/*********************************************************************
 * @fn      BKP_GetFlagStatus
 *
 * @brief   Checks whether the Tamper Pin Event flag is set or not.
 *
 * @return  FlagStatus - SET or RESET.
 */
FlagStatus BKP_GetFlagStatus(void)
{
    if(BKP->TPCSR & (1 << 8))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*********************************************************************
 * @fn      BKP_ClearFlag
 *
 * @brief   Clears Tamper Pin Event pending flag.
 *
 * @return  none
 */
void BKP_ClearFlag(void)
{
    BKP->TPCSR |= BKP_CTE;
}

/*********************************************************************
 * @fn      BKP_GetITStatus
 *
 * @brief   Checks whether the Tamper Pin Interrupt has occurred or not.
 *
 * @return  ITStatus - SET or RESET.
 */
ITStatus BKP_GetITStatus(void)
{
    if(BKP->TPCSR & (1 << 9))
    {
        return SET;
    }
    else
    {
        return RESET;
    }
}

/*********************************************************************
 * @fn      BKP_ClearITPendingBit
 *
 * @brief   Clears Tamper Pin Interrupt pending bit.
 *
 * @return  none
 */
void BKP_ClearITPendingBit(void)
{
    BKP->TPCSR |= BKP_CTI;
}
