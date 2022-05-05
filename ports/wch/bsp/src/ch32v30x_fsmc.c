/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_fsmc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the FSMC firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_fsmc.h"
#include "ch32v30x_rcc.h"

/* FSMC BCRx Mask */
#define BCR_MBKEN_Set          ((uint32_t)0x00000001)
#define BCR_MBKEN_Reset        ((uint32_t)0x000FFFFE)
#define BCR_FACCEN_Set         ((uint32_t)0x00000040)

/* FSMC PCRx Mask */
#define PCR_PBKEN_Set          ((uint32_t)0x00000004)
#define PCR_PBKEN_Reset        ((uint32_t)0x000FFFFB)
#define PCR_ECCEN_Set          ((uint32_t)0x00000040)
#define PCR_ECCEN_Reset        ((uint32_t)0x000FFFBF)
#define PCR_MemoryType_NAND    ((uint32_t)0x00000008)

/*********************************************************************
 * @fn      FSMC_NORSRAMDeInit
 *
 * @brief   Deinitializes the FSMC NOR/SRAM Banks registers to their default
 *        reset values.
 *
 * @param   FSMC_Bank-
 *            FSMC_Bank1_NORSRAM1 - FSMC Bank1 NOR/SRAM1.
 *
 * @return  none
 */
void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank)
{
    if(FSMC_Bank == FSMC_Bank1_NORSRAM1)
    {
        FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030DB;
    }
    else
    {
        FSMC_Bank1->BTCR[FSMC_Bank] = 0x000030D2;
    }
    FSMC_Bank1->BTCR[FSMC_Bank + 1] = 0x0FFFFFFF;
    FSMC_Bank1E->BWTR[FSMC_Bank] = 0x0FFFFFFF;
}

/*********************************************************************
 * @fn      FSMC_NANDDeInit
 *
 * @brief   Deinitializes the FSMC NAND Banks registers to their default
 *        reset values.
 *
 * @param   FSMC_Bank -
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND.
 *
 * @return  none
 */
void FSMC_NANDDeInit(uint32_t FSMC_Bank)
{
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        FSMC_Bank2->PCR2 = 0x00000018;
        FSMC_Bank2->SR2 = 0x00000040;
        FSMC_Bank2->PMEM2 = 0xFCFCFCFC;
        FSMC_Bank2->PATT2 = 0xFCFCFCFC;
    }
}

/*********************************************************************
 * @fn      FSMC_NORSRAMInit
 *
 * @brief   Initializes the FSMC NOR/SRAM Banks according to the specified
 *        parameters in the FSMC_NORSRAMInitStruct.
 *
 * @param   SMC_NORSRAMInitStruct:pointer to a FSMC_NORSRAMInitTypeDef
 *        structure that contains the configuration information for the FSMC NOR/SRAM
 *        specified Banks.
 *
 * @return  none
 */
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef *FSMC_NORSRAMInitStruct)
{
    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] =
        (uint32_t)FSMC_NORSRAMInitStruct->FSMC_DataAddressMux |
        FSMC_NORSRAMInitStruct->FSMC_MemoryType |
        FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth |
        FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode |
        FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait |
        FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity |
        FSMC_NORSRAMInitStruct->FSMC_WrapMode |
        FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive |
        FSMC_NORSRAMInitStruct->FSMC_WriteOperation |
        FSMC_NORSRAMInitStruct->FSMC_WaitSignal |
        FSMC_NORSRAMInitStruct->FSMC_ExtendedMode |
        FSMC_NORSRAMInitStruct->FSMC_WriteBurst;

    if(FSMC_NORSRAMInitStruct->FSMC_MemoryType == FSMC_MemoryType_NOR)
    {
        FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank] |= (uint32_t)BCR_FACCEN_Set;
    }

    FSMC_Bank1->BTCR[FSMC_NORSRAMInitStruct->FSMC_Bank + 1] =
        (uint32_t)FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime |
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime << 4) |
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime << 8) |
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration << 16) |
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision << 20) |
        (FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency << 24) |
        FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode;

    if(FSMC_NORSRAMInitStruct->FSMC_ExtendedMode == FSMC_ExtendedMode_Enable)
    {
        FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] =
            (uint32_t)FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime |
            (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime << 4) |
            (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime << 8) |
            (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision << 20) |
            (FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency << 24) |
            FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode;
    }
    else
    {
        FSMC_Bank1E->BWTR[FSMC_NORSRAMInitStruct->FSMC_Bank] = 0x0FFFFFFF;
    }
}

