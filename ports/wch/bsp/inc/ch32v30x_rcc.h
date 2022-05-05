/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v30x_rcc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2021/06/06
* Description        : This file provides all the RCC firmware functions.
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* SPDX-License-Identifier: Apache-2.0
*******************************************************************************/ 
#ifndef __CH32V30x_RCC_H
#define __CH32V30x_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32v30x.h"

/* RCC_Exported_Types */
typedef struct
{
  uint32_t SYSCLK_Frequency;  /* returns SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency;    /* returns HCLK clock frequency expressed in Hz */
  uint32_t PCLK1_Frequency;   /* returns PCLK1 clock frequency expressed in Hz */
  uint32_t PCLK2_Frequency;   /* returns PCLK2 clock frequency expressed in Hz */
  uint32_t ADCCLK_Frequency;  /* returns ADCCLK clock frequency expressed in Hz */
}RCC_ClocksTypeDef;

/* HSE_configuration */
#define RCC_HSE_OFF                      ((uint32_t)0x00000000)
#define RCC_HSE_ON                       ((uint32_t)0x00010000)
#define RCC_HSE_Bypass                   ((uint32_t)0x00040000)

/* PLL_entry_clock_source */
#define RCC_PLLSource_HSI_Div2           ((uint32_t)0x00000000)

#ifdef CH32V30x_D8
#define RCC_PLLSource_HSE_Div1           ((uint32_t)0x00010000)
#define RCC_PLLSource_HSE_Div2           ((uint32_t)0x00030000)

#else
#define RCC_PLLSource_PREDIV1            ((uint32_t)0x00010000)

#endif

/* PLL_multiplication_factor */
#ifdef CH32V30x_D8
#define RCC_PLLMul_2                     ((uint32_t)0x00000000)
#define RCC_PLLMul_3                     ((uint32_t)0x00040000)
#define RCC_PLLMul_4                     ((uint32_t)0x00080000)
#define RCC_PLLMul_5                     ((uint32_t)0x000C0000)
#define RCC_PLLMul_6                     ((uint32_t)0x00100000)
#define RCC_PLLMul_7                     ((uint32_t)0x00140000)
#define RCC_PLLMul_8                     ((uint32_t)0x00180000)
#define RCC_PLLMul_9                     ((uint32_t)0x001C0000)
#define RCC_PLLMul_10                    ((uint32_t)0x00200000)
#define RCC_PLLMul_11                    ((uint32_t)0x00240000)
#define RCC_PLLMul_12                    ((uint32_t)0x00280000)
#define RCC_PLLMul_13                    ((uint32_t)0x002C0000)
#define RCC_PLLMul_14                    ((uint32_t)0x00300000)
#define RCC_PLLMul_15                    ((uint32_t)0x00340000)
#define RCC_PLLMul_16                    ((uint32_t)0x00380000)
#define RCC_PLLMul_18                    ((uint32_t)0x003C0000)

#else
#define RCC_PLLMul_18_EXTEN              ((uint32_t)0x00000000)
#define RCC_PLLMul_3_EXTEN               ((uint32_t)0x00040000)
#define RCC_PLLMul_4_EXTEN               ((uint32_t)0x00080000)
#define RCC_PLLMul_5_EXTEN               ((uint32_t)0x000C0000)
#define RCC_PLLMul_6_EXTEN               ((uint32_t)0x00100000)
#define RCC_PLLMul_7_EXTEN               ((uint32_t)0x00140000)
#define RCC_PLLMul_8_EXTEN               ((uint32_t)0x00180000)
#define RCC_PLLMul_9_EXTEN               ((uint32_t)0x001C0000)
#define RCC_PLLMul_10_EXTEN              ((uint32_t)0x00200000)
#define RCC_PLLMul_11_EXTEN              ((uint32_t)0x00240000)
#define RCC_PLLMul_12_EXTEN              ((uint32_t)0x00280000)
#define RCC_PLLMul_13_EXTEN              ((uint32_t)0x002C0000)
#define RCC_PLLMul_14_EXTEN              ((uint32_t)0x00300000)
#define RCC_PLLMul_6_5_EXTEN             ((uint32_t)0x00340000)
#define RCC_PLLMul_15_EXTEN              ((uint32_t)0x00380000)
#define RCC_PLLMul_16_EXTEN              ((uint32_t)0x003C0000)

