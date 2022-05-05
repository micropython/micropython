/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_SDIO.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the SDIO firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#include "ch32v30x_sdio.h"
#include "ch32v30x_rcc.h"

#define SDIO_OFFSET         (SDIO_BASE - PERIPH_BASE)

/* CLKCR register clear mask */
#define CLKCR_CLEAR_MASK    ((uint32_t)0xFFFF8100)

/* SDIO PWRCTRL Mask */
#define PWR_PWRCTRL_MASK    ((uint32_t)0xFFFFFFFC)

/* SDIO DCTRL Clear Mask */
#define DCTRL_CLEAR_MASK    ((uint32_t)0xFFFFFF08)

/* CMD Register clear mask */
#define CMD_CLEAR_MASK      ((uint32_t)0xFFFFF800)

/* SDIO RESP Registers Address */
#define SDIO_RESP_ADDR      ((uint32_t)(SDIO_BASE + 0x14))

/*********************************************************************
 * @fn      SDIO_DeInit
 *
 * @brief   Deinitializes the SDIO peripheral registers to their default
 *        reset values.
 *
 * @return  RTC counter value
 */
void SDIO_DeInit(void)
{
    SDIO->POWER = 0x00000000;
    SDIO->CLKCR = 0x00000000;
    SDIO->ARG = 0x00000000;
    SDIO->CMD = 0x00000000;
    SDIO->DTIMER = 0x00000000;
    SDIO->DLEN = 0x00000000;
    SDIO->DCTRL = 0x00000000;
    SDIO->ICR = 0x00C007FF;
    SDIO->MASK = 0x00000000;
}

/*********************************************************************
 * @fn      SDIO_Init
 *
 * @brief   Initializes the SDIO peripheral according to the specified
 *        parameters in the SDIO_InitStruct.
 *
 * @param   SDIO_InitStruct - pointer to a SDIO_InitTypeDef structure
 *        that contains the configuration information for the SDIO peripheral.
 *
 * @return  None
 */
void SDIO_Init(SDIO_InitTypeDef *SDIO_InitStruct)
{
    uint32_t tmpreg = 0;

    tmpreg = SDIO->CLKCR;
    tmpreg &= CLKCR_CLEAR_MASK;
    tmpreg |= (SDIO_InitStruct->SDIO_ClockDiv | SDIO_InitStruct->SDIO_ClockPowerSave |
               SDIO_InitStruct->SDIO_ClockBypass | SDIO_InitStruct->SDIO_BusWide |
               SDIO_InitStruct->SDIO_ClockEdge | SDIO_InitStruct->SDIO_HardwareFlowControl);

    SDIO->CLKCR = tmpreg;
}

/*********************************************************************
 * @fn      SDIO_StructInit
 *
 * @brief   Fills each SDIO_InitStruct member with its default value.
 *
 * @param   SDIO_InitStruct - pointer to an SDIO_InitTypeDef structure which
 *        will be initialized.
 *
 * @return  none
 */
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct)
{
    SDIO_InitStruct->SDIO_ClockDiv = 0x00;
    SDIO_InitStruct->SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStruct->SDIO_ClockBypass = SDIO_ClockBypass_Disable;
    SDIO_InitStruct->SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;
    SDIO_InitStruct->SDIO_BusWide = SDIO_BusWide_1b;
    SDIO_InitStruct->SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;
}

/*********************************************************************
 * @fn      SDIO_ClockCmd
 *
 * @brief   Enables or disables the SDIO Clock.
 *
 * @param   SDIO_InitStruct - pointer to an SDIO_InitTypeDef structure which
 *        will be initialized.
 *
 * @return  none
 */
void SDIO_ClockCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CLKCR |= (1 << 8);
    else
        SDIO->CLKCR &= ~(1 << 8);
}

/*********************************************************************
 * @fn      SDIO_SetPowerState
 *
 * @brief   Sets the power status of the controller.
 *
 * @param   SDIO_PowerState - new state of the Power state.
 *            SDIO_PowerState_OFF
 *            SDIO_PowerState_ON
 *
 * @return  none
 */
void SDIO_SetPowerState(uint32_t SDIO_PowerState)
{
    SDIO->POWER &= PWR_PWRCTRL_MASK;
    SDIO->POWER |= SDIO_PowerState;
}

/*********************************************************************
 * @fn      SDIO_GetPowerState
 *
 * @brief   Gets the power status of the controller.
 *
 * @param   CounterValue - RTC counter new value.
 *
 * @return  power state -
 *            0x00 - Power OFF
 *            0x02 - Power UP
 *            0x03 - Power ON
 */