/*********************************************************************
 * @fn      FSMC_NANDInit
 *
 * @brief   Initializes the FSMC NAND Banks according to the specified
 *        parameters in the FSMC_NANDInitStruct.
 *
 * @param   FSMC_NANDInitStruct - pointer to a FSMC_NANDInitTypeDef
 *        structure that contains the configuration information for the FSMC
 *        NAND specified Banks.
 *
 * @return  none
 */
void FSMC_NANDInit(FSMC_NANDInitTypeDef *FSMC_NANDInitStruct)
{
    uint32_t tmppcr = 0x00000000, tmppmem = 0x00000000, tmppatt = 0x00000000;

    tmppcr = (uint32_t)FSMC_NANDInitStruct->FSMC_Waitfeature |
             PCR_MemoryType_NAND |
             FSMC_NANDInitStruct->FSMC_MemoryDataWidth |
             FSMC_NANDInitStruct->FSMC_ECC |
             FSMC_NANDInitStruct->FSMC_ECCPageSize |
             (FSMC_NANDInitStruct->FSMC_TCLRSetupTime << 9) |
             (FSMC_NANDInitStruct->FSMC_TARSetupTime << 13);

    tmppmem = (uint32_t)FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime |
              (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
              (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime << 16) |
              (FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime << 24);

    tmppatt = (uint32_t)FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime |
              (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime << 8) |
              (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime << 16) |
              (FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime << 24);

    if(FSMC_NANDInitStruct->FSMC_Bank == FSMC_Bank2_NAND)
    {
        FSMC_Bank2->PCR2 = tmppcr;
        FSMC_Bank2->PMEM2 = tmppmem;
        FSMC_Bank2->PATT2 = tmppatt;
    }
}

/*********************************************************************
 * @fn      FSMC_NORSRAMStructInit
 *
 * @brief   Fills each FSMC_NORSRAMInitStruct member with its default value.
 *
 * @param   FSMC_NORSRAMInitStruct - pointer to a FSMC_NORSRAMInitTypeDef
 *        structure which will be initialized.
 *
 * @return  none
 */
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef *FSMC_NORSRAMInitStruct)
{
    FSMC_NORSRAMInitStruct->FSMC_Bank = FSMC_Bank1_NORSRAM1;
    FSMC_NORSRAMInitStruct->FSMC_DataAddressMux = FSMC_DataAddressMux_Enable;
    FSMC_NORSRAMInitStruct->FSMC_MemoryType = FSMC_MemoryType_SRAM;
    FSMC_NORSRAMInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NORSRAMInitStruct->FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
    FSMC_NORSRAMInitStruct->FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
    FSMC_NORSRAMInitStruct->FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
    FSMC_NORSRAMInitStruct->FSMC_WrapMode = FSMC_WrapMode_Disable;
    FSMC_NORSRAMInitStruct->FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
    FSMC_NORSRAMInitStruct->FSMC_WriteOperation = FSMC_WriteOperation_Enable;
    FSMC_NORSRAMInitStruct->FSMC_WaitSignal = FSMC_WaitSignal_Enable;
    FSMC_NORSRAMInitStruct->FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
    FSMC_NORSRAMInitStruct->FSMC_WriteBurst = FSMC_WriteBurst_Disable;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressSetupTime = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AddressHoldTime = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataSetupTime = 0xFF;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_BusTurnAroundDuration = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_CLKDivision = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_DataLatency = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_ReadWriteTimingStruct->FSMC_AccessMode = FSMC_AccessMode_A;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressSetupTime = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AddressHoldTime = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataSetupTime = 0xFF;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_BusTurnAroundDuration = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_CLKDivision = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_DataLatency = 0xF;
    FSMC_NORSRAMInitStruct->FSMC_WriteTimingStruct->FSMC_AccessMode = FSMC_AccessMode_A;
}

