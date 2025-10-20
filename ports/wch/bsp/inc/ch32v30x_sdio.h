/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_sdio.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the SDIO
*                      firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32V30x_SDIO_H
#define __CH32V30x_SDIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v30x.h"

/* SDIO Init structure definition */
typedef struct
{
    uint32_t SDIO_ClockEdge;          /* Specifies the clock transition on which the bit capture is made.
                                         This parameter can be a value of @ref SDIO_Clock_Edge */

    uint32_t SDIO_ClockBypass;        /* Specifies whether the SDIO Clock divider bypass is
                                         enabled or disabled.
                                         This parameter can be a value of @ref SDIO_Clock_Bypass */

    uint32_t SDIO_ClockPowerSave;     /* Specifies whether SDIO Clock output is enabled or
                                         disabled when the bus is idle.
                                         This parameter can be a value of @ref SDIO_Clock_Power_Save */

    uint32_t SDIO_BusWide;            /* Specifies the SDIO bus width.
                                         This parameter can be a value of @ref SDIO_Bus_Wide */

    uint32_t SDIO_HardwareFlowControl; /* Specifies whether the SDIO hardware flow control is enabled or disabled.
                                         This parameter can be a value of @ref SDIO_Hardware_Flow_Control */

    uint8_t SDIO_ClockDiv;            /* Specifies the clock frequency of the SDIO controller.
                                         This parameter can be a value between 0x00 and 0xFF. */

} SDIO_InitTypeDef;


typedef struct
{
    uint32_t SDIO_Argument; /* Specifies the SDIO command argument which is sent
                              to a card as part of a command message. If a command
                              contains an argument, it must be loaded into this register
                              before writing the command to the command register */

    uint32_t SDIO_CmdIndex; /* Specifies the SDIO command index. It must be lower than 0x40. */

    uint32_t SDIO_Response; /* Specifies the SDIO response type.
                              This parameter can be a value of @ref SDIO_Response_Type */

    uint32_t SDIO_Wait;    /* Specifies whether SDIO wait-for-interrupt request is enabled or disabled.
                              This parameter can be a value of @ref SDIO_Wait_Interrupt_State */

    uint32_t SDIO_CPSM;    /* Specifies whether SDIO Command path state machine (CPSM)
                              is enabled or disabled.
                              This parameter can be a value of @ref SDIO_CPSM_State */
} SDIO_CmdInitTypeDef;

typedef struct
{
    uint32_t SDIO_DataTimeOut;  /* Specifies the data timeout period in card bus clock periods. */

    uint32_t SDIO_DataLength;   /* Specifies the number of data bytes to be transferred. */

    uint32_t SDIO_DataBlockSize; /* Specifies the data block size for block transfer.
                                   This parameter can be a value of @ref SDIO_Data_Block_Size */

    uint32_t SDIO_TransferDir;  /* Specifies the data transfer direction, whether the transfer
                                   is a read or write.
                                   This parameter can be a value of @ref SDIO_Transfer_Direction */

    uint32_t SDIO_TransferMode; /* Specifies whether data transfer is in stream or block mode.
                                   This parameter can be a value of @ref SDIO_Transfer_Type */

    uint32_t SDIO_DPSM;         /* Specifies whether SDIO Data path state machine (DPSM)
                                   is enabled or disabled.
                                   This parameter can be a value of @ref SDIO_DPSM_State */
} SDIO_DataInitTypeDef;


/* SDIO_Clock_Edge */
#define SDIO_ClockEdge_Rising               ((uint32_t)0x00000000)
#define SDIO_ClockEdge_Falling              ((uint32_t)0x00002000)

/* SDIO_Clock_Bypass */
#define SDIO_ClockBypass_Disable             ((uint32_t)0x00000000)
#define SDIO_ClockBypass_Enable              ((uint32_t)0x00000400)

/* SDIO_Clock_Power_Save */
#define SDIO_ClockPowerSave_Disable         ((uint32_t)0x00000000)
#define SDIO_ClockPowerSave_Enable          ((uint32_t)0x00000200)

/* SDIO_Bus_Wide */
#define SDIO_BusWide_1b                     ((uint32_t)0x00000000)
#define SDIO_BusWide_4b                     ((uint32_t)0x00000800)
#define SDIO_BusWide_8b                     ((uint32_t)0x00001000)

/* SDIO_Hardware_Flow_Control */
#define SDIO_HardwareFlowControl_Disable    ((uint32_t)0x00000000)
#define SDIO_HardwareFlowControl_Enable     ((uint32_t)0x00004000)

/* SDIO_Power_State */
#define SDIO_PowerState_OFF                 ((uint32_t)0x00000000)
#define SDIO_PowerState_ON                  ((uint32_t)0x00000003)