#endif

/* PREDIV1_division_factor */
#ifdef CH32V30x_D8C
#define RCC_PREDIV1_Div1                 ((uint32_t)0x00000000)
#define RCC_PREDIV1_Div2                 ((uint32_t)0x00000001)
#define RCC_PREDIV1_Div3                 ((uint32_t)0x00000002)
#define RCC_PREDIV1_Div4                 ((uint32_t)0x00000003)
#define RCC_PREDIV1_Div5                 ((uint32_t)0x00000004)
#define RCC_PREDIV1_Div6                 ((uint32_t)0x00000005)
#define RCC_PREDIV1_Div7                 ((uint32_t)0x00000006)
#define RCC_PREDIV1_Div8                 ((uint32_t)0x00000007)
#define RCC_PREDIV1_Div9                 ((uint32_t)0x00000008)
#define RCC_PREDIV1_Div10                ((uint32_t)0x00000009)
#define RCC_PREDIV1_Div11                ((uint32_t)0x0000000A)
#define RCC_PREDIV1_Div12                ((uint32_t)0x0000000B)
#define RCC_PREDIV1_Div13                ((uint32_t)0x0000000C)
#define RCC_PREDIV1_Div14                ((uint32_t)0x0000000D)
#define RCC_PREDIV1_Div15                ((uint32_t)0x0000000E)
#define RCC_PREDIV1_Div16                ((uint32_t)0x0000000F)

#endif

/* PREDIV1_clock_source */
#ifdef CH32V30x_D8C
#define RCC_PREDIV1_Source_HSE           ((uint32_t)0x00000000)
#define RCC_PREDIV1_Source_PLL2          ((uint32_t)0x00010000)

#endif

/* PREDIV2_division_factor */
#ifdef CH32V30x_D8C
#define RCC_PREDIV2_Div1                 ((uint32_t)0x00000000)
#define RCC_PREDIV2_Div2                 ((uint32_t)0x00000010)
#define RCC_PREDIV2_Div3                 ((uint32_t)0x00000020)
#define RCC_PREDIV2_Div4                 ((uint32_t)0x00000030)
#define RCC_PREDIV2_Div5                 ((uint32_t)0x00000040)
#define RCC_PREDIV2_Div6                 ((uint32_t)0x00000050)
#define RCC_PREDIV2_Div7                 ((uint32_t)0x00000060)
#define RCC_PREDIV2_Div8                 ((uint32_t)0x00000070)
#define RCC_PREDIV2_Div9                 ((uint32_t)0x00000080)
#define RCC_PREDIV2_Div10                ((uint32_t)0x00000090)
#define RCC_PREDIV2_Div11                ((uint32_t)0x000000A0)
#define RCC_PREDIV2_Div12                ((uint32_t)0x000000B0)
#define RCC_PREDIV2_Div13                ((uint32_t)0x000000C0)
#define RCC_PREDIV2_Div14                ((uint32_t)0x000000D0)
#define RCC_PREDIV2_Div15                ((uint32_t)0x000000E0)
#define RCC_PREDIV2_Div16                ((uint32_t)0x000000F0)

#endif

/* PLL2_multiplication_factor */
#ifdef CH32V30x_D8C
#define RCC_PLL2Mul_2_5                  ((uint32_t)0x00000000)
#define RCC_PLL2Mul_12_5                 ((uint32_t)0x00000100)
#define RCC_PLL2Mul_4                    ((uint32_t)0x00000200)
#define RCC_PLL2Mul_5                    ((uint32_t)0x00000300)
#define RCC_PLL2Mul_6                    ((uint32_t)0x00000400)
#define RCC_PLL2Mul_7                    ((uint32_t)0x00000500)
#define RCC_PLL2Mul_8                    ((uint32_t)0x00000600)
#define RCC_PLL2Mul_9                    ((uint32_t)0x00000700)
#define RCC_PLL2Mul_10                   ((uint32_t)0x00000800)
#define RCC_PLL2Mul_11                   ((uint32_t)0x00000900)
#define RCC_PLL2Mul_12                   ((uint32_t)0x00000A00)
#define RCC_PLL2Mul_13                   ((uint32_t)0x00000B00)
#define RCC_PLL2Mul_14                   ((uint32_t)0x00000C00)
#define RCC_PLL2Mul_15                   ((uint32_t)0x00000D00)
#define RCC_PLL2Mul_16                   ((uint32_t)0x00000E00)
#define RCC_PLL2Mul_20                   ((uint32_t)0x00000F00)