/*********************************************************************
 * @fn      FSMC_NANDStructInit
 *
 * @brief   Fills each FSMC_NANDInitStruct member with its default value.
 *
 * @param   FSMC_NANDInitStruct - pointer to a FSMC_NANDInitTypeDef
 *        structure which will be initialized.
 *
 * @return  none
 */
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef *FSMC_NANDInitStruct)
{
    FSMC_NANDInitStruct->FSMC_Bank = FSMC_Bank2_NAND;
    FSMC_NANDInitStruct->FSMC_Waitfeature = FSMC_Waitfeature_Disable;
    FSMC_NANDInitStruct->FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_8b;
    FSMC_NANDInitStruct->FSMC_ECC = FSMC_ECC_Disable;
    FSMC_NANDInitStruct->FSMC_ECCPageSize = FSMC_ECCPageSize_256Bytes;
    FSMC_NANDInitStruct->FSMC_TCLRSetupTime = 0x0;
    FSMC_NANDInitStruct->FSMC_TARSetupTime = 0x0;
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_SetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_CommonSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_SetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_WaitSetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HoldSetupTime = 0xFC;
    FSMC_NANDInitStruct->FSMC_AttributeSpaceTimingStruct->FSMC_HiZSetupTime = 0xFC;
}

/*********************************************************************
 * @fn      FSMC_NORSRAMCmd
 *
 * @brief   Enables or disables the specified NOR/SRAM Memory Bank.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank1_NORSRAM1 - FSMC Bank1 NOR/SRAM1
 *            FSMC_Bank1_NORSRAM2 - FSMC Bank1 NOR/SRAM2
 *            FSMC_Bank1_NORSRAM3 - FSMC Bank1 NOR/SRAM3
 *            FSMC_Bank1_NORSRAM4 - FSMC Bank1 NOR/SRAM4
 *          NewState£ºENABLE or DISABLE.
 *
 * @return  none
 */
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        FSMC_Bank1->BTCR[FSMC_Bank] |= BCR_MBKEN_Set;
    }
    else
    {
        FSMC_Bank1->BTCR[FSMC_Bank] &= BCR_MBKEN_Reset;
    }
}

/*********************************************************************
 * @fn      FSMC_NANDCmd
 *
 * @brief   Enables or disables the specified NAND Memory Bank.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *            FSMC_Bank3_NAND - FSMC Bank3 NAND
 *          NewStat - ENABLE or DISABLE.
 *
 * @return  none
 */
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 |= PCR_PBKEN_Set;
        }
    }
    else
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 &= PCR_PBKEN_Reset;
        }
    }
}

/*********************************************************************
 * @fn      FSMC_NANDECCCmd
 *
 * @brief   Enables or disables the FSMC NAND ECC feature.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 |= PCR_ECCEN_Set;
        }
    }
    else
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->PCR2 &= PCR_ECCEN_Reset;
        }
    }
}

/*********************************************************************
 * @fn      FSMC_GetECC
 *
 * @brief   Returns the error correction code register value.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          NewState - ENABLE or DISABLE.
 *
 * @return  eccval - The Error Correction Code (ECC) value.
 */
uint32_t FSMC_GetECC(uint32_t FSMC_Bank)
{
    uint32_t eccval = 0x00000000;

    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        eccval = FSMC_Bank2->ECCR2;
    }

    return (eccval);
}

