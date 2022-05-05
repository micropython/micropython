/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_flash.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the FLASH firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
***************************************************************************************/
#include "ch32v30x_flash.h"

/* Flash Control Register bits */
#define CR_PG_Set                  ((uint32_t)0x00000001)
#define CR_PG_Reset                ((uint32_t)0x00001FFE)
#define CR_PER_Set                 ((uint32_t)0x00000002)
#define CR_PER_Reset               ((uint32_t)0x00001FFD)
#define CR_MER_Set                 ((uint32_t)0x00000004)
#define CR_MER_Reset               ((uint32_t)0x00001FFB)
#define CR_OPTPG_Set               ((uint32_t)0x00000010)
#define CR_OPTPG_Reset             ((uint32_t)0x00001FEF)
#define CR_OPTER_Set               ((uint32_t)0x00000020)
#define CR_OPTER_Reset             ((uint32_t)0x00001FDF)
#define CR_STRT_Set                ((uint32_t)0x00000040)
#define CR_LOCK_Set                ((uint32_t)0x00000080)
#define CR_FAST_LOCK_Set           ((uint32_t)0x00008000)
#define CR_PAGE_PG                 ((uint32_t)0x00010000)
#define CR_PAGE_ER                 ((uint32_t)0x00020000)
#define CR_BER32                   ((uint32_t)0x00040000)
#define CR_BER64                   ((uint32_t)0x00080000)
#define CR_PG_STRT                 ((uint32_t)0x00200000)

/* FLASH Status Register bits */
#define SR_BSY                     ((uint32_t)0x00000001)
#define SR_WR_BSY                  ((uint32_t)0x00000002)
#define SR_WRPRTERR                ((uint32_t)0x00000010)
#define SR_EOP                     ((uint32_t)0x00000020)

/* FLASH Mask */
#define RDPRT_Mask                 ((uint32_t)0x00000002)
#define WRP0_Mask                  ((uint32_t)0x000000FF)
#define WRP1_Mask                  ((uint32_t)0x0000FF00)
#define WRP2_Mask                  ((uint32_t)0x00FF0000)
#define WRP3_Mask                  ((uint32_t)0xFF000000)
#define OB_USER_BFB2               ((uint16_t)0x0008)

/* FLASH Keys */
#define RDP_Key                    ((uint16_t)0x00A5)
#define FLASH_KEY1                 ((uint32_t)0x45670123)
#define FLASH_KEY2                 ((uint32_t)0xCDEF89AB)

/* FLASH BANK address */
#define FLASH_BANK1_END_ADDRESS    ((uint32_t)0x807FFFF)

/* Delay definition */
#define EraseTimeout               ((uint32_t)0x000B0000)
#define ProgramTimeout             ((uint32_t)0x00005000)

/*********************************************************************
 * @fn      FLASH_Unlock
 *
 * @brief   Unlocks the FLASH Program Erase Controller.
 *
 * @return  none
 */