uint32_t SDIO_GetPowerState(void)
{
    return (SDIO->POWER & (~PWR_PWRCTRL_MASK));
}

/*********************************************************************
 * @fn      SDIO_ITConfig
 *
 * @brief   Enables or disables the SDIO interrupts.
 *
 * @param   DIO_IT - specifies the SDIO interrupt sources to be enabled or disabled.
 *            SDIO_IT_CCRCFAIL
 *            SDIO_IT_DCRCFAIL
 *            SDIO_IT_CTIMEOUT
 *            SDIO_IT_DTIMEOUT
 *            SDIO_IT_TXUNDERR
 *            SDIO_IT_RXOVERR
 *            SDIO_IT_CMDREND
 *            SDIO_IT_CMDSENT
 *            SDIO_IT_DATAEND
 *            SDIO_IT_STBITERR
 *            SDIO_IT_DBCKEND
 *            SDIO_IT_CMDACT
 *            SDIO_IT_TXACT
 *            SDIO_IT_RXACT
 *            SDIO_IT_TXFIFOHE
 *            SDIO_IT_RXFIFOHF
 *            SDIO_IT_TXFIFOF
 *            SDIO_IT_RXFIFOF
 *            SDIO_IT_TXFIFOE
 *            SDIO_IT_RXFIFOE
 *            SDIO_IT_TXDAVL
 *            SDIO_IT_RXDAVL
 *            SDIO_IT_SDIOIT
 *            SDIO_IT_CEATAEND
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_ITConfig(uint32_t SDIO_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        SDIO->MASK |= SDIO_IT;
    }
    else
    {
        SDIO->MASK &= ~SDIO_IT;
    }
}

/*********************************************************************
 * @fn      SDIO_DMACmd
 *
 * @brief   Enables or disables the SDIO DMA request.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_DMACmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 3);
    else
        SDIO->DCTRL &= ~(1 << 3);
}

/*********************************************************************
 * @fn      SDIO_SendCommand
 *
 * @brief   Initializes the SDIO Command according to the specified
 *        parameters in the SDIO_CmdInitStruct and send the command.
 * @param   SDIO_CmdInitStruct - pointer to a SDIO_CmdInitTypeDef
 *        structure that contains the configuration information for
 *        ddthe SDIO command.
 *
 * @return  none
 */
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct)
{
    uint32_t tmpreg = 0;

    SDIO->ARG = SDIO_CmdInitStruct->SDIO_Argument;

    tmpreg = SDIO->CMD;
    tmpreg &= CMD_CLEAR_MASK;
    tmpreg |= (uint32_t)SDIO_CmdInitStruct->SDIO_CmdIndex | SDIO_CmdInitStruct->SDIO_Response | SDIO_CmdInitStruct->SDIO_Wait | SDIO_CmdInitStruct->SDIO_CPSM;

    SDIO->CMD = tmpreg;
}

/*********************************************************************
 * @fn      SDIO_CmdStructInit
 *
 * @brief   Fills each SDIO_CmdInitStruct member with its default value.
 *
 * @param   SDIO_CmdInitStruct - pointer to an SDIO_CmdInitTypeDef
 *        structure which will be initialized.
 *
 * @return  none
 */
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct)
{
    SDIO_CmdInitStruct->SDIO_Argument = 0x00;
    SDIO_CmdInitStruct->SDIO_CmdIndex = 0x00;
    SDIO_CmdInitStruct->SDIO_Response = SDIO_Response_No;
    SDIO_CmdInitStruct->SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStruct->SDIO_CPSM = SDIO_CPSM_Disable;
}

/*********************************************************************
 * @fn      SDIO_GetCommandResponse
 *
 * @brief   Returns command index of last command for which response received.
 *
 * @return  Returns the command index of the last command response received.
 */
uint8_t SDIO_GetCommandResponse(void)
{
    return (uint8_t)(SDIO->RESPCMD);
}

/*********************************************************************
 * @fn      SDIO_GetResponse
 *
 * @brief   Returns response received from the card for the last command.
 *
 * @param   SDIO_RESP - Specifies the SDIO response register.
 *            SDIO_RESP1 - Response Register 1
 *            SDIO_RESP2 - Response Register 2
 *            SDIO_RESP3 - Response Register 3
 *            SDIO_RESP4 - Response Register 4
 *
 * @return  Returns the command index of the last command response received.
 */
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP)
{
    __IO uint32_t tmp = 0;

    tmp = SDIO_RESP_ADDR + SDIO_RESP;

    return (*(__IO uint32_t *)tmp);
}

