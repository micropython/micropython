/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dma.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      DMA firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/  
#ifndef __CH32V30x_DMA_H
#define __CH32V30x_DMA_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"
 
/* DMA Init structure definition */
typedef struct
{
  uint32_t DMA_PeripheralBaseAddr; /* Specifies the peripheral base address for DMAy Channelx. */

  uint32_t DMA_MemoryBaseAddr;     /* Specifies the memory base address for DMAy Channelx. */

  uint32_t DMA_DIR;                /* Specifies if the peripheral is the source or destination.
                                      This parameter can be a value of @ref DMA_data_transfer_direction */

  uint32_t DMA_BufferSize;         /* Specifies the buffer size, in data unit, of the specified Channel. 
                                      The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                      or DMA_MemoryDataSize members depending in the transfer direction. */

  uint32_t DMA_PeripheralInc;      /* Specifies whether the Peripheral address register is incremented or not.
                                      This parameter can be a value of @ref DMA_peripheral_incremented_mode */

  uint32_t DMA_MemoryInc;          /* Specifies whether the memory address register is incremented or not.
                                      This parameter can be a value of @ref DMA_memory_incremented_mode */

  uint32_t DMA_PeripheralDataSize; /* Specifies the Peripheral data width.
                                      This parameter can be a value of @ref DMA_peripheral_data_size */

  uint32_t DMA_MemoryDataSize;     /* Specifies the Memory data width.
                                      This parameter can be a value of @ref DMA_memory_data_size */

  uint32_t DMA_Mode;               /* Specifies the operation mode of the DMAy Channelx.
                                      This parameter can be a value of @ref DMA_circular_normal_mode.
                                      @note: The circular buffer mode cannot be used if the memory-to-memory
                                            data transfer is configured on the selected Channel */

  uint32_t DMA_Priority;           /* Specifies the software priority for the DMAy Channelx.
                                      This parameter can be a value of @ref DMA_priority_level */

  uint32_t DMA_M2M;                /* Specifies if the DMAy Channelx will be used in memory-to-memory transfer.
                                      This parameter can be a value of @ref DMA_memory_to_memory */
}DMA_InitTypeDef;

/* DMA_data_transfer_direction */
#define DMA_DIR_PeripheralDST              ((uint32_t)0x00000010)
#define DMA_DIR_PeripheralSRC              ((uint32_t)0x00000000)

/* DMA_peripheral_incremented_mode */
#define DMA_PeripheralInc_Enable           ((uint32_t)0x00000040)
#define DMA_PeripheralInc_Disable          ((uint32_t)0x00000000)
											
/* DMA_memory_incremented_mode */
#define DMA_MemoryInc_Enable               ((uint32_t)0x00000080)
#define DMA_MemoryInc_Disable              ((uint32_t)0x00000000)
										
/* DMA_peripheral_data_size */
#define DMA_PeripheralDataSize_Byte        ((uint32_t)0x00000000)
#define DMA_PeripheralDataSize_HalfWord    ((uint32_t)0x00000100)
#define DMA_PeripheralDataSize_Word        ((uint32_t)0x00000200)

/* DMA_memory_data_size */
#define DMA_MemoryDataSize_Byte            ((uint32_t)0x00000000)
#define DMA_MemoryDataSize_HalfWord        ((uint32_t)0x00000400)
#define DMA_MemoryDataSize_Word            ((uint32_t)0x00000800)

/* DMA_circular_normal_mode */
#define DMA_Mode_Circular                  ((uint32_t)0x00000020)
#define DMA_Mode_Normal                    ((uint32_t)0x00000000)

/* DMA_priority_level */
#define DMA_Priority_VeryHigh              ((uint32_t)0x00003000)
#define DMA_Priority_High                  ((uint32_t)0x00002000)
#define DMA_Priority_Medium                ((uint32_t)0x00001000)
#define DMA_Priority_Low                   ((uint32_t)0x00000000)

/* DMA_memory_to_memory */
#define DMA_M2M_Enable                     ((uint32_t)0x00004000)
#define DMA_M2M_Disable                    ((uint32_t)0x00000000)