void FLASH_Unlock(void)
{
    /* Authorize the FPEC of Bank1 Access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/*********************************************************************
 * @fn      FLASH_UnlockBank1
 *
 * @brief   Unlocks the FLASH Bank1 Program Erase Controller.
 *          equivalent to FLASH_Unlock function.
 *
 * @return  none
 */
void FLASH_UnlockBank1(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/*********************************************************************
 * @fn      FLASH_Lock
 *
 * @brief   Locks the FLASH Program Erase Controller.
 *
 * @return  none
 */
void FLASH_Lock(void)
{
    FLASH->CTLR |= CR_LOCK_Set;
}

/*********************************************************************
 * @fn      FLASH_LockBank1
 *
 * @brief   Locks the FLASH Bank1 Program Erase Controller.
 *
 * @return  none
 */
void FLASH_LockBank1(void)
{
    FLASH->CTLR |= CR_LOCK_Set;
}

/*********************************************************************
 * @fn      FLASH_ErasePage
 *
 * @brief   Erases a specified FLASH page(page size 4KB).
 *
 * @param   Page_Address - The page address to be erased.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_WaitForLastOperation(EraseTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_PER_Set;
        FLASH->ADDR = Page_Address;
        FLASH->CTLR |= CR_STRT_Set;

        status = FLASH_WaitForLastOperation(EraseTimeout);

        FLASH->CTLR &= CR_PER_Reset;
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_EraseAllPages
 *
 * @brief   Erases all FLASH pages.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_EraseAllPages(void)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_MER_Set;
        FLASH->CTLR |= CR_STRT_Set;

        status = FLASH_WaitForLastOperation(EraseTimeout);

        FLASH->CTLR &= CR_MER_Reset;
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_EraseAllBank1Pages
 *
 * @brief   Erases all Bank1 FLASH pages.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_EraseAllBank1Pages(void)
{
    FLASH_Status status = FLASH_COMPLETE;
    status = FLASH_WaitForLastBank1Operation(EraseTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_MER_Set;
        FLASH->CTLR |= CR_STRT_Set;

        status = FLASH_WaitForLastBank1Operation(EraseTimeout);

        FLASH->CTLR &= CR_MER_Reset;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_EraseOptionBytes
 *
 * @brief   Erases the FLASH option bytes.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_EraseOptionBytes(void)
{
    uint16_t     rdptmp = RDP_Key;
    uint32_t     Address = 0x1FFFF800;
    __IO uint8_t i;

    FLASH_Status status = FLASH_COMPLETE;
    if(FLASH_GetReadOutProtectionStatus() != RESET)
    {
        rdptmp = 0x00;
    }
    status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    {
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        status = FLASH_WaitForLastOperation(EraseTimeout);

        if(status == FLASH_COMPLETE)
        {
            FLASH->CTLR &= CR_OPTER_Reset;
            FLASH->CTLR |= CR_OPTPG_Set;
            OB->RDPR = (uint16_t)rdptmp;
            status = FLASH_WaitForLastOperation(ProgramTimeout);

            if(status != FLASH_TIMEOUT)
            {
                FLASH->CTLR &= CR_OPTPG_Reset;
            }
        }
        else
        {
            if(status != FLASH_TIMEOUT)
            {
                FLASH->CTLR &= CR_OPTPG_Reset;
            }
        }

        /* Write 0xFF */
        FLASH->CTLR |= CR_OPTPG_Set;

        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Address + 2 * i) = 0x00FF;
            while(FLASH->STATR & SR_BSY)
                ;
        }

        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_ProgramWord
 *
 * @brief   Programs a word at a specified address.
 *
 * @param   Address - specifies the address to be programmed.
 *          Data - specifies the data to be programmed.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ProgramWord(uint32_t Address, uint32_t Data)
{
    FLASH_Status  status = FLASH_COMPLETE;
    __IO uint32_t tmp = 0;

    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_PG_Set;

        *(__IO uint16_t *)Address = (uint16_t)Data;
        status = FLASH_WaitForLastOperation(ProgramTimeout);

        if(status == FLASH_COMPLETE)
        {
            tmp = Address + 2;
            *(__IO uint16_t *)tmp = Data >> 16;
            status = FLASH_WaitForLastOperation(ProgramTimeout);
            FLASH->CTLR &= CR_PG_Reset;
        }
        else
        {
            FLASH->CTLR &= CR_PG_Reset;
        }
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_ProgramHalfWord
 *
 * @brief   Programs a half word at a specified address.
 *
 * @param   Address - specifies the address to be programmed.
 *          Data - specifies the data to be programmed.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ProgramHalfWord(uint32_t Address, uint16_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->CTLR |= CR_PG_Set;
        *(__IO uint16_t *)Address = Data;
        status = FLASH_WaitForLastOperation(ProgramTimeout);
        FLASH->CTLR &= CR_PG_Reset;
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_ProgramOptionByteData
 *
 * @brief   Programs a half word at a specified Option Byte Data address.
 *
 * @param   Address - specifies the address to be programmed.
 *          Data - specifies the data to be programmed.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ProgramOptionByteData(uint32_t Address, uint8_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];

    status = FLASH_WaitForLastOperation(ProgramTimeout);
    if(status == FLASH_COMPLETE)
    {
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* Read optionbytes */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* Erase optionbytes */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* Write optionbytes */
        pbuf[((Address - 0x1FFFF800) / 2)] = ((((uint16_t) ~(Data)) << 8) | ((uint16_t)Data));

        FLASH->CTLR |= CR_OPTPG_Set;

        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }

        FLASH->CTLR &= ~CR_OPTPG_Set;
    }

    return status;
}