/*********************************************************************
 * @fn      SDIO_DataConfig
 *
 * @brief   Initializes the SDIO data path according to the specified
 *
 * @param   SDIO_DataInitStruct - pointer to a SDIO_DataInitTypeDef structure that
 *        contains the configuration information for the SDIO command.
 *
 * @return  none
 */
void SDIO_DataConfig(SDIO_DataInitTypeDef *SDIO_DataInitStruct)
{
    uint32_t tmpreg = 0;

    SDIO->DTIMER = SDIO_DataInitStruct->SDIO_DataTimeOut;
    SDIO->DLEN = SDIO_DataInitStruct->SDIO_DataLength;
    tmpreg = SDIO->DCTRL;
    tmpreg &= DCTRL_CLEAR_MASK;
    tmpreg |= (uint32_t)SDIO_DataInitStruct->SDIO_DataBlockSize | SDIO_DataInitStruct->SDIO_TransferDir | SDIO_DataInitStruct->SDIO_TransferMode | SDIO_DataInitStruct->SDIO_DPSM;

    SDIO->DCTRL = tmpreg;
}

/*********************************************************************
 * @fn      SDIO_DataStructInit
 *
 * @brief   Fills each SDIO_DataInitStruct member with its default value.
 *
 * @param   SDIO_DataInitStruct - pointer to an SDIO_DataInitTypeDef
 *        structure which will be initialized.
 *
 * @return  RTC counter value
 */
void SDIO_DataStructInit(SDIO_DataInitTypeDef *SDIO_DataInitStruct)
{
    SDIO_DataInitStruct->SDIO_DataTimeOut = 0xFFFFFFFF;
    SDIO_DataInitStruct->SDIO_DataLength = 0x00;
    SDIO_DataInitStruct->SDIO_DataBlockSize = SDIO_DataBlockSize_1b;
    SDIO_DataInitStruct->SDIO_TransferDir = SDIO_TransferDir_ToCard;
    SDIO_DataInitStruct->SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStruct->SDIO_DPSM = SDIO_DPSM_Disable;
}

/*********************************************************************
 * @fn      SDIO_GetDataCounter
 *
 * @brief   Returns number of remaining data bytes to be transferred.
 *
 * @return  Number of remaining data bytes to be transferred
 */
uint32_t SDIO_GetDataCounter(void)
{
    return SDIO->DCOUNT;
}

/*********************************************************************
 * @fn      SDIO_ReadData
 *
 * @brief   Read one data word from Rx FIFO.
 *
 * @return  Data received
 */
uint32_t SDIO_ReadData(void)
{
    return SDIO->FIFO;
}

/*********************************************************************
 * @fn      SDIO_WriteData
 *
 * @brief   Write one data word to Tx FIFO.
 *
 * @param   Data - 32-bit data word to write.
 *
 * @return  RTC counter value
 */
void SDIO_WriteData(uint32_t Data)
{
    SDIO->FIFO = Data;
}

/*********************************************************************
 * @fn      SDIO_GetFIFOCount
 *
 * @brief   Returns the number of words left to be written to or read from FIFO.
 *
 * @return  Remaining number of words.
 */
uint32_t SDIO_GetFIFOCount(void)
{
    return SDIO->FIFOCNT;
}

/*********************************************************************
 * @fn      SDIO_StartSDIOReadWait
 *
 * @brief   Starts the SD I/O Read Wait operation.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_StartSDIOReadWait(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 8);
    else
        SDIO->DCTRL &= ~(1 << 8);
}

/*********************************************************************
 * @fn      SDIO_StopSDIOReadWait
 *
 * @brief   Stops the SD I/O Read Wait operation.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_StopSDIOReadWait(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 9);
    else
        SDIO->DCTRL &= ~(1 << 9);
}

/*********************************************************************
 * @fn      SDIO_SetSDIOReadWaitMode
 *
 * @brief   Sets one of the two options of inserting read wait interval.
 *
 * @param   SDIO_ReadWaitMode - SD I/O Read Wait operation mode.
 *            SDIO_ReadWaitMode_CLK - Read Wait control by stopping SDIOCLK
 *            SDIO_ReadWaitMode_DATA2 - Read Wait control using SDIO_DATA2
 *
 * @return  none
 */
void SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode)
{
    if(SDIO_ReadWaitMode)
        SDIO->DCTRL |= (1 << 10);
    else
        SDIO->DCTRL &= ~(1 << 10);
}

