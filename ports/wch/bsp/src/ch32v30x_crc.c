/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_crc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the CRC firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_crc.h"

/*********************************************************************
 * @fn      CRC_ResetDR
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void CRC_ResetDR(void)
{
    CRC->CTLR = CRC_CTLR_RESET;
}

/*********************************************************************
 * @fn      CRC_CalcCRC
 *
 * @brief   Computes the 32-bit CRC of a given data word(32-bit).
 *
 * @param   Data - data word(32-bit) to compute its CRC.
 *
 * @return  32-bit CRC.
 */
uint32_t CRC_CalcCRC(uint32_t Data)
{
    CRC->DATAR = Data;

    return (CRC->DATAR);
}

/*********************************************************************
 * @fn      CRC_CalcBlockCRC
 *
 * @brief   Computes the 32-bit CRC of a given buffer of data word(32-bit).
 *
 * @param   pBuffer - pointer to the buffer containing the data to be computed.
 *          BufferLength - length of the buffer to be computed.
 *
 * @return  32-bit CRC.
 */
uint32_t CRC_CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength)
{
    uint32_t index = 0;

    for(index = 0; index < BufferLength; index++)
    {
        CRC->DATAR = pBuffer[index];
    }

    return (CRC->DATAR);
}

/*********************************************************************
 * @fn      CRC_GetCRC
 *
 * @brief   Returns the current CRC value.
 *
 * @return  32-bit CRC.
 */
uint32_t CRC_GetCRC(void)
{
    return (CRC->IDATAR);
}

/*********************************************************************
 * @fn      CRC_SetIDRegister
 *
 * @brief   Stores a 8-bit data in the Independent Data(ID) register.
 *
 * @param   IDValue - 8-bit value to be stored in the ID register.
 *
 * @return  none
 */
void CRC_SetIDRegister(uint8_t IDValue)
{
    CRC->IDATAR = IDValue;
}

/*********************************************************************
 * @fn      CRC_GetIDRegister
 *
 * @brief   Returns the 8-bit data stored in the Independent Data(ID) register.
 *
 * @return  8-bit value of the ID register.
 */
uint8_t CRC_GetIDRegister(void)
{
    return (CRC->IDATAR);
}