/*********************************************************************
 * @fn      FLASH_EnableWriteProtection
 *
 * @brief   Write protects the desired sectors
 *
 * @param   FLASH_Sectors - specifies the address of the pages to be write protected.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Sectors)
{
    uint16_t     WRP0_Data = 0xFFFF, WRP1_Data = 0xFFFF, WRP2_Data = 0xFFFF, WRP3_Data = 0xFFFF;
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];

    FLASH_Sectors = (uint32_t)(~FLASH_Sectors);
    WRP0_Data = (uint16_t)(FLASH_Sectors & WRP0_Mask);
    WRP1_Data = (uint16_t)((FLASH_Sectors & WRP1_Mask) >> 8);
    WRP2_Data = (uint16_t)((FLASH_Sectors & WRP2_Mask) >> 16);
    WRP3_Data = (uint16_t)((FLASH_Sectors & WRP3_Mask) >> 24);

    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* Read optionbytes */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* Erase optionbytes */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* Write optionbytes */
        pbuf[4] = WRP0_Data;
        pbuf[5] = WRP1_Data;
        pbuf[6] = WRP2_Data;
        pbuf[7] = WRP3_Data;

        FLASH->CTLR |= CR_OPTPG_Set;
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_ReadOutProtection
 *
 * @brief   Enables or disables the read out protection.
 *
 * @param   Newstate - new state of the ReadOut Protection(ENABLE or DISABLE).
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];

    status = FLASH_WaitForLastOperation(EraseTimeout);
    if(status == FLASH_COMPLETE)
    {
        FLASH->OBKEYR = FLASH_KEY1;
        FLASH->OBKEYR = FLASH_KEY2;

        /* Read optionbytes */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* Erase optionbytes */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* Write optionbytes */
        if(NewState == DISABLE)
            pbuf[0] = 0x5AA5;
        else
            pbuf[0] = 0x00FF;

        FLASH->CTLR |= CR_OPTPG_Set;
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_UserOptionByteConfig
 *
 * @brief   Programs the FLASH User Option Byte - IWDG_SW / RST_STOP / RST_STDBY.
 *
 * @param   OB_IWDG - Selects the IWDG mode
 *            OB_IWDG_SW - Software IWDG selected
 *            OB_IWDG_HW - Hardware IWDG selected
 *          OB_STOP - Reset event when entering STOP mode.
 *            OB_STOP_NoRST - No reset generated when entering in STOP
 *            OB_STOP_RST - Reset generated when entering in STOP
 *          OB_STDBY - Reset event when entering Standby mode.
 *            OB_STDBY_NoRST - No reset generated when entering in STANDBY
 *            OB_STDBY_RST - Reset generated when entering in STANDBY
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STOP, uint16_t OB_STDBY)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t     Addr = 0x1FFFF800;
    __IO uint8_t i;
    uint16_t     pbuf[8];

    FLASH->OBKEYR = FLASH_KEY1;
    FLASH->OBKEYR = FLASH_KEY2;
    status = FLASH_WaitForLastOperation(ProgramTimeout);

    if(status == FLASH_COMPLETE)
    {
        /* Read optionbytes */
        for(i = 0; i < 8; i++)
        {
            pbuf[i] = *(uint16_t *)(Addr + 2 * i);
        }

        /* Erase optionbytes */
        FLASH->CTLR |= CR_OPTER_Set;
        FLASH->CTLR |= CR_STRT_Set;
        while(FLASH->STATR & SR_BSY)
            ;
        FLASH->CTLR &= ~CR_OPTER_Set;

        /* Write optionbytes */
        pbuf[1] = OB_IWDG | (uint16_t)(OB_STOP | (uint16_t)(OB_STDBY | ((uint16_t)0xF8)));

        FLASH->CTLR |= CR_OPTPG_Set;
        for(i = 0; i < 8; i++)
        {
            *(uint16_t *)(Addr + 2 * i) = pbuf[i];
            while(FLASH->STATR & SR_BSY)
                ;
        }
        FLASH->CTLR &= ~CR_OPTPG_Set;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_GetUserOptionByte
 *
 * @brief   Returns the FLASH User Option Bytes values.
 *
 * @return  The FLASH User Option Bytes values:IWDG_SW(Bit0), RST_STOP(Bit1)
 *        and RST_STDBY(Bit2).
 */
uint32_t FLASH_GetUserOptionByte(void)
{
    return (uint32_t)(FLASH->OBR >> 2);
}

/*********************************************************************
 * @fn      FLASH_GetWriteProtectionOptionByte
 *
 * @brief   Returns the FLASH Write Protection Option Bytes Register value.
 *
 * @return  The FLASH Write Protection Option Bytes Register value.
 */
uint32_t FLASH_GetWriteProtectionOptionByte(void)
{
    return (uint32_t)(FLASH->WPR);
}

/*********************************************************************
 * @fn      FLASH_GetReadOutProtectionStatus
 *
 * @brief   Checks whether the FLASH Read Out Protection Status is set or not.
 *
 * @return  FLASH ReadOut Protection Status(SET or RESET)
 */
FlagStatus FLASH_GetReadOutProtectionStatus(void)
{
    FlagStatus readoutstatus = RESET;
    if((FLASH->OBR & RDPRT_Mask) != (uint32_t)RESET)
    {
        readoutstatus = SET;
    }
    else
    {
        readoutstatus = RESET;
    }
    return readoutstatus;
}

/*********************************************************************
 * @fn      FLASH_ITConfig
 *
 * @brief   Enables or disables the specified FLASH interrupts.
 *
 * @param   FLASH_IT - specifies the FLASH interrupt sources to be enabled or disabled.
 *            FLASH_IT_ERROR - FLASH Error Interrupt
 *            FLASH_IT_EOP - FLASH end of operation Interrupt
 *          NewState - new state of the specified Flash interrupts(ENABLE or DISABLE).
 *
 * @return  FLASH Prefetch Buffer Status (SET or RESET).
 */
void FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        FLASH->CTLR |= FLASH_IT;
    }
    else
    {
        FLASH->CTLR &= ~(uint32_t)FLASH_IT;
    }
}