/* DMA_interrupts_definition */
#define DMA_IT_TC                          ((uint32_t)0x00000002)
#define DMA_IT_HT                          ((uint32_t)0x00000004)
#define DMA_IT_TE                          ((uint32_t)0x00000008)

#define DMA1_IT_GL1                        ((uint32_t)0x00000001)
#define DMA1_IT_TC1                        ((uint32_t)0x00000002)
#define DMA1_IT_HT1                        ((uint32_t)0x00000004)
#define DMA1_IT_TE1                        ((uint32_t)0x00000008)
#define DMA1_IT_GL2                        ((uint32_t)0x00000010)
#define DMA1_IT_TC2                        ((uint32_t)0x00000020)
#define DMA1_IT_HT2                        ((uint32_t)0x00000040)
#define DMA1_IT_TE2                        ((uint32_t)0x00000080)
#define DMA1_IT_GL3                        ((uint32_t)0x00000100)
#define DMA1_IT_TC3                        ((uint32_t)0x00000200)
#define DMA1_IT_HT3                        ((uint32_t)0x00000400)
#define DMA1_IT_TE3                        ((uint32_t)0x00000800)
#define DMA1_IT_GL4                        ((uint32_t)0x00001000)
#define DMA1_IT_TC4                        ((uint32_t)0x00002000)
#define DMA1_IT_HT4                        ((uint32_t)0x00004000)
#define DMA1_IT_TE4                        ((uint32_t)0x00008000)
#define DMA1_IT_GL5                        ((uint32_t)0x00010000)
#define DMA1_IT_TC5                        ((uint32_t)0x00020000)
#define DMA1_IT_HT5                        ((uint32_t)0x00040000)
#define DMA1_IT_TE5                        ((uint32_t)0x00080000)
#define DMA1_IT_GL6                        ((uint32_t)0x00100000)
#define DMA1_IT_TC6                        ((uint32_t)0x00200000)
#define DMA1_IT_HT6                        ((uint32_t)0x00400000)
#define DMA1_IT_TE6                        ((uint32_t)0x00800000)
#define DMA1_IT_GL7                        ((uint32_t)0x01000000)
#define DMA1_IT_TC7                        ((uint32_t)0x02000000)
#define DMA1_IT_HT7                        ((uint32_t)0x04000000)
#define DMA1_IT_TE7                        ((uint32_t)0x08000000)

#define DMA2_IT_GL1                        ((uint32_t)0x10000001)
#define DMA2_IT_TC1                        ((uint32_t)0x10000002)
#define DMA2_IT_HT1                        ((uint32_t)0x10000004)
#define DMA2_IT_TE1                        ((uint32_t)0x10000008)
#define DMA2_IT_GL2                        ((uint32_t)0x10000010)
#define DMA2_IT_TC2                        ((uint32_t)0x10000020)
#define DMA2_IT_HT2                        ((uint32_t)0x10000040)
#define DMA2_IT_TE2                        ((uint32_t)0x10000080)
#define DMA2_IT_GL3                        ((uint32_t)0x10000100)
#define DMA2_IT_TC3                        ((uint32_t)0x10000200)
#define DMA2_IT_HT3                        ((uint32_t)0x10000400)
#define DMA2_IT_TE3                        ((uint32_t)0x10000800)
#define DMA2_IT_GL4                        ((uint32_t)0x10001000)
#define DMA2_IT_TC4                        ((uint32_t)0x10002000)
#define DMA2_IT_HT4                        ((uint32_t)0x10004000)
#define DMA2_IT_TE4                        ((uint32_t)0x10008000)
#define DMA2_IT_GL5                        ((uint32_t)0x10010000)
#define DMA2_IT_TC5                        ((uint32_t)0x10020000)
#define DMA2_IT_HT5                        ((uint32_t)0x10040000)
#define DMA2_IT_TE5                        ((uint32_t)0x10080000)
#define DMA2_IT_GL6                        ((uint32_t)0x10100000)
#define DMA2_IT_TC6                        ((uint32_t)0x10200000)
#define DMA2_IT_HT6                        ((uint32_t)0x10400000)
#define DMA2_IT_TE6                        ((uint32_t)0x10800000)
#define DMA2_IT_GL7                        ((uint32_t)0x11000000)
#define DMA2_IT_TC7                        ((uint32_t)0x12000000)
#define DMA2_IT_HT7                        ((uint32_t)0x14000000)
#define DMA2_IT_TE7                        ((uint32_t)0x18000000)