#endif

/* PLL3_multiplication_factor */
#ifdef CH32V30x_D8C
#define RCC_PLL3Mul_2_5                  ((uint32_t)0x00000000)
#define RCC_PLL3Mul_12_5                 ((uint32_t)0x00001000)
#define RCC_PLL3Mul_4                    ((uint32_t)0x00002000)
#define RCC_PLL3Mul_5                    ((uint32_t)0x00003000)
#define RCC_PLL3Mul_6                    ((uint32_t)0x00004000)
#define RCC_PLL3Mul_7                    ((uint32_t)0x00005000)
#define RCC_PLL3Mul_8                    ((uint32_t)0x00006000)
#define RCC_PLL3Mul_9                    ((uint32_t)0x00007000)
#define RCC_PLL3Mul_10                   ((uint32_t)0x00008000)
#define RCC_PLL3Mul_11                   ((uint32_t)0x00009000)
#define RCC_PLL3Mul_12                   ((uint32_t)0x0000A000)
#define RCC_PLL3Mul_13                   ((uint32_t)0x0000B000)
#define RCC_PLL3Mul_14                   ((uint32_t)0x0000C000)
#define RCC_PLL3Mul_15                   ((uint32_t)0x0000D000)
#define RCC_PLL3Mul_16                   ((uint32_t)0x0000E000)
#define RCC_PLL3Mul_20                   ((uint32_t)0x0000F000)

#endif

/* System_clock_source */
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001)
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002)

/* AHB_clock_source */
#define RCC_SYSCLK_Div1                  ((uint32_t)0x00000000)
#define RCC_SYSCLK_Div2                  ((uint32_t)0x00000080)
#define RCC_SYSCLK_Div4                  ((uint32_t)0x00000090)
#define RCC_SYSCLK_Div8                  ((uint32_t)0x000000A0)
#define RCC_SYSCLK_Div16                 ((uint32_t)0x000000B0)
#define RCC_SYSCLK_Div64                 ((uint32_t)0x000000C0)
#define RCC_SYSCLK_Div128                ((uint32_t)0x000000D0)
#define RCC_SYSCLK_Div256                ((uint32_t)0x000000E0)
#define RCC_SYSCLK_Div512                ((uint32_t)0x000000F0)

/* APB1_APB2_clock_source */
#define RCC_HCLK_Div1                    ((uint32_t)0x00000000)
#define RCC_HCLK_Div2                    ((uint32_t)0x00000400)
#define RCC_HCLK_Div4                    ((uint32_t)0x00000500)
#define RCC_HCLK_Div8                    ((uint32_t)0x00000600)
#define RCC_HCLK_Div16                   ((uint32_t)0x00000700)

/* RCC_Interrupt_source */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_LSERDY                    ((uint8_t)0x02)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_PLLRDY                    ((uint8_t)0x10)
#define RCC_IT_CSS                       ((uint8_t)0x80)

#ifdef CH32V30x_D8C
#define RCC_IT_PLL2RDY                   ((uint8_t)0x20)
#define RCC_IT_PLL3RDY                   ((uint8_t)0x40)

#endif

/* USB_OTG_FS_clock_source */
#define RCC_OTGFSCLKSource_PLLCLK_Div1   ((uint8_t)0x00)
#define RCC_OTGFSCLKSource_PLLCLK_Div2   ((uint8_t)0x01)
#define RCC_OTGFSCLKSource_PLLCLK_Div3   ((uint8_t)0x02)

/* I2S2_clock_source */
#ifdef CH32V30x_D8C
#define RCC_I2S2CLKSource_SYSCLK         ((uint8_t)0x00)
#define RCC_I2S2CLKSource_PLL3_VCO       ((uint8_t)0x01)