/*********************************************************************
 * @fn      SDIO_SetSDIOOperation
 *
 * @brief   Enables or disables the SD I/O Mode Operation.
 *
 * @param   NewState: ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_SetSDIOOperation(FunctionalState NewState)
{
    if(NewState)
        SDIO->DCTRL |= (1 << 11);
    else
        SDIO->DCTRL &= ~(1 << 11);
}

/*********************************************************************
 * @fn      SDIO_SendSDIOSuspendCmd
 *
 * @brief   Enables or disables the SD I/O Mode suspend command sending.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_SendSDIOSuspendCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 11);
    else
        SDIO->CMD &= ~(1 << 11);
}

/*********************************************************************
 * @fn      SDIO_CommandCompletionCmd
 *
 * @brief   Enables or disables the command completion signal.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_CommandCompletionCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 12);
    else
        SDIO->CMD &= ~(1 << 12);
}

/*********************************************************************
 * @fn      SDIO_CEATAITCmd
 *
 * @brief   Enables or disables the CE-ATA interrupt.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void SDIO_CEATAITCmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 13);
    else
        SDIO->CMD &= ~(1 << 13);
}

/*********************************************************************
 * @fn      SDIO_SendCEATACmd
 *
 * @brief   Sends CE-ATA command (CMD61).
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  RTC counter value
 */
void SDIO_SendCEATACmd(FunctionalState NewState)
{
    if(NewState)
        SDIO->CMD |= (1 << 14);
    else
        SDIO->CMD &= ~(1 << 14);
}

/*********************************************************************
 * @fn      SDIO_GetFlagStatus
 *
 * @brief   Checks whether the specified SDIO flag is set or not.
 *
 * @param   SDIO_FLAG - specifies the flag to check.
 *            SDIO_FLAG_CCRCFAIL - Command response received (CRC check failed)
 *            SDIO_FLAG_DCRCFAIL - Data block sent/received (CRC check failed)
 *            SDIO_FLAG_CTIMEOUT - Command response timeout
 *            SDIO_FLAG_DTIMEOUT - Data timeout
 *            SDIO_FLAG_TXUNDERR - Transmit FIFO underrun error
 *            SDIO_FLAG_RXOVERR - Received FIFO overrun error
 *            SDIO_FLAG_CMDREND - Command response received (CRC check passed)
 *            SDIO_FLAG_CMDSENT - Command sent (no response required)
 *            SDIO_FLAG_DATAEND - Data end (data counter, SDIDCOUNT, is zero)
 *            SDIO_FLAG_STBITERR - Start bit not detected on all data signals
 *          in wide bus mode.
 *            SDIO_FLAG_DBCKEND - Data block sent/received (CRC check passed)
 *            SDIO_FLAG_CMDACT - Command transfer in progress
 *            SDIO_FLAG_TXACT - Data transmit in progress
 *            SDIO_FLAG_RXACT - Data receive in progress
 *            SDIO_FLAG_TXFIFOHE - Transmit FIFO Half Empty
 *            SDIO_FLAG_RXFIFOHF - Receive FIFO Half Full
 *            SDIO_FLAG_TXFIFOF - Transmit FIFO full
 *            SDIO_FLAG_RXFIFOF - Receive FIFO full
 *            SDIO_FLAG_TXFIFOE - Transmit FIFO empty
 *            SDIO_FLAG_RXFIFOE - Receive FIFO empty
 *            SDIO_FLAG_TXDAVL - Data available in transmit FIFO
 *            SDIO_FLAG_RXDAVL - Data available in receive FIFO
 *            SDIO_FLAG_SDIOIT - SD I/O interrupt received
 *            SDIO_FLAG_CEATAEND - CE-ATA command completion signal received
 *          for CMD61
 *
 * @return  ITStatus - SET or RESET
 */
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((SDIO->STA & SDIO_FLAG) != (uint32_t)RESET)
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
 * @fn      SDIO_ClearFlag
 *
 * @brief   Clears the SDIO's pending flags.
 *
 * @param   SDIO_FLAG - specifies the flag to clear.
 *            SDIO_FLAG_CCRCFAIL - Command response received (CRC check failed)
 *            SDIO_FLAG_DCRCFAIL - Data block sent/received (CRC check failed)
 *            SDIO_FLAG_CTIMEOUT - Command response timeout
 *            SDIO_FLAG_DTIMEOUT - Data timeout
 *            SDIO_FLAG_TXUNDERR - Transmit FIFO underrun error
 *            SDIO_FLAG_RXOVERR - Received FIFO overrun error
 *            SDIO_FLAG_CMDREND - Command response received (CRC check passed)
 *            SDIO_FLAG_CMDSENT - Command sent (no response required)
 *            SDIO_FLAG_DATAEND - Data end (data counter, SDIDCOUNT, is zero)
 *            SDIO_FLAG_STBITERR - Start bit not detected on all data signals
 *        in wide bus mode
 *            SDIO_FLAG_DBCKEND - Data block sent/received (CRC check passed)
 *            SDIO_FLAG_SDIOIT - SD I/O interrupt received
 *            SDIO_FLAG_CEATAEND - CE-ATA command completion signal received for CMD61
 *
 * @return  none
 */