#define DMA2_IT_GL8                        ((uint32_t)0x20000001)
#define DMA2_IT_TC8                        ((uint32_t)0x20000002)
#define DMA2_IT_HT8                        ((uint32_t)0x20000004)
#define DMA2_IT_TE8                        ((uint32_t)0x20000008)
#define DMA2_IT_GL9                        ((uint32_t)0x20000010)
#define DMA2_IT_TC9                        ((uint32_t)0x20000020)
#define DMA2_IT_HT9                        ((uint32_t)0x20000040)
#define DMA2_IT_TE9                        ((uint32_t)0x20000080)
#define DMA2_IT_GL10                       ((uint32_t)0x20000100)
#define DMA2_IT_TC10                       ((uint32_t)0x20000200)
#define DMA2_IT_HT10                       ((uint32_t)0x20000400)
#define DMA2_IT_TE10                       ((uint32_t)0x20000800)
#define DMA2_IT_GL11                       ((uint32_t)0x20001000)
#define DMA2_IT_TC11                       ((uint32_t)0x20002000)
#define DMA2_IT_HT11                       ((uint32_t)0x20004000)
#define DMA2_IT_TE11                       ((uint32_t)0x20008000)

/* DMA_flags_definition */
#define DMA1_FLAG_GL1                      ((uint32_t)0x00000001)
#define DMA1_FLAG_TC1                      ((uint32_t)0x00000002)
#define DMA1_FLAG_HT1                      ((uint32_t)0x00000004)
#define DMA1_FLAG_TE1                      ((uint32_t)0x00000008)
#define DMA1_FLAG_GL2                      ((uint32_t)0x00000010)
#define DMA1_FLAG_TC2                      ((uint32_t)0x00000020)
#define DMA1_FLAG_HT2                      ((uint32_t)0x00000040)
#define DMA1_FLAG_TE2                      ((uint32_t)0x00000080)
#define DMA1_FLAG_GL3                      ((uint32_t)0x00000100)
#define DMA1_FLAG_TC3                      ((uint32_t)0x00000200)
#define DMA1_FLAG_HT3                      ((uint32_t)0x00000400)
#define DMA1_FLAG_TE3                      ((uint32_t)0x00000800)
#define DMA1_FLAG_GL4                      ((uint32_t)0x00001000)
#define DMA1_FLAG_TC4                      ((uint32_t)0x00002000)
#define DMA1_FLAG_HT4                      ((uint32_t)0x00004000)
#define DMA1_FLAG_TE4                      ((uint32_t)0x00008000)
#define DMA1_FLAG_GL5                      ((uint32_t)0x00010000)
#define DMA1_FLAG_TC5                      ((uint32_t)0x00020000)
#define DMA1_FLAG_HT5                      ((uint32_t)0x00040000)
#define DMA1_FLAG_TE5                      ((uint32_t)0x00080000)
#define DMA1_FLAG_GL6                      ((uint32_t)0x00100000)
#define DMA1_FLAG_TC6                      ((uint32_t)0x00200000)
#define DMA1_FLAG_HT6                      ((uint32_t)0x00400000)
#define DMA1_FLAG_TE6                      ((uint32_t)0x00800000)
#define DMA1_FLAG_GL7                      ((uint32_t)0x01000000)
#define DMA1_FLAG_TC7                      ((uint32_t)0x02000000)
#define DMA1_FLAG_HT7                      ((uint32_t)0x04000000)
#define DMA1_FLAG_TE7                      ((uint32_t)0x08000000)