#endif

/* I2S3_clock_source */
#ifdef CH32V30x_D8C
#define RCC_I2S3CLKSource_SYSCLK         ((uint8_t)0x00)
#define RCC_I2S3CLKSource_PLL3_VCO       ((uint8_t)0x01)

#endif

/* ADC_clock_source */
#define RCC_PCLK2_Div2                   ((uint32_t)0x00000000)
#define RCC_PCLK2_Div4                   ((uint32_t)0x00004000)
#define RCC_PCLK2_Div6                   ((uint32_t)0x00008000)
#define RCC_PCLK2_Div8                   ((uint32_t)0x0000C000)

/* LSE_configuration */
#define RCC_LSE_OFF                      ((uint8_t)0x00)
#define RCC_LSE_ON                       ((uint8_t)0x01)
#define RCC_LSE_Bypass                   ((uint8_t)0x04)

/* RTC_clock_source */
#define RCC_RTCCLKSource_LSE             ((uint32_t)0x00000100)
#define RCC_RTCCLKSource_LSI             ((uint32_t)0x00000200)
#define RCC_RTCCLKSource_HSE_Div128      ((uint32_t)0x00000300)

/* AHB_peripheral */
#define RCC_AHBPeriph_DMA1               ((uint32_t)0x00000001)
#define RCC_AHBPeriph_DMA2               ((uint32_t)0x00000002)
#define RCC_AHBPeriph_SRAM               ((uint32_t)0x00000004)
#define RCC_AHBPeriph_CRC                ((uint32_t)0x00000040)
#define RCC_AHBPeriph_FSMC               ((uint32_t)0x00000100)
#define RCC_AHBPeriph_RNG                ((uint32_t)0x00000200)
#define RCC_AHBPeriph_SDIO               ((uint32_t)0x00000400)
#define RCC_AHBPeriph_USBHS              ((uint32_t)0x00000800)
#define RCC_AHBPeriph_OTG_FS             ((uint32_t)0x00001000)
#define RCC_AHBPeriph_DVP                ((uint32_t)0x00002000)
#define RCC_AHBPeriph_ETH_MAC            ((uint32_t)0x00004000)
#define RCC_AHBPeriph_ETH_MAC_Tx         ((uint32_t)0x00008000)
#define RCC_AHBPeriph_ETH_MAC_Rx         ((uint32_t)0x00010000)

/* APB2_peripheral */
#define RCC_APB2Periph_AFIO              ((uint32_t)0x00000001)
#define RCC_APB2Periph_GPIOA             ((uint32_t)0x00000004)
#define RCC_APB2Periph_GPIOB             ((uint32_t)0x00000008)
#define RCC_APB2Periph_GPIOC             ((uint32_t)0x00000010)
#define RCC_APB2Periph_GPIOD             ((uint32_t)0x00000020)
#define RCC_APB2Periph_GPIOE             ((uint32_t)0x00000040)
#define RCC_APB2Periph_ADC1              ((uint32_t)0x00000200)
#define RCC_APB2Periph_ADC2              ((uint32_t)0x00000400)
#define RCC_APB2Periph_TIM1              ((uint32_t)0x00000800)
#define RCC_APB2Periph_SPI1              ((uint32_t)0x00001000)
#define RCC_APB2Periph_TIM8              ((uint32_t)0x00002000)
#define RCC_APB2Periph_USART1            ((uint32_t)0x00004000)
#define RCC_APB2Periph_TIM9              ((uint32_t)0x00080000)
#define RCC_APB2Periph_TIM10             ((uint32_t)0x00100000)

