/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_fsmc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the FSMC
*                      firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32V30x_FSMC_H
#define __CH32V30x_FSMC_H

#ifdef __cplusplus
 extern "C" {
#endif


#include "ch32v30x.h"


/* FSMC Init structure definition */
typedef struct
{
  uint32_t FSMC_AddressSetupTime;       /* Defines the number of HCLK cycles to configure
                                           the duration of the address setup time.
                                           This parameter can be a value between 0 and 0xF.
                                           @note: It is not used with synchronous NOR Flash memories. */

  uint32_t FSMC_AddressHoldTime;        /* Defines the number of HCLK cycles to configure
                                           the duration of the address hold time.
                                           This parameter can be a value between 0 and 0xF.
                                           @note: It is not used with synchronous NOR Flash memories.*/

  uint32_t FSMC_DataSetupTime;          /* Defines the number of HCLK cycles to configure
                                           the duration of the data setup time.
                                           This parameter can be a value between 0 and 0xFF.
                                           @note: It is used for SRAMs, ROMs and asynchronous multiplexed NOR Flash memories. */

  uint32_t FSMC_BusTurnAroundDuration;  /* Defines the number of HCLK cycles to configure
                                           the duration of the bus turnaround.
                                           This parameter can be a value between 0 and 0xF.
                                           @note: It is only used for multiplexed NOR Flash memories. */

  uint32_t FSMC_CLKDivision;            /* Defines the period of CLK clock output signal, expressed in number of HCLK cycles.
                                           This parameter can be a value between 1 and 0xF.
                                           @note: This parameter is not used for asynchronous NOR Flash, SRAM or ROM accesses. */

  uint32_t FSMC_DataLatency;            /* Defines the number of memory clock cycles to issue
                                           to the memory before getting the first data.
                                           The value of this parameter depends on the memory type as shown below:
                                              - It must be set to 0 in case of a CRAM
                                              - It is don't care in asynchronous NOR, SRAM or ROM accesses
                                              - It may assume a value between 0 and 0xF in NOR Flash memories
                                                with synchronous burst mode enable */

  uint32_t FSMC_AccessMode;             /* Specifies the asynchronous access mode.
                                            This parameter can be a value of @ref FSMC_Access_Mode */
}FSMC_NORSRAMTimingInitTypeDef;


typedef struct
{
  uint32_t FSMC_Bank;                /* Specifies the NOR/SRAM memory bank that will be used.
                                        This parameter can be a value of @ref FSMC_NORSRAM_Bank */

  uint32_t FSMC_DataAddressMux;      /* Specifies whether the address and data values are
                                        multiplexed on the databus or not.
                                        This parameter can be a value of @ref FSMC_Data_Address_Bus_Multiplexing */

  uint32_t FSMC_MemoryType;          /* Specifies the type of external memory attached to
                                        the corresponding memory bank.
                                        This parameter can be a value of @ref FSMC_Memory_Type */

  uint32_t FSMC_MemoryDataWidth;     /* Specifies the external memory device width.
                                        This parameter can be a value of @ref FSMC_Data_Width */

  uint32_t FSMC_BurstAccessMode;     /* Enables or disables the burst access mode for Flash memory,
                                        valid only with synchronous burst Flash memories.
                                        This parameter can be a value of @ref FSMC_Burst_Access_Mode */
                                       
  uint32_t FSMC_AsynchronousWait;    /* Enables or disables wait signal during asynchronous transfers,
                                        valid only with asynchronous Flash memories.
                                        This parameter can be a value of @ref FSMC_AsynchronousWait */

  uint32_t FSMC_WaitSignalPolarity;  /* Specifies the wait signal polarity, valid only when accessing
                                        the Flash memory in burst mode.
                                        This parameter can be a value of @ref FSMC_Wait_Signal_Polarity */

  uint32_t FSMC_WrapMode;            /* Enables or disables the Wrapped burst access mode for Flash
                                        memory, valid only when accessing Flash memories in burst mode.
                                        This parameter can be a value of @ref FSMC_Wrap_Mode */

  uint32_t FSMC_WaitSignalActive;    /* Specifies if the wait signal is asserted by the memory one
                                        clock cycle before the wait state or during the wait state,
                                        valid only when accessing memories in burst mode.
                                        This parameter can be a value of @ref FSMC_Wait_Timing */

  uint32_t FSMC_WriteOperation;      /* Enables or disables the write operation in the selected bank by the FSMC.
                                        This parameter can be a value of @ref FSMC_Write_Operation */

  uint32_t FSMC_WaitSignal;          /* Enables or disables the wait-state insertion via wait
                                        signal, valid for Flash memory access in burst mode.
                                        This parameter can be a value of @ref FSMC_Wait_Signal */

  uint32_t FSMC_ExtendedMode;        /* Enables or disables the extended mode.
                                        This parameter can be a value of @ref FSMC_Extended_Mode */

  uint32_t FSMC_WriteBurst;          /* Enables or disables the write burst operation.
                                        This parameter can be a value of @ref FSMC_Write_Burst */

  FSMC_NORSRAMTimingInitTypeDef* FSMC_ReadWriteTimingStruct; /* Timing Parameters for write and read access if the  ExtendedMode is not used*/

  FSMC_NORSRAMTimingInitTypeDef* FSMC_WriteTimingStruct;     /* Timing Parameters for write access if the  ExtendedMode is used*/
}FSMC_NORSRAMInitTypeDef;


typedef struct
{
  uint32_t FSMC_SetupTime;      /* Defines the number of HCLK cycles to setup address before
                                   the command assertion for NAND-Flash read or write access
                                   to common/Attribute or I/O memory space (depending on
                                   the memory space timing to be configured).
                                   This parameter can be a value between 0 and 0xFF.*/

  uint32_t FSMC_WaitSetupTime;  /* Defines the minimum number of HCLK cycles to assert the
                                   command for NAND-Flash read or write access to
                                   common/Attribute or I/O memory space (depending on the
                                   memory space timing to be configured).
                                   This parameter can be a number between 0x00 and 0xFF */

  uint32_t FSMC_HoldSetupTime;  /* Defines the number of HCLK clock cycles to hold address
                                   (and data for write access) after the command deassertion
                                   for NAND-Flash read or write access to common/Attribute
                                   or I/O memory space (depending on the memory space timing
                                   to be configured).
                                   This parameter can be a number between 0x00 and 0xFF */

  uint32_t FSMC_HiZSetupTime;   /* Defines the number of HCLK clock cycles during which the
                                   databus is kept in HiZ after the start of a NAND-Flash
                                   write access to common/Attribute or I/O memory space (depending
                                   on the memory space timing to be configured).
                                   This parameter can be a number between 0x00 and 0xFF */
}FSMC_NAND_PCCARDTimingInitTypeDef;


typedef struct
{
  uint32_t FSMC_Bank;             /* Specifies the NAND memory bank that will be used.
                                     This parameter can be a value of @ref FSMC_NAND_Bank */

  uint32_t FSMC_Waitfeature;      /* Enables or disables the Wait feature for the NAND Memory Bank.
                                     This parameter can be any value of @ref FSMC_Wait_feature */

  uint32_t FSMC_MemoryDataWidth;  /* Specifies the external memory device width.
                                     This parameter can be any value of @ref FSMC_Data_Width */

  uint32_t FSMC_ECC;              /* Enables or disables the ECC computation.
                                     This parameter can be any value of @ref FSMC_ECC */

  uint32_t FSMC_ECCPageSize;      /* Defines the page size for the extended ECC.
                                     This parameter can be any value of @ref FSMC_ECC_Page_Size */

  uint32_t FSMC_TCLRSetupTime;    /* Defines the number of HCLK cycles to configure the
                                     delay between CLE low and RE low.
                                     This parameter can be a value between 0 and 0xFF. */

  uint32_t FSMC_TARSetupTime;     /* Defines the number of HCLK cycles to configure the
                                     delay between ALE low and RE low.
                                     This parameter can be a number between 0x0 and 0xFF */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_CommonSpaceTimingStruct;   /* FSMC Common Space Timing */

  FSMC_NAND_PCCARDTimingInitTypeDef*  FSMC_AttributeSpaceTimingStruct; /* FSMC Attribute Space Timing */
}FSMC_NANDInitTypeDef;


/* FSMC_NORSRAM_Bank */
#define FSMC_Bank1_NORSRAM1                             ((uint32_t)0x00000000)

/* FSMC_NAND_Bank */
#define FSMC_Bank2_NAND                                 ((uint32_t)0x00000010)

/* FSMC_Data_Address_Bus_Multiplexing */
#define FSMC_DataAddressMux_Disable                     ((uint32_t)0x00000000)
#define FSMC_DataAddressMux_Enable                      ((uint32_t)0x00000002)

/* FSMC_Memory_Type */
#define FSMC_MemoryType_SRAM                            ((uint32_t)0x00000000)
#define FSMC_MemoryType_PSRAM                           ((uint32_t)0x00000004)
#define FSMC_MemoryType_NOR                             ((uint32_t)0x00000008)

/* FSMC_Data_Width */
#define FSMC_MemoryDataWidth_8b                         ((uint32_t)0x00000000)
#define FSMC_MemoryDataWidth_16b                        ((uint32_t)0x00000010)

/* FSMC_Burst_Access_Mode */
#define FSMC_BurstAccessMode_Disable                    ((uint32_t)0x00000000) 
#define FSMC_BurstAccessMode_Enable                     ((uint32_t)0x00000100)

/* FSMC_AsynchronousWait */
#define FSMC_AsynchronousWait_Disable                   ((uint32_t)0x00000000)
#define FSMC_AsynchronousWait_Enable                    ((uint32_t)0x00008000)

/* FSMC_Wait_Signal_Polarity */
#define FSMC_WaitSignalPolarity_Low                     ((uint32_t)0x00000000)
#define FSMC_WaitSignalPolarity_High                    ((uint32_t)0x00000200)

/* FSMC_Wrap_Mode */
#define FSMC_WrapMode_Disable                           ((uint32_t)0x00000000)
#define FSMC_WrapMode_Enable                            ((uint32_t)0x00000400) 

/* FSMC_Wait_Timing */
#define FSMC_WaitSignalActive_BeforeWaitState           ((uint32_t)0x00000000)
#define FSMC_WaitSignalActive_DuringWaitState           ((uint32_t)0x00000800) 

/* FSMC_Write_Operation */
#define FSMC_WriteOperation_Disable                     ((uint32_t)0x00000000)
#define FSMC_WriteOperation_Enable                      ((uint32_t)0x00001000)

/* FSMC_Wait_Signal */
#define FSMC_WaitSignal_Disable                         ((uint32_t)0x00000000)
#define FSMC_WaitSignal_Enable                          ((uint32_t)0x00002000) 

/* FSMC_Extended_Mode */
#define FSMC_ExtendedMode_Disable                       ((uint32_t)0x00000000)
#define FSMC_ExtendedMode_Enable                        ((uint32_t)0x00004000)

/* FSMC_Write_Burst */
#define FSMC_WriteBurst_Disable                         ((uint32_t)0x00000000)
#define FSMC_WriteBurst_Enable                          ((uint32_t)0x00080000) 

/* FSMC_Access_Mode */
#define FSMC_AccessMode_A                               ((uint32_t)0x00000000)
#define FSMC_AccessMode_B                               ((uint32_t)0x10000000) 
#define FSMC_AccessMode_C                               ((uint32_t)0x20000000)
#define FSMC_AccessMode_D                               ((uint32_t)0x30000000)

/* FSMC_Wait_feature */
#define FSMC_Waitfeature_Disable                        ((uint32_t)0x00000000)
#define FSMC_Waitfeature_Enable                         ((uint32_t)0x00000002)

/* FSMC_ECC */
#define FSMC_ECC_Disable                                ((uint32_t)0x00000000)
#define FSMC_ECC_Enable                                 ((uint32_t)0x00000040)

/* FSMC_ECC_Page_Size */
#define FSMC_ECCPageSize_256Bytes                       ((uint32_t)0x00000000)
#define FSMC_ECCPageSize_512Bytes                       ((uint32_t)0x00020000)
#define FSMC_ECCPageSize_1024Bytes                      ((uint32_t)0x00040000)
#define FSMC_ECCPageSize_2048Bytes                      ((uint32_t)0x00060000)
#define FSMC_ECCPageSize_4096Bytes                      ((uint32_t)0x00080000)
#define FSMC_ECCPageSize_8192Bytes                      ((uint32_t)0x000A0000)

/* FSMC_Interrupt_sources */
#define FSMC_IT_RisingEdge                              ((uint32_t)0x00000008)
#define FSMC_IT_Level                                   ((uint32_t)0x00000010)
#define FSMC_IT_FallingEdge                             ((uint32_t)0x00000020)

/* FSMC_Flags */
#define FSMC_FLAG_RisingEdge                            ((uint32_t)0x00000001)
#define FSMC_FLAG_Level                                 ((uint32_t)0x00000002)
#define FSMC_FLAG_FallingEdge                           ((uint32_t)0x00000004)
#define FSMC_FLAG_FEMPT                                 ((uint32_t)0x00000040)


void FSMC_NORSRAMDeInit(uint32_t FSMC_Bank);
void FSMC_NANDDeInit(uint32_t FSMC_Bank);
void FSMC_NORSRAMInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);
void FSMC_NANDInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);
void FSMC_NORSRAMStructInit(FSMC_NORSRAMInitTypeDef* FSMC_NORSRAMInitStruct);
void FSMC_NANDStructInit(FSMC_NANDInitTypeDef* FSMC_NANDInitStruct);
void FSMC_NORSRAMCmd(uint32_t FSMC_Bank, FunctionalState NewState);
void FSMC_NANDCmd(uint32_t FSMC_Bank, FunctionalState NewState);
void FSMC_NANDECCCmd(uint32_t FSMC_Bank, FunctionalState NewState);
uint32_t FSMC_GetECC(uint32_t FSMC_Bank);
void FSMC_ITConfig(uint32_t FSMC_Bank, uint32_t FSMC_IT, FunctionalState NewState);
FlagStatus FSMC_GetFlagStatus(uint32_t FSMC_Bank, uint32_t FSMC_FLAG);
void FSMC_ClearFlag(uint32_t FSMC_Bank, uint32_t FSMC_FLAG);
ITStatus FSMC_GetITStatus(uint32_t FSMC_Bank, uint32_t FSMC_IT);
void FSMC_ClearITPendingBit(uint32_t FSMC_Bank, uint32_t FSMC_IT);

#ifdef __cplusplus
}
#endif

#endif 