/* SDIO_Interrupt_sources */
#define SDIO_IT_CCRCFAIL                    ((uint32_t)0x00000001)
#define SDIO_IT_DCRCFAIL                    ((uint32_t)0x00000002)
#define SDIO_IT_CTIMEOUT                    ((uint32_t)0x00000004)
#define SDIO_IT_DTIMEOUT                    ((uint32_t)0x00000008)
#define SDIO_IT_TXUNDERR                    ((uint32_t)0x00000010)
#define SDIO_IT_RXOVERR                     ((uint32_t)0x00000020)
#define SDIO_IT_CMDREND                     ((uint32_t)0x00000040)
#define SDIO_IT_CMDSENT                     ((uint32_t)0x00000080)
#define SDIO_IT_DATAEND                     ((uint32_t)0x00000100)
#define SDIO_IT_STBITERR                    ((uint32_t)0x00000200)
#define SDIO_IT_DBCKEND                     ((uint32_t)0x00000400)
#define SDIO_IT_CMDACT                      ((uint32_t)0x00000800)
#define SDIO_IT_TXACT                       ((uint32_t)0x00001000)
#define SDIO_IT_RXACT                       ((uint32_t)0x00002000)
#define SDIO_IT_TXFIFOHE                    ((uint32_t)0x00004000)
#define SDIO_IT_RXFIFOHF                    ((uint32_t)0x00008000)
#define SDIO_IT_TXFIFOF                     ((uint32_t)0x00010000)
#define SDIO_IT_RXFIFOF                     ((uint32_t)0x00020000)
#define SDIO_IT_TXFIFOE                     ((uint32_t)0x00040000)
#define SDIO_IT_RXFIFOE                     ((uint32_t)0x00080000)
#define SDIO_IT_TXDAVL                      ((uint32_t)0x00100000)
#define SDIO_IT_RXDAVL                      ((uint32_t)0x00200000)
#define SDIO_IT_SDIOIT                      ((uint32_t)0x00400000)
#define SDIO_IT_CEATAEND                    ((uint32_t)0x00800000)

/* SDIO_Response_Type */
#define SDIO_Response_No                    ((uint32_t)0x00000000)
#define SDIO_Response_Short                 ((uint32_t)0x00000040)
#define SDIO_Response_Long                  ((uint32_t)0x000000C0)

/* SDIO_Wait_Interrupt_State */
#define SDIO_Wait_No                        ((uint32_t)0x00000000)
#define SDIO_Wait_IT                        ((uint32_t)0x00000100)
#define SDIO_Wait_Pend                      ((uint32_t)0x00000200)

/* SDIO_CPSM_State */
#define SDIO_CPSM_Disable                    ((uint32_t)0x00000000)
#define SDIO_CPSM_Enable                     ((uint32_t)0x00000400)

/* SDIO_Response_Registers */
#define SDIO_RESP1                          ((uint32_t)0x00000000)
#define SDIO_RESP2                          ((uint32_t)0x00000004)
#define SDIO_RESP3                          ((uint32_t)0x00000008)
#define SDIO_RESP4                          ((uint32_t)0x0000000C)

/* SDIO_Data_Block_Size */
#define SDIO_DataBlockSize_1b               ((uint32_t)0x00000000)
#define SDIO_DataBlockSize_2b               ((uint32_t)0x00000010)
#define SDIO_DataBlockSize_4b               ((uint32_t)0x00000020)
#define SDIO_DataBlockSize_8b               ((uint32_t)0x00000030)
#define SDIO_DataBlockSize_16b              ((uint32_t)0x00000040)
#define SDIO_DataBlockSize_32b              ((uint32_t)0x00000050)
#define SDIO_DataBlockSize_64b              ((uint32_t)0x00000060)
#define SDIO_DataBlockSize_128b             ((uint32_t)0x00000070)
#define SDIO_DataBlockSize_256b             ((uint32_t)0x00000080)
#define SDIO_DataBlockSize_512b             ((uint32_t)0x00000090)
#define SDIO_DataBlockSize_1024b            ((uint32_t)0x000000A0)
#define SDIO_DataBlockSize_2048b            ((uint32_t)0x000000B0)
#define SDIO_DataBlockSize_4096b            ((uint32_t)0x000000C0)
#define SDIO_DataBlockSize_8192b            ((uint32_t)0x000000D0)
#define SDIO_DataBlockSize_16384b           ((uint32_t)0x000000E0)

/* SDIO_Transfer_Direction */
#define SDIO_TransferDir_ToCard             ((uint32_t)0x00000000)
#define SDIO_TransferDir_ToSDIO             ((uint32_t)0x00000002)