/* APB1_peripheral */
#define RCC_APB1Periph_TIM2              ((uint32_t)0x00000001)
#define RCC_APB1Periph_TIM3              ((uint32_t)0x00000002)
#define RCC_APB1Periph_TIM4              ((uint32_t)0x00000004)
#define RCC_APB1Periph_TIM5              ((uint32_t)0x00000008)
#define RCC_APB1Periph_TIM6              ((uint32_t)0x00000010)
#define RCC_APB1Periph_TIM7              ((uint32_t)0x00000020)
#define RCC_APB1Periph_UART6             ((uint32_t)0x00000040)
#define RCC_APB1Periph_UART7             ((uint32_t)0x00000080)
#define RCC_APB1Periph_UART8             ((uint32_t)0x00000100)
#define RCC_APB1Periph_WWDG              ((uint32_t)0x00000800)
#define RCC_APB1Periph_SPI2              ((uint32_t)0x00004000)
#define RCC_APB1Periph_SPI3              ((uint32_t)0x00008000)
#define RCC_APB1Periph_USART2            ((uint32_t)0x00020000)
#define RCC_APB1Periph_USART3            ((uint32_t)0x00040000)
#define RCC_APB1Periph_UART4             ((uint32_t)0x00080000)
#define RCC_APB1Periph_UART5             ((uint32_t)0x00100000)
#define RCC_APB1Periph_I2C1              ((uint32_t)0x00200000)
#define RCC_APB1Periph_I2C2              ((uint32_t)0x00400000)
#define RCC_APB1Periph_USB               ((uint32_t)0x00800000)
#define RCC_APB1Periph_CAN1              ((uint32_t)0x02000000)
#define RCC_APB1Periph_CAN2              ((uint32_t)0x04000000)
#define RCC_APB1Periph_BKP               ((uint32_t)0x08000000)
#define RCC_APB1Periph_PWR               ((uint32_t)0x10000000)
#define RCC_APB1Periph_DAC               ((uint32_t)0x20000000)

/* Clock_source_to_output_on_MCO_pin */
#define RCC_MCO_NoClock                  ((uint8_t)0x00)
#define RCC_MCO_SYSCLK                   ((uint8_t)0x04)
#define RCC_MCO_HSI                      ((uint8_t)0x05)
#define RCC_MCO_HSE                      ((uint8_t)0x06)
#define RCC_MCO_PLLCLK_Div2              ((uint8_t)0x07)

#ifdef CH32V30x_D8C
#define RCC_MCO_PLL2CLK                  ((uint8_t)0x08)
#define RCC_MCO_PLL3CLK_Div2             ((uint8_t)0x09)
#define RCC_MCO_XT1                      ((uint8_t)0x0A)
#define RCC_MCO_PLL3CLK                  ((uint8_t)0x0B)

#endif

/* RCC_Flag */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_PLLRDY                  ((uint8_t)0x39)
#define RCC_FLAG_LSERDY                  ((uint8_t)0x41)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x61)
#define RCC_FLAG_PINRST                  ((uint8_t)0x7A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x7B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x7C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x7D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x7E)
#define RCC_FLAG_LPWRRST                 ((uint8_t)0x7F)

#ifdef CH32V30x_D8C
#define RCC_FLAG_PLL2RDY                 ((uint8_t)0x3B)
#define RCC_FLAG_PLL3RDY                 ((uint8_t)0x3D)

#endif

/* SysTick_clock_source */
#define SysTick_CLKSource_HCLK_Div8      ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK           ((uint32_t)0x00000004)

/* RNG_clock_source */
#ifdef CH32V30x_D8C
#define RCC_RNGCLKSource_SYSCLK          ((uint32_t)0x00)
#define RCC_RNGCLKSource_PLL3_VCO        ((uint32_t)0x01)

#endif

/* ETH1G_clock_source */
#ifdef CH32V30x_D8C
#define RCC_ETH1GCLKSource_PLL2_VCO      ((uint32_t)0x00)
#define RCC_ETH1GCLKSource_PLL3_VCO      ((uint32_t)0x01)
#define RCC_ETH1GCLKSource_PB1_IN        ((uint32_t)0x02)

#endif

/* USBFS_clock_source */
#ifdef CH32V30x_D8C
#define RCC_USBPLL_Div1                  ((uint32_t)0x00)
#define RCC_USBPLL_Div2                  ((uint32_t)0x01)
#define RCC_USBPLL_Div3                  ((uint32_t)0x02)
#define RCC_USBPLL_Div4                  ((uint32_t)0x03)
#define RCC_USBPLL_Div5                  ((uint32_t)0x04)
#define RCC_USBPLL_Div6                  ((uint32_t)0x05)
#define RCC_USBPLL_Div7                  ((uint32_t)0x06)
#define RCC_USBPLL_Div8                  ((uint32_t)0x07)

