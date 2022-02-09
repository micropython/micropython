/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_gpio.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file contains all the functions prototypes for the 
*                      GPIO firmware library.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_GPIO_H
#define __CH32V30x_GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"
                                 
/* Output Maximum frequency selection */
typedef enum
{ 
  GPIO_Speed_10MHz = 1,
  GPIO_Speed_2MHz, 
  GPIO_Speed_50MHz
}GPIOSpeed_TypeDef;

/* Configuration Mode enumeration */
typedef enum
{ GPIO_Mode_AIN = 0x0,
  GPIO_Mode_IN_FLOATING = 0x04,
  GPIO_Mode_IPD = 0x28,
  GPIO_Mode_IPU = 0x48,
  GPIO_Mode_Out_OD = 0x14,
  GPIO_Mode_Out_PP = 0x10,
  GPIO_Mode_AF_OD = 0x1C,
  GPIO_Mode_AF_PP = 0x18
}GPIOMode_TypeDef;

/* GPIO Init structure definition */
typedef struct
{
  uint16_t GPIO_Pin;             /* Specifies the GPIO pins to be configured.
                                    This parameter can be any value of @ref GPIO_pins_define */

  GPIOSpeed_TypeDef GPIO_Speed;  /* Specifies the speed for the selected pins.
                                    This parameter can be a value of @ref GPIOSpeed_TypeDef */

  GPIOMode_TypeDef GPIO_Mode;    /* Specifies the operating mode for the selected pins.
                                    This parameter can be a value of @ref GPIOMode_TypeDef */
}GPIO_InitTypeDef;

/* Bit_SET and Bit_RESET enumeration */
typedef enum
{
	Bit_RESET = 0,
  Bit_SET
}BitAction;

/* GPIO_pins_define */
#define GPIO_Pin_0                  ((uint16_t)0x0001)  /* Pin 0 selected */
#define GPIO_Pin_1                  ((uint16_t)0x0002)  /* Pin 1 selected */
#define GPIO_Pin_2                  ((uint16_t)0x0004)  /* Pin 2 selected */
#define GPIO_Pin_3                  ((uint16_t)0x0008)  /* Pin 3 selected */
#define GPIO_Pin_4                  ((uint16_t)0x0010)  /* Pin 4 selected */
#define GPIO_Pin_5                  ((uint16_t)0x0020)  /* Pin 5 selected */
#define GPIO_Pin_6                  ((uint16_t)0x0040)  /* Pin 6 selected */
#define GPIO_Pin_7                  ((uint16_t)0x0080)  /* Pin 7 selected */
#define GPIO_Pin_8                  ((uint16_t)0x0100)  /* Pin 8 selected */
#define GPIO_Pin_9                  ((uint16_t)0x0200)  /* Pin 9 selected */
#define GPIO_Pin_10                 ((uint16_t)0x0400)  /* Pin 10 selected */
#define GPIO_Pin_11                 ((uint16_t)0x0800)  /* Pin 11 selected */
#define GPIO_Pin_12                 ((uint16_t)0x1000)  /* Pin 12 selected */
#define GPIO_Pin_13                 ((uint16_t)0x2000)  /* Pin 13 selected */
#define GPIO_Pin_14                 ((uint16_t)0x4000)  /* Pin 14 selected */
#define GPIO_Pin_15                 ((uint16_t)0x8000)  /* Pin 15 selected */
#define GPIO_Pin_All                ((uint16_t)0xFFFF)  /* All pins selected */