void SDIO_ClearFlag(uint32_t SDIO_FLAG)
{
    SDIO->ICR = SDIO_FLAG;
}

/*********************************************************************
 * @fn      SDIO_GetITStatus
 *
 * @brief   Checks whether the specified SDIO interrupt has occurred or not.
 *
 * @param   SDIO_IT: specifies the SDIO interrupt source to check.
 *            SDIO_IT_CCRCFAIL - Command response received (CRC check failed) interrupt
 *            SDIO_IT_DCRCFAIL - Data block sent/received (CRC check failed) interrupt
 *            SDIO_IT_CTIMEOUT - Command response timeout interrupt
 *            SDIO_IT_DTIMEOUT - Data timeout interrupt
 *            SDIO_IT_TXUNDERR - Transmit FIFO underrun error interrupt
 *            SDIO_IT_RXOVERR - Received FIFO overrun error interrupt
 *            SDIO_IT_CMDREND - Command response received (CRC check passed) interrupt
 *            SDIO_IT_CMDSENT - Command sent (no response required) interrupt
 *            SDIO_IT_DATAEND -  Data end (data counter, SDIDCOUNT, is zero) interrupt
 *            SDIO_IT_STBITERR - Start bit not detected on all data signals in wide
 *        bus mode interrupt
 *            SDIO_IT_DBCKEND - Data block sent/received (CRC check passed) interrupt
 *            SDIO_IT_CMDACT - Command transfer in progress interrupt
 *            SDIO_IT_TXACT - Data transmit in progress interrupt
 *            SDIO_IT_RXACT - Data receive in progress interrupt
 *            SDIO_IT_TXFIFOHE - Transmit FIFO Half Empty interrupt
 *            SDIO_IT_RXFIFOHF - Receive FIFO Half Full interrupt
 *            SDIO_IT_TXFIFOF - Transmit FIFO full interrupt
 *            SDIO_IT_RXFIFOF - Receive FIFO full interrupt
 *            SDIO_IT_TXFIFOE - Transmit FIFO empty interrupt
 *            SDIO_IT_RXFIFOE - Receive FIFO empty interrupt
 *            SDIO_IT_TXDAVL - Data available in transmit FIFO interrupt
 *            SDIO_IT_RXDAVL - Data available in receive FIFO interrupt
 *            SDIO_IT_SDIOIT - SD I/O interrupt received interrupt
 *            SDIO_IT_CEATAEND - CE-ATA command completion signal received for CMD61 interrupt
 *
 * @return  ITStatus£ºSET or RESET
 */
ITStatus SDIO_GetITStatus(uint32_t SDIO_IT)
{
    ITStatus bitstatus = RESET;

    if((SDIO->STA & SDIO_IT) != (uint32_t)RESET)
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
 * @fn      SDIO_ClearITPendingBit
 *
 * @brief   Clears the SDIO's interrupt pending bits.
 *
 * @param   SDIO_IT - specifies the interrupt pending bit to clear.
 *            SDIO_IT_CCRCFAIL - Command response received (CRC check failed) interrupt
 *            SDIO_IT_DCRCFAIL - Data block sent/received (CRC check failed) interrupt
 *            SDIO_IT_CTIMEOUT - Command response timeout interrupt
 *            SDIO_IT_DTIMEOUT - Data timeout interrupt
 *            SDIO_IT_TXUNDERR - Transmit FIFO underrun error interrupt
 *            SDIO_IT_RXOVERR - Received FIFO overrun error interrupt
 *            SDIO_IT_CMDREND - Command response received (CRC check passed) interrupt
 *            SDIO_IT_CMDSENT - Command sent (no response required) interrupt
 *            SDIO_IT_DATAEND - Data end (data counter, SDIDCOUNT, is zero) interrupt
 *            SDIO_IT_STBITERR - Start bit not detected on all data signals in wide
 *        bus mode interrupt
 *            SDIO_IT_SDIOIT - SD I/O interrupt received interrupt
 *            SDIO_IT_CEATAEND - CE-ATA command completion signal received for CMD61
 *
 * @return  RTC counter value
 */
void SDIO_ClearITPendingBit(uint32_t SDIO_IT)
{
    SDIO->ICR = SDIO_IT;
}