#endif

/* USBHSPLL_clock_source */
#ifdef CH32V30x_D8C
#define RCC_HSBHSPLLCLKSource_HSE        ((uint32_t)0x00)
#define RCC_HSBHSPLLCLKSource_HSI        ((uint32_t)0x01)

#endif

/* USBHSPLLCKREF_clock_select */
#ifdef CH32V30x_D8C
#define RCC_USBHSPLLCKREFCLK_3M          ((uint32_t)0x00)
#define RCC_USBHSPLLCKREFCLK_4M          ((uint32_t)0x01)
#define RCC_USBHSPLLCKREFCLK_8M          ((uint32_t)0x02)
#define RCC_USBHSPLLCKREFCLK_5M          ((uint32_t)0x03)

#endif

/* OTGUSBCLK48M_clock_source */
#define RCC_USBCLK48MCLKSource_PLLCLK    ((uint32_t)0x00)
#define RCC_USBCLK48MCLKSource_USBPHY    ((uint32_t)0x01)


void RCC_DeInit(void);
void RCC_HSEConfig(uint32_t RCC_HSE);
ErrorStatus RCC_WaitForHSEStartUp(void);
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue);
void RCC_HSICmd(FunctionalState NewState);
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul);
void RCC_PLLCmd(FunctionalState NewState);
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t RCC_GetSYSCLKSource(void);
void RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void RCC_PCLK1Config(uint32_t RCC_HCLK);
void RCC_PCLK2Config(uint32_t RCC_HCLK);
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState);
void RCC_ADCCLKConfig(uint32_t RCC_PCLK2);
void RCC_LSEConfig(uint8_t RCC_LSE);
void RCC_LSICmd(FunctionalState NewState);
void RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
void RCC_RTCCLKCmd(FunctionalState NewState);
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks);
void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState); 
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState);
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState);
void RCC_BackupResetCmd(FunctionalState NewState);
void RCC_ClockSecuritySystemCmd(FunctionalState NewState);
void RCC_MCOConfig(uint8_t RCC_MCO);
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG);
void RCC_ClearFlag(void);
ITStatus RCC_GetITStatus(uint8_t RCC_IT);
void RCC_ClearITPendingBit(uint8_t RCC_IT);
void RCC_ADCCLKADJcmd(FunctionalState NewState);
void RCC_OTGFSCLKConfig(uint32_t RCC_OTGFSCLKSource);
void RCC_USBCLK48MConfig(uint32_t RCC_USBCLK48MSource);

#ifdef CH32V30x_D8C
void RCC_PREDIV1Config(uint32_t RCC_PREDIV1_Source, uint32_t RCC_PREDIV1_Div);
void RCC_PREDIV2Config(uint32_t RCC_PREDIV2_Div);
void RCC_PLL2Config(uint32_t RCC_PLL2Mul);
void RCC_PLL2Cmd(FunctionalState NewState);
void RCC_PLL3Config(uint32_t RCC_PLL3Mul);
void RCC_PLL3Cmd(FunctionalState NewState);
void RCC_I2S2CLKConfig(uint32_t RCC_I2S2CLKSource);
void RCC_I2S3CLKConfig(uint32_t RCC_I2S3CLKSource);
void RCC_AHBPeriphResetCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState);
void RCC_RNGCLKConfig(uint32_t RCC_RNGCLKSource);
void RCC_ETH1GCLKConfig(uint32_t RCC_ETH1GCLKSource);
void RCC_ETH1G_125Mcmd(FunctionalState NewState);
void RCC_USBHSConfig(uint32_t RCC_USBHS);
void RCC_USBHSPLLCLKConfig(uint32_t RCC_USBHSPLLCLKSource);
void RCC_USBHSPLLCKREFCLKConfig(uint32_t RCC_USBHSPLLCKREFCLKSource);
void RCC_USBHSPHYPLLALIVEcmd(FunctionalState NewState);

#endif

#ifdef __cplusplus
}
#endif

#endif 