/* GPIO_Remap_define */
/* PCFR1 */
#define GPIO_Remap_SPI1             ((uint32_t)0x00000001)  /* SPI1 Alternate Function mapping */
#define GPIO_Remap_I2C1             ((uint32_t)0x00000002)  /* I2C1 Alternate Function mapping */
#define GPIO_Remap_USART1           ((uint32_t)0x00000004)  /* USART1 Alternate Function mapping low bit */
#define GPIO_Remap_USART2           ((uint32_t)0x00000008)  /* USART2 Alternate Function mapping */
#define GPIO_PartialRemap_USART3    ((uint32_t)0x00140010)  /* USART3 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART3       ((uint32_t)0x00140030)  /* USART3 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM1      ((uint32_t)0x00160040)  /* TIM1 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM1         ((uint32_t)0x001600C0)  /* TIM1 Full Alternate Function mapping */
#define GPIO_PartialRemap1_TIM2     ((uint32_t)0x00180100)  /* TIM2 Partial1 Alternate Function mapping */
#define GPIO_PartialRemap2_TIM2     ((uint32_t)0x00180200)  /* TIM2 Partial2 Alternate Function mapping */
#define GPIO_FullRemap_TIM2         ((uint32_t)0x00180300)  /* TIM2 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM3      ((uint32_t)0x001A0800)  /* TIM3 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM3         ((uint32_t)0x001A0C00)  /* TIM3 Full Alternate Function mapping */
#define GPIO_Remap_TIM4             ((uint32_t)0x00001000)  /* TIM4 Alternate Function mapping */
#define GPIO_Remap1_CAN1            ((uint32_t)0x001D4000)  /* CAN1 Alternate Function mapping */
#define GPIO_Remap2_CAN1            ((uint32_t)0x001D6000)  /* CAN1 Alternate Function mapping */
#define GPIO_Remap_PD01             ((uint32_t)0x00008000)  /* PD01 Alternate Function mapping */
#define GPIO_Remap_TIM5CH4_LSI      ((uint32_t)0x00200001)  /* LSI connected to TIM5 Channel4 input capture for calibration */
#define GPIO_Remap_ADC1_ETRGINJ     ((uint32_t)0x00200002)  /* ADC1 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC1_ETRGREG     ((uint32_t)0x00200004)  /* ADC1 External Trigger Regular Conversion remapping */
#define GPIO_Remap_ADC2_ETRGINJ     ((uint32_t)0x00200008)  /* ADC2 External Trigger Injected Conversion remapping */
#define GPIO_Remap_ADC2_ETRGREG     ((uint32_t)0x00200010)  /* ADC2 External Trigger Regular Conversion remapping */
#define GPIO_Remap_ETH              ((uint32_t)0x00200020)  /* Ethernet remapping (only for Connectivity line devices) */
#define GPIO_Remap_CAN2             ((uint32_t)0x00200040)  /* CAN2 remapping (only for Connectivity line devices) */
#define GPIO_Remap_MII_RMII_SEL     ((uint32_t)0x00200080)  /* MII or RMII selection */
#define GPIO_Remap_SWJ_NoJTRST      ((uint32_t)0x00300100)  /* Full SWJ Enabled (JTAG-DP + SW-DP) but without JTRST */
#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /* JTAG-DP Disabled and SW-DP Enabled */
#define GPIO_Remap_SWJ_Disable      ((uint32_t)0x00300400)  /* Full SWJ Disabled (JTAG-DP + SW-DP) */
#define GPIO_Remap_SPI3             ((uint32_t)0x00201000)  /* SPI3/I2S3 Alternate Function mapping (only for Connectivity line devices) */
#define GPIO_Remap_TIM2ITR1_PTP_SOF ((uint32_t)0x00202000)  /* Ethernet PTP output or USB OTG SOF (Start of Frame) connected
                                                               to TIM2 Internal Trigger 1 for calibration
                                                               (only for Connectivity line devices) */
#define GPIO_Remap_PTP_PPS          ((uint32_t)0x00204000)  /* Ethernet MAC PPS_PTS output on PB05 (only for Connectivity line devices) */