#define DMA2_FLAG_GL1                      ((uint32_t)0x10000001)
#define DMA2_FLAG_TC1                      ((uint32_t)0x10000002)
#define DMA2_FLAG_HT1                      ((uint32_t)0x10000004)
#define DMA2_FLAG_TE1                      ((uint32_t)0x10000008)
#define DMA2_FLAG_GL2                      ((uint32_t)0x10000010)
#define DMA2_FLAG_TC2                      ((uint32_t)0x10000020)
#define DMA2_FLAG_HT2                      ((uint32_t)0x10000040)
#define DMA2_FLAG_TE2                      ((uint32_t)0x10000080)
#define DMA2_FLAG_GL3                      ((uint32_t)0x10000100)
#define DMA2_FLAG_TC3                      ((uint32_t)0x10000200)
#define DMA2_FLAG_HT3                      ((uint32_t)0x10000400)
#define DMA2_FLAG_TE3                      ((uint32_t)0x10000800)
#define DMA2_FLAG_GL4                      ((uint32_t)0x10001000)
#define DMA2_FLAG_TC4                      ((uint32_t)0x10002000)
#define DMA2_FLAG_HT4                      ((uint32_t)0x10004000)
#define DMA2_FLAG_TE4                      ((uint32_t)0x10008000)
#define DMA2_FLAG_GL5                      ((uint32_t)0x10010000)
#define DMA2_FLAG_TC5                      ((uint32_t)0x10020000)
#define DMA2_FLAG_HT5                      ((uint32_t)0x10040000)
#define DMA2_FLAG_TE5                      ((uint32_t)0x10080000)
#define DMA2_FLAG_GL6                      ((uint32_t)0x10100000)
#define DMA2_FLAG_TC6                      ((uint32_t)0x10200000)
#define DMA2_FLAG_HT6                      ((uint32_t)0x10400000)
#define DMA2_FLAG_TE6                      ((uint32_t)0x10800000)
#define DMA2_FLAG_GL7                      ((uint32_t)0x11000000)
#define DMA2_FLAG_TC7                      ((uint32_t)0x12000000)
#define DMA2_FLAG_HT7                      ((uint32_t)0x14000000)
#define DMA2_FLAG_TE7                      ((uint32_t)0x18000000)

#define DMA2_FLAG_GL8                      ((uint32_t)0x20000001)
#define DMA2_FLAG_TC8                      ((uint32_t)0x20000002)
#define DMA2_FLAG_HT8                      ((uint32_t)0x20000004)
#define DMA2_FLAG_TE8                      ((uint32_t)0x20000008)
#define DMA2_FLAG_GL9                      ((uint32_t)0x20000010)
#define DMA2_FLAG_TC9                      ((uint32_t)0x20000020)
#define DMA2_FLAG_HT9                      ((uint32_t)0x20000040)
#define DMA2_FLAG_TE9                      ((uint32_t)0x20000080)
#define DMA2_FLAG_GL10                     ((uint32_t)0x20000100)
#define DMA2_FLAG_TC10                     ((uint32_t)0x20000200)
#define DMA2_FLAG_HT10                     ((uint32_t)0x20000400)
#define DMA2_FLAG_TE10                     ((uint32_t)0x20000800)
#define DMA2_FLAG_GL11                     ((uint32_t)0x20001000)
#define DMA2_FLAG_TC11                     ((uint32_t)0x20002000)
#define DMA2_FLAG_HT11                     ((uint32_t)0x20004000)
#define DMA2_FLAG_TE11                     ((uint32_t)0x20008000)


void DMA_DeInit(DMA_Channel_TypeDef* DMAy_Channelx);
void DMA_Init(DMA_Channel_TypeDef* DMAy_Channelx, DMA_InitTypeDef* DMA_InitStruct);
void DMA_StructInit(DMA_InitTypeDef* DMA_InitStruct);
void DMA_Cmd(DMA_Channel_TypeDef* DMAy_Channelx, FunctionalState NewState);
void DMA_ITConfig(DMA_Channel_TypeDef* DMAy_Channelx, uint32_t DMA_IT, FunctionalState NewState);
void DMA_SetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber); 
uint16_t DMA_GetCurrDataCounter(DMA_Channel_TypeDef* DMAy_Channelx);
FlagStatus DMA_GetFlagStatus(uint32_t DMAy_FLAG);
void DMA_ClearFlag(uint32_t DMAy_FLAG);
ITStatus DMA_GetITStatus(uint32_t DMAy_IT);
void DMA_ClearITPendingBit(uint32_t DMAy_IT);

#ifdef __cplusplus
}
#endif

#endif 

