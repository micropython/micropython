/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_dac.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      DAC firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/
#ifndef __CH32V30x_DAC_H
#define __CH32V30x_DAC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* DAC Init structure definition */
typedef struct
{
  uint32_t DAC_Trigger;                      /* Specifies the external trigger for the selected DAC channel.
                                                This parameter can be a value of @ref DAC_trigger_selection */

  uint32_t DAC_WaveGeneration;               /* Specifies whether DAC channel noise waves or triangle waves
                                                are generated, or whether no wave is generated.
                                                This parameter can be a value of @ref DAC_wave_generation */

  uint32_t DAC_LFSRUnmask_TriangleAmplitude; /* Specifies the LFSR mask for noise wave generation or
                                                the maximum amplitude triangle generation for the DAC channel. 
                                                This parameter can be a value of @ref DAC_lfsrunmask_triangleamplitude */

  uint32_t DAC_OutputBuffer;                 /* Specifies whether the DAC channel output buffer is enabled or disabled.
                                                This parameter can be a value of @ref DAC_output_buffer */
}DAC_InitTypeDef;


/* DAC_trigger_selection */
#define DAC_Trigger_None                   ((uint32_t)0x00000000) /* Conversion is automatic once the DAC1_DHRxxxx register 
                                                                     has been loaded, and not by external trigger */
#define DAC_Trigger_T6_TRGO                ((uint32_t)0x00000004) /* TIM6 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T8_TRGO                ((uint32_t)0x0000000C) /* TIM8 TRGO selected as external conversion trigger for DAC channel
                                                                     only in High-density devices*/
#define DAC_Trigger_T7_TRGO                ((uint32_t)0x00000014) /* TIM7 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T5_TRGO                ((uint32_t)0x0000001C) /* TIM5 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T2_TRGO                ((uint32_t)0x00000024) /* TIM2 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_T4_TRGO                ((uint32_t)0x0000002C) /* TIM4 TRGO selected as external conversion trigger for DAC channel */
#define DAC_Trigger_Ext_IT9                ((uint32_t)0x00000034) /* EXTI Line9 event selected as external conversion trigger for DAC channel */
#define DAC_Trigger_Software               ((uint32_t)0x0000003C) /* Conversion started by software trigger for DAC channel */

/* DAC_wave_generation */
#define DAC_WaveGeneration_None            ((uint32_t)0x00000000)
#define DAC_WaveGeneration_Noise           ((uint32_t)0x00000040)
#define DAC_WaveGeneration_Triangle        ((uint32_t)0x00000080)


/* DAC_lfsrunmask_triangleamplitude */
#define DAC_LFSRUnmask_Bit0                ((uint32_t)0x00000000) /* Unmask DAC channel LFSR bit0 for noise wave generation */
#define DAC_LFSRUnmask_Bits1_0             ((uint32_t)0x00000100) /* Unmask DAC channel LFSR bit[1:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits2_0             ((uint32_t)0x00000200) /* Unmask DAC channel LFSR bit[2:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits3_0             ((uint32_t)0x00000300) /* Unmask DAC channel LFSR bit[3:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits4_0             ((uint32_t)0x00000400) /* Unmask DAC channel LFSR bit[4:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits5_0             ((uint32_t)0x00000500) /* Unmask DAC channel LFSR bit[5:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits6_0             ((uint32_t)0x00000600) /* Unmask DAC channel LFSR bit[6:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits7_0             ((uint32_t)0x00000700) /* Unmask DAC channel LFSR bit[7:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits8_0             ((uint32_t)0x00000800) /* Unmask DAC channel LFSR bit[8:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits9_0             ((uint32_t)0x00000900) /* Unmask DAC channel LFSR bit[9:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits10_0            ((uint32_t)0x00000A00) /* Unmask DAC channel LFSR bit[10:0] for noise wave generation */
#define DAC_LFSRUnmask_Bits11_0            ((uint32_t)0x00000B00) /* Unmask DAC channel LFSR bit[11:0] for noise wave generation */
#define DAC_TriangleAmplitude_1            ((uint32_t)0x00000000) /* Select max triangle amplitude of 1 */
#define DAC_TriangleAmplitude_3            ((uint32_t)0x00000100) /* Select max triangle amplitude of 3 */
#define DAC_TriangleAmplitude_7            ((uint32_t)0x00000200) /* Select max triangle amplitude of 7 */
#define DAC_TriangleAmplitude_15           ((uint32_t)0x00000300) /* Select max triangle amplitude of 15 */
#define DAC_TriangleAmplitude_31           ((uint32_t)0x00000400) /* Select max triangle amplitude of 31 */
#define DAC_TriangleAmplitude_63           ((uint32_t)0x00000500) /* Select max triangle amplitude of 63 */
#define DAC_TriangleAmplitude_127          ((uint32_t)0x00000600) /* Select max triangle amplitude of 127 */
#define DAC_TriangleAmplitude_255          ((uint32_t)0x00000700) /* Select max triangle amplitude of 255 */
#define DAC_TriangleAmplitude_511          ((uint32_t)0x00000800) /* Select max triangle amplitude of 511 */
#define DAC_TriangleAmplitude_1023         ((uint32_t)0x00000900) /* Select max triangle amplitude of 1023 */
#define DAC_TriangleAmplitude_2047         ((uint32_t)0x00000A00) /* Select max triangle amplitude of 2047 */
#define DAC_TriangleAmplitude_4095         ((uint32_t)0x00000B00) /* Select max triangle amplitude of 4095 */

/* DAC_output_buffer */
#define DAC_OutputBuffer_Enable            ((uint32_t)0x00000000)
#define DAC_OutputBuffer_Disable           ((uint32_t)0x00000002)

/* DAC_Channel_selection */
#define DAC_Channel_1                      ((uint32_t)0x00000000)
#define DAC_Channel_2                      ((uint32_t)0x00000010)
																 
/* DAC_data_alignment */
#define DAC_Align_12b_R                    ((uint32_t)0x00000000)
#define DAC_Align_12b_L                    ((uint32_t)0x00000004)
#define DAC_Align_8b_R                     ((uint32_t)0x00000008)

/* DAC_wave_generation */
#define DAC_Wave_Noise                     ((uint32_t)0x00000040)
#define DAC_Wave_Triangle                  ((uint32_t)0x00000080)


void DAC_DeInit(void);
void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);
void DAC_StructInit(DAC_InitTypeDef* DAC_InitStruct);
void DAC_Cmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_DMACmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
void DAC_DualSoftwareTriggerCmd(FunctionalState NewState);
void DAC_WaveGenerationCmd(uint32_t DAC_Channel, uint32_t DAC_Wave, FunctionalState NewState);
void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);
void DAC_SetChannel2Data(uint32_t DAC_Align, uint16_t Data);
void DAC_SetDualChannelData(uint32_t DAC_Align, uint16_t Data2, uint16_t Data1);
uint16_t DAC_GetDataOutputValue(uint32_t DAC_Channel);

#ifdef __cplusplus
}
#endif

#endif 