/* PCFR2 */
#define GPIO_Remap_TIM8             ((uint32_t)0x80000004)  /* TIM8 Alternate Function mapping */
#define GPIO_PartialRemap_TIM9      ((uint32_t)0x80130008)  /* TIM9 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM9         ((uint32_t)0x80130010)  /* TIM9 Full Alternate Function mapping */
#define GPIO_PartialRemap_TIM10     ((uint32_t)0x80150020)  /* TIM10 Partial Alternate Function mapping */
#define GPIO_FullRemap_TIM10        ((uint32_t)0x80150040)  /* TIM10 Full Alternate Function mapping */
#define GPIO_Remap_FSMC_NADV        ((uint32_t)0x80000400)  /* FSMC_NADV Alternate Function mapping */
#define GPIO_PartialRemap_USART4    ((uint32_t)0x80300001)  /* USART4 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART4       ((uint32_t)0x80300002)  /* USART4 Full Alternate Function mapping */
#define GPIO_PartialRemap_USART5    ((uint32_t)0x80320004)  /* USART5 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART5       ((uint32_t)0x80320008)  /* USART5 Full Alternate Function mapping */
#define GPIO_PartialRemap_USART6    ((uint32_t)0x80340010)  /* USART6 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART6       ((uint32_t)0x80340020)  /* USART6 Full Alternate Function mapping */
#define GPIO_PartialRemap_USART7    ((uint32_t)0x80360040)  /* USART7 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART7       ((uint32_t)0x80360080)  /* USART7 Full Alternate Function mapping */
#define GPIO_PartialRemap_USART8    ((uint32_t)0x80380100)  /* USART8 Partial Alternate Function mapping */
#define GPIO_FullRemap_USART8       ((uint32_t)0x80380200)  /* USART8 Full Alternate Function mapping */
#define GPIO_Remap_USART1_HighBit   ((uint32_t)0x80200400)  /* USART1 Alternate Function mapping high bit */


/* GPIO_Port_Sources */
#define GPIO_PortSourceGPIOA        ((uint8_t)0x00)
#define GPIO_PortSourceGPIOB        ((uint8_t)0x01)
#define GPIO_PortSourceGPIOC        ((uint8_t)0x02)
#define GPIO_PortSourceGPIOD        ((uint8_t)0x03)
#define GPIO_PortSourceGPIOE        ((uint8_t)0x04)
#define GPIO_PortSourceGPIOF        ((uint8_t)0x05)
#define GPIO_PortSourceGPIOG        ((uint8_t)0x06)

/* GPIO_Pin_sources */
#define GPIO_PinSource0             ((uint8_t)0x00)
#define GPIO_PinSource1             ((uint8_t)0x01)
#define GPIO_PinSource2             ((uint8_t)0x02)
#define GPIO_PinSource3             ((uint8_t)0x03)
#define GPIO_PinSource4             ((uint8_t)0x04)
#define GPIO_PinSource5             ((uint8_t)0x05)
#define GPIO_PinSource6             ((uint8_t)0x06)
#define GPIO_PinSource7             ((uint8_t)0x07)
#define GPIO_PinSource8             ((uint8_t)0x08)
#define GPIO_PinSource9             ((uint8_t)0x09)
#define GPIO_PinSource10            ((uint8_t)0x0A)
#define GPIO_PinSource11            ((uint8_t)0x0B)
#define GPIO_PinSource12            ((uint8_t)0x0C)
#define GPIO_PinSource13            ((uint8_t)0x0D)
#define GPIO_PinSource14            ((uint8_t)0x0E)
#define GPIO_PinSource15            ((uint8_t)0x0F)

/* Ethernet_Media_Interface */
#define GPIO_ETH_MediaInterface_MII    ((u32)0x00000000)
#define GPIO_ETH_MediaInterface_RMII   ((u32)0x00000001)


void GPIO_DeInit(GPIO_TypeDef* GPIOx);
void GPIO_AFIODeInit(void);
void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
void GPIO_StructInit(GPIO_InitTypeDef* GPIO_InitStruct);
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadInputData(GPIO_TypeDef* GPIOx);
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_ReadOutputData(GPIO_TypeDef* GPIOx);
void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, BitAction BitVal);
void GPIO_Write(GPIO_TypeDef* GPIOx, uint16_t PortVal);
void GPIO_PinLockConfig(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_EventOutputConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_EventOutputCmd(FunctionalState NewState);
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState);
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource);
void GPIO_ETH_MediaInterfaceConfig(uint32_t GPIO_ETH_MediaInterface);

#ifdef __cplusplus
}
#endif

#endif 