/* SDIO_Transfer_Type */
#define SDIO_TransferMode_Block             ((uint32_t)0x00000000)
#define SDIO_TransferMode_Stream            ((uint32_t)0x00000004)

/* SDIO_DPSM_State */
#define SDIO_DPSM_Disable                    ((uint32_t)0x00000000)
#define SDIO_DPSM_Enable                     ((uint32_t)0x00000001)

/* SDIO_Flags */
#define SDIO_FLAG_CCRCFAIL                  ((uint32_t)0x00000001)
#define SDIO_FLAG_DCRCFAIL                  ((uint32_t)0x00000002)
#define SDIO_FLAG_CTIMEOUT                  ((uint32_t)0x00000004)
#define SDIO_FLAG_DTIMEOUT                  ((uint32_t)0x00000008)
#define SDIO_FLAG_TXUNDERR                  ((uint32_t)0x00000010)
#define SDIO_FLAG_RXOVERR                   ((uint32_t)0x00000020)
#define SDIO_FLAG_CMDREND                   ((uint32_t)0x00000040)
#define SDIO_FLAG_CMDSENT                   ((uint32_t)0x00000080)
#define SDIO_FLAG_DATAEND                   ((uint32_t)0x00000100)
#define SDIO_FLAG_STBITERR                  ((uint32_t)0x00000200)
#define SDIO_FLAG_DBCKEND                   ((uint32_t)0x00000400)
#define SDIO_FLAG_CMDACT                    ((uint32_t)0x00000800)
#define SDIO_FLAG_TXACT                     ((uint32_t)0x00001000)
#define SDIO_FLAG_RXACT                     ((uint32_t)0x00002000)
#define SDIO_FLAG_TXFIFOHE                  ((uint32_t)0x00004000)
#define SDIO_FLAG_RXFIFOHF                  ((uint32_t)0x00008000)
#define SDIO_FLAG_TXFIFOF                   ((uint32_t)0x00010000)
#define SDIO_FLAG_RXFIFOF                   ((uint32_t)0x00020000)
#define SDIO_FLAG_TXFIFOE                   ((uint32_t)0x00040000)
#define SDIO_FLAG_RXFIFOE                   ((uint32_t)0x00080000)
#define SDIO_FLAG_TXDAVL                    ((uint32_t)0x00100000)
#define SDIO_FLAG_RXDAVL                    ((uint32_t)0x00200000)
#define SDIO_FLAG_SDIOIT                    ((uint32_t)0x00400000)
#define SDIO_FLAG_CEATAEND                  ((uint32_t)0x00800000)

/* SDIO_Read_Wait_Mode */
#define SDIO_ReadWaitMode_CLK               ((uint32_t)0x00000001)
#define SDIO_ReadWaitMode_DATA2             ((uint32_t)0x00000000)


void SDIO_DeInit(void);
void SDIO_Init(SDIO_InitTypeDef *SDIO_InitStruct);
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct);
void SDIO_ClockCmd(FunctionalState NewState);
void SDIO_SetPowerState(uint32_t SDIO_PowerState);
uint32_t SDIO_GetPowerState(void);
void SDIO_ITConfig(uint32_t SDIO_IT, FunctionalState NewState);
void SDIO_DMACmd(FunctionalState NewState);
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);
uint8_t SDIO_GetCommandResponse(void);
uint32_t SDIO_GetResponse(uint32_t SDIO_RESP);
void SDIO_DataConfig(SDIO_DataInitTypeDef *SDIO_DataInitStruct);
void SDIO_DataStructInit(SDIO_DataInitTypeDef *SDIO_DataInitStruct);
uint32_t SDIO_GetDataCounter(void);
uint32_t SDIO_ReadData(void);
void SDIO_WriteData(uint32_t Data);
uint32_t SDIO_GetFIFOCount(void);
void SDIO_StartSDIOReadWait(FunctionalState NewState);
void SDIO_StopSDIOReadWait(FunctionalState NewState);
void SDIO_SetSDIOReadWaitMode(uint32_t SDIO_ReadWaitMode);
void SDIO_SetSDIOOperation(FunctionalState NewState);
void SDIO_SendSDIOSuspendCmd(FunctionalState NewState);
void SDIO_CommandCompletionCmd(FunctionalState NewState);
void SDIO_CEATAITCmd(FunctionalState NewState);
void SDIO_SendCEATACmd(FunctionalState NewState);
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG);
void SDIO_ClearFlag(uint32_t SDIO_FLAG);
ITStatus SDIO_GetITStatus(uint32_t SDIO_IT);
void SDIO_ClearITPendingBit(uint32_t SDIO_IT);

#ifdef __cplusplus
}
#endif

#endif