/*********************************************************************
 * @fn      FSMC_ITConfig
 *
 * @brief   Enables or disables the specified FSMC interrupts.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          FSMC_IT - specifies the FSMC interrupt sources to be enabled or disabled.
 *            FSMC_IT_RisingEdge - Rising edge detection interrupt.
 *            FSMC_IT_Level - Level edge detection interrupt.
 *            FSMC_IT_FallingEdge - Falling edge detection interrupt.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void FSMC_ITConfig(uint32_t FSMC_Bank, uint32_t FSMC_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->SR2 |= FSMC_IT;
        }
    }
    else
    {
        if(FSMC_Bank == FSMC_Bank2_NAND)
        {
            FSMC_Bank2->SR2 &= (uint32_t)~FSMC_IT;
        }
    }
}

/*********************************************************************
 * @fn      FSMC_GetFlagStatus
 *
 * @brief   Checks whether the specified FSMC flag is set or not.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          FSMC_FLAG - specifies the flag to check.
 *            FSMC_FLAG_RisingEdge - Rising egde detection Flag.
 *            FSMC_FLAG_Level - Level detection Flag.
 *            FSMC_FLAG_FallingEdge - Falling egde detection Flag.
 *            FSMC_FLAG_FEMPT - Fifo empty Flag.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  FlagStatus - The new state of FSMC_FLAG (SET or RESET).
 */
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
    FlagStatus bitstatus = RESET;
    uint32_t   tmpsr = 0x00000000;

    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        tmpsr = FSMC_Bank2->SR2;
    }

    if((tmpsr & FSMC_FLAG) != (uint16_t)RESET)
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
 * @fn      FSMC_ClearFlag
 *
 * @brief   Clears the FSMC's pending flags.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          FSMC_FLAG - specifies the flag to check.
 *            FSMC_FLAG_RisingEdge - Rising egde detection Flag.
 *            FSMC_FLAG_Level - Level detection Flag.
 *            FSMC_FLAG_FallingEdge - Falling egde detection Flag.
 *
 * @return  none
 */
void FSMC_ClearFlag(uint32_t FSMC_Bank, uint32_t FSMC_FLAG)
{
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        FSMC_Bank2->SR2 &= ~FSMC_FLAG;
    }
}

/*********************************************************************
 * @fn      FSMC_GetITStatus
 *
 * @brief   Checks whether the specified FSMC interrupt has occurred or not.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          FSMC_IT - specifies the FSMC interrupt source to check.
 *            FSMC_IT_RisingEdge - Rising edge detection interrupt.
 *            FSMC_IT_Level - Level edge detection interrupt.
 *            FSMC_IT_FallingEdge - Falling edge detection interrupt.
 *
 * @return  ITStatus - The new state of FSMC_IT (SET or RESET).
 */
ITStatus FSMC_GetITStatus(uint32_t FSMC_Bank, uint32_t FSMC_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t tmpsr = 0x0, itstatus = 0x0, itenable = 0x0;

    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        tmpsr = FSMC_Bank2->SR2;
    }

    itstatus = tmpsr & FSMC_IT;

    itenable = tmpsr & (FSMC_IT >> 3);
    if((itstatus != (uint32_t)RESET) && (itenable != (uint32_t)RESET))
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
 * @fn      FSMC_ClearITPendingBit
 *
 * @brief   Clears the FSMC's interrupt pending bits.
 *
 * @param   FSMC_Bank - specifies the FSMC Bank to be used
 *            FSMC_Bank2_NAND - FSMC Bank2 NAND
 *          FSMC_IT - specifies the FSMC interrupt source to check.
 *            FSMC_IT_RisingEdge - Rising edge detection interrupt.
 *            FSMC_IT_Level - Level edge detection interrupt.
 *            FSMC_IT_FallingEdge - Falling edge detection interrupt.
 *
 * @return  none
 */
void FSMC_ClearITPendingBit(uint32_t FSMC_Bank, uint32_t FSMC_IT)
{
    if(FSMC_Bank == FSMC_Bank2_NAND)
    {
        FSMC_Bank2->SR2 &= ~(FSMC_IT >> 3);
    }
}