/*********************************************************************
 * @fn      FLASH_GetFlagStatus
 *
 * @brief   Checks whether the specified FLASH flag is set or not.
 *
 * @param   FLASH_FLAG - specifies the FLASH flag to check.
 *            FLASH_FLAG_BSY - FLASH Busy flag
 *            FLASH_FLAG_PGERR - FLASH Program error flag
 *            FLASH_FLAG_WRPRTERR - FLASH Write protected error flag
 *            FLASH_FLAG_EOP - FLASH End of Operation flag
 *            FLASH_FLAG_OPTERR - FLASH Option Byte error flag
 *
 * @return  The new state of FLASH_FLAG (SET or RESET).
 */
FlagStatus FLASH_GetFlagStatus(uint32_t FLASH_FLAG)
{
    FlagStatus bitstatus = RESET;

    if(FLASH_FLAG == FLASH_FLAG_OPTERR)
    {
        if((FLASH->OBR & FLASH_FLAG_OPTERR) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    else
    {
        if((FLASH->STATR & FLASH_FLAG) != (uint32_t)RESET)
        {
            bitstatus = SET;
        }
        else
        {
            bitstatus = RESET;
        }
    }
    return bitstatus;
}

/*********************************************************************
 * @fn      FLASH_ClearFlag
 *
 * @brief   Clears the FLASH's pending flags.
 *
 * @param   FLASH_FLAG - specifies the FLASH flags to clear.
 *            FLASH_FLAG_PGERR - FLASH Program error flag
 *            FLASH_FLAG_WRPRTERR - FLASH Write protected error flag
 *            FLASH_FLAG_EOP - FLASH End of Operation flag
 *
 * @return  none
 */
void FLASH_ClearFlag(uint32_t FLASH_FLAG)
{
    FLASH->STATR = FLASH_FLAG;
}

/*********************************************************************
 * @fn      FLASH_GetStatus
 *
 * @brief   Returns the FLASH Status.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP or FLASH_COMPLETE.
 */
FLASH_Status FLASH_GetStatus(void)
{
    FLASH_Status flashstatus = FLASH_COMPLETE;

    if((FLASH->STATR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY)
    {
        flashstatus = FLASH_BUSY;
    }
    else
    {
        if((FLASH->STATR & FLASH_FLAG_PGERR) != 0)
        {
            flashstatus = FLASH_ERROR_PG;
        }
        else
        {
            if((FLASH->STATR & FLASH_FLAG_WRPRTERR) != 0)
            {
                flashstatus = FLASH_ERROR_WRP;
            }
            else
            {
                flashstatus = FLASH_COMPLETE;
            }
        }
    }
    return flashstatus;
}

/*********************************************************************
 * @fn      FLASH_GetBank1Status
 *
 * @brief   Returns the FLASH Bank1 Status.
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP or FLASH_COMPLETE.
 */
FLASH_Status FLASH_GetBank1Status(void)
{
    FLASH_Status flashstatus = FLASH_COMPLETE;

    if((FLASH->STATR & FLASH_FLAG_BANK1_BSY) == FLASH_FLAG_BSY)
    {
        flashstatus = FLASH_BUSY;
    }
    else
    {
        if((FLASH->STATR & FLASH_FLAG_BANK1_PGERR) != 0)
        {
            flashstatus = FLASH_ERROR_PG;
        }
        else
        {
            if((FLASH->STATR & FLASH_FLAG_BANK1_WRPRTERR) != 0)
            {
                flashstatus = FLASH_ERROR_WRP;
            }
            else
            {
                flashstatus = FLASH_COMPLETE;
            }
        }
    }
    return flashstatus;
}

/*********************************************************************
 * @fn      FLASH_WaitForLastOperation
 *
 * @brief   Waits for a Flash operation to complete or a TIMEOUT to occur.
 *
 * @param   Timeout - FLASH programming Timeout
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP or FLASH_COMPLETE.
 */
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_GetBank1Status();
    while((status == FLASH_BUSY) && (Timeout != 0x00))
    {
        status = FLASH_GetBank1Status();
        Timeout--;
    }
    if(Timeout == 0x00)
    {
        status = FLASH_TIMEOUT;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_WaitForLastBank1Operation
 *
 * @brief   Waits for a Flash operation on Bank1 to complete or a TIMEOUT to occur.
 *
 * @param   Timeout - FLASH programming Timeout
 *
 * @return  FLASH Status - The returned value can be: FLASH_BUSY, FLASH_ERROR_PG,
 *        FLASH_ERROR_WRP or FLASH_COMPLETE.
 */
FLASH_Status FLASH_WaitForLastBank1Operation(uint32_t Timeout)
{
    FLASH_Status status = FLASH_COMPLETE;

    status = FLASH_GetBank1Status();
    while((status == FLASH_FLAG_BANK1_BSY) && (Timeout != 0x00))
    {
        status = FLASH_GetBank1Status();
        Timeout--;
    }
    if(Timeout == 0x00)
    {
        status = FLASH_TIMEOUT;
    }
    return status;
}

/*********************************************************************
 * @fn      FLASH_Unlock_Fast
 *
 * @brief   Unlocks the Fast Program Erase Mode.
 *
 * @return  none
 */
void FLASH_Unlock_Fast(void)
{
    /* Authorize the FPEC of Bank1 Access */
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    /* Fast program mode unlock */
    FLASH->MODEKEYR = FLASH_KEY1;
    FLASH->MODEKEYR = FLASH_KEY2;
}

/*********************************************************************
 * @fn      FLASH_Lock_Fast
 *
 * @brief   Locks the Fast Program Erase Mode.
 *
 * @return  none
 */
void FLASH_Lock_Fast(void)
{
    FLASH->CTLR |= CR_LOCK_Set;
}

/*********************************************************************
 * @fn      FLASH_ErasePage_Fast
 *
 * @brief   Erases a specified FLASH page (1page = 256Byte).
 *
 * @param   Page_Address - The page address to be erased.
 *
 * @return  none
 */
void FLASH_ErasePage_Fast(uint32_t Page_Address)
{
    Page_Address &= 0xFFFFFF00;

    FLASH->CTLR |= CR_PAGE_ER;
    FLASH->ADDR = Page_Address;
    FLASH->CTLR |= CR_STRT_Set;
    while(FLASH->STATR & SR_BSY)
        ;
    FLASH->CTLR &= ~CR_PAGE_ER;
}

/*********************************************************************
 * @fn      FLASH_EraseBlock_32K_Fast
 *
 * @brief   Erases a specified FLASH Block (1Block = 32KByte).
 *
 * @param   Block_Address - The block address to be erased.
 *
 * @return  none
 */
void FLASH_EraseBlock_32K_Fast(uint32_t Block_Address)
{
    Block_Address &= 0xFFFF8000;

    FLASH->CTLR |= CR_BER32;
    FLASH->ADDR = Block_Address;
    FLASH->CTLR |= CR_STRT_Set;
    while(FLASH->STATR & SR_BSY)
        ;
    FLASH->CTLR &= ~CR_BER32;
}

/*********************************************************************
 * @fn      FLASH_EraseBlock_64K_Fast
 *
 * @brief   Erases a specified FLASH Block (1Block = 64KByte).
 *
 * @param   Block_Address - The block address to be erased.
 *
 * @return  none
 */
void FLASH_EraseBlock_64K_Fast(uint32_t Block_Address)
{
    Block_Address &= 0xFFFF0000;

    FLASH->CTLR |= CR_BER64;
    FLASH->ADDR = Block_Address;
    FLASH->CTLR |= CR_STRT_Set;
    while(FLASH->STATR & SR_BSY)
        ;
    FLASH->CTLR &= ~CR_BER64;
}

/*********************************************************************
 * @fn      FLASH_ProgramPage_Fast
 *
 * @brief   Program a specified FLASH page (1page = 256Byte).
 *
 * @param   Page_Address - The page address to be programed.
 *
 * @return  none
 */
void FLASH_ProgramPage_Fast(uint32_t Page_Address, uint32_t *pbuf)
{
    uint8_t size = 64;

    Page_Address &= 0xFFFFFF00;

    FLASH->CTLR |= CR_PAGE_PG;
    while(FLASH->STATR & SR_BSY)
        ;
    while(FLASH->STATR & SR_WR_BSY)
        ;

    while(size)
    {
        *(uint32_t *)Page_Address = *(uint32_t *)pbuf;
        Page_Address += 4;
        pbuf += 1;
        size -= 1;
        while(FLASH->STATR & SR_WR_BSY)
            ;
    }

    FLASH->CTLR |= CR_PG_STRT;
    while(FLASH->STATR & SR_BSY)
        ;
    FLASH->CTLR &= ~CR_PAGE_PG;
}

/*********************************************************************
 * @fn      FLASH_Enhance_Mode
 *
 * @brief   Read FLASH Enhance Mode
 *
 * @param   FLASH_Enhance_CLK -
 *            FLASH_Enhance_SYSTEM_HALF - System clock/2
 *            FLASH_Enhance_SYSTEM - System clock
 *          Newstate - new state of the ReadOut Protection(ENABLE or DISABLE).
 *
 * @return  none
 */
void FLASH_Enhance_Mode(uint32_t FLASH_Enhance_CLK, FunctionalState NewState)
{
    FLASH->CTLR &= ~(1 << 25);
    FLASH->CTLR |= FLASH_Enhance_CLK;

    if(NewState)
    {
        FLASH->CTLR |= (1 << 24);
    }
    else
    {
        FLASH->CTLR &= ~(1 << 24);
        FLASH->CTLR |= (1 << 22);
    }
}
